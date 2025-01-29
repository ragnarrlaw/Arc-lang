#ifndef REPR_H
#define REPR_H

#include "ast.h"
#include "object_t.h"
#include "string_t.h"
#include "token.h"
#include "util_error.h"
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>

#define PRINT_FIELD_INT(structure, field)                                      \
  printf(#field ": %d\n", structure->field)
#define PRINT_FIELD_STR(structure, field)                                      \
  printf(#field ": %s\n", structure->field)
#define PRINT_FIELD_CHAR_ARRAY(structure, field, len)                          \
  do {                                                                         \
    printf(#field ": ");                                                       \
    for (size_t i = 0; i < len; i++) {                                         \
      printf("%c", structure->field[i]);                                       \
    }                                                                          \
    printf("\n");                                                              \
  } while (0)

#define PRINT_CHAR_ARRAY(arr, len)                                             \
  do {                                                                         \
    for (size_t i = 0; i < len; i++) {                                         \
      printf("%c", arr[i]);                                                    \
    }                                                                          \
    printf("\n");                                                              \
  } while (0)

/**
 * print a representation of a statement
 */
void t_stmt_repr(struct statement *, string_t *str);

/**
 * print a representation of a block statement
 */
void t_block_stmt_repr(struct block_statement *b_stmt, string_t *str);

/**
 * print a representation of an expression
 */
void t_expr_repr(struct expression *expr, string_t *str);

/**
 * print a representation of an object
 */
void t_object_repr(struct obj_t *object, string_t *str);

#endif // !REPR_H
