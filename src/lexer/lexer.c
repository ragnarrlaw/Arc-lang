#include "lexer.h"
#include "../token/token.h"
#include "../util/error/error.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void lexer_advance(struct lexer *l);
void lexer_transition(struct lexer *l);
struct token *lexer_indent_or_key(struct lexer *l);
struct token *lexer_numerical(struct lexer *l);
struct token *lexer_string_literal(struct lexer *l);
struct token *lexer_operator(struct lexer *l);
struct token *lexer_punctuation(struct lexer *l);
struct token *lexer_comment(struct lexer *l);
struct token *lexer_error(struct lexer *l, const char *start, int len);
void lexer_whitespace(struct lexer *l);

struct lexer *lexer_init(const char *buffer, long length) {
  struct lexer *l = (struct lexer *)malloc(sizeof(struct lexer));
  if (!l) {
    ERROR_LOG("error while allocating memory");
    return NULL;
  }
  l->position = buffer;
  l->next_position = buffer;
  l->buffer = buffer;
  l->length = length;
  l->line = 1;
  l->column = 0;
  l->current_state = STATE_START;
  lexer_advance(l);
  return l;
}

struct token *lexer_next_token(struct lexer *l) {
  lexer_whitespace(l);

  if (isalpha(l->current_char)) {
    l->current_state = STATE_INDET_OR_KEY;
    return lexer_indent_or_key(l);
  }

  if (isdigit(l->current_char)) {
    l->current_state = STATE_NUMERICAL;
    return lexer_numerical(l);
  }

  if (l->current_char == '"') {
    l->current_state = STATE_STRING_LITERAL;
    return lexer_string_literal(l);
  }

  if (l->current_char == ':' || l->current_char == '+' ||
      l->current_char == '-' || l->current_char == '/' ||
      l->current_char == '=' || l->current_char == '>' ||
      l->current_char == '<' || l->current_char == '*') {
    l->current_state = STATE_OPERATOR;
    return lexer_operator(l);
  }

  if (ispunct(l->current_char)) {
    l->current_state = STATE_PUNCTUATION;
    return lexer_punctuation(l);
  }

  if (l->current_char == '#') {
    l->current_state = STATE_COMMENT;
    return lexer_comment(l);
  }

  if (l->current_char == 0) {
    l->current_state = STATE_EOF;
    return token_init(END_OF_FILE, l->position, 0, l->line, l->column);
  }

  return lexer_error(l, l->position, 1);
}

void lexer_advance(struct lexer *l) {
  if (l->next_position >= (l->buffer + l->length)) {
    l->current_char = 0;
  } else {
    l->current_char = *l->next_position;
    if (l->current_char == '\n') {
      l->line++;
      l->column = 0;
    } else {
      l->column++;
    }
  }
  l->position = l->next_position;
  l->next_position++;
}

struct token *lexer_indent_or_key(struct lexer *l) {
  int len = 0;
  int column = l->column;
  int line = l->line;
  const char *start = l->position;

  while (isalnum(l->current_char) || l->current_char == '_') {
    len++;
    lexer_advance(l);
  }

  if (strncmp(start, "let", len) == 0) {
    return token_init(LET, start, len, line, column);
  } else if (strncmp(start, "fn", len) == 0) {
    return token_init(FUNCTION, start, len, line, column);
  } else if (strncmp(start, "match", len) == 0) {
    return token_init(MATCH, start, len, line, column);
  } else if (strncmp(start, "case", len) == 0) {
    return token_init(CASE, start, len, line, column);
  } else if (strncmp(start, "return", len) == 0) {
    return token_init(RETURN, start, len, line, column);
  } else if (strncmp(start, "float", len) == 0) {
    return token_init(FLOAT, start, len, line, column);
  } else if (strncmp(start, "int", len) == 0) {
    return token_init(INT, start, len, line, column);
  } else {
    return token_init(IDENTIFIER, start, len, line, column);
  }
}

struct token *lexer_numerical(struct lexer *l) {
  int len = 0;
  const char *start = l->position;
  int has_dot = 0;

