#include "evaluator.h"
#include "ast.h"
#include "environment.h"
#include "error_t.h"
#include "gc.h"
#include "object_t.h"
#include "token.h"
#include "util_error.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clang-format off

static const struct obj_t OBJ_SENTINEL = {.type = OBJECT_SENTINEL, .gc_next = NULL, .marked = false};
static const struct obj_t OBJ_TRUE = {.type = OBJECT_BOOL, .bool_value = true, .gc_next = NULL, .marked = false};
static const struct obj_t OBJ_FALSE = {.type = OBJECT_BOOL, .bool_value = false, .gc_next = NULL, .marked = false};

struct obj_t *evaluate_statements(struct environment *env, struct statement **, size_t);
struct obj_t *evaluate_statement(struct environment *env, struct statement *);
struct obj_t *evaluate_let_statement(struct environment *env, struct statement *);
struct obj_t *evaluate_return_statement(struct environment *env, struct statement *);

struct obj_t *evaluate_literal_expr(struct expression *);
struct obj_t *evaluate_prefix_expr(struct token *, struct obj_t *);
struct obj_t *evaluate_infix_expr(struct token *, struct obj_t *, struct obj_t *);
struct obj_t *evaluate_postfix_expr(struct token *, struct obj_t *);

struct obj_t *evaluate_identifier_expr(struct environment *env, char *, struct token *);

struct obj_t **evaluate_expressions(struct environment *env, struct expression **, size_t);

struct obj_t *evaluate_block_statements(struct environment *env, struct block_statement *);
struct obj_t *evaluate_if_expression(struct environment *env, struct expression *);
struct obj_t *evaluate_fn_expression(struct environment *env, struct expression *);
struct obj_t *evaluate_fn_call_expression(struct environment *env, struct expression *);

struct obj_t *evaluate_prefix_bang_operator_expr(struct token*, struct obj_t *);
struct obj_t *evaluate_prefix_minus_operator_expr(struct token*, struct obj_t *);
struct obj_t *evaluate_prefix_plus_operator_expr(struct token*, struct obj_t *);
struct obj_t *evaluate_prefix_increment_operator_expr(struct token*, struct obj_t *);
struct obj_t *evaluate_prefix_decrement_operator_expr(struct token*, struct obj_t *);

struct obj_t *evaluate_infix_integer_expr(struct token *, struct obj_t *, struct obj_t *);
struct obj_t *evaluate_infix_boolean_expr(struct token *, struct obj_t *, struct obj_t *);
struct obj_t *evaluate_infix_float_expr(struct token *, struct obj_t *, struct obj_t *);
struct obj_t *evaluate_infix_char_expr(struct token *, struct obj_t *, struct obj_t *);
struct obj_t *evaluate_infix_string_expr(struct token *, struct obj_t *, struct obj_t *);

struct obj_t *evaluate_postfix_integer_expr(struct token *, struct obj_t *);
struct obj_t *evaluate_postfix_float_expr(struct token *, struct obj_t *);

bool is_truthy(struct obj_t*);
bool has_error(struct obj_t *);

// clang-format on

struct obj_t *evaluate_program(struct environment *env,
                               struct program *program) {
  if (!program) {
    return (struct obj_t *)&OBJ_SENTINEL;
  }
  return evaluate_statements(env, program->statements,
                             program->statement_count);
}

