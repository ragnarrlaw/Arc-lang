#ifndef STRING_T_H
#define STRING_T_H

#include <stdio.h>
#include <stdlib.h>

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
 * print a representation of the content of the string_t to a file
 */
void frepr_string_t(FILE *restrict, struct string_t *);

#endif // !STRING_T_H