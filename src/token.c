#include "token.h"
#include "util_error.h"
#include "util_repr.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static const char *token_types[] = {
    "illegal", // ILLEGAL
    "eof",     // END_OF_FILE

    "identifier",     // IDENTIFIER
    "string_literal", // STRING_LITERAL
    "int",            // INT
    "float",          // FLOAT
    "bool",           // BOOL
    "true",           // TRUE
    "false",          // FALSE
    "char",           // CHAR

    "assign",     // ASSIGN
    "function_r", // FUNCTION_R
    "equal",      // EQUAL
    "plus",       // PLUS
    "slash",      // SLASH
    "asterisk",   // ASTERISK
    "minus",      // MINUS
    "gt",         // GT
    "lt",         // LT
    "lt_eq",      // LT_EQ
    "gt_eq",      // GT_EQ
    "eq_eq",      // EQ_EQ
    "bang",       // BANG
    "not_eq",     // NOT_EQ
    "and",        // AND
    "or",         // OR
    "mod",        // MOD
    "inc",        // INC
    "dec",        // DEC

    "comma",     // COMMA
    "semicolon", // SEMICOLON

    "lparen",   // LPAREN
    "rparen",   // RPAREN
    "lsqrbrac", // LSQRBRAC
    "rsqrbrac", // RSQRBRAC
    "lbrace",   // LBRACE
    "rbrace",   // RBRACE

    "function", // FUNCTION
    "let",      // LET
    "match",    // MATCH
    "case",     // CASE
    "return",   // RETURN
    "if",       // IF
    "else",     // ELSE
    "for",      // FOR
    "while",    // WHILE

    "single_line_comment", // SINGLE_LINE_COMMENT
    "multiline_comment",   // MULTILINE_COMMENT
};

struct token *token_init(enum TOKEN_TYPE type, const char *literal_start,
                         size_t literal_len, uint line, uint col,
                         const char *line_start_pos) {
  struct token *token = (struct token *)malloc(sizeof(struct token));
  if (token == NULL) {
    ERROR_LOG("error while allocating memory");
    return NULL;
  }
  token->type = type;
  token->literal = literal_start;
  token->literal_len = literal_len;
  token->line_number = line;
  token->col_number = col;
  token->line_start_pos = line_start_pos;
  return token;
}

void token_free(struct token *t) {
  if (t != NULL) {
    switch (t->type) {
    case STRING_LITERAL: {
      free((char *)t->literal);
    };
    default:
      free(t);
    };
  }
  t = NULL;
}

void token_repr(struct token *t) {
  printf("Token {:\n");
  PRINT_CHAR_ARRAY(token_types[t->type], strlen(token_types[t->type]));
  PRINT_FIELD_CHAR_ARRAY(t, literal, t->literal_len);
  PRINT_FIELD_INT(t, line_number);
  PRINT_FIELD_INT(t, col_number);
  printf("}\n");
}

const char *token_type_to_str(enum TOKEN_TYPE type) {
  return token_types[type];
}