  while (isdigit(l->current_char) || (!has_dot && l->current_char == '.')) {
    if (l->current_char == '.') {
      has_dot = 1;
    }
    len++;
    lexer_advance(l);
  }

  return token_init(NUMERICAL, start, len, l->line, l->column);
}

struct token *lexer_string_literal(struct lexer *l) {
  int len = 0;
  int column = l->column;
  int line = l->line;
  const char *start = l->position;

  lexer_advance(l); // Skip opening quote
  while (l->current_char != '"' && l->current_char != 0) {
    if (l->current_char == '\\') {
      lexer_advance(l); // Skip escape character
    }
    len++;
    lexer_advance(l);
  }

  if (l->current_char == '"') {
    lexer_advance(l); // Skip closing quote
    return token_init(STRING_LITERAL, start, len, line, column);
  }

  return lexer_error(l, start, len);
}

struct token *lexer_operator(struct lexer *l) {
  int len = 0;
  int column = l->column;
  int line = l->line;
  const char *start = l->position;
  while (ispunct(l->current_char) && !isspace(l->current_char) &&
         l->current_char != 0) {
    len++;
    lexer_advance(l);
  }
  if (len == 2 && strncmp(start, ":=", 2) == 0) {
    return token_init(ASSIGN, start, len, line, column);
  } else if (len == 2 && strncmp(start, "->", 2) == 0) {
    return token_init(FUNCTION_R, start, len, line, column);
  } else if (len == 1 && strncmp(start, "+", 1) == 0) {
    return token_init(PLUS, start, len, line, column);
  } else if (len == 1 && strncmp(start, "-", 1) == 0) {
    return token_init(SUBTRACTION, start, len, line, column);
  } else if (len == 1 && strncmp(start, "*", 1) == 0) {
    return token_init(MULTIPLICATION, start, len, line, column);
  } else if (len == 1 && strncmp(start, "/", 1) == 0) {
    return token_init(DIVISION, start, len, line, column);
  } else if (len == 1 && strncmp(start, "=", 1) == 0) {
    return token_init(EQUALS, start, len, line, column);
  } else if (len == 1 && strncmp(start, ">", 1) == 0) {
    return token_init(GREATER, start, len, line, column);
  } else if (len == 1 && strncmp(start, "<", 1) == 0) {
    return token_init(LESSER, start, len, line, column);
  } else if (len == 2 && strncmp(start, "<=", 2) == 0) {
    return token_init(LTEQ, start, len, line, column);
  } else if (len == 2 && strncmp(start, ">=", 2) == 0) {
    return token_init(GTEQ, start, len, line, column);
  } else {
    return lexer_error(l, start, len);
  }
  return NULL;
}

struct token *lexer_punctuation(struct lexer *l) {
  const char *start = l->position;
  lexer_advance(l);

  switch (*start) {
  case ',':
    return token_init(COMMA, start, 1, l->line, l->column);
  case ';':
    return token_init(SEMICOLON, start, 1, l->line, l->column);
  case '(':
    return token_init(LPAREN, start, 1, l->line, l->column);
  case ')':
    return token_init(RPAREN, start, 1, l->line, l->column);
  case '[':
    return token_init(LSQRBRAC, start, 1, l->line, l->column);
  case ']':
    return token_init(RSQRBRAC, start, 1, l->line, l->column);
  case '{':
    return token_init(LBRACE, start, 1, l->line, l->column);
  case '}':
    return token_init(RBRACE, start, 1, l->line, l->column);
  default:
    return lexer_error(l, start, 1);
  }
}

struct token *lexer_comment(struct lexer *l) {
  const char *start = l->position;
  while (l->current_char != '\n' && l->current_char != 0) {
    lexer_advance(l);
  }

  return token_init(SINGLE_LINE_COMMENT, start, l->position - start, l->line,
                    l->column);
}

struct token *lexer_error(struct lexer *l, const char *start, int len) {
  lexer_advance(l);
  return token_init(ILLEGAL, start, len, l->line, l->column);
}

void lexer_whitespace(struct lexer *l) {
  while (isspace(l->current_char)) {
    lexer_advance(l);
  }
}

void lexer_free(struct lexer *l) {
  if (l != NULL) {
    free(l);
  }
}
