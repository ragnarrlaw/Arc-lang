#ifndef AST_H
#define AST_H

#include "token.h"

enum EXPRESSION_TYPE {
  EXPR_LITERAL,  // 5;
  EXPR_VARIABLE, // identifier cases -> a;
  EXPR_BINARY,   // binary operators -> a + b;
  EXPR_UNARY,    // binary operators -> -10;
  EXPR_FN_CALL,  // add(a, b);
  EXPR_IF,       // if statement -> if (condition) { ... }
  EXPR_FOR,      // for loop -> for (init; condition; update) { ... }
  EXPR_WHILE,    // while loop -> while (condition) { ... }
};

struct expression {
  enum EXPRESSION_TYPE type;
  union {
    // literal expressions (e.g. 3, 4.5, "string")
    struct {
      struct token *value;
    } literal_expr;

    // variable expressions (e.g. x, y)
    struct {
      struct token *name;
    } variable_expr;

    // binary expressions (e.g. a + b)
    struct {
      struct expression *left;
      struct token *op;
      struct expression *right;
    } binary_expr;

    // unary operator expressions (e.g. -a)
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
      struct token *name;              // if
      struct expression *condition;    // if condition
      struct expression **consequence; // if block
      size_t consequence_count;        // number of statements in the if block
      struct expression **alternative; // else block
      size_t alternative_count;        // number of statements in the else block
    } if_expr;

    // for expressions (e.g. for (init; condition; update) { ... })
    struct {
      struct token *name;           // for
      struct statement *init;       // init statement
      struct expression *condition; // condition
      struct statement *update;     // update statement
      struct expression **body;     // for block
      size_t body_count;            // number of statements in the for block
    } for_expr;

    // while expressions (e.g. while (condition) { ... })
    struct {
      struct token *name;           // while
      struct expression *condition; // condition
      struct expression **body;     // while block
      size_t body_count;            // number of statements in the while block
    } while_expr;
  };
};

enum STATEMENT_TYPE {
  STMT_LET,
  STMT_RETURN,
  STMT_FUNCTION_DEF,
  STMT_EXPRESSION,
  STMT_IF,
  STMT_FOR,
  STMT_WHILE,
};

struct statement {
  enum STATEMENT_TYPE type;
  union {
    // let statements (e.g. let x := 5)
    struct {
      struct token *name;
      struct token *identifier;
      struct expression *value;
    } let_stmt;

    // return statements (e.g. return 5)
    struct {
      struct expression *value;
    } return_stmt;

    // function definition statements (e.g. fn add(int a, int b) int { return a
    // + b; })
    struct {
      struct token *name;
      struct token **param_types;
      struct token **param_names;
      size_t param_count;
      struct token *return_type;
      struct statement **body;
      size_t body_count; // number of statements in the body
    } fn_def_stmt;

    // expression statements (e.g. 5 + 5;)
    struct {
      struct expression *expr;
    } expr_stmt;

    // if statements (e.g. if (condition) { ... })
    struct {
      struct token *name;             // if
      struct expression *condition;   // if condition
      struct statement **consequence; // if block
      size_t consequence_count;       // number of statements in the if block
      struct statement **alternative; // else block
      size_t alternative_count;       // number of statements in the else block
    } if_stmt;

    // for statements (e.g. for (init; condition; update) { ... })
    struct {
      struct token *name;           // for
      struct statement *init;       // init statement
      struct expression *condition; // condition
      struct statement *update;     // update statement
      struct statement **body;      // for block
      size_t body_count;            // number of statements in the for block
    } for_stmt;

    // while statements (e.g. while (condition) { ... })
    struct {
      struct token *name;           // while
      struct expression *condition; // condition
      struct statement **body;      // while block
      size_t body_count;            // number of statements in the while block
    } while_stmt;
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

#endif // !AST_H