struct obj_t *evaluate_statement(struct environment *env,
                                 struct statement *stmt) {
  if (stmt) {
    switch (stmt->type) {
    case STMT_LET: {
      return evaluate_let_statement(env, stmt);
    }; break;
    case STMT_RETURN: {
      return evaluate_return_statement(env, stmt);
    }; break;
    case STMT_EXPRESSION: {
      return evaluate_expression(env, stmt->expr_stmt.expr);
    }; break;
    case STMT_FUNCTION_DEF: {
    }; break;
    }
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_expression(struct environment *env,
                                  struct expression *expr) {
  if (!expr) {
    return (struct obj_t *)&OBJ_SENTINEL;
  }
  switch (expr->type) {
  case EXPR_LITERAL: {
    return evaluate_literal_expr(expr);
  };
  case EXPR_IDENTIFIER: {
    return evaluate_identifier_expr(env, expr->identifier_expr.identifier,
                                    expr->identifier_expr.token);
  }; break;
  case EXPR_PREFIX: {
    struct obj_t *right = evaluate_expression(env, expr->prefix_expr.right);
    if (has_error(right)) {
      return right;
    }
    return evaluate_prefix_expr(expr->prefix_expr.op, right);
  };
  case EXPR_INFIX: {
    struct obj_t *left = evaluate_expression(env, expr->infix_expr.left);
    if (has_error(left)) {
      return left;
    }
    struct obj_t *right = evaluate_expression(env, expr->infix_expr.right);
    if (has_error(right)) {
      return right;
    }
    return evaluate_infix_expr(expr->infix_expr.op, left, right);
  };
  case EXPR_CONDITIONAL: {
    return evaluate_if_expression(env, expr);
  };
  case EXPR_FUNCTION: {
    return evaluate_fn_expression(env, expr);
  }; break;
  case EXPR_FUNCTION_CALL: {
    return evaluate_fn_call_expression(env, expr);
  }; break;
  default:
    return (struct obj_t *)&OBJ_SENTINEL;
  }
}

struct obj_t **evaluate_expressions(struct environment *env,
                                    struct expression **exprs,
                                    size_t expr_count) {
  struct obj_t **results =
      (struct obj_t **)malloc(sizeof(struct obj_t *) * expr_count);
  if (!results) {
    ERROR_LOG("error while allocating memory\n");
    return NULL;
  }
  for (size_t i = 0; i < expr_count; i++) {
    struct obj_t *result = evaluate_expression(env, exprs[i]);
    if (has_error(result)) {
      results[0] = result;
      return results;
    }
    results[i] = result;
  }
  return results;
}

struct obj_t *evaluate_statements(struct environment *env,
                                  struct statement **stmts, size_t stmt_count) {
  struct obj_t *result;
  for (size_t i = 0; i < stmt_count; i++) {
    result = evaluate_statement(env, stmts[i]);
    if (result && result->type == OBJECT_RETURN) {
      return result->return_value.value;
    } else if (result && result->type == OBJECT_ERROR) {
      return result;
    }
  }
  return result;
}

struct obj_t *evaluate_let_statement(struct environment *env,
                                     struct statement *stmt) {
  if (stmt) {
    struct obj_t *value = evaluate_expression(env, stmt->let_stmt.value);
    if (has_error(value)) {
      return value;
    }
    env_define(env, stmt->let_stmt.ident, value);
    return value;
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_block_statements(struct environment *env,
                                        struct block_statement *block) {

  if (block) {
    struct obj_t *result;
    for (size_t i = 0; i < block->statement_count; i++) {
      result = evaluate_statement(env, block->statements[i]);
      if (result && result->type == OBJECT_RETURN) {
        return result;
      } else if (result && result->type == OBJECT_ERROR) {
        return result;
      }
    }
    return result;
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_return_statement(struct environment *env,
                                        struct statement *stmt) {
  if (stmt) {
    struct obj_t *value = evaluate_expression(env, stmt->return_stmt.value);
    if (has_error(value)) {
      return value;
    }
    struct obj_t *object = gc_alloc(OBJECT_RETURN);
    if (object) {
      object->return_value.value = value;
      return object;
    }
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_if_expression(struct environment *env,
                                     struct expression *expr) {
  if (expr) {
    struct obj_t *condition =
        evaluate_expression(env, expr->conditional.condition);
    if (has_error(condition)) {
      return condition;
    }
    if (is_truthy(condition)) {
      return evaluate_block_statements(env, expr->conditional.consequence);
    } else if (expr->conditional.alternative) {
      return evaluate_block_statements(env, expr->conditional.alternative);
    } else {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_fn_expression(struct environment *env,
                                     struct expression *expr) {
  if (expr) {
    struct obj_t *obj = gc_alloc(OBJECT_FUNCTION);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->function_value.env = env;
    obj->function_value.param_capacity = expr->function.param_capacity;
    obj->function_value.param_count = expr->function.param_count;
    obj->function_value.parameters = expr->function.parameters;
    obj->function_value.blk_stmts = expr->function.body;
    return obj;
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_fn_call_expression(struct environment *env,
                                          struct expression *expr) {
  if (expr) {
    struct obj_t *function =
        evaluate_expression(env, expr->function_call.function);
    if (has_error(function)) {
      return function;
    }
    if (function->type != OBJECT_FUNCTION) {
      struct obj_t *err = gc_alloc(OBJECT_ERROR);
      error_t_format_err(err->err_value, expr->function_call.token,
                         "invalid function call",
                         "only functions can be called");
      return err;
    }

    // evaluate the arguments
    struct obj_t **args = evaluate_expressions(
        env, expr->function_call.arguments, expr->function_call.arg_count);
    if (!args) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    for (size_t i = 0; i < expr->function_call.arg_count; i++) {
      if (has_error(args[i])) {
        free(args);
        return args[i];
      } else {
      }
    }

    struct environment *child = env_init();
    if (!child) {
      free(args);
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    child->parent = function->function_value.env;

    for (size_t i = 0; i < function->function_value.param_count; i++) {
      env_define(child, function->function_value.parameters[i]->id, args[i]);
    }

    struct obj_t *result =
        evaluate_block_statements(child, function->function_value.blk_stmts);
    if (result && result->type == OBJECT_RETURN) {
      return result->return_value.value;
    }

    free(args);
    return result;
  }
  return (struct obj_t *)&OBJ_SENTINEL;
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
    struct obj_t *obj = gc_alloc(OBJECT_INT);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->int_value = expr->literal.value.int_value;
    return obj;
  };
  case LITERAL_FLOAT: {
    struct obj_t *obj = gc_alloc(OBJECT_DOUBLE);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->double_value = expr->literal.value.float_value;
    return obj;
  };
  case LITERAL_STRING: {
    struct obj_t *obj = gc_alloc(OBJECT_STRING);
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
    struct obj_t *obj = gc_alloc(OBJECT_CHAR);
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

struct obj_t *evaluate_prefix_expr(struct token *operator,
                                   struct obj_t * right) {
  switch (operator->type) {
  case BANG: {
    return evaluate_prefix_bang_operator_expr(operator, right);
  }
  case MINUS: {
    return evaluate_prefix_minus_operator_expr(operator, right);
  }
  case INC: {
    return evaluate_prefix_increment_operator_expr(operator, right);
  }
  case DEC: {
    return evaluate_prefix_decrement_operator_expr(operator, right);
  }
  default: {
    struct obj_t *err = gc_alloc(OBJECT_ERROR);
    error_t_format_err(
        err->err_value, operator, "prefix operator not found",
        "!, -, ++, and -- are the only prefix operators permitted");
    return err;
  };
  }
}

struct obj_t *evaluate_prefix_bang_operator_expr(struct token *operator,
                                                 struct obj_t * right) {
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

struct obj_t *evaluate_prefix_minus_operator_expr(struct token *operator,
                                                  struct obj_t * right) {
  if (right->type == OBJECT_INT) {
    struct obj_t *obj = gc_alloc(OBJECT_INT);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->int_value = -right->int_value;
    return obj;
  } else if (right->type == OBJECT_DOUBLE) {
    struct obj_t *obj = gc_alloc(OBJECT_DOUBLE);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->double_value = -right->double_value;
    return obj;
  } else {
    struct obj_t *err = gc_alloc(OBJECT_ERROR);
    error_t_format_err(
        err->err_value, operator, "invalid use of prefix operator",
        "only numerical types can be used with prefix operator -");
    return err;
  }
}

struct obj_t *evaluate_prefix_plus_operator_expr(struct token *token,
                                                 struct obj_t *right) {
  if (right->type == OBJECT_INT) {
    struct obj_t *obj = gc_alloc(OBJECT_INT);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->int_value = +right->int_value;
    return obj;
  } else if (right->type == OBJECT_DOUBLE) {
    struct obj_t *obj = gc_alloc(OBJECT_DOUBLE);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->double_value = +right->double_value;
    return obj;
  } else {
    struct obj_t *err = gc_alloc(OBJECT_ERROR);
    error_t_format_err(
        err->err_value, token, "invalid use of prefix operator",
        "only numerical types can be used with prefix operator +");
    return err;
  }
}

struct obj_t *evaluate_prefix_increment_operator_expr(struct token *token,
                                                      struct obj_t *right) {
  if (right->type == OBJECT_INT) {
    struct obj_t *obj = gc_alloc(OBJECT_INT);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->int_value = right->int_value + 1;
    return obj;
  } else if (right->type == OBJECT_DOUBLE) {
    struct obj_t *obj = gc_alloc(OBJECT_DOUBLE);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->double_value = right->double_value + 1;
    return obj;
  } else {
    struct obj_t *err = gc_alloc(OBJECT_ERROR);
    error_t_format_err(
        err->err_value, token, "invalid use of prefix operator",
        "only numerical types can be used with prefix operator ++");
    return err;
  }
}

struct obj_t *evaluate_prefix_decrement_operator_expr(struct token *operator,
                                                      struct obj_t * right) {
  if (right->type == OBJECT_INT) {
    struct obj_t *obj = gc_alloc(OBJECT_INT);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->int_value = right->int_value - 1;
    return obj;
  } else if (right->type == OBJECT_DOUBLE) {
    struct obj_t *obj = gc_alloc(OBJECT_DOUBLE);
    if (!obj) {
      return (struct obj_t *)&OBJ_SENTINEL;
    }
    obj->double_value = right->double_value - 1;
    return obj;
  } else {
    struct obj_t *err = gc_alloc(OBJECT_ERROR);
    error_t_format_err(
        err->err_value, operator, "invalid use of prefix operator",
        "only numerical types can be used with prefix operator --");
    return err;
  }
}

struct obj_t *evaluate_infix_expr(struct token *operator, struct obj_t * left,
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
    } else {
      struct obj_t *err = gc_alloc(OBJECT_ERROR);
      error_t_format_err(err->err_value, operator, "type mismatch", "");
      return err;
    }
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_infix_integer_expr(struct token *operator,
                                          struct obj_t * left,
                                          struct obj_t *right) {
  if (left && right) {
    switch (operator->type) {
    case PLUS: {
      struct obj_t *obj = gc_alloc(OBJECT_INT);
      if (obj) {
        obj->int_value = left->int_value + right->int_value;
        return obj;
      }
    }; break;
    case MINUS: {
      struct obj_t *obj = gc_alloc(OBJECT_INT);
      if (obj) {
        obj->int_value = left->int_value - right->int_value;
        return obj;
      }
    }; break;
    case ASTERISK: {
      struct obj_t *obj = gc_alloc(OBJECT_INT);
      if (obj) {
        obj->int_value = left->int_value * right->int_value;
        return obj;
      }
    }; break;
    case SLASH: {
      struct obj_t *obj = gc_alloc(OBJECT_INT);
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
      struct obj_t *obj = gc_alloc(OBJECT_INT);
      if (obj) {
        obj->int_value = left->int_value % right->int_value;
        return obj;
      }
    }; break;
    case GT:
      return left->int_value > right->int_value ? (struct obj_t *)&OBJ_TRUE
                                                : (struct obj_t *)&OBJ_FALSE;
    case GT_EQ:
      return left->int_value >= right->int_value ? (struct obj_t *)&OBJ_TRUE
                                                 : (struct obj_t *)&OBJ_FALSE;
    case LT:
      return left->int_value < right->int_value ? (struct obj_t *)&OBJ_TRUE
                                                : (struct obj_t *)&OBJ_FALSE;
    case LT_EQ:
      return left->int_value <= right->int_value ? (struct obj_t *)&OBJ_TRUE
                                                 : (struct obj_t *)&OBJ_FALSE;
    case EQ_EQ:
      return left->int_value == right->int_value ? (struct obj_t *)&OBJ_TRUE
                                                 : (struct obj_t *)&OBJ_FALSE;
    case NOT_EQ:
      return left->double_value != right->double_value
                 ? (struct obj_t *)&OBJ_TRUE
                 : (struct obj_t *)&OBJ_FALSE;
    default: {
      struct obj_t *err = gc_alloc(OBJECT_ERROR);
      error_t_format_err(err->err_value, operator, "operator not found",
                         "allowed binary operators for integers -> +, -, /, *, "
                         "==, != , >, <, >=, <=, %");
      return err;
    }; break;
    }
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_infix_boolean_expr(struct token *operator,
                                          struct obj_t * left,
                                          struct obj_t *right) {
  if (left && right) {
    switch (operator->type) {
    case EQ_EQ:
      return left->bool_value == right->bool_value ? (struct obj_t *)&OBJ_TRUE
                                                   : (struct obj_t *)&OBJ_FALSE;
    case NOT_EQ:
      return left->bool_value != right->bool_value ? (struct obj_t *)&OBJ_TRUE
                                                   : (struct obj_t *)&OBJ_FALSE;
    default: {
      struct obj_t *err = gc_alloc(OBJECT_ERROR);
      error_t_format_err(err->err_value, operator, "operator not found",
                         "allowed binary operators for boolean -> ==, !=");
      return err;
    }; break;
    }
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_infix_float_expr(struct token *operator,
                                        struct obj_t * left,
                                        struct obj_t *right) {
  if (left && right) {
    switch (operator->type) {
    case PLUS: {
      struct obj_t *obj = gc_alloc(OBJECT_DOUBLE);
      if (obj) {
        obj->double_value = left->double_value + right->double_value;
        return obj;
      }
    }; break;
    case MINUS: {
      struct obj_t *obj = gc_alloc(OBJECT_DOUBLE);
      if (obj) {
        obj->double_value = left->double_value - right->double_value;
        return obj;
      }
    }; break;
    case ASTERISK: {
      struct obj_t *obj = gc_alloc(OBJECT_DOUBLE);
      if (obj) {
        obj->double_value = left->double_value * right->double_value;
        return obj;
      }
    }; break;
    case SLASH: {
      struct obj_t *obj = gc_alloc(OBJECT_DOUBLE);
      if (obj) {
        if (left->double_value != 0) {
          obj->double_value = left->double_value / right->double_value;
          return obj;
        } else {
          object_t_free(obj);
        }
      }
    }; break;
    case GT:
      return left->double_value > right->double_value
                 ? (struct obj_t *)&OBJ_TRUE
                 : (struct obj_t *)&OBJ_FALSE;
    case GT_EQ:
      return left->double_value >= right->double_value
                 ? (struct obj_t *)&OBJ_TRUE
                 : (struct obj_t *)&OBJ_FALSE;
    case LT:
      return left->double_value < right->double_value
                 ? (struct obj_t *)&OBJ_TRUE
                 : (struct obj_t *)&OBJ_FALSE;
    case LT_EQ:
      return left->double_value <= right->double_value
                 ? (struct obj_t *)&OBJ_TRUE
                 : (struct obj_t *)&OBJ_FALSE;
    case EQ_EQ:
      return left->double_value == right->double_value
                 ? (struct obj_t *)&OBJ_TRUE
                 : (struct obj_t *)&OBJ_FALSE;
    case NOT_EQ:
      return left->double_value != right->double_value
                 ? (struct obj_t *)&OBJ_TRUE
                 : (struct obj_t *)&OBJ_FALSE;
    default: {
      struct obj_t *err = gc_alloc(OBJECT_ERROR);
      error_t_format_err(err->err_value, operator, "operator not found",
                         "allowed binary operators for floats -> +, -, /, *, "
                         "==, != , >, <, >=, <=");
      return err;
    }; break;
    }
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_infix_char_expr(struct token *operator,
                                       struct obj_t * left,
                                       struct obj_t *right) {
  if (left && right) {
    switch (operator->type) {
    case EQ_EQ:
      return left->rune_value == right->rune_value ? (struct obj_t *)&OBJ_TRUE
                                                   : (struct obj_t *)&OBJ_FALSE;
    case NOT_EQ:
      return left->rune_value != right->rune_value ? (struct obj_t *)&OBJ_TRUE
                                                   : (struct obj_t *)&OBJ_FALSE;
    default: {
      struct obj_t *err = gc_alloc(OBJECT_ERROR);
      error_t_format_err(err->err_value, operator, "operator not found",
                         "allowed binary operators for char -> ==, !=");
      return err;
    }; break;
    }
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}
struct obj_t *evaluate_infix_string_expr(struct token *operator,
                                         struct obj_t * left,
                                         struct obj_t *right) {
  if (left && right) {
  }
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_postfix_expr(struct token *operator,
                                    struct obj_t * left) {
  return (struct obj_t *)&OBJ_SENTINEL;
}

struct obj_t *evaluate_identifier_expr(struct environment *env,
                                       char *identifier, struct token *token) {
  struct obj_t *value = env_look_up(env, identifier);
  if (!value) {
    struct obj_t *err = gc_alloc(OBJECT_ERROR);
    if (err) {
      error_t_format_err(err->err_value, token, "identifier not found", "");
    }
    return err;
  }
  return value;
}

bool is_truthy(struct obj_t *object) {
  if (object == (struct obj_t *)&OBJ_TRUE) {
    return true;
  } else if (object == (struct obj_t *)&OBJ_FALSE) {
    return false;
  } else if (object == (struct obj_t *)&OBJ_SENTINEL) {
    return false;
  } else {
    return true;
  }
}

bool has_error(struct obj_t *obj) { return obj && (obj->type == OBJECT_ERROR); }