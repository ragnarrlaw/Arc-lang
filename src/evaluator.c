#include "evaluator.h"
#include "ast.h"
#include "object_t.h"
#include "token.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// clang-format off



static const struct obj_t OBJ_SENTINEL = {.type = OBJECT_SENTINEL};
static const struct obj_t OBJ_TRUE = {.type = OBJECT_BOOL, .bool_value = true};
static const struct obj_t OBJ_FALSE = {.type = OBJECT_BOOL, .bool_value = false};

struct obj_t *evaluate_literal_expr(struct expression *);
struct obj_t *evaluate_prefix_expr(enum TOKEN_TYPE, struct obj_t *);
struct obj_t *evaluate_infix_expr(enum TOKEN_TYPE, struct obj_t *, struct obj_t *);
struct obj_t *evaluate_postfix_expr(enum TOKEN_TYPE, struct obj_t *);

struct obj_t *evaluate_prefix_bang_operator_expr(struct obj_t *);
struct obj_t *evaluate_prefix_minus_operator_expr(struct obj_t *);
struct obj_t *evaluate_prefix_plus_operator_expr(struct obj_t *);
struct obj_t *evaluate_prefix_increment_operator_expr(struct obj_t *);
struct obj_t *evaluate_prefix_decrement_operator_expr(struct obj_t *);

struct obj_t *evaluate_infix_integer_expr(enum TOKEN_TYPE, struct obj_t *, struct obj_t *);
struct obj_t *evaluate_infix_boolean_expr(enum TOKEN_TYPE, struct obj_t *, struct obj_t *);
struct obj_t *evaluate_infix_float_expr(enum TOKEN_TYPE, struct obj_t *, struct obj_t *);
struct obj_t *evaluate_infix_char_expr(enum TOKEN_TYPE, struct obj_t *, struct obj_t *);
struct obj_t *evaluate_infix_string_expr(enum TOKEN_TYPE, struct obj_t *, struct obj_t *);

struct obj_t *evaluate_postfix_integer_expr(enum TOKEN_TYPE, struct obj_t *);
struct obj_t *evaluate_postfix_float_expr(enum TOKEN_TYPE, struct obj_t *);

// clang-format on

