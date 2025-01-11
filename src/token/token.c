#include "token.h"
#include "../util/repr/repr.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

static const char *token_types[] = {
    "illegal", // ILLEGAL
    "eof",     // END_OF_FILE

    "identifier",     // IDENTIFIER
    "numerical",      // NUMERICAL
    "string_literal", // STRING_LITERAL
    "int",            // INT
    "float",          // FLOAT

    "assignment_op",      // ASSIGN
    "function_return",    // FUNCTION_R
    "equals",             // EQUALS
    "plus",               // PLUS
    "division",           // DIVISION
    "multiplication",     // MULTIPLICATION
    "subtraction",        // SUBTRACTION
    "greater",            // GREATER
    "lesser",             // LESSER
    "less_than_equal",    // LTEQ
    "greater_than_equal", // GTEQ

    "comma",     // COMMA
    "semicolon", // SEMICOLON

    "left_parenthesis",     // LPAREN
    "right_parenthesis",    // RPAREN
    "left_square_bracket",  // LSQRBRAC
    "right_square_bracket", // RSQRBRAC
    "left_brace",           // LBRACE
    "right_brace",          // RBRACE

    "function", // FUNCTION
    "let",      // LET
    "match",    // MATCH
    "case",     // CASE
    "return",   // RETURN

    "single_line_comment", // SINGLE_LINE_COMMENT
    "multiline_comment",   // MILTILINE_COMMENT
};

struct token *token_init(enum TOKEN_TYPE type, const char *literal_start,
                         size_t literal_len, uint line, uint col) {
  struct token *token = (struct token *)malloc(sizeof(struct token));
  if (token == NULL) {
    return NULL;
  }
  token->type = type;
  token->literal = literal_start;
  token->literal_len = literal_len;
  token->line_number = line;
  token->col_number = col;
  return token;
}

void token_free(struct token *t) {
  if (t != NULL) {
    free(t);
  }
  t = NULL;
}

void token_repr(struct token *t) {
  PRINT_FIELD_INT(t, type);
  PRINT_FIELD_CHAR_ARRAY(t, literal, t->literal_len);
  PRINT_FIELD_INT(t, line_number);
  PRINT_FIELD_INT(t, col_number);
}
