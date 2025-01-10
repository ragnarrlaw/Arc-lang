#include "token.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

static const char *token_types[] = {
    "illegal", // ILLEGAL
    "eof",     // EOF

    "identifier", // IDENTIFIER
    "integer",    // INT

    "assignment_op", // ASSIGN
    "equals",        // EQUALS
    "plus",          // PLUS

    "comma",     // COMMA
    "semicolon", // SEMICOLON

    "left parenthesis",  // LPAREN
    "right parenthesis", // RPAREN
    "left brace",        // LBRACE
    "right brace",       // RBRACE

    "function", // FUNCTION
    "let",      // LET
    "match",    // MATCH
};

struct token *token_init(enum TOKEN_TYPE type, const char *literal_start,
                         size_t literal_len, uint line, uint col) {
  struct token *token = (struct token *)malloc(sizeof(struct token));
  if (token == NULL) {
    return NULL;
  }
  token->type = type;
  token->literal = literal_start;
  token->literal_len = literal_len;
  token->line_number = line;
  token->col_number = col;
  return token;
}

void token_free(struct token *t) {
  if (t != NULL) {
    free(t);
  }
  t = NULL;
}

void token_str(struct token *t) {
  printf("Type: %d\n", t->type);
  for (size_t i = 0; i < t->literal_len; i++) {
    printf("%c", t->literal[i]);
  }
  printf("\n");
  printf("Line: %d\n", t->line_number);
  printf("Column: %d\n", t->col_number);
}
