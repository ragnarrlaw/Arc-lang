#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include "token.h"
#include "util_error.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// prefix operator
typedef struct expression *(*parser_parse_prefix_fn)(struct parser *p);

// infix operator
typedef struct expression *(*parser_parse_infix_fn)(struct parser *p,
                                                    struct expression *left);

// postfix operator
typedef struct expression *(*parser_parse_postfix_fn)(struct parser *p,
                                                      struct expression *left);

struct statement *parser_parse_statement(struct parser *);

// advance the parser to the next token
void parser_next_token(struct parser *);
// assert current token's type
bool parser_current_token_is(struct parser *p, enum TOKEN_TYPE);
// assert next token's type
bool parser_next_token_is(struct parser *p, enum TOKEN_TYPE);
/**
 * if the next token is the expected token advance the parser, used to enforce
 * constructs */
bool parser_expect_next_token(struct parser *p, enum TOKEN_TYPE);

// statements
struct statement *parser_parse_let_statement(struct parser *);
struct statement *parser_parse_return_statement(struct parser *);
struct statement *parser_parse_expression_statement(struct parser *);
struct statement *parser_parse_if_statement(struct parser *);
struct statement *parser_parse_for_statement(struct parser *);
struct statement *parser_parse_while_statement(struct parser *);
struct statement *parser_parse_fn_def_statement(struct parser *);

// expressions
struct expression *parser_parse_expression(struct parser *p);

struct parser *parser_init(struct lexer *l) {
  struct parser *p = (struct parser *)malloc(sizeof(struct parser));
  if (p == NULL) {
    ERROR_LOG("error while allocating memory");
    return NULL;
  }
  p->lexer = l;
  p->errors = NULL;
  parser_next_token(p); // fill the next token field with a token
  parser_next_token(p); // fill the current token field with a token
  return p;
}

void parser_free(struct parser *p) {
  if (p != NULL) {
    if (parser_has_errors(p)) {
      struct parser_error *error = p->errors;
      while (error) {
        struct parser_error *tmp = error;
        error = tmp->next;
        free(tmp->message);
        free(tmp);
      }
    }
    free(p);
  }
}

struct program *parser_parse_program(struct parser *p) {
  struct program *program = (struct program *)malloc(sizeof(struct program));

  while (!parser_current_token_is(p, END_OF_FILE)) {
    struct statement *stmt = parser_parse_statement(p);
    if (stmt != NULL) {
      ast_program_push_statement(program, stmt);
    } else {
      TODO
    }
    parser_next_token(p);
  }
  return program;
}

void parser_add_error(struct parser *p, const char *format, ...) {
  va_list args;
  va_start(args, format);
  char *message = malloc(PARSER_ERR_MSG_LEN);
  vsnprintf(message, PARSER_ERR_MSG_LEN, format, args);
  va_end(args);

  struct parser_error *error =
      (struct parser_error *)malloc(sizeof(struct parser_error));
  error->message = message;
  error->line = p->current_token->line_number;
  error->column = p->current_token->col_number;
  error->next = p->errors;
}

void parser_print_errors(struct parser *p) {
  struct parser_error *error = p->errors;
  while (error) {
    fprintf(stderr, "Error at line %d, column %d: %s\n", error->line,
            error->column, error->message);
    error = error->next;
  }
}

bool parser_has_errors(struct parser *p) { return p->errors != NULL; }

void parser_next_token(struct parser *p) {
  p->current_token = p->next_token;
  p->next_token = lexer_next_token(p->lexer);
}

bool parser_current_token_is(struct parser *p, enum TOKEN_TYPE type) {
  return p->current_token->type == type;
}

bool parser_next_token_is(struct parser *p, enum TOKEN_TYPE type) {
  return p->next_token->type == type;
}

bool parser_expect_next_token(struct parser *p, enum TOKEN_TYPE type) {
  if (parser_next_token_is(p, type)) {
    parser_next_token(p);
    return true;
  } else {
    parser_add_error(p, "Expected %s, got %s", token_type_to_str(type),
                     token_type_to_str(p->next_token->type));
    return false;
  }
}

struct statement *parser_parse_statement(struct parser *p) {
  switch (p->current_token->type) {
  case LET:
    return parser_parse_let_statement(p);
  case RETURN:
    return parser_parse_return_statement(p);
  default:
    return NULL;
  }
  return NULL;
}

// let <identifier> := <expression>;
struct statement *parser_parse_let_statement(struct parser *p) {
  struct statement *stmt = ast_statement_init(STMT_LET);
  if (stmt != NULL) {
    stmt->let_stmt.token = p->current_token;
    if (!parser_expect_next_token(p, IDENTIFIER)) {
      free(stmt);
      return NULL;
    }
    stmt->let_stmt.identifier = p->current_token;

    if (!parser_expect_next_token(p, ASSIGN)) {
      free(stmt);
      return NULL;
    }
    struct expression *expr = parser_parse_expression(p);
    if (expr == NULL) {
      free(stmt);
      return NULL;
    }
    stmt->let_stmt.value = expr;

    return stmt;
  }
  return NULL;
}

// return <expression>;
struct statement *parser_parse_return_statement(struct parser *p) {
  struct statement *stmt = ast_statement_init(STMT_RETURN);
  if (stmt == NULL) {
    return NULL;
  }
  stmt->return_stmt.token = p->current_token;
  parser_next_token(p);
  struct expression *expr = parser_parse_expression(p);
  if (expr == NULL) {
    free(stmt);
    return NULL;
  }
  stmt->return_stmt.value = expr;
  return stmt;
}

struct statement *parser_parse_expression_statement(struct parser *p) {
  return NULL;
}

struct statement *parser_parse_if_statement(struct parser *p) { return NULL; }

struct statement *parser_parse_for_statement(struct parser *p) { return NULL; }

struct statement *parser_parse_while_statement(struct parser *p) {
  return NULL;
}

struct statement *parser_parse_fn_def_statement(struct parser *p) {
  return NULL;
}

struct expression *parser_parse_expression(struct parser *p) { return NULL; }