#include "ast.h"
#include "util_error.h"
#include <stdlib.h>

struct statement *ast_statement_init(enum STATEMENT_TYPE type) {
  struct statement *s = (struct statement *)malloc(sizeof(struct statement));
  s->type = type;
  return s;
}

void ast_statement_free(struct statement *s) {
  if (s != NULL) {
    switch (s->type) {
    case STMT_LET:
      free(s->let_stmt.token);
      free(s->let_stmt.identifier);
      ast_expression_free(s->let_stmt.value);
      break;
    case STMT_RETURN:
      free(s->return_stmt.token);
      ast_expression_free(s->return_stmt.value);
      break;
    case STMT_EXPRESSION:
      free(s->expr_stmt.token);
      ast_expression_free(s->expr_stmt.expr);
      break;
    }
    free(s);
  }
  s = NULL;
}

struct expression *ast_expression_init(enum EXPRESSION_TYPE e) {
  struct expression *expr =
      (struct expression *)malloc(sizeof(struct expression));
  if (expr == NULL) {
    ERROR_LOG("error while allocating memory");
    return NULL;
  }
  expr->type = e;
  return expr;
}

void ast_expression_free(struct expression *e) {
  if (e != NULL) {
    switch (e->type) {
    case EXPR_LITERAL:
      ast_literal_free(&e->literal);
      break;
    case EXPR_IDENTIFIER:
      free(e->identifier_expr.token);
      break;
    case EXPR_BINARY:
      ast_expression_free(e->binary_expr.left);
      free(e->binary_expr.op);
      ast_expression_free(e->binary_expr.left);
      break;
    case EXPR_UNARY:
      free(e->unary_expr.op);
      ast_expression_free(e->unary_expr.right);
      break;
    case EXPR_IF:
      free(e->if_expr.token);
      if (e->if_expr.condition)
        ast_expression_free(e->if_expr.condition);
      for (size_t i = 0; i < e->if_expr.consequence_count; i++) {
        ast_expression_free(e->if_expr.consequence[i]);
      }
      for (size_t i = 0; i < e->if_expr.alternative_count; i++) {
        ast_expression_free(e->if_expr.alternative[i]);
      }
      break;
    case EXPR_FOR:
      free(e->for_expr.token);
      if (e->for_expr.init)
        ast_statement_free(e->for_expr.init);
      if (e->for_expr.condition)
        ast_expression_free(e->for_expr.condition);
      if (e->for_expr.update)
        ast_statement_free(e->for_expr.update);
      for (size_t i = 0; i < e->for_expr.body_count; i++) {
        ast_expression_free(e->for_expr.body[i]);
      }
      break;
    case EXPR_WHILE:
      free(e->while_expr.token);
      if (e->while_expr.condition)
        ast_expression_free(e->while_expr.condition);
      for (size_t i = 0; i < e->while_expr.body_count; i++) {
        ast_expression_free(e->while_expr.body[i]);
      }
      break;
    case EXPR_FN_CALL:
      free(e->fn_call_expr.fn_name);
      for (size_t i = 0; i < e->fn_call_expr.arg_count; i++) {
        ast_expression_free(e->fn_call_expr.args[i]);
      }
      break;
    }
    free(e);
  }
  e = NULL;
}

struct literal *ast_literal_init(enum LITERAL_TYPE type) {
  struct literal *l = (struct literal *)malloc(sizeof(struct literal));
  if (l == NULL) {
    ERROR_LOG("error while allocating memory");
    return NULL;
  }
  l->literal_type = type;
  return l;
}

void ast_literal_free(struct literal *literal) {
  if (!literal)
    return;

  switch (literal->literal_type) {
  case LITERAL_STRING:
    free(literal->value.string_literal->value);
    free(literal->value.string_literal);
    free(literal->token);
    break;
  case LITERAL_ARRAY:
    for (size_t i = 0; i < literal->value.array_literal->count; i++) {
      ast_literal_free(&literal->value.array_literal->elements[i]);
    }
    free(literal->value.array_literal->elements);
    free(literal->value.array_literal);
    free(literal->token);
    break;
  case LITERAL_STRUCT:
    for (size_t i = 0; i < literal->value.struct_literal->field_count; i++) {
      ast_literal_free(literal->value.struct_literal->fields[i]);
      free(literal->value.struct_literal->fieldnames[i]);
    }
    free(literal->value.struct_literal->fields);
    free(literal->value.struct_literal->fieldnames);
    free(literal->value.struct_literal);
    free(literal->token);
    break;
  default:
    break;
  }
  /** NOTE: not needed since the struct of expression not holding to heap
   * allocated memory */
  // free(literal);
}

struct program *ast_program_init() {
  struct program *p = (struct program *)malloc(sizeof(struct program));
  if (p == NULL) {
    ERROR_LOG("error while allocating memory");
    return NULL;
  }
  p->statements = (struct statement **)malloc(sizeof(struct statement *));
  if (p->statements == NULL) {
    ERROR_LOG("error while allocating memory");
    return NULL;
  }
  p->statement_count = 0;
  return p;
}

void ast_program_push_statement(struct program *p, struct statement *s) {
  p->statements = (struct statement **)realloc(
      p->statements, sizeof(struct statement *) * (p->statement_count + 1));
  if (p->statements == NULL) {
    ERROR_LOG("error while allocating memory");
    return;
  }
  p->statements[p->statement_count] = s;
  p->statement_count++;
}

void ast_program_free(struct program *p) {
  for (size_t i = 0; i < p->statement_count; i++) {
    ast_statement_free(p->statements[i]);
  }
  free(p->statements);
  free(p);
}