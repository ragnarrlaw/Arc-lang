#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include "token.h"
#include <stdbool.h>

#define PARSER_ERR_MSG_LEN 1024

struct parser {
  struct lexer *lexer;
  struct token *current_token; // current token
  struct token *next_token;    // next token
  struct parser_error *errors; // parser errors -> linked list
};

struct parser_error {
  char *message;
  int line;
  int column;
  struct parser_error *next;
};

struct parser *parser_init(struct lexer *);
void parser_free(struct parser *);

struct program *parser_parse_program(struct parser *);

void parser_add_error(struct parser *p, const char *format, ...);
bool parser_has_errors(struct parser *p);
void parser_print_errors(struct parser *p);

#endif // !PARSER_H
