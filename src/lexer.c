#include "lexer.h"
#include "token.h"
#include "util_error.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// initializes a token pool.
struct token_pool *token_pool_init();

// pushes a token into the token pool.
void token_pool_push(struct token_pool *, struct token *);

// frees the memory allocated for the token pool.
void token_pool_free(struct token_pool *);

// advances the lexer to the next character.
void lexer_advance(struct lexer *l);

// transitions the lexer state.
void lexer_transition(struct lexer *l);

// processes an identifier or keyword token.
struct token *lexer_indent_or_key(struct lexer *l);

// processes a numerical token.
struct token *lexer_numerical(struct lexer *l);

// processes a string literal token.
struct token *lexer_string_literal(struct lexer *l);

// processes a character literal token.
struct token *lexer_char_literal(struct lexer *l);

// processes an operator token.
struct token *lexer_operator(struct lexer *l);

// processes a punctuation token.
struct token *lexer_punctuation(struct lexer *l);

// processes a comment token.
struct token *lexer_comment(struct lexer *l);

// generates an error token.
struct token *lexer_error(struct lexer *l, const char *start, int len);

// skips whitespace characters.
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
  node->next = NULL;
  if (!pool->head) {
    pool->head = node;
  } else {
    node->next = pool->head;
    pool->head = node;
  }
  pool->count++;
}

void token_pool_free(struct token_pool *pool) {
  if (pool) {
    for (struct token_node *node = pool->head; node;) {
      struct token_node *tmp = node;
      node = tmp->next;
      if (tmp) {
        if (tmp->t) {
          token_free(tmp->t);
        }
        free(tmp);
      }
    }
    free(pool);
  }
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
  l->line_start_pos = buffer;
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
  } else if (l->current_char == '\'') {
    l->current_state = STATE_CHAR_LITERAL;
    t = lexer_char_literal(l);
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
    t = token_init(END_OF_FILE, l->position, 0, l->line, l->column,
                   l->line_start_pos);
  } else {
    t = lexer_error(l, l->position, 1);
  }
  if (t->type != ASTERISK && t->type != SLASH && t->type != MOD &&
      t->type != PLUS && t->type != MINUS && t->type != EQUAL &&
      t->type != LT && t->type != GT && t->type != LT_EQ && t->type != GT_EQ &&
      t->type != BANG && t->type != EQ_EQ && t->type != NOT_EQ &&
      t->type != AND && t->type != OR && t->type != INC && t->type != DEC) {
    token_pool_push(l->pool, t);
  }
  return t;
}

