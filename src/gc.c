#include "gc.h"
#include "environment.h"
#include "object_t.h"
#include <stdio.h>

static void gc_mark(struct obj_t *obj);
static void gc_sweep();

// starting point of all objects tracked by gc
static struct obj_t *gc_object_list = NULL;

struct obj_t *gc_alloc(enum OBJECT_TYPE type) {
  struct obj_t *obj = object_t_init(type);
  if (!obj) {
    return NULL;
  }
  obj->gc_next = gc_object_list;
  gc_object_list = obj;
  return obj;
}

static void gc_mark(struct obj_t *obj) {
  if (!obj || obj->marked) {
    return;
  }
  obj->marked = true;

  // recursively mark child objects
  // for lists, closures, return objects, etc.
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
    if (!(*curr)->marked) {
      // unmarked objects are considered as unused -> free any unused objects
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
