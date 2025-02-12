#include "gc.h"
#include "environment.h"
#include "object_t.h"
#include <stdio.h>

// Static objects
static const struct obj_t OBJ_SENTINEL = {
    .type = OBJECT_SENTINEL, .gc_next = NULL, .marked = false};
static const struct obj_t OBJ_TRUE = {
    .type = OBJECT_BOOL, .bool_value = true, .gc_next = NULL, .marked = false};
static const struct obj_t OBJ_FALSE = {
    .type = OBJECT_BOOL, .bool_value = false, .gc_next = NULL, .marked = false};

// Starting point of all objects tracked by GC
static struct obj_t *gc_object_list = NULL;

struct obj_t *gc_alloc(enum OBJECT_TYPE type) {
  if (type == OBJECT_SENTINEL) {
    return (struct obj_t *)&OBJ_SENTINEL;
  } else if (type == OBJECT_BOOL_TRUE) {
    return (struct obj_t *)&OBJ_TRUE;
  } else if (type == OBJECT_BOOL_FALSE) {
    return (struct obj_t *)&OBJ_FALSE;
  } else {
    struct obj_t *obj = object_t_init(type);
    if (!obj) {
      return NULL;
    }
    obj->gc_next = gc_object_list;
    gc_object_list = obj;
    return obj;
  }
}

static void gc_mark(struct obj_t *obj) {
  if (!obj || obj->marked || obj->type == OBJECT_SENTINEL ||
      obj->type == OBJECT_BOOL) {
    return;
  }
  obj->marked = true;

  // recursively mark child objects
  switch (obj->type) {
    /*
            case OBJECT_LIST:
            for (int i = 0; i < obj->list_length; i++) {
                gc_mark(obj->list_elements[i]);
            }
            break;
        case OBJECT_CLOSURE:
            gc_mark_environment(obj->closure.env);
            break;
     */
  case OBJECT_FUNCTION: {
    gc_mark_environment(obj->function_value.env);
  }; break;
  case OBJECT_RETURN: {
    gc_mark(obj->return_value.value);
  }; break;
  default: {
  }; break;
  }
}

void gc_mark_environment(struct environment *env) {
  if (env) {
    struct environment *current_env = env;
    while (current_env) {
      hash_table_iterator it = hash_table_iterate(env->symbols);
      const char *key;
      struct obj_t *value;
      while (hash_table_next(&it, &key, (void **)&value)) {
        gc_mark(value);
      }
      current_env = current_env->parent;
    }
  }
}

static void gc_sweep() {
  struct obj_t **curr = &gc_object_list;
  while (*curr) {
    if (!(*curr)->marked && (*curr)->type != OBJECT_SENTINEL &&
        (*curr)->type != OBJECT_BOOL) {
      // unmarked objects are considered unused -> free them
      struct obj_t *unreached = *curr;
      *curr = unreached->gc_next;
      object_t_free(unreached);
    } else {
      // reset mark for next cycle -> if not marked in the next then cleaned up
      (*curr)->marked = false;
      curr = &(*curr)->gc_next;
    }
  }
}

/**
 * run a full gc cycle
 */
void gc_collect(struct environment *env) {
  gc_mark_environment(env); // first perform marking
  gc_sweep();               // then sweep unused objects
}
