#ifndef LEXER_H
#define LEXER_H

#include "token.h"

typedef unsigned char byte;

struct token_node {
  struct token *t;
  struct token_node *next;
};

struct token_pool {
  struct token_node *head;
  size_t count;
};

enum LEXER_STATE {
  STATE_START,
  STATE_IDENT_OR_KEY,
  STATE_STRING_LITERAL,
  STATE_NUMERICAL,
  STATE_ERROR,
  STATE_COMMENT,
  STATE_OPERATOR,
  STATE_PUNCTUATION,
  STATE_EOF,
};

struct lexer {
  const char *buffer;
  long length;
  const char *position;
  const char *next_position;
  byte current_char;
  long line;
  long column;
  enum LEXER_STATE current_state;
  struct token_pool *pool;
};

struct lexer *lexer_init(const char *buffer, long length);
struct token *lexer_next_token(struct lexer *l);
char lexer_current_char(struct lexer *l);
char lexer_peek(struct lexer *l);
void lexer_free(struct lexer *l);

#endif // LEXER_H
