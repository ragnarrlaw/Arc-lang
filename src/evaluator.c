#include "evaluator.h"
#include "ast.h"
#include "object_t.h"
#include "token.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clang-format off
static const struct obj_t OBJ_SENTINEL = {.type = OBJECT_SENTINEL};
static const struct obj_t OBJ_TRUE = {.type = OBJECT_BOOL, .bool_value = true};
static const struct obj_t OBJ_FALSE = {.type = OBJECT_BOOL, .bool_value = false};
// clang-format on

struct obj_t *evaluate_literal_expr(struct expression *);
struct obj_t *evaluate_prefix_expr(enum TOKEN_TYPE, struct obj_t *);

// clang-format off
struct obj_t *evaluate_prefix_bang_operator_expr(struct obj_t *);
struct obj_t *evaluate_prefix_minus_operator_expr(struct obj_t *);
struct obj_t *evaluate_prefix_plus_operator_expr(struct obj_t *);
struct obj_t *evaluate_prefix_increment_operator_expr(struct obj_t *);
struct obj_t *evaluate_prefix_decrement_operator_expr(struct obj_t *);
// clang-format on

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
  case EXPR_PREFIX: {
    struct obj_t *right = evaluate_expression(expr->prefix_expr.right);
    if (!right) {
      return NULL;
    }
    return evaluate_prefix_expr(expr->prefix_expr.op->type, right);
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
    return expr->literal.value.bool_value ? (struct obj_t *)&OBJ_TRUE
                                          : (struct obj_t *)&OBJ_FALSE;
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

struct obj_t *evaluate_prefix_expr(enum TOKEN_TYPE operator,
                                   struct obj_t * right) {
  switch (operator) {
  case BANG: {
    return evaluate_prefix_bang_operator_expr(right);
  }; break;
  case MINUS: {
    return evaluate_prefix_minus_operator_expr(right);
  }; break;
  case INC: {
    return evaluate_prefix_increment_operator_expr(right);
  }; break;
  case DEC: {
    return evaluate_prefix_decrement_operator_expr(right);
  }; break;
  default:
    return NULL;
  }
}

struct obj_t *evaluate_prefix_bang_operator_expr(struct obj_t *right) {
  if (right == &OBJ_SENTINEL) {
    return (struct obj_t *)&OBJ_TRUE;
  } else if (right == &OBJ_TRUE) {
    return (struct obj_t *)&OBJ_FALSE;
  } else if (right == &OBJ_FALSE) {
    return (struct obj_t *)&OBJ_TRUE;
  } else {
    return (struct obj_t *)&OBJ_FALSE;
  }
}

struct obj_t *evaluate_prefix_minus_operator_expr(struct obj_t *right) {
  if (right->type == OBJECT_INT) {
    struct obj_t *obj = object_t_init(OBJECT_INT);
    if (!obj) {
      return NULL;
    }
    obj->int_value = -right->int_value;
    return obj;
  } else if (right->type == OBJECT_DOUBLE) {
    struct obj_t *obj = object_t_init(OBJECT_DOUBLE);
    if (!obj) {
      return NULL;
    }
    obj->double_value = -right->double_value;
    return obj;
  } else {
    return NULL;
  }
}

struct obj_t *evaluate_prefix_plus_operator_expr(struct obj_t *right) {
  if (right->type == OBJECT_INT) {
    struct obj_t *obj = object_t_init(OBJECT_INT);
    if (!obj) {
      return NULL;
    }
    obj->int_value = +right->int_value;
    return obj;
  } else if (right->type == OBJECT_DOUBLE) {
    struct obj_t *obj = object_t_init(OBJECT_DOUBLE);
    if (!obj) {
      return NULL;
    }
    obj->double_value = +right->double_value;
    return obj;
  } else {
    return NULL;
  }
}

struct obj_t *evaluate_prefix_increment_operator_expr(struct obj_t *right) {
  if (right->type == OBJECT_INT) {
    struct obj_t *obj = object_t_init(OBJECT_INT);
    if (!obj) {
      return NULL;
    }
    obj->int_value = right->int_value + 1;
    return obj;
  } else {
    return NULL;
  }
}

struct obj_t *evaluate_prefix_decrement_operator_expr(struct obj_t *right) {
  if (right->type == OBJECT_INT) {
    struct obj_t *obj = object_t_init(OBJECT_INT);
    if (!obj) {
      return NULL;
    }
    obj->int_value = right->int_value - 1;
    return obj;
  } else {
    return NULL;
  }
}