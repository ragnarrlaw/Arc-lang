#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>
#include <sys/types.h>

enum TOKEN_TYPE {

  ILLEGAL,     // for invalid tokens
  END_OF_FILE, // end of file

  IDENTIFIER,     // add, foobar
  NUMERICAL,      // 12345 or 123.45
  STRING_LITERAL, // "foobar"
  INT,            // int
  FLOAT,          // float

  ASSIGN,     // :=
  FUNCTION_R, // ->
  EQUAL,      // =
  PLUS,       // +
  SLASH,      // /
  ASTERISK,   // *
  MINUS,      // -
  GT,         // >
  LT,         // <
  LT_EQ,      // <=
  GT_EQ,      // >=
  EQ_EQ,      // ==
  BANG,       // !
  NOT_EQ,     // !=

  COMMA,     // ,
  SEMICOLON, // ;

  LPAREN,   // (
  RPAREN,   // )
  LSQRBRAC, // [
  RSQRBRAC, // ]
  LBRACE,   // {
  RBRACE,   // }

  FUNCTION, // fn
  LET,      // let
  MATCH,    // match
  CASE,     // case - match arm
  RETURN,   // return
  IF,       // if
  ELSE,     // else

  SINGLE_LINE_COMMENT, // #
  MULTILINE_COMMENT,   // ## -> currently only supports single line comments
};

struct token {
  enum TOKEN_TYPE type;
  const char *literal;
  size_t literal_len;
  uint line_number;
  uint col_number;
};

struct token *token_init(enum TOKEN_TYPE type, const char *literal_start,
                         size_t literal_len, uint line, uint col);
void token_repr(struct token *t);
void token_free(struct token *t);

#endif // !TOKEN_H
