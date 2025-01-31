#include "util_repr.h"
#include "object_t.h"
#include "token.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    string_t_cat(str, "return ");
    t_expr_repr(stmt->return_stmt.value, str);
    string_t_ncat(str, ";", 1);
  }; break;
  case STMT_EXPRESSION: {
    t_expr_repr(stmt->expr_stmt.expr, str);
  }; break;
  case STMT_FUNCTION_DEF: {
    string_t_ncat(str, (char *)stmt->fn_def_stmt.token->literal,
                  stmt->fn_def_stmt.token->literal_len);
    string_t_ncat(str, " ", 1);
    string_t_ncat(str, (char *)stmt->fn_def_stmt.name->literal,
                  stmt->fn_def_stmt.name->literal_len);
    string_t_ncat(str, " ", 1);
    string_t_ncat(str, "(", 1);
    for (size_t i = 0; i < stmt->fn_def_stmt.params_count; i++) {
      string_t_ncat(str, (char *)stmt->fn_def_stmt.params[i]->token->literal,
                    stmt->fn_def_stmt.params[i]->token->literal_len);
      string_t_ncat(str, ",", 1);
    }
    string_t_ncat(str, ") ", 2);
    string_t_ncat(str, "{", 1);
    t_block_stmt_repr(stmt->fn_def_stmt.body, str);
    string_t_ncat(str, "}", 1);
  }; break;
  }
}

void t_block_stmt_repr(struct block_statement *b_stmt, string_t *str) {
  if (b_stmt) {
    for (size_t i = 0; i < b_stmt->statement_count; i++) {
      t_stmt_repr(b_stmt->statements[i], str);
    }
  }
}

void t_expr_repr(struct expression *expr, string_t *str) {
  switch (expr->type) {
  case EXPR_LITERAL: {
    switch (expr->literal.literal_type) {
    case LITERAL_INT: {
      char buffer[expr->literal.token->literal_len + 1];
      snprintf(buffer, sizeof(buffer), "%d", expr->literal.value.int_value);
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
    string_t_cat(str, expr->identifier_expr.identifier);
  }; break;
  case EXPR_PREFIX: {
    string_t_ncat(str, "(", 1);
    string_t_cat(str, expr->prefix_expr.op_str);
    t_expr_repr(expr->prefix_expr.right, str);
    string_t_ncat(str, ")", 1);
  }; break;
  case EXPR_INFIX: {
    string_t_ncat(str, "(", 1);
    t_expr_repr(expr->infix_expr.left, str);
    string_t_cat(str, expr->infix_expr.op_str);
    t_expr_repr(expr->infix_expr.right, str);
    string_t_ncat(str, ")", 1);
  }; break;
  case EXPR_POSTFIX: {
    string_t_ncat(str, "(", 1);
    t_expr_repr(expr->postfix_expr.left, str);
    string_t_cat(str, expr->postfix_expr.op_str);
    string_t_ncat(str, ")", 1);
  }; break;
  case EXPR_CONDITIONAL: {
    string_t_cat(str, "if");
    string_t_ncat(str, "(", 1);
    t_expr_repr(expr->conditional.condition, str);
    string_t_ncat(str, ")", 1);
    string_t_ncat(str, "{", 1);
    t_block_stmt_repr(expr->conditional.consequence, str);
    string_t_ncat(str, "}", 1);
    if (expr->conditional.alternative) {
      string_t_ncat(str, " else {", 7);
      t_block_stmt_repr(expr->conditional.alternative, str);
      string_t_ncat(str, "}", 1);
    }
  }; break;
  case EXPR_FUNCTION: {
    string_t_cat(str, "fn");
    string_t_ncat(str, "(", 1);
    for (size_t i = 0; i < expr->function.param_count; i++) {
      string_t_ncat(str, (char *)expr->function.parameters[i]->token->literal,
                    expr->function.parameters[i]->token->literal_len);
      string_t_ncat(str, ",", 1);
    }
    string_t_ncat(str, ")", 1);
    string_t_ncat(str, "{", 1);
    t_block_stmt_repr(expr->function.body, str);
    string_t_ncat(str, "}", 1);
  }; break;
  case EXPR_FUNCTION_CALL: {
    string_t_ncat(str, "(", 1);
    t_expr_repr(expr->function_call.function, str);
    string_t_ncat(str, "(", 1);
    for (size_t i = 0; i < expr->function_call.arg_count; i++) {
      t_expr_repr(expr->function_call.arguments[i], str);
      string_t_ncat(str, ",", 1);
    }
    string_t_ncat(str, ")", 1);
    string_t_ncat(str, ")", 1);
  }; break;
  }
}

void t_object_repr(struct obj_t *object, string_t *str) {
  if (!object) {
    return;
  }
  switch (object->type) {
  case OBJECT_INT: {
    size_t number_len = snprintf(NULL, 0, "%d", object->int_value);
    char buffer[number_len + 1];
    snprintf(buffer, sizeof(buffer), "%d", object->int_value);
    string_t_cat(str, "<integer>(");
    string_t_cat(str, buffer);
    string_t_cat(str, ")");
  }; break;
  case OBJECT_DOUBLE: {
    size_t number_len = snprintf(NULL, 0, "%lf", object->double_value);
    char buffer[number_len + 1];
    snprintf(buffer, sizeof(buffer), "%f", object->double_value);
    string_t_cat(str, "<float>(");
    string_t_cat(str, buffer);
    string_t_cat(str, ")");
  }; break;
  case OBJECT_BOOL: {
    string_t_cat(str, "<boolean>(");
    if (object->bool_value) {
      string_t_cat(str, "true");
    } else {
      string_t_cat(str, "false");
    }
    string_t_cat(str, ")");
  }; break;
  case OBJECT_STRING: {
    string_t_cat(str, "<string>(");
    string_t_ncat(str, object->string_value.data, object->string_value.length);
    string_t_cat(str, ")");
  }; break;
  case OBJECT_SENTINEL: {
    string_t_cat(str, "<sentinel value>(");
    string_t_cat(str, "null");
    string_t_cat(str, ")");
  }; break;
  case OBJECT_CHAR: {
    string_t_cat(str, "<char>(");
    string_t_cat_char(str, object->rune_value);
    string_t_cat(str, ")");
  }; break;
  case OBJECT_RETURN: {
    string_t_cat(str, "<return value>(");
    t_object_repr(object->return_value.value, str);
    string_t_cat(str, ")");
  }; break;
  case OBJECT_FUNCTION: {
    string_t_cat(str, "<function>(");
    string_t_cat(str, "<parameters>(");
    for (size_t i = 0; i < object->function_value.param_count; i++) {
      string_t_cat(str, (char *)object->function_value.parameters[i]->id);
      string_t_cat(str, ",");
    }
    string_t_cat(str, ")");
    string_t_cat(str, "{");
    t_block_stmt_repr(object->function_value.blk_stmts, str);
    string_t_cat(str, "}");
    string_t_cat(str, ")");
  }; break;
  case OBJECT_ERROR: {
  }; break;
  }
}
