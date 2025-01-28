#include "evaluator.h"
#include "ast.h"
#include "object_t.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct obj_t *evaluate_literal_expr(struct expression *expr);

struct obj_t *evaluate_program(struct program *program) {
  if (!program) {
    return NULL;
  }
  for (size_t i = 0; i < program->statement_count; i++) {
    switch (program->statements[i]->type) {
    case STMT_LET: {
    }; break;
    case STMT_RETURN: {
    }; break;
    case STMT_EXPRESSION: {
      return evaluate_expression(program->statements[i]->expr_stmt.expr);
    }; break;
    case STMT_FUNCTION_DEF: {
    }; break;
    }
  }
  return NULL;
}

struct obj_t *evaluate_expression(struct expression *expr) {
  if (!expr) {
    return NULL;
  }
  switch (expr->type) {
  case EXPR_LITERAL: {
    return evaluate_literal_expr(expr);
  };
  default:
    return NULL;
  }
}

struct obj_t *evaluate_fn_def_stmt(struct expression *expr) {
  // TODO: complete function
  return NULL;
}

struct obj_t *evaluate_literal_expr(struct expression *expr) {
  if (!expr) {
    return NULL;
  }
  switch (expr->literal.literal_type) {
  case LITERAL_BOOL: {
    struct obj_t *obj = object_t_init(OBJECT_BOOL);
    if (!obj) {
      return NULL;
    }
    obj->bool_value = expr->literal.value.bool_value;
    return obj;
  };
  case LITERAL_INT: {
    struct obj_t *obj = object_t_init(OBJECT_INT);
    if (!obj) {
      return NULL;
    }
    obj->int_value = expr->literal.value.int_value;
    return obj;
  };
  case LITERAL_FLOAT: {
    struct obj_t *obj = object_t_init(OBJECT_DOUBLE);
    if (!obj) {
      return NULL;
    }
    obj->double_value = expr->literal.value.float_value;
    return obj;
  };
  case LITERAL_STRING: {
    struct obj_t *obj = object_t_init(OBJECT_STRING);
    if (!obj) {
      return NULL;
    }
    obj->string_value.data =
        strndup(expr->literal.value.string_literal->value,
                expr->literal.value.string_literal->length);
    obj->string_value.length = expr->literal.value.string_literal->length;
    return obj;
  };
  case LITERAL_CHAR: {
    struct obj_t *obj = object_t_init(OBJECT_CHAR);
    if (!obj) {
      return NULL;
    }
    obj->rune_value = expr->literal.value.char_value;
    return obj;
  };
  default:
    return NULL;
  }
}