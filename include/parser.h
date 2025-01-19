#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include "token.h"
#include <stdbool.h>
#include <sys/types.h>

#define PARSER_ERR_MSG_LEN 1024

struct parser;
struct parser_error;
struct operator_precedence;

enum OPERATOR_PRECEDENCE {
  LOWEST,         // Lowest precedence (e.g., commas)
  ASSIGNMENT,     // :=, +=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=
  CONDITIONAL,    // ?: (ternary conditional)
  LOGICAL_OR,     // ||
  LOGICAL_AND,    // &&
  BITWISE_OR,     // |
  BITWISE_XOR,    // ^
  BITWISE_AND,    // &
  EQUALITY,       // ==, !=
  RELATIONAL,     // <, <=, >, >=
  SHIFT,          // <<, >>
  ADDITIVE,       // +, -
  MULTIPLICATIVE, // *, /, %
  PREFIX,  // -, !, ~, ++, --, sizeof, & (address-of), * (dereference), cast
  POSTFIX, // ++, --, () (function call), [] (array access), . (member access),
           // -> (pointer member access)
  HIGHEST  // Highest precedence (e.g., primary expressions like literals and
           // parentheses)
};

// prefix operator
typedef struct expression *(*parser_parse_prefix_fn)(struct parser *p);

// infix operator
typedef struct expression *(*parser_parse_infix_fn)(struct parser *p,
                                                    struct expression *left);

// postfix operator
typedef struct expression *(*parser_parse_postfix_fn)(struct parser *p,
                                                      struct expression *left);

struct token_precedence {
  enum OPERATOR_PRECEDENCE lbp; // left binding power
  enum OPERATOR_PRECEDENCE rbp; // right binding power
};

struct parser {
  struct lexer *lexer;
  struct token *current_token; // current token
  struct token *next_token;    // next token
  struct parser_error *errors; // parser errors -> linked list
};

struct parser_error {
  char *message;
  int line;
  int column;
  struct parser_error *next;
};

struct parser *parser_init(struct lexer *);
void parser_free(struct parser *);

struct program *parser_parse_program(struct parser *);

void parser_add_error(struct parser *p, const char *format, ...);
bool parser_has_errors(struct parser *p);
void parser_print_errors(struct parser *p);

#endif // !PARSER_H
