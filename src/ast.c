#include "ast.h"
#include "util_error.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define INITIAL_STATEMENTS_CAPACITY 4

struct statement *ast_statement_init(enum STATEMENT_TYPE type) {
  struct statement *s = (struct statement *)malloc(sizeof(struct statement));
  if (!s) {
    ERROR_LOG("error while allocating memory\n");
    return NULL;
  }
  s->type = type;
  switch (s->type) {
  case STMT_LET: {
    s->let_stmt.token = NULL;
    s->let_stmt.identifier = NULL;
    s->let_stmt.value = NULL;

  }; break;
  case STMT_RETURN: {
    s->return_stmt.token = NULL;
    s->return_stmt.value = NULL;
  }; break;
  case STMT_EXPRESSION: {
    s->expr_stmt.expr = NULL;
    s->expr_stmt.token = NULL;
  }; break;
  case STMT_FUNCTION_DEF: {
    s->fn_def_stmt.token = NULL;
    s->fn_def_stmt.name = NULL;
    s->fn_def_stmt.params = NULL;
    s->fn_def_stmt.params_count = 0;
    s->fn_def_stmt.params_capacity = 0;
    s->fn_def_stmt.body = NULL;
  }; break;
  }
  return s;
}

void ast_statement_free(struct statement *s) {
  if (s != NULL) {
    switch (s->type) {
    case STMT_LET:
      ast_expression_free(s->let_stmt.value);
      break;
    case STMT_RETURN:
      ast_expression_free(s->return_stmt.value);
      break;
    case STMT_EXPRESSION:
      ast_expression_free(s->expr_stmt.expr);
      break;
    case STMT_FUNCTION_DEF: {
        if (s->fn_def_stmt.params) {
          for (size_t i = 0; i < s->fn_def_stmt.params_count; i++) {
            free(s->fn_def_stmt.params[i]);
            s->fn_def_stmt.params[i] = NULL;
          }
          free(s->fn_def_stmt.params);
        }
        ast_block_statement_free(s->fn_def_stmt.body);
    }; break;
    }
    free(s);
  }
  s = NULL;
}

struct expression *ast_expression_init(enum EXPRESSION_TYPE e) {
  struct expression *expr =
      (struct expression *)malloc(sizeof(struct expression));
  if (expr == NULL) {
    ERROR_LOG("error while allocating memory\n");
    return NULL;
  }
  expr->type = e;
  switch (expr->type) {
  case EXPR_LITERAL: {
    expr->literal.token = NULL;
    expr->literal.literal_type = LITERAL_INT;
    expr->literal.value.int_value = 0;
  }; break;
  case EXPR_IDENTIFIER: {
    expr->identifier_expr.token = NULL;
  }; break;
  case EXPR_INFIX: {
    expr->infix_expr.left = NULL;
    expr->infix_expr.op = NULL;
    expr->infix_expr.right = NULL;
  }; break;
  case EXPR_PREFIX: {
    expr->prefix_expr.op = NULL;
    expr->prefix_expr.right = NULL;
  }; break;
  case EXPR_POSTFIX: {
    expr->postfix_expr.left = NULL;
    expr->postfix_expr.op = NULL;
  }; break;
  case EXPR_CONDITIONAL: {
    expr->conditional.token = NULL;
    expr->conditional.condition = NULL;
    expr->conditional.consequence = NULL;
    expr->conditional.alternative = NULL;
  }; break;
  case EXPR_FUNCTION: {
    expr->function.token = NULL;
    expr->function.parameters = NULL;
    expr->function.param_count = 0;
    expr->function.param_capacity = 0;
    expr->function.body = NULL;
  }; break;
  }
  return expr;
}

void ast_expression_free(struct expression *e) {
  if (e != NULL) {
    switch (e->type) {
    case EXPR_LITERAL: {
      ast_literal_free(&e->literal);
    }; break;
    case EXPR_IDENTIFIER: {
    }; break;
    case EXPR_PREFIX: {
      ast_expression_free(e->prefix_expr.right);
    }; break;
    case EXPR_INFIX: {
      ast_expression_free(e->infix_expr.left);
      ast_expression_free(e->infix_expr.right);
    }; break;
    case EXPR_POSTFIX: {
      ast_expression_free(e->postfix_expr.left);
    }; break;
    case EXPR_CONDITIONAL: {
      ast_expression_free(e->conditional.condition);
      ast_block_statement_free(e->conditional.consequence);
      ast_block_statement_free(e->conditional.alternative);
    }; break;
    case EXPR_FUNCTION: {
      ast_block_statement_free(e->function.body);
      for (size_t i = 0; i < e->function.param_count; i++)
        ast_identifier_free(e->function.parameters[i]);
    }; break;
    }
    free(e);
  }
  e = NULL;
}

struct literal *ast_literal_init(enum LITERAL_TYPE type) {
  struct literal *l = (struct literal *)malloc(sizeof(struct literal));
  if (l == NULL) {
    ERROR_LOG("error while allocating memory\n");
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
    break;
  default:
    break;
  }
  /** NOTE: not needed since the struct of expression not holding to heap
   * allocated memory */
  // free(literal);
}

struct identifier *ast_identifier_init() {
  struct identifier *ident = malloc(sizeof(struct identifier));
  if (!ident) {
    return NULL;
  }
  ident->token = NULL;
  return ident;
}

void ast_identifier_free(struct identifier *ident) {
  if (ident) {
    free(ident);
  }
  ident = NULL;
}

struct block_statement *ast_block_statement_init() {
  struct block_statement *b_s = malloc(sizeof(struct block_statement));
  if (!b_s) {
    ERROR_LOG("error while allocating memory\n");
    return NULL;
  }
  b_s->token = NULL;
  b_s->statements = NULL;
  b_s->statement_count = 0;
  b_s->statement_capacity = 0;
  return b_s;
}

void ast_block_statement_free(struct block_statement *b_s) {
  if (b_s) {
    if (b_s->statements) {
      for (size_t i = 0; i < b_s->statement_count; i++) {
        free(b_s->statements[i]);
      }
      free(b_s->statements);
      b_s->statements = NULL;
    }
  };
  free(b_s);
  b_s = NULL;
}

void ast_block_statements_push_stmt(struct block_statement *b_s,
                                    struct statement *s) {
  if (!b_s && !s) {
    return;
  }
  if (b_s->statements == NULL) {
    struct statement **stmts =
        malloc(sizeof(struct statement *) * INITIAL_STATEMENTS_CAPACITY);
    if (!stmts) {
      ERROR_LOG("error allocating memory\n");
      return;
    }
    b_s->statements = stmts;
    b_s->statement_count = 0;
    b_s->statement_capacity = INITIAL_STATEMENTS_CAPACITY;
  } else if (b_s->statement_count >= b_s->statement_capacity) {
    size_t new_capacity = b_s->statement_count * 2;
    struct statement **new_stmts =
        realloc(b_s->statements, sizeof(struct statement *) * new_capacity);
    if (!new_stmts) {
      ERROR_LOG("error allocating memory\n");
      return;
    }
    b_s->statement_capacity = new_capacity;
    b_s->statements = new_stmts;
  }

  b_s->statements[b_s->statement_count] = s;
  b_s->statement_count++;
}

struct program *ast_program_init() {
  struct program *p = (struct program *)malloc(sizeof(struct program));
  if (p == NULL) {
    ERROR_LOG("error while allocating memory\n");
    return NULL;
  }
  p->statement_capacity = INITIAL_STATEMENTS_CAPACITY;
  p->statements =
      malloc(sizeof(struct statement *) * INITIAL_STATEMENTS_CAPACITY);
  if (p->statements == NULL) {
    ERROR_LOG("error while allocating memory\n");
    free(p);
    return NULL;
  }
  p->statement_count = 0;
  return p;
}

void ast_program_push_statement(struct program *p, struct statement *s) {
  if (p == NULL || s == NULL) {
    return;
  }

  if (p->statements == NULL) {
    p->statement_capacity = INITIAL_STATEMENTS_CAPACITY;
    p->statements = malloc(sizeof(struct statement *) * p->statement_capacity);
    if (p->statements == NULL) {
      ERROR_LOG("error while allocating memory\n");
      return;
    }
    p->statement_count = 0;
  } else {
    if (p->statement_count >= p->statement_capacity) {
      size_t new_capacity = p->statement_capacity * 2;
      struct statement **new =
          realloc(p->statements, sizeof(struct statement *) * new_capacity);
      if (!new) {
        ERROR_LOG("error while allocating memory\n");
        return;
      }
      p->statement_capacity = new_capacity;
      p->statements = new;
    }
  }

  p->statements[p->statement_count] = s;
  p->statement_count++;
}

void ast_program_free(struct program *p) {
  if (p) {
    for (size_t i = 0; i < p->statement_count; i++) {
      ast_statement_free(p->statements[i]);
    }
    free(p->statements);
    p->statements = NULL;
    free(p);
    p = NULL;
  }
}