void lexer_advance(struct lexer *l) {
  if (l->next_position >= (l->buffer + l->length)) {
    l->current_char = 0;
  } else {
    l->current_char = *l->next_position;
    if (l->current_char == '\n') {
      l->line_start_pos = l->next_position;
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
    return token_init(LET, start, len, line, column, l->line_start_pos);
  } else if (len == 2 && strncmp(start, "fn", len) == 0) {
    return token_init(FUNCTION, start, len, line, column, l->line_start_pos);
  } else if (len == 5 && strncmp(start, "match", len) == 0) {
    return token_init(MATCH, start, len, line, column, l->line_start_pos);
  } else if (len == 4 && strncmp(start, "case", len) == 0) {
    return token_init(CASE, start, len, line, column, l->line_start_pos);
  } else if (len == 6 && strncmp(start, "return", len) == 0) {
    return token_init(RETURN, start, len, line, column, l->line_start_pos);
  } else if (len == 5 && strncmp(start, "float", len) == 0) {
    return token_init(FLOAT, start, len, line, column, l->line_start_pos);
  } else if (len == 3 && strncmp(start, "int", len) == 0) {
    return token_init(INT, start, len, line, column, l->line_start_pos);
  } else if (len == 2 && strncmp(start, "if", len) == 0) {
    return token_init(IF, start, len, line, column, l->line_start_pos);
  } else if (len == 4 && strncmp(start, "else", len) == 0) {
    return token_init(ELSE, start, len, line, column, l->line_start_pos);
  } else if (len == 3 && strncmp(start, "for", len) == 0) {
    return token_init(FOR, start, len, line, column, l->line_start_pos);
  } else if (len == 5 && strncmp(start, "while", len) == 0) {
    return token_init(WHILE, start, len, line, column, l->line_start_pos);
  } else if (len == 4 && strncmp(start, "true", len) == 0) {
    return token_init(TRUE, start, len, line, column, l->line_start_pos);
  } else if (len == 5 && strncmp(start, "false", len) == 0) {
    return token_init(FALSE, start, len, line, column, l->line_start_pos);
  } else {
    return token_init(IDENTIFIER, start, len, line, column, l->line_start_pos);
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

  return token_init(type, start, len, l->line, l->column, l->line_start_pos);
}

struct token *lexer_string_literal(struct lexer *l) {
  int column = l->column;
  int line = l->line;

  lexer_advance(l);
  char *buffer = malloc(strlen(l->position) + 1);
  if (!buffer) {
    return lexer_error(l, l->position, 0);
  }

  char *p = buffer;

  while (l->current_char != '"' && l->current_char != 0) {
    if (l->current_char == '\\') {
      lexer_advance(l);
      switch (l->current_char) {
      case 'n':
        *p++ = '\n';
        break;
      case 't':
        *p++ = '\t';
        break;
      case '\\':
        *p++ = '\\';
        break;
      case '"':
        *p++ = '"';
        break;
      default:
        *p++ = l->current_char;
        break;
      }
    } else {
      *p++ = l->current_char;
    }
    lexer_advance(l);
  }

  *p = '\0';

  if (l->current_char == '"') {
    lexer_advance(l);
    return token_init(STRING_LITERAL, buffer, p - buffer, line, column,
                      l->line_start_pos);
  }

  free(buffer);
  return lexer_error(l, l->position, 0);
}

struct token *lexer_char_literal(struct lexer *l) {
  int len = 0;
  int column = l->column;
  int line = l->line;
  const char *start = l->position;
  lexer_advance(l); // skip opening quote
  if (l->current_char == '\\') {
    len++;            // include escape character
    lexer_advance(l); // advance past escape character
    if (l->current_char != 0) {
      len++;            // include escaped character
      lexer_advance(l); // advance past escaped character
    }
  } else {
    len++;
    lexer_advance(l);
  }

  if (l->current_char == '\'') {
    lexer_advance(l); // skip closing quote
    return token_init(CHAR, start, len, line, column, l->line_start_pos);
  }
  return lexer_error(l, start, len);
}

struct token *lexer_operator(struct lexer *l) {
  int column = l->column;
  int line = l->line;
  const char *start = l->position;
  char current_char = l->current_char;

  if (ispunct(current_char) && current_char != ';' && !isspace(current_char) &&
      current_char != 0) {
    char next_char = lexer_peek(l);
    char two_char_op[3] = {current_char, next_char, '\0'};

    if (strncmp(two_char_op, ":=", 2) == 0) {
      lexer_advance(l);
      lexer_advance(l);
      return token_init(ASSIGN, start, 2, line, column, l->line_start_pos);
    } else if (strncmp(two_char_op, "->", 2) == 0) {
      lexer_advance(l);
      lexer_advance(l);
      return token_init(FUNCTION_R, start, 2, line, column, l->line_start_pos);
    } else if (strncmp(two_char_op, "<=", 2) == 0) {
      lexer_advance(l);
      lexer_advance(l);
      return token_init(LT_EQ, start, 2, line, column, l->line_start_pos);
    } else if (strncmp(two_char_op, ">=", 2) == 0) {
      lexer_advance(l);
      lexer_advance(l);
      return token_init(GT_EQ, start, 2, line, column, l->line_start_pos);
    } else if (strncmp(two_char_op, "==", 2) == 0) {
      lexer_advance(l);
      lexer_advance(l);
      return token_init(EQ_EQ, start, 2, line, column, l->line_start_pos);
    } else if (strncmp(two_char_op, "!=", 2) == 0) {
      lexer_advance(l);
      lexer_advance(l);
      return token_init(NOT_EQ, start, 2, line, column, l->line_start_pos);
    } else if (strncmp(two_char_op, "--", 2) == 0) {
      lexer_advance(l);
      lexer_advance(l);
      return token_init(DEC, start, 2, line, column, l->line_start_pos);
    } else if (strncmp(two_char_op, "++", 2) == 0) {
      lexer_advance(l);
      lexer_advance(l);
      return token_init(INC, start, 2, line, column, l->line_start_pos);
    }
  }

  if (ispunct(current_char) && current_char != ';' && !isspace(current_char) &&
      current_char != 0) {
    char op_char = current_char;
    lexer_advance(l);

    switch (op_char) {
    case '+':
      return token_init(PLUS, start, 1, line, column, l->line_start_pos);
    case '-':
      return token_init(MINUS, start, 1, line, column, l->line_start_pos);
    case '!':
      return token_init(BANG, start, 1, line, column, l->line_start_pos);
    case '%':
      return token_init(MOD, start, 1, line, column, l->line_start_pos);
    case '*':
      return token_init(ASTERISK, start, 1, line, column, l->line_start_pos);
    case '/':
      return token_init(SLASH, start, 1, line, column, l->line_start_pos);
    case '=':
      return token_init(EQUAL, start, 1, line, column, l->line_start_pos);
    case '>':
      return token_init(GT, start, 1, line, column, l->line_start_pos);
    case '<':
      return token_init(LT, start, 1, line, column, l->line_start_pos);
    default:
      return lexer_error(l, start, 1);
    }
  }

  return lexer_error(l, start, 0);
}

struct token *lexer_punctuation(struct lexer *l) {
  const char *start = l->position;
  lexer_advance(l);

  switch (*start) {
  case ',':
    return token_init(COMMA, start, 1, l->line, l->column, l->line_start_pos);
  case ';':
    return token_init(SEMICOLON, start, 1, l->line, l->column,
                      l->line_start_pos);
  case '(':
    return token_init(LPAREN, start, 1, l->line, l->column, l->line_start_pos);
  case ')':
    return token_init(RPAREN, start, 1, l->line, l->column, l->line_start_pos);
  case '[':
    return token_init(LSQRBRAC, start, 1, l->line, l->column,
                      l->line_start_pos);
  case ']':
    return token_init(RSQRBRAC, start, 1, l->line, l->column,
                      l->line_start_pos);
  case '{':
    return token_init(LBRACE, start, 1, l->line, l->column, l->line_start_pos);
  case '}':
    return token_init(RBRACE, start, 1, l->line, l->column, l->line_start_pos);
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
                    l->column, l->line_start_pos);
}

struct token *lexer_error(struct lexer *l, const char *start, int len) {
  lexer_advance(l);
  return token_init(ILLEGAL, start, len, l->line, l->column, l->line_start_pos);
}

void lexer_whitespace(struct lexer *l) {
  while (isspace(l->current_char)) {
    lexer_advance(l);
  }
}

char lexer_peek(struct lexer *l) {
  if (l->next_position >= (l->buffer + l->length)) {
    return -1;
  }
  return *l->next_position;
}

void lexer_free(struct lexer *l) {
  if (l != NULL) {
    if (l->pool) {
      token_pool_free(l->pool);
    }
    free(l);
  }
}
