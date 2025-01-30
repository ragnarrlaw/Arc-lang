#ifndef GC_H
#define GC_H

/**
 * mark and sweep garbage collector
 */

#include "environment.h"
#include "object_t.h"
#include <stddef.h>

typedef struct root_set_t {
  struct obj_t **roots;
  size_t count;
  size_t capacity;
} root_set_t;

struct obj_t *gc_alloc(enum OBJECT_TYPE type);

void gc_collect(struct environment *env);
// mark the roots - called before sweeping
void gc_mark_environment(struct environment *env);

#endif // !GC_H