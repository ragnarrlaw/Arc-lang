#ifndef OBJECT_T_H
#define OBJECT_T_H

#include <stdbool.h>
#include <stddef.h>

struct obj_t;

enum OBJECT_TYPE {
  OBJECT_INT,
  OBJECT_DOUBLE,
  OBJECT_STRING,
  OBJECT_BOOL,
  OBJECT_CHAR,
  OBJECT_SENTINEL, // basically a null value
  OBJECT_RETURN,
};

struct obj_t {
  enum OBJECT_TYPE type;
  union {
    int int_value;
    double double_value;
    bool bool_value;
    char rune_value;

    struct {
      char *data;
      size_t length;
      size_t capacity;
    } string_value;

    struct {
      struct obj_t *value;
    } return_value;

    struct {
    } sentinel; // basically a null value
  };
};

struct obj_t *object_t_init(enum OBJECT_TYPE t);
void object_t_free(struct obj_t *v);

#endif // !OBJECT_T_H
