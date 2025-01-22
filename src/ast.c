#include "ast.h"
#include "util_error.h"
#include <stdio.h>
#include <stdlib.h>

#define INITIAL_STATEMENTS_CAPACITY 4

struct statement *ast_statement_init(enum STATEMENT_TYPE type) {
  struct statement *s = (struct statement *)malloc(sizeof(struct statement));
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
  }
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
  p->statement_capacity = INITIAL_STATEMENTS_CAPACITY;
  p->statements =
      malloc(sizeof(struct statement *) * INITIAL_STATEMENTS_CAPACITY);
  if (p->statements == NULL) {
    ERROR_LOG("error while allocating memory");
    free(p);
    return NULL;
  }
  p->statement_count = 0;
  return p;
}

void ast_program_push_statement(struct program *p, struct statement *s) {
  if (p == NULL || s == NULL) {
    ERROR_LOG("program or statement is null");
    return;
  }

  if (p->statements == NULL) {
    p->statement_capacity = INITIAL_STATEMENTS_CAPACITY;
    p->statements = malloc(sizeof(struct statement *) * p->statement_capacity);
    if (p->statements == NULL) {
      ERROR_LOG("error while allocating memory");
      return;
    }
    p->statement_count = 0;
  } else {
    if (p->statement_count >= p->statement_capacity) {
      size_t new_capacity = p->statement_capacity * 2;
      struct statement **new =
          realloc(p->statements, sizeof(struct statement *) * new_capacity);
      if (!new) {
        ERROR_LOG("error while allocating memory");
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
