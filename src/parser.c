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
#include <string.h>

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

// expressions
struct expression *parser_parse_expression(struct parser *,
                                           enum OPERATOR_PRECEDENCE);
struct expression *parser_parse_identifier(struct parser *);
struct expression *parser_parse_unary_operator(struct parser *);
/** this is the only function for parsing binary operators infix operators */
struct expression *parser_parse_binary_operator(struct parser *,
                                                struct expression *);
struct expression *parser_parse_integer_literal(struct parser *);
struct expression *parser_parse_float_literal(struct parser *);
struct expression *parser_parse_string_literal(struct parser *);
struct expression *parser_parse_boolean_literal(struct parser *p);
struct expression *parser_parse_if_expression(struct parser *);
struct expression *parser_parse_for_expression(struct parser *);
struct expression *parser_parse_while_expression(struct parser *);
struct expression *parser_parse_fn_def_expression(struct parser *);

// prefix_fn, infix_fn, and postfix_fn -> for literals
parser_parse_prefix_fn parser_get_prefix_fn(enum TOKEN_TYPE type);
parser_parse_infix_fn parser_get_infix_fn(enum TOKEN_TYPE type);
parser_parse_postfix_fn parser_get_postfix_fn(enum TOKEN_TYPE type);

// precedence of operators -> for parsing expressions
struct token_precedence parser_get_token_precedence(enum TOKEN_TYPE type);
enum OPERATOR_PRECEDENCE parser_get_next_token_precedence(struct parser *p);
enum OPERATOR_PRECEDENCE parser_get_current_token_precedence(struct parser *p);

/**
defines the precedence table for the operators
right associative token have a higher right binding
value and a lower left binding value than their right
 */
struct token_precedence parser_get_token_precedence(enum TOKEN_TYPE type) {
  switch (type) {
  case EQ_EQ:
  case NOT_EQ:
    return (struct token_precedence){EQUALITY, EQUALITY};
  case LT:
  case GT:
    return (struct token_precedence){RELATIONAL, RELATIONAL};
  case PLUS:
  case MINUS:
    return (struct token_precedence){ADDITIVE, ADDITIVE};
  case ASTERISK:
  case SLASH:
    return (struct token_precedence){MULTIPLICATIVE, MULTIPLICATIVE};
  case AND:
    return (struct token_precedence){LOGICAL_AND, LOGICAL_AND};
  case OR:
    return (struct token_precedence){LOGICAL_OR, LOGICAL_OR};
  case ASSIGN:
    return (struct token_precedence){ASSIGNMENT,
                                     ASSIGNMENT + 1}; // right-associative
  case INC:
  case DEC:
    return (struct token_precedence){POSTFIX, POSTFIX};
  case COMMA:
    return (struct token_precedence){LOWEST, LOWEST};
  default:
    return (struct token_precedence){LOWEST, LOWEST};
  }
}

enum OPERATOR_PRECEDENCE parser_get_next_token_precedence(struct parser *p) {
  return parser_get_token_precedence(p->next_token->type).lbp;
}

enum OPERATOR_PRECEDENCE parser_get_current_token_precedence(struct parser *p) {
  return parser_get_token_precedence(p->current_token->type).rbp;
}

parser_parse_prefix_fn parser_get_prefix_fn(enum TOKEN_TYPE type) {
  switch (type) {
  case IDENTIFIER:
    return parser_parse_identifier;
  case INT:
    return parser_parse_integer_literal;
  case FLOAT:
    return parser_parse_float_literal;
  case STRING_LITERAL:
    return parser_parse_string_literal;
  case TRUE:
  case FALSE:
    return parser_parse_boolean_literal;
  case BANG:
  case MINUS:
    return parser_parse_unary_operator;
  default:
    return NULL;
  }
}

parser_parse_infix_fn parser_get_infix_fn(enum TOKEN_TYPE type) {
  return parser_parse_binary_operator; // for all the binary operators, one
                                       // parsing function
}

parser_parse_postfix_fn parser_get_postfix_fn(enum TOKEN_TYPE type) {
  return NULL;
}

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
    lexer_free(p->lexer);
    free(p);
  }
}

struct program *parser_parse_program(struct parser *p) {
  struct program *program = (struct program *)malloc(sizeof(struct program));

