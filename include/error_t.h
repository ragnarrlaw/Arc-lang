#ifndef ERROR_T_H
#define ERROR_T_H

#include "string_t.h"
#include "token.h"

struct error_t {
  struct string_t *message;
};

// clang-format off

struct error_t *init_error_t();
void error_t_format_err(struct error_t *err, struct token *token, const char *message, const char *help);
void free_error_t(struct error_t *);

// clang-format on

#endif // !ERROR_T_H