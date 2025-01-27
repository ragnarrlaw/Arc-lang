#ifndef REPR_H
#define REPR_H

#include "ast.h"
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
 * string_t offers better support for string operations
 */
typedef struct string_t {
  char *str;
  size_t len;
  size_t cap;
} string_t;

/**
 * initialize a string_t with a capacity
 */
struct string_t *init_string_t(size_t capacity);

/**
 * deallocate(free) a string_t
 */
void free_string_t(struct string_t *);

/**
 * concatenate a null terminated string to string_t
 */
size_t string_t_cat(struct string_t *dst, char *src);

/**
 * concatenate a string with a known lenght to string_t
 */
size_t string_t_ncat(struct string_t *dst, char *src, size_t len);

/**
 * concatenate a character to string_t
 */
size_t string_t_cat_char(struct string_t *, char);

/**
 * compare a null terminated string with a string_t
 * internally it uses strncmp
 * return -1 if either lhs or rhs is NULL
 */
int string_t_cmp(struct string_t *lhs, char *rhs);

/**
 * compare n character of a string with a string_t
 * internally it uses strncmp
 * return -1 if either lhs or rhs is NULL
 */
int string_t_ncmp(struct string_t *lhs, char *rhs, size_t len);

/**
 * return the lenght of the string stored in string_t
 */
size_t len_string_t(struct string_t *);

/**
 * return the capacity of the string stored in string_t
 */
size_t cap_string_t(struct string_t *);

/**
 * print a representation of the content of the string_t
 */
void repr_string_t(struct string_t *);

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

#endif // !REPR_H