  while (!parser_current_token_is(p, END_OF_FILE)) {
    struct statement *stmt = parser_parse_statement(p);
    if (stmt != NULL) {
      ast_program_push_statement(program, stmt);
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
    return parser_parse_expression_statement(p);
  }
}

// let <identifier> := <expression>;
struct statement *parser_parse_let_statement(struct parser *p) {
  struct statement *stmt = ast_statement_init(STMT_LET);
  if (stmt != NULL) {
    stmt->let_stmt.token = p->current_token;
    if (!parser_expect_next_token(p, IDENTIFIER)) {
      ast_statement_free(stmt);
      return NULL;
    }
    stmt->let_stmt.identifier = p->current_token;

    if (!parser_expect_next_token(p, ASSIGN)) {
      ast_statement_free(stmt);
      return NULL;
    }

    parser_next_token(p);

    struct expression *expr = parser_parse_expression(p, LOWEST);
    if (expr == NULL) {
      ast_statement_free(stmt);
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
  struct expression *expr = parser_parse_expression(p, LOWEST);
  if (expr == NULL) {
    ast_statement_free(stmt);
    return NULL;
  }
  stmt->return_stmt.value = expr;
  return stmt;
}

// <expression>
struct statement *parser_parse_expression_statement(struct parser *p) {
  struct statement *stmt = ast_statement_init(STMT_EXPRESSION);
  if (stmt == NULL) {
    return NULL;
  }
  stmt->expr_stmt.token = p->current_token;
  struct expression *expr = parser_parse_expression(p, LOWEST);

  if (expr == NULL) {
    ast_statement_free(stmt);
    return NULL;
  }

  if (parser_next_token_is(p, SEMICOLON)) {
    parser_next_token(p);
  }

  return stmt;
}

struct expression *
parser_parse_expression(struct parser *p, enum OPERATOR_PRECEDENCE precedence) {
  parser_parse_prefix_fn prefix_fn =
      parser_get_prefix_fn(p->current_token->type);

  if (prefix_fn == NULL) {
    parser_add_error(p, "no prefix parse function for %s",
                     token_type_to_str(p->current_token->type));
    return NULL;
  }

  struct expression *left = prefix_fn(p);

  while (!parser_next_token_is(p, SEMICOLON) &&
         precedence < parser_get_next_token_precedence(p)) {
    parser_parse_infix_fn infix_fn = parser_get_infix_fn(p->next_token->type);
    if (infix_fn == NULL) {
      return left;
    }
    parser_next_token(p);
    left = infix_fn(p, left);
  }
  return left;
}

struct expression *parser_parse_identifier(struct parser *p) {
  struct expression *expr = ast_expression_init(EXPR_IDENTIFIER);
  if (!expr) {
    return NULL;
  }
  expr->identifier_expr.token = p->current_token;
  return expr;
}

// unary_operator<expression> -> -<expression> or !<expression>
struct expression *parser_parse_unary_operator(struct parser *p) {
  struct expression *expr = ast_expression_init(EXPR_UNARY);
  if (!expr) {
    return NULL;
  }
  expr->unary_expr.op = p->current_token;
  parser_next_token(p); // advance to the right-hand side of the operator
  expr->unary_expr.right = parser_parse_expression(
      p, PREFIX); // parse the right-hand side expression
  if (!expr->unary_expr.right) {
    ast_expression_free(expr);
    return NULL;
  }
  return expr;
}

// <expression>binary_operator<expression>
struct expression *parser_parse_binary_operator(struct parser *p,
                                                struct expression *left) {
  struct expression *expr = ast_expression_init(EXPR_BINARY);
  if (!expr) {
    return NULL;
  }
  expr->binary_expr.left = left;
  expr->binary_expr.op = p->current_token;
  struct token_precedence prec =
      parser_get_token_precedence(p->current_token->type);
  parser_next_token(p);
  expr->binary_expr.right = parser_parse_expression(p, prec.rbp);
  return expr;
}

struct expression *parser_parse_integer_literal(struct parser *p) {
  struct expression *expr = ast_expression_init(EXPR_LITERAL);
  if (!expr) {
    return NULL;
  }
  expr->literal.token = p->current_token;
  expr->literal.literal_type = LITERAL_INT;
  char buffer[p->current_token->literal_len + 1];
  memcpy(buffer, p->current_token->literal, p->current_token->literal_len);
  buffer[p->current_token->literal_len] = '\0';

  long value = strtol(buffer, NULL, 10);
  expr->literal.value.int_value = value;

  return expr;
}

struct expression *parser_parse_float_literal(struct parser *p) {
  struct expression *expr = ast_expression_init(EXPR_LITERAL);
  if (!expr) {
    return NULL;
  }
  expr->literal.token = p->current_token;
  expr->literal.literal_type = LITERAL_FLOAT;
  char buffer[p->current_token->literal_len + 1];
  memcpy(buffer, p->current_token->literal, p->current_token->literal_len);
  buffer[p->current_token->literal_len] = '\0';

  float value = strtof(buffer, NULL);
  expr->literal.value.float_value = value;

  return expr;
}

struct expression *parser_parse_string_literal(struct parser *p) {
  struct expression *expr = ast_expression_init(EXPR_LITERAL);
  if (!expr) {
    return NULL;
  }

  expr->literal.token = p->current_token;
  expr->literal.literal_type = LITERAL_STRING;
  expr->literal.value.string_literal = malloc(sizeof(struct string_literal));
  if (!expr->literal.value.string_literal) {
    ast_expression_free(expr);
    return NULL;
  }

  expr->literal.value.string_literal->value =
      strndup(p->current_token->literal, p->current_token->literal_len);
  expr->literal.value.string_literal->length = p->current_token->literal_len;

  return expr;
}

struct expression *parser_parse_boolean_literal(struct parser *p) {
  struct expression *expr = ast_expression_init(EXPR_LITERAL);
  if (!expr) {
    return NULL;
  }
  expr->literal.token = p->current_token;
  expr->literal.literal_type = LITERAL_BOOL;
  expr->literal.value.bool_value = p->current_token->type == TRUE;
  return expr;
}