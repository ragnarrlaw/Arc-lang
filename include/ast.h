#ifndef AST_H
#define AST_H

#include "token.h"
#include <stdbool.h>

enum LITERAL_TYPE {
  LITERAL_INT,
  LITERAL_FLOAT,
  LITERAL_STRING,
  LITERAL_CHAR,
  LITERAL_BOOL,
};

/**
  types that require meta data will have literal struts associated with them
 */

struct literal;

/**
 NOTE: at the time of initializing a string literal
       duplicate the string literal and make a new copy
       as the lexer it not storing the string but pointers.
 */
struct string_literal {
  char *value;
  size_t length;
};

union literal_value {
  long int_value;
  float float_value;
  char char_value;
  bool bool_value;
  struct string_literal *string_literal;
};

struct literal {
  enum LITERAL_TYPE literal_type;
  struct token *token;
  union literal_value value;
};

enum EXPRESSION_TYPE {
  EXPR_LITERAL,    // 5; or 5
  EXPR_IDENTIFIER, // identifier cases -> a; or a
  EXPR_PREFIX,  /** binary operators -> -10 or !true -> operators !, ++, -- and
                 * - are prefix operators */
  EXPR_INFIX,   // binary operators -> a + b;
  EXPR_POSTFIX, // unary operators -> ++, --
};

struct expression {
  enum EXPRESSION_TYPE type;
  union {
    struct literal literal;

    struct {
      struct token *token;
    } identifier_expr;

    struct {
      struct token *op;
      struct expression *right;
    } prefix_expr;

    struct {
      struct expression *left;
      struct token *op;
      struct expression *right;
    } infix_expr;

    struct {
      struct expression *left;
      struct token *op;
    } postfix_expr;
  };
};

enum STATEMENT_TYPE {
  STMT_LET,
  STMT_RETURN,
  STMT_EXPRESSION,
};

struct statement {
  enum STATEMENT_TYPE type;
  union {
    struct {
      struct token *token;
      struct token *identifier;
      struct token *assign;
      struct expression *value;
    } let_stmt;

    struct {
      struct token *token;
      struct expression *value;
    } return_stmt;

    struct {
      struct token *token;
      struct expression *expr;
    } expr_stmt;
  };
};

struct program {
  struct statement **statements; // a program is a bunch of statements
  size_t statement_count;        // number of statements in the program
};

struct program *ast_program_init();
void ast_program_push_statement(struct program *, struct statement *);
void ast_program_free(struct program *);

struct statement *ast_statement_init(enum STATEMENT_TYPE);
void ast_statement_free(struct statement *);

struct expression *ast_expression_init(enum EXPRESSION_TYPE);
void ast_expression_free(struct expression *);

struct literal *ast_literal_init(enum LITERAL_TYPE);
void ast_literal_free(struct literal *literal);

#endif // !AST_H