struct obj_t *evaluate_program(struct program *program) {
  if (!program) {
    return (struct obj_t *)&OBJ_SENTINEL;
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
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_expression(struct expression *expr) {
  if (!expr) {
    return (struct obj_t *)&OBJ_SENTINEL;
  }
  switch (expr->type) {
  case EXPR_LITERAL: {
    return evaluate_literal_expr(expr);
  };
  case EXPR_PREFIX: {
    struct obj_t *right = evaluate_expression(expr->prefix_expr.right);
    if (!right) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    return evaluate_prefix_expr(expr->prefix_expr.op->type, right);
  };
  case EXPR_INFIX: {
    struct obj_t *left = evaluate_expression(expr->infix_expr.left);
    struct obj_t *right = evaluate_expression(expr->infix_expr.right);
    if (!left || !right) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    return evaluate_infix_expr(expr->infix_expr.op->type, left, right);
  };
  default:
    return (struct obj_t *)&OBJ_SENTINEL;
  }
}

struct obj_t *evaluate_fn_def_stmt(struct expression *expr) {
  // TODO: complete function
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_literal_expr(struct expression *expr) {
  if (!expr) {
    return (struct obj_t *)&OBJ_SENTINEL;
  }
  switch (expr->literal.literal_type) {
  case LITERAL_BOOL: {
    return expr->literal.value.bool_value ? (struct obj_t *)&OBJ_TRUE
                                          : (struct obj_t *)&OBJ_FALSE;
  };
  case LITERAL_INT: {
    struct obj_t *obj = object_t_init(OBJECT_INT);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->int_value = expr->literal.value.int_value;
    return obj;
  };
  case LITERAL_FLOAT: {
    struct obj_t *obj = object_t_init(OBJECT_DOUBLE);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->double_value = expr->literal.value.float_value;
    return obj;
  };
  case LITERAL_STRING: {
    struct obj_t *obj = object_t_init(OBJECT_STRING);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
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
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->rune_value = expr->literal.value.char_value;
    return obj;
  };
  default:
    return (struct obj_t *)&OBJ_SENTINEL;
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
    return (struct obj_t *)&OBJ_SENTINEL;
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
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->int_value = -right->int_value;
    return obj;
  } else if (right->type == OBJECT_DOUBLE) {
    struct obj_t *obj = object_t_init(OBJECT_DOUBLE);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->double_value = -right->double_value;
    return obj;
  } else {
    return (struct obj_t *)&OBJ_SENTINEL;
  }
}

struct obj_t *evaluate_prefix_plus_operator_expr(struct obj_t *right) {
  if (right->type == OBJECT_INT) {
    struct obj_t *obj = object_t_init(OBJECT_INT);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->int_value = +right->int_value;
    return obj;
  } else if (right->type == OBJECT_DOUBLE) {
    struct obj_t *obj = object_t_init(OBJECT_DOUBLE);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->double_value = +right->double_value;
    return obj;
  } else {
    return (struct obj_t *)&OBJ_SENTINEL;
  }
}

struct obj_t *evaluate_prefix_increment_operator_expr(struct obj_t *right) {
  if (right->type == OBJECT_INT) {
    struct obj_t *obj = object_t_init(OBJECT_INT);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->int_value = right->int_value + 1;
    return obj;
  } else if (right->type == OBJECT_DOUBLE) {
    struct obj_t *obj = object_t_init(OBJECT_DOUBLE);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->double_value = right->double_value + 1;
    return obj;
  } else {
    return (struct obj_t *)&OBJ_SENTINEL;
  }
}

struct obj_t *evaluate_prefix_decrement_operator_expr(struct obj_t *right) {
  if (right->type == OBJECT_INT) {
    struct obj_t *obj = object_t_init(OBJECT_INT);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->int_value = right->int_value - 1;
    return obj;
  } else if (right->type == OBJECT_DOUBLE) {
    struct obj_t *obj = object_t_init(OBJECT_DOUBLE);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->double_value = right->double_value - 1;
    return obj;
  } else {
    return (struct obj_t *)&OBJ_SENTINEL;
  }
}

struct obj_t *evaluate_infix_expr(enum TOKEN_TYPE operator, struct obj_t * left,
                                  struct obj_t *right) {
  if (left && right) {
    if (left->type == OBJECT_INT && right->type == OBJECT_INT) {
      return evaluate_infix_integer_expr(operator, left, right);
    } else if (left->type == OBJECT_DOUBLE && right->type == OBJECT_DOUBLE) {
      return evaluate_infix_float_expr(operator, left, right);
    } else if (left->type == OBJECT_CHAR && right->type == OBJECT_CHAR) {
      return evaluate_infix_char_expr(operator, left, right);
    } else if (left->type == OBJECT_BOOL && right->type == OBJECT_BOOL) {
      return evaluate_infix_boolean_expr(operator, left, right);
    } else if (left->type == OBJECT_STRING && right->type == OBJECT_STRING) {
      return evaluate_infix_string_expr(operator, left, right);
    }
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_infix_integer_expr(enum TOKEN_TYPE operator,
                                          struct obj_t * left,
                                          struct obj_t *right) {
  if (left && right) {
    switch (operator) {
    case PLUS: {
      struct obj_t *obj = object_t_init(OBJECT_INT);
      if (obj) {
        obj->int_value = left->int_value + right->int_value;
        return obj;
      }
    }; break;
    case MINUS: {
      struct obj_t *obj = object_t_init(OBJECT_INT);
      if (obj) {
        obj->int_value = left->int_value - right->int_value;
        return obj;
      }
    }; break;
    case ASTERISK: {
      struct obj_t *obj = object_t_init(OBJECT_INT);
      if (obj) {
        obj->int_value = left->int_value * right->int_value;
        return obj;
      }
    }; break;
    case SLASH: {
      struct obj_t *obj = object_t_init(OBJECT_INT);
      if (obj) {
        if (left->int_value != 0) {
          obj->int_value = left->int_value / right->int_value;
          return obj;
        } else {
          object_t_free(obj);
          return (struct obj_t *)&OBJ_SENTINEL;
        }
      }
    }; break;
    case MOD: {
      struct obj_t *obj = object_t_init(OBJECT_INT);
      if (obj) {
        obj->int_value = left->int_value % right->int_value;
        return obj;
      }
    }; break;
    case GT: {
      struct obj_t *obj = object_t_init(OBJECT_BOOL);
      if (obj) {
        obj->bool_value = left->int_value > right->int_value;
        return obj;
      }
    }; break;
    case GT_EQ: {
      struct obj_t *obj = object_t_init(OBJECT_BOOL);
      if (obj) {
        obj->bool_value = left->int_value >= right->int_value;
        return obj;
      }
    }; break;
    case LT: {
      struct obj_t *obj = object_t_init(OBJECT_BOOL);
      if (obj) {
        obj->bool_value = left->int_value < right->int_value;
        return obj;
      }
    }; break;
    case LT_EQ: {
      struct obj_t *obj = object_t_init(OBJECT_BOOL);
      if (obj) {
        obj->bool_value = left->int_value <= right->int_value;
        return obj;
      }
    }; break;
    case EQ_EQ: {
      struct obj_t *obj = object_t_init(OBJECT_BOOL);
      if (obj) {
        obj->bool_value = left->int_value == right->int_value;
        return obj;
      }
    }; break;
    case NOT_EQ: {
      struct obj_t *obj = object_t_init(OBJECT_BOOL);
      if (obj) {
        obj->bool_value = left->double_value != right->double_value;
        return obj;
      }
    }; break;
    default:
      break;
    }
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_infix_boolean_expr(enum TOKEN_TYPE operator,
                                          struct obj_t * left,
                                          struct obj_t *right) {
  if (left && right) {
    switch (operator) {
    case EQ_EQ: {
      struct obj_t *obj = object_t_init(OBJECT_BOOL);
      if (obj) {
        obj->bool_value = left->bool_value == right->bool_value;
        return obj;
      }
    }; break;
    case NOT_EQ: {
      struct obj_t *obj = object_t_init(OBJECT_BOOL);
      if (obj) {
        obj->bool_value = left->bool_value != right->bool_value;
        return obj;
      }
    }; break;
    default:
      break;
    }
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_infix_float_expr(enum TOKEN_TYPE operator,
                                        struct obj_t * left,
                                        struct obj_t *right) {
  if (left && right) {
    switch (operator) {
    case PLUS: {
      struct obj_t *obj = object_t_init(OBJECT_DOUBLE);
      if (obj) {
        obj->double_value = left->double_value + right->double_value;
        return obj;
      }
    }; break;
    case MINUS: {
      struct obj_t *obj = object_t_init(OBJECT_DOUBLE);
      if (obj) {
        obj->double_value = left->double_value - right->double_value;
        return obj;
      }
    }; break;
    case ASTERISK: {
      struct obj_t *obj = object_t_init(OBJECT_DOUBLE);
      if (obj) {
        obj->double_value = left->double_value * right->double_value;
        return obj;
      }
    }; break;
    case SLASH: {
      struct obj_t *obj = object_t_init(OBJECT_DOUBLE);
      if (obj) {
        if (left->double_value != 0) {
          obj->double_value = left->double_value / right->double_value;
          return obj;
        } else {
          object_t_free(obj);
        }
      }
    }; break;
    case GT: {
      struct obj_t *obj = object_t_init(OBJECT_BOOL);
      if (obj) {
        obj->bool_value = left->double_value > right->double_value;
        return obj;
      }
    }; break;
    case GT_EQ: {
      struct obj_t *obj = object_t_init(OBJECT_BOOL);
      if (obj) {
        obj->bool_value = left->double_value >= right->double_value;
        return obj;
      }
    }; break;
    case LT: {
      struct obj_t *obj = object_t_init(OBJECT_BOOL);
      if (obj) {
        obj->bool_value = left->double_value < right->double_value;
        return obj;
      }
    }; break;
    case LT_EQ: {
      struct obj_t *obj = object_t_init(OBJECT_BOOL);
      if (obj) {
        obj->bool_value = left->double_value <= right->double_value;
        return obj;
      }
    }; break;
    case EQ_EQ: {
      struct obj_t *obj = object_t_init(OBJECT_BOOL);
      if (obj) {
        obj->bool_value = left->double_value == right->double_value;
        return obj;
      }
    }; break;
    case NOT_EQ: {
      struct obj_t *obj = object_t_init(OBJECT_BOOL);
      if (obj) {
        obj->bool_value = left->double_value != right->double_value;
        return obj;
      }
    }; break;
    default:
      break;
    }
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}
struct obj_t *evaluate_infix_char_expr(enum TOKEN_TYPE operator,
                                       struct obj_t * left,
                                       struct obj_t *right) {
  if (left && right) {
    switch (operator) {
    case EQ_EQ: {
      struct obj_t *obj = object_t_init(OBJECT_BOOL);
      if (obj) {
        obj->bool_value = left->rune_value == right->rune_value;
        return obj;
      }
    }; break;
    case NOT_EQ: {
      struct obj_t *obj = object_t_init(OBJECT_BOOL);
      if (obj) {
        obj->bool_value = left->rune_value != right->rune_value;
        return obj;
      }
    }; break;
    default:
      break;
    }
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}
struct obj_t *evaluate_infix_string_expr(enum TOKEN_TYPE operator,
                                         struct obj_t * left,
                                         struct obj_t *right) {
  if (left && right) {
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_postfix_expr(enum TOKEN_TYPE operator,
                                    struct obj_t * left) {
  return (struct obj_t *)&OBJ_SENTINEL;
}
