#include "lexer.h"
#include "token.h"
#include "util_error.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct token_pool *token_pool_init();
void token_pool_push(struct token_pool *, struct token *);
void token_pool_free(struct token_pool *);

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

struct token_pool *token_pool_init() {
  struct token_pool *pool =
      (struct token_pool *)malloc(sizeof(struct token_pool));
  if (!pool) {
    ERROR_LOG("error while allocating memory");
    return NULL;
  }
  pool->head = NULL;
  pool->count = 0;
  return pool;
}

void token_pool_push(struct token_pool *pool, struct token *t) {
  if (!pool) {
    return;
  }
  struct token_node *node =
      (struct token_node *)malloc(sizeof(struct token_node));
  if (!node) {
    ERROR_LOG("error while allocating memory");
    return;
  }
  node->t = t;
  if (!pool->head) {
    pool->head = node;
  } else {
    node->next = pool->head;
    pool->head = node;
  }
  pool->count++;
}

void token_pool_free(struct token_pool *pool) {
  if (!pool) {
    pool = NULL;
  }
  struct token_node *node = pool->head;
  while (node) {
    struct token_node *tmp = node;
    node = node->next;
    token_free(tmp->t);
    free(tmp);
  }
  free(pool);
  pool = NULL;
}

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
  l->pool = token_pool_init();
  lexer_advance(l);
  return l;
}

struct token *lexer_next_token(struct lexer *l) {
  lexer_whitespace(l);
  struct token *t = NULL;
  if (l->current_char == '#') {
    l->current_state = STATE_COMMENT;
    t = lexer_comment(l);
  } else if (isalpha(l->current_char)) {
    l->current_state = STATE_IDENT_OR_KEY;
    t = lexer_indent_or_key(l);
  } else if (isdigit(l->current_char)) {
    l->current_state = STATE_NUMERICAL;
    t = lexer_numerical(l);
  } else if (l->current_char == '"') {
    l->current_state = STATE_STRING_LITERAL;
    t = lexer_string_literal(l);
  } else if (l->current_char == ':' || l->current_char == '+' ||
             l->current_char == '-' || l->current_char == '/' ||
             l->current_char == '%' || l->current_char == '=' ||
             l->current_char == '>' || l->current_char == '!' ||
             l->current_char == '<' || l->current_char == '*') {
    l->current_state = STATE_OPERATOR;
    t = lexer_operator(l);
  } else if (ispunct(l->current_char)) {
    l->current_state = STATE_PUNCTUATION;
    t = lexer_punctuation(l);
  } else if (l->current_char == 0) {
    l->current_state = STATE_EOF;
    t = token_init(END_OF_FILE, l->position, 0, l->line, l->column);
  } else {
    t = lexer_error(l, l->position, 1);
  }
  token_pool_push(l->pool, t);
  return t;
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

  if (len == 3 && strncmp(start, "let", len) == 0) {
    return token_init(LET, start, len, line, column);
  } else if (len == 2 && strncmp(start, "fn", len) == 0) {
    return token_init(FUNCTION, start, len, line, column);
  } else if (len == 5 && strncmp(start, "match", len) == 0) {
    return token_init(MATCH, start, len, line, column);
  } else if (len == 4 && strncmp(start, "case", len) == 0) {
    return token_init(CASE, start, len, line, column);
  } else if (len == 6 && strncmp(start, "return", len) == 0) {
    return token_init(RETURN, start, len, line, column);
  } else if (len == 5 && strncmp(start, "float", len) == 0) {
    return token_init(FLOAT, start, len, line, column);
  } else if (len == 3 && strncmp(start, "int", len) == 0) {
    return token_init(INT, start, len, line, column);
  } else if (len == 2 && strncmp(start, "if", len) == 0) {
    return token_init(IF, start, len, line, column);
  } else if (len == 4 && strncmp(start, "else", len) == 0) {
    return token_init(ELSE, start, len, line, column);
  } else if (len == 3 && strncmp(start, "for", len) == 0) {
    return token_init(FOR, start, len, line, column);
  } else if (len == 5 && strncmp(start, "while", len) == 0) {
    return token_init(WHILE, start, len, line, column);
  } else if (len == 4 && strncmp(start, "true", len) == 0) {
    return token_init(TRUE, start, len, line, column);
  } else if (len == 5 && strncmp(start, "false", len) == 0) {
    return token_init(FALSE, start, len, line, column);
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

  // Determine the token type based on the presence of a dot
  enum TOKEN_TYPE type = has_dot ? FLOAT : INT;

  return token_init(type, start, len, l->line, l->column);
}

struct token *lexer_string_literal(struct lexer *l) {
  int len = 0;
  int column = l->column;
  int line = l->line;

  lexer_advance(l); // Skip opening quote
  const char *start = l->position;
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
  while (len < 2 && ispunct(l->current_char) && l->current_char != ';' &&
         !isspace(l->current_char) && l->current_char != 0) {
    len++;
    lexer_advance(l);
  }
  if (len == 2 && strncmp(start, ":=", 2) == 0) {
    return token_init(ASSIGN, start, len, line, column);
  } else if (len == 2 && strncmp(start, "->", 2) == 0) {
    return token_init(FUNCTION_R, start, len, line, column);
  } else if (len == 1 && strncmp(start, "+", 1) == 0) {
    return token_init(PLUS, start, len, line, column);
  } else if (len == 1 && strncmp(start, "!", 1) == 0) {
    return token_init(BANG, start, len, line, column);
  } else if (len == 1 && strncmp(start, "-", 1) == 0) {
    return token_init(MINUS, start, len, line, column);
  } else if (len == 1 && strncmp(start, "%", 1) == 0) {
    return token_init(MOD, start, len, line, column);
  } else if (len == 1 && strncmp(start, "*", 1) == 0) {
    return token_init(ASTERISK, start, len, line, column);
  } else if (len == 1 && strncmp(start, "/", 1) == 0) {
    return token_init(SLASH, start, len, line, column);
  } else if (len == 1 && strncmp(start, "=", 1) == 0) {
    return token_init(EQUAL, start, len, line, column);
  } else if (len == 1 && strncmp(start, ">", 1) == 0) {
    return token_init(GT, start, len, line, column);
  } else if (len == 1 && strncmp(start, "<", 1) == 0) {
    return token_init(LT, start, len, line, column);
  } else if (len == 2 && strncmp(start, "<=", 2) == 0) {
    return token_init(LT_EQ, start, len, line, column);
  } else if (len == 2 && strncmp(start, ">=", 2) == 0) {
    return token_init(GT_EQ, start, len, line, column);
  } else if (len == 2 && strncmp(start, "==", 2) == 0) {
    return token_init(EQ_EQ, start, len, line, column);
  } else if (len == 2 && strncmp(start, "!=", 2) == 0) {
    return token_init(NOT_EQ, start, len, line, column);
  } else if (len == 2 && strncmp(start, "--", 2) == 0) {
    return token_init(DEC, start, len, line, column);
  } else if (len == 2 && strncmp(start, "++", 2) == 0) {
    return token_init(INC, start, len, line, column);
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
    if (l->pool) {
      token_pool_free(l->pool);
    }
    free(l);
  }
}
