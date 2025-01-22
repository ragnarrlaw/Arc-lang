#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include "token.h"
#include "util_error.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TRACE_ON
#define TRACE_FN printf("%s:%d\n", __func__, __LINE__)
#else
#define TRACE_FN
#endif

struct OP_POWER {
  int_fast8_t lbp;
  int_fast8_t rbp;
};

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
struct expression *parser_parse_expr_bp(struct parser *p,
                                        enum OPERATOR_PRECEDENCE min_bp);

struct expression *parser_parse_identifier(struct parser *);
struct expression *parser_parse_unary_operator(struct parser *);
/** this is the only function for parsing binary operators infix operators */
struct expression *parser_parse_binary_operator(struct parser *,
                                                struct expression *);
struct expression *parser_parse_postfix_operator(struct parser *,
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
struct OP_POWER parser_prefix_binding_power(enum TOKEN_TYPE type);
struct OP_POWER parser_infix_binding_power(enum TOKEN_TYPE type);
struct OP_POWER parser_postfix_binding_power(enum TOKEN_TYPE type);

// ========================================================================

/**
 * initialize a parser
 */
struct parser *parser_init(struct lexer *l) {
  TRACE_FN;
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

/**
 * free (deallocate) a parser
 */
void parser_free(struct parser *p) {
  TRACE_FN;
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

/**
 * start parsing a program
 */
struct program *parser_parse_program(struct parser *p) {
  TRACE_FN;
  struct program *program = ast_program_init();
  if (!program) {
    return NULL;
  }

  while (!parser_current_token_is(p, END_OF_FILE)) {
    struct statement *stmt = parser_parse_statement(p);
    if (stmt != NULL) {
      ast_program_push_statement(program, stmt);
    }
    parser_next_token(p);
  }
  return program;
}

/**
 * add an error to the parser error collection
 */
void parser_add_error(struct parser *p, const char *format, ...) {
  TRACE_FN;
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
  p->errors = error;
}

/**
 * print all the errors in the parser
 */
void parser_print_errors(struct parser *p) {
  TRACE_FN;
  struct parser_error *error = p->errors;
  while (error) {
    fprintf(stderr, "Error at line %d, column %d: %s\n", error->line,
            error->column, error->message);
    error = error->next;
  }
}

bool parser_has_errors(struct parser *p) {
  TRACE_FN;
  return p->errors != NULL;
}

/**
 * advance the parser to the next token
 */
void parser_next_token(struct parser *p) {
  TRACE_FN;
  p->current_token = p->next_token;
  p->next_token = lexer_next_token(p->lexer);
}

/**
 * assert current token's type
 */
bool parser_current_token_is(struct parser *p, enum TOKEN_TYPE type) {
  TRACE_FN;
  return p->current_token->type == type;
}

/**
 * assert peek token's type
 */
bool parser_next_token_is(struct parser *p, enum TOKEN_TYPE type) {
  TRACE_FN;
  return p->next_token->type == type;
}

/**
 * if the next token is the expected token advance the parser, used to enforce
 * constructs
 */
bool parser_expect_next_token(struct parser *p, enum TOKEN_TYPE type) {
  TRACE_FN;
  if (parser_next_token_is(p, type)) {
    parser_next_token(p);
    return true;
  } else {
    parser_add_error(p, "Expected %s, got %s", token_type_to_str(type),
                     token_type_to_str(p->next_token->type));
    return false;
  }
}

/**
 * defines the binding power for the prefix(unary) operators
 */
struct OP_POWER parser_prefix_binding_power(enum TOKEN_TYPE type) {
  TRACE_FN;
  switch (type) {
  case BANG:
  case PLUS:
  case MINUS:
  case INC:
  case DEC:
    return (struct OP_POWER){PREFIX, PREFIX + 1};
  default:
    return (struct OP_POWER){LOWEST, LOWEST + 1};
  }
}

/**
 * defines the binding power for the infix(binary) operators
 */
struct OP_POWER parser_infix_binding_power(enum TOKEN_TYPE type) {
  TRACE_FN;
  switch (type) {
  case PLUS:
  case MINUS:
    return (struct OP_POWER){ADDITIVE, ADDITIVE + 1};
  case ASTERISK:
  case SLASH:
  case MOD:
    return (struct OP_POWER){MULTIPLICATIVE, MULTIPLICATIVE + 1};
  case EQ_EQ:
  case NOT_EQ:
    return (struct OP_POWER){EQUALITY, EQUALITY + 1};
  case LT:
  case GT:
  case LT_EQ:
  case GT_EQ:
    return (struct OP_POWER){RELATIONAL, RELATIONAL + 1};
  case AND:
    return (struct OP_POWER){LOGICAL_AND, LOGICAL_AND + 1};
  case OR:
    return (struct OP_POWER){LOGICAL_OR, LOGICAL_OR + 1};
  case ASSIGN:
    return (struct OP_POWER){ASSIGNMENT, ASSIGNMENT - 1};
  default:
    return (struct OP_POWER){LOWEST, LOWEST + 1};
  }
}

/**
 * defines the binding power for the postfix operators
 */
struct OP_POWER parser_postfix_binding_power(enum TOKEN_TYPE type) {
  TRACE_FN;
  switch (type) {
  case INC:
  case DEC:
    return (struct OP_POWER){POSTFIX, POSTFIX + 1};
  default:
    return (struct OP_POWER){-1, -1};
  }
}

// ========================================================================

/**
 * get the prefix parse function for the given token type
 */
parser_parse_prefix_fn parser_get_prefix_fn(enum TOKEN_TYPE type) {
  TRACE_FN;
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

/**
 * get the infix parse function for the given token type
 */
parser_parse_infix_fn parser_get_infix_fn(enum TOKEN_TYPE type) {
  TRACE_FN;
  return parser_parse_binary_operator;
}

/**
 * get the postfix parse function for the given token type
 */
parser_parse_postfix_fn parser_get_postfix_fn(enum TOKEN_TYPE type) {
  TRACE_FN;
  return parser_parse_postfix_operator;
}

// ========================================================================

/**
 * parse a statement -> let, return, or expression (expressions along are valid
 * statements)
 */
struct statement *parser_parse_statement(struct parser *p) {
  TRACE_FN;
  switch (p->current_token->type) {
  case LET:
    return parser_parse_let_statement(p);
  case RETURN:
    return parser_parse_return_statement(p);
  default:
    return parser_parse_expression_statement(p);
  }
}
/**
 * parse let statements
 * let <identifier> := <expression>;
 */
struct statement *parser_parse_let_statement(struct parser *p) {
  TRACE_FN;
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

    struct expression *expr = parser_parse_expr_bp(p, LOWEST);
    if (!expr) {
      ast_statement_free(stmt);
      return NULL;
    }
    stmt->let_stmt.value = expr;
    parser_next_token(p);
    return stmt;
  }
  return NULL;
}

/**
 * parse return statements
 * return <expression>;
 */
struct statement *parser_parse_return_statement(struct parser *p) {
  TRACE_FN;
  struct statement *stmt = ast_statement_init(STMT_RETURN);
  if (!stmt) {
    return NULL;
  }
  stmt->return_stmt.token = p->current_token;
  parser_next_token(p);
  struct expression *expr = parser_parse_expr_bp(p, LOWEST);
  if (!expr) {
    ast_statement_free(stmt);
    return NULL;
  }
  stmt->return_stmt.value = expr;
  return stmt;
}

/**
 * parse expression statements
 * <expression>; or <expression> (in this case the semicolon is optional)
 */
struct statement *parser_parse_expression_statement(struct parser *p) {
  TRACE_FN;
  struct statement *stmt = ast_statement_init(STMT_EXPRESSION);
  if (stmt == NULL) {
    return NULL;
  }
  stmt->expr_stmt.token = p->current_token;

  struct expression *expr = parser_parse_expr_bp(p, LOWEST);

  if (expr == NULL) {
    ast_statement_free(stmt);
    return NULL;
  }

  if (parser_next_token_is(p, SEMICOLON)) {
    parser_next_token(p);
  }

  return stmt;
}

/**
 * parse the expression with the given binding power
 */
struct expression *parser_parse_expr_bp(struct parser *p,
                                        enum OPERATOR_PRECEDENCE min_bp) {
  TRACE_FN;
  parser_parse_prefix_fn prefix_fn =
      parser_get_prefix_fn(p->current_token->type);
  if (prefix_fn == NULL) {
    parser_add_error(p, "no prefix parse function for %s",
                     token_type_to_str(p->current_token->type));
    return NULL;
  }

  struct expression *lhs = prefix_fn(p);

  for (;;) {
    if (parser_next_token_is(p, SEMICOLON) ||
        parser_next_token_is(p, END_OF_FILE))
      break;

    struct OP_POWER binding_power =
        parser_postfix_binding_power(p->next_token->type);
    if (binding_power.lbp > -1) {
      if (binding_power.lbp < min_bp) {
        break;
      }
      parser_next_token(p);
      parser_parse_postfix_fn postfix_fn =
          parser_get_postfix_fn(p->current_token->type);
      if (postfix_fn == NULL) {
        parser_add_error(p, "no postfix parse function for %s",
                         token_type_to_str(p->current_token->type));
        return NULL;
      }
      lhs = postfix_fn(p, lhs);
    }

    binding_power = parser_infix_binding_power(p->next_token->type);

    if (binding_power.lbp < min_bp) {
      break;
    }

    parser_next_token(p);

    parser_parse_infix_fn infix_fn =
        parser_get_infix_fn(p->current_token->type);
    if (infix_fn == NULL) {
      parser_add_error(p, "no infix parse function for %s",
                       token_type_to_str(p->current_token->type));
      return NULL;
    }
    lhs = infix_fn(p, lhs);
  }
  return lhs;
}

/**
 * parse unary(prefix expressions)
 * unary_operator<expression> -> -<expression> or !<expression>
 */
struct expression *parser_parse_unary_operator(struct parser *p) {
  TRACE_FN;
  struct expression *expr = ast_expression_init(EXPR_PREFIX);
  if (!expr) {
    return NULL;
  }
  expr->prefix_expr.op = p->current_token;
  struct OP_POWER b_pr = parser_prefix_binding_power(p->current_token->type);
  parser_next_token(p);
  expr->prefix_expr.right = parser_parse_expr_bp(p, b_pr.rbp);
  if (!expr->prefix_expr.right) {
    ast_expression_free(expr);
    return NULL;
  }
  return expr;
}

/**
 * parse binary(infix) operator related expressions
 * <expression>binary_operator<expression>
 */
struct expression *parser_parse_binary_operator(struct parser *p,
                                                struct expression *left) {
  TRACE_FN;
  struct expression *expr = ast_expression_init(EXPR_INFIX);
  if (!expr) {
    return NULL;
  }
  expr->infix_expr.left = left;
  expr->infix_expr.op = p->current_token;

  struct OP_POWER precedence =
      parser_infix_binding_power(p->current_token->type);
  parser_next_token(p);
  expr->infix_expr.right = parser_parse_expr_bp(p, precedence.rbp);
  return expr;
}

/**
 * parse postfix expressions
 * <expression>postfix_operator
 */
struct expression *parser_parse_postfix_operator(struct parser *p,
                                                 struct expression *left) {
  TRACE_FN;
  struct expression *expr = ast_expression_init(EXPR_POSTFIX);
  if (!expr) {
    return NULL;
  }
  expr->postfix_expr.left = left;
  expr->postfix_expr.op = p->current_token;
  parser_next_token(p);
  return expr;
}

// ========================================================================

/**
 * parse identifiers -> a, isTrue, etc.
 */
struct expression *parser_parse_identifier(struct parser *p) {
  TRACE_FN;
  struct expression *expr = ast_expression_init(EXPR_IDENTIFIER);
  if (!expr) {
    return NULL;
  }
  expr->identifier_expr.token = p->current_token;
  return expr;
}

/**
 * parse integer literals -> 10, 100, etc.
 */
struct expression *parser_parse_integer_literal(struct parser *p) {
  TRACE_FN;
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

/**
 * parse float-point numbers -> 10.1, 100.1, etc.
 */
struct expression *parser_parse_float_literal(struct parser *p) {
  TRACE_FN;
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

/**
 * parse string literals -> "s", "string", etc.
 */
struct expression *parser_parse_string_literal(struct parser *p) {
  TRACE_FN;
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

/**
 * parse boolean literals -> true, false
 */
struct expression *parser_parse_boolean_literal(struct parser *p) {
  TRACE_FN;
  struct expression *expr = ast_expression_init(EXPR_LITERAL);
  if (!expr) {
    return NULL;
  }
  expr->literal.token = p->current_token;
  expr->literal.literal_type = LITERAL_BOOL;
  expr->literal.value.bool_value = p->current_token->type == TRUE;
  return expr;
}
