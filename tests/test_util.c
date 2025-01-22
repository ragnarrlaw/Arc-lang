#include "test_util.h"
#include "ast.h"
#include "token.h"
#include "util_repr.h"
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

void t_stmt_repr(struct statement *stmt, string_t *str) {
  switch (stmt->type) {
  case STMT_LET: {
    string_t_ncat(str, (char *)stmt->let_stmt.token->literal, 3);
    string_t_ncat(str, " ", 1);
    string_t_ncat(str, (char *)stmt->let_stmt.identifier->literal,
                  stmt->let_stmt.identifier->literal_len);
    string_t_ncat(str, " := ", 4);
    t_expr_repr(stmt->let_stmt.value, str);
    string_t_ncat(str, ";", 1);
  }; break;
  case STMT_RETURN: {
    string_t_ncat(str, (char *)stmt->return_stmt.token->literal,
                  stmt->return_stmt.token->literal_len);
    string_t_ncat(str, " ", 1);
    t_expr_repr(stmt->return_stmt.value, str);
    string_t_ncat(str, ";", 1);
  }; break;
  case STMT_EXPRESSION: {
    t_expr_repr(stmt->return_stmt.value, str);
  }; break;
  }
}

void t_expr_repr(struct expression *expr, string_t *str) {
  switch (expr->type) {
  case EXPR_LITERAL: {
    switch (expr->literal.literal_type) {
    case LITERAL_INT: {
      char buffer[expr->literal.token->literal_len + 1];
      snprintf(buffer, sizeof(buffer), "%ld", expr->literal.value.int_value);
      string_t_cat(str, buffer);
    }; break;
    case LITERAL_FLOAT: {
      char buffer[expr->literal.token->literal_len + 1];
      snprintf(buffer, sizeof(buffer), "%f", expr->literal.value.float_value);
      string_t_cat(str, buffer);
    }; break;
    case LITERAL_STRING: {
      string_t_ncat(str, (char *)expr->literal.token->literal,
                    expr->literal.token->literal_len);
    }; break;
    case LITERAL_BOOL: {
      string_t_ncat(str, (char *)expr->literal.token->literal,
                    expr->literal.token->literal_len);
    }; break;
    case LITERAL_CHAR: {
      string_t_ncat(str, (char *)expr->literal.token->literal,
                    expr->literal.token->literal_len);
    }; break;
    }
  }; break;
  case EXPR_IDENTIFIER: {
    string_t_ncat(str, (char *)expr->identifier_expr.token->literal,
                  expr->identifier_expr.token->literal_len);
  }; break;
  case EXPR_PREFIX: {
    string_t_ncat(str, "(", 1);
    string_t_ncat(str, (char *)expr->prefix_expr.op->literal, expr->prefix_expr.op->literal_len);
    t_expr_repr(expr->prefix_expr.right, str);
    string_t_ncat(str, ")", 1);
  }; break;
  case EXPR_INFIX: {
    string_t_ncat(str, "(", 1);
    t_expr_repr(expr->infix_expr.left, str);
    string_t_ncat(str, (char*)expr->infix_expr.op->literal, expr->infix_expr.op->literal_len);
    t_expr_repr(expr->infix_expr.right, str);
    string_t_ncat(str, ")", 1);
  }; break;
  case EXPR_POSTFIX: {
    string_t_ncat(str, "(", 1);
    t_expr_repr(expr->postfix_expr.left, str);
    string_t_ncat(str, (char *)expr->postfix_expr.op->literal, expr->postfix_expr.op->literal_len);
    string_t_ncat(str, ")", 1);
  }; break;
  }
}
