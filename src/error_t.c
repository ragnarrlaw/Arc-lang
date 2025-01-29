#include "error_t.h"
#include "string_t.h"
#include "token.h"
#include "util_error.h"
#include <string.h>

struct error_t *init_error_t() {
  struct error_t *err = malloc(sizeof(struct error_t));
  if (!err) {
    ERROR_LOG("error while allocating memory\n");
    return NULL;
  }
  err->message = NULL;
  return err;
}

static void format_error(struct error_t *err, struct token *token,
                         const char *message, const char *help) {
  string_t_cat(err->message, "error: ");
  string_t_cat(err->message, (char *)message);
  string_t_cat(err->message, "\n --> ");

  char loc_buf[64];
  snprintf(loc_buf, sizeof(loc_buf), "%u:%u\n", token->line_number,
           token->col_number);
  string_t_cat(err->message, loc_buf);

  string_t_cat(err->message, "   |\n");

  char line_buf[256];
  snprintf(line_buf, sizeof(line_buf), "%4u | %s\n", token->line_number,
           token->line_start_pos);
  string_t_cat(err->message, line_buf);

  string_t_cat(err->message, "   | ");
  for (uint i = 0; i < token->col_number + 6;
       i++) { // +6 for line number padding
    string_t_cat_char(err->message, (i == token->col_number - 1) ? '^' : ' ');
  }

  for (size_t i = 0; i < token->literal_len - 1; i++) {
    string_t_cat_char(err->message, '^');
  }
  string_t_cat(err->message, "\n");

  if (help) {
    string_t_cat(err->message, "   = help: ");
    string_t_cat(err->message, (char *)help);
    string_t_cat(err->message, "\n");
  }
}

void error_t_format_err(struct error_t *err, struct token *token,
                        const char *message, const char *help) {
  err->message = init_string_t(256);
  format_error(err, token, message, help);
}

void free_error_t(struct error_t *err) {
  if (err) {
    if (err->message) {
      free_string_t(err->message);
      err->message = NULL;
    }
  }
  err = NULL;
}
