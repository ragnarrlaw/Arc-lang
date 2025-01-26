#ifndef AST_H
#define AST_H

#include "token.h"
#include <stdbool.h>
#include <stddef.h>

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

struct expression;
struct string_literal;
union literal_value;
struct literal;
struct condition_expr;
struct block_statement;
struct function_literal;
struct identifier;
struct function_def_stmt;

/**
 *NOTE: at the time of initializing a string literal
 *    duplicate the string literal and make a new copy
 *    as the lexer it not storing the string but pointers.
 */
struct string_literal {
  char *value;
  size_t length;
};

/**
 * literal_value holds the values of a literal
 * literal values can be of type int, float, string, char, or bool
 */
union literal_value {
  long int_value;
  float float_value;
  char char_value;
  bool bool_value;
  struct string_literal *string_literal;
};

/**
 * literals represent the data related to different data
 * types provided by the language
 */
struct literal {
  enum LITERAL_TYPE literal_type;
  struct token *token;
  union literal_value value;
};

/**
 * identifier represents an identifier in the language
 * each identifier can be associated with a type
 */
struct identifier {
  struct token *token;
};

/**
 * conditional statements follow the following format
 * if (<condition>) <consequence> else <alternative>
 */
struct conditional_expr {
  struct token *token;
  struct expression *condition;
  struct block_statement *consequence;
  struct block_statement *alternative;
};

/**
 * statements that appear inside of the block { <statements> }
 */
struct block_statement {
  struct token *token; // LBRACE -> {
  struct statement **statements;
  size_t statement_count;
  size_t statement_capacity;
};

struct function_literal {
  struct token *token; // FUNCTION -> fn
  struct identifier **parameters;
  size_t param_count;
  size_t param_capacity;
  struct block_statement *body;
};

enum EXPRESSION_TYPE {
  EXPR_LITERAL,    // 5; or 5
  EXPR_IDENTIFIER, // identifier cases -> a; or a
  EXPR_PREFIX,  /** binary operators -> -10 or !true -> operators !, ++, -- and
                 * - are prefix operators */
  EXPR_INFIX,   // binary operators -> a + b;
  EXPR_POSTFIX, // unary operators -> ++, --
  EXPR_CONDITIONAL,   // if-else expression
  EXPR_FUNCTION,      // fn -> functions
  EXPR_FUNCTION_CALL, // fn -> function call
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

    struct conditional_expr conditional;

    struct function_literal function;

    struct {
      struct token *token;
// clang-format off
      struct expression *function; // can be  a function literal or an identifier
// clang-format on
      struct expression **arguments;
      size_t arg_count;
      size_t arg_size;
    } function_call;
  };
};

enum STATEMENT_TYPE {
  STMT_LET,
  STMT_RETURN,
  STMT_EXPRESSION,
  STMT_FUNCTION_DEF,
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

    struct {
      struct token *token; // fn token
      struct token *name;  // name token
      struct identifier **params;
      size_t params_count;
      size_t params_capacity;
      struct block_statement *body;
    } fn_def_stmt;
  };
};

struct program {
  struct statement **statements; // statement pointer array
  size_t statement_count;        // number of statements in the program
  size_t statement_capacity;     // capacity of the statements array
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

struct block_statement *ast_block_statement_init();
void ast_block_statements_push_stmt(struct block_statement *,
                                    struct statement *);
void ast_block_statement_free(struct block_statement *);

struct identifier *ast_identifier_init();
void ast_identifier_free(struct identifier *);

#endif // !AST_H
