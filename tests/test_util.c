#include "test_util.h"
#include "ast.h"
#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *str_concat(const char *str1, size_t len1, const char *str2, size_t len2) {
  char *result = malloc(len1 + len2 + 1); // +1 for the null-terminator
  if (!result) {
    fprintf(stderr, "failed to allocate memory for repr string\n");
    return NULL;
  }
  strncpy(result, str1, len1);
  strncat(result, str2, len2);
  return result;
}

char *t_stmt_repr(struct statement *stmt) {
  char *repr = NULL;

  switch (stmt->type) {
  case STMT_LET: {
    repr = str_concat(stmt->let_stmt.token->literal,
                      stmt->let_stmt.token->literal_len,
                      stmt->let_stmt.identifier->literal,
                      stmt->let_stmt.identifier->literal_len);
    char *assign = str_concat(repr, strlen(repr), ":=", 2);
    free(repr);
    repr = assign;
    char *value = t_expr_repr(stmt->let_stmt.value);
    char *final_repr = str_concat(repr, strlen(repr), value, strlen(value));
    free(repr);
    free(value);
    return final_repr;
  }
  case STMT_RETURN: {
    repr = str_concat(stmt->return_stmt.token->literal,
                      stmt->return_stmt.token->literal_len, "", 0);
    char *value = t_expr_repr(stmt->return_stmt.value);
    char *final_repr = str_concat(repr, strlen(repr), value, strlen(value));
    free(repr);
    free(value);
    return final_repr;
  }
  case STMT_EXPRESSION: {
    return t_expr_repr(stmt->expr_stmt.expr);
  }
  default:
    return strdup("");
  }
}

char *t_expr_repr(struct expression *expr) {
  char *repr = NULL;

  switch (expr->type) {
  case EXPR_LITERAL: {
    switch (expr->literal.literal_type) {
    case LITERAL_INT: {
      char buffer[expr->literal.token->literal_len + 1];
      snprintf(buffer, sizeof(buffer), "%ld", expr->literal.value.int_value);
      buffer[expr->literal.token->literal_len] = '\0';
      return strndup(buffer, expr->literal.token->literal_len);
    }; break;
    case LITERAL_FLOAT: {
      char buffer[expr->literal.token->literal_len + 1];
      snprintf(buffer, sizeof(buffer), "%f", expr->literal.value.float_value);
      return strndup(buffer, expr->literal.token->literal_len);
    }; break;
    case LITERAL_STRING: {
      return strndup(expr->literal.token->literal,
                     expr->literal.token->literal_len);
    }; break;
    case LITERAL_BOOL: {
      return strndup(expr->literal.token->literal,
                     expr->literal.token->literal_len);
    }; break;
    case LITERAL_CHAR: {
      return strndup(expr->literal.token->literal,
                     expr->literal.token->literal_len);
    }; break;
    }
  }
  case EXPR_IDENTIFIER: {
    return strndup(expr->identifier_expr.token->literal,
                   expr->identifier_expr.token->literal_len);
  }
  case EXPR_PREFIX: {
    repr = str_concat("(", 1, expr->infix_expr.op->literal,
                      expr->infix_expr.op->literal_len);
    char *right = t_expr_repr(expr->infix_expr.right);
    char *temp = str_concat(repr, strlen(repr), right, strlen(right));
    free(repr);
    repr = str_concat(temp, strlen(temp), ")", 1);
    free(temp);
    free(right);
    return repr;
  }
  case EXPR_INFIX: {
    repr = str_concat("(", 1, "", 0);
    char *left = t_expr_repr(expr->infix_expr.left);
    char *temp = str_concat(repr, strlen(repr), left, strlen(left));
    free(repr);
    repr = str_concat(temp, strlen(temp), expr->infix_expr.op->literal,
                      expr->infix_expr.op->literal_len);
    free(temp);
    char *right = t_expr_repr(expr->infix_expr.right);
    temp = str_concat(repr, strlen(repr), right, strlen(right));
    free(repr);
    repr = str_concat(temp, strlen(temp), ")", 1);
    free(temp);
    free(left);
    free(right);
    return repr;
  }
  default:
    return strdup("");
  }
}

void test_repr() {
  struct statement *stmt = ast_statement_init(STMT_LET);
  stmt->let_stmt.token = malloc(sizeof(struct token));
  stmt->let_stmt.token->literal = "let";
  stmt->let_stmt.token->literal_len = 3;
  stmt->let_stmt.identifier = malloc(sizeof(struct token));
  stmt->let_stmt.identifier->literal = "x";
  stmt->let_stmt.identifier->literal_len = 1;
  stmt->let_stmt.assign = malloc(sizeof(struct token));
  stmt->let_stmt.assign->literal = ":=";
  stmt->let_stmt.assign->literal_len = 2;
  stmt->let_stmt.value = ast_expression_init(EXPR_LITERAL);
  stmt->let_stmt.value->literal.token = malloc(sizeof(struct token));
  stmt->let_stmt.value->literal.token->literal = "5";
  stmt->let_stmt.value->literal.token->literal_len = 1;
  char *repr = t_stmt_repr(stmt);
  printf("%s\n", repr);
  free(repr);
  ast_statement_free(stmt);
}