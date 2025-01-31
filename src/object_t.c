#include "object_t.h"
#include "ast.h"
#include "environment.h"
#include "error_t.h"
#include "util_error.h"
#include <stdio.h>
#include <stdlib.h>

struct obj_t *object_t_init(enum OBJECT_TYPE type) {
  struct obj_t *v = malloc(sizeof(struct obj_t));
  if (!v) {
    ERROR_LOG("error while allocation memory\n");
    return NULL;
  }
  v->type = type;
  v->gc_next = NULL;
  v->marked = false;

  switch (type) {
  case OBJECT_INT:
    v->int_value = 0;
    break;
  case OBJECT_DOUBLE:
    v->double_value = 0.0;
    break;
  case OBJECT_STRING:
    v->string_value.data = NULL;
    v->string_value.length = 0;
    v->string_value.capacity = 0;
    break;
  case OBJECT_BOOL:
    v->bool_value = false;
    break;
  case OBJECT_CHAR:
    v->rune_value = '\0';
  case OBJECT_SENTINEL:
    break;
  case OBJECT_RETURN:
    v->return_value.value = NULL;
    break;
  case OBJECT_ERROR:
    v->err_value = init_error_t();
    break;
  case OBJECT_FUNCTION: {
    v->function_value.blk_stmts = NULL;
    v->function_value.env = NULL;
    v->function_value.param_capacity = 0;
    v->function_value.param_count = 0;
    v->function_value.parameters = 0;
  }; break;
  default: {
    free(v);
    v = NULL;
  }; break;
  }
  return v;
}

void object_t_free(struct obj_t *v) {
  if (v && v->type != OBJECT_BOOL && v->type != OBJECT_SENTINEL) {
    switch (v->type) {
    case OBJECT_STRING:
      if (v->string_value.data)
        free(v->string_value.data);
      break;
    case OBJECT_ERROR: {
      free_error_t(v->err_value);
    }; break;
    case OBJECT_FUNCTION: {
      if (v->function_value.parameters) {
        for (size_t i = 0; i < v->function_value.param_count; i++) {
          ast_identifier_free(v->function_value.parameters[i]);
        }
        free(v->function_value.parameters);
      }
      if (v->function_value.blk_stmts) {
        ast_block_statement_free(v->function_value.blk_stmts);
      }
      if (v->function_value.env) {
        env_free(v->function_value.env);
      }
    }; break;
    default:
      break;
    }
    free(v);
  }
  v = NULL;
}
