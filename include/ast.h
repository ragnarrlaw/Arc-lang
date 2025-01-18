#ifndef AST_H
#define AST_H

#include "token.h"

enum LITERAL_TYPE {
  LITERAL_INT,
  LITERAL_FLOAT,
  LITERAL_STRING,
  LITERAL_CHAR,
  LITERAL_ARRAY,
  LITERAL_STRUCT,
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

struct array_literal {
  struct literal *elements;
  size_t count;
};

/**
 NOTE: at the time of initializing a struct literal make sure to
       use the strndup to make duplicates of the field names and
 */
struct struct_literal {
  char *name;
  size_t name_len;
  struct literal **fields;
  char **fieldnames;
  size_t field_count;
};

union literal_value {
  long int_value;
  float float_value;
  char char_value;
  struct string_literal *string_literal;
  struct array_literal *array_literal;
  struct struct_literal *struct_literal;
};

struct literal {
  enum LITERAL_TYPE literal_type;
  struct token *token;
  union literal_value value;
};

enum OPS_PRECEDENCE {
  LOWEST,
  EQUALS,      // ==
  LESSGREATER, // > or <
  SUM,         // +
  PRODUCT,     // *
  PREFIX,      // -X or !X
  CALL,        // myFunction(X)
};

enum EXPRESSION_TYPE {
  EXPR_LITERAL,    // 5; or 5
  EXPR_IDENTIFIER, // identifier cases -> a; or a
  EXPR_UNARY,   /** binary operators -> -10 or !true -> operators !, ++, -- and
                 * - are prefix operators */
  EXPR_BINARY,  // binary operators -> a + b;
  EXPR_FN_CALL, // add(a, b);
  EXPR_IF,      // if statement -> if (condition) { ... }
  EXPR_FOR,     // for loop -> for (init; condition; update) { ... }
  EXPR_WHILE,   // while loop -> while (condition) { ... }
};

struct expression {
  enum EXPRESSION_TYPE type;
  union {
    // literal expressions (e.g. 3, 4.5, "string")
    struct literal literal;

    // variable expressions (e.g. x, y)
    struct {
      struct token *token;
    } identifier_expr;

    // binary expressions (e.g. a + b)
    struct {
      struct token *bin_token;
      struct expression *left;
      struct token *op;
      struct expression *right;
    } binary_expr;

    /** unary operator expressions (e.g. -a and !true) and the prefix operators
     * -> this is the prefix expression */
    struct {
      struct token *op;
      struct expression *right;
    } unary_expr;

    // function call expressions (e.g. add(a, b))
    struct {
      struct token *fn_name;
      struct expression **args;
      size_t arg_count;
    } fn_call_expr;

    // if expressions (e.g. if (condition) { ... })
    struct {
      struct token *token;             // if
      struct expression *condition;    // if condition
      struct expression **consequence; // if block
      size_t consequence_count;        // number of statements in the if block
      struct expression **alternative; // else block
      size_t alternative_count;        // number of statements in the else block
    } if_expr;

    // for expressions (e.g. for (init; condition; update) { ... })
    struct {
      struct token *token;          // for
      struct statement *init;       // init statement
      struct expression *condition; // condition
      struct statement *update;     // update statement
      struct expression **body;     // for block
      size_t body_count;            // number of statements in the for block
    } for_expr;

    // while expressions (e.g. while (condition) { ... })
    struct {
      struct token *token;          // while
      struct expression *condition; // condition
      struct expression **body;     // while block
      size_t body_count;            // number of statements in the while block
    } while_expr;
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
    // let statements (e.g. let x := 5)
    struct {
      struct token *token;
      struct token *identifier;
      struct expression *value;
    } let_stmt;

    // return statements (e.g. return 5)
    struct {
      struct token *token;
      struct expression *value;
    } return_stmt;

    // expression statements (e.g. 5 + 5;)
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
