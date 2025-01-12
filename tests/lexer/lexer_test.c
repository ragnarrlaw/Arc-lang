#include "lexer_test.h"
#include "../../src/lexer/lexer.h"
#include "../../src/token/token.h"
#include "../utils/util.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT_TOKEN_TYPE(token, type) assert_token_type(token, type)

void run_all_tests() {
  test_single_line_function();
  test_multi_line_function();
  test_multi_line_function_with_braces();
  test_single_line_let();
  test_multi_line_let();
  test_punctuation();
  test_single_line_comment();
  test_underscore_separated_identifiers();
  test_function_call();
  test_floating_point_number();
  test_if_else_statement();
  test_complex_expression();
  test_program();
  test_operators();
}

void assert_token_type(struct token *t, enum TOKEN_TYPE type) {
  if (t->type != type) {
    printf("expected: %d\n", type);
    token_repr(t);
  }
  ASSERT(t->type == type);
  free(t);
}

static const char *single_line_function = "fn add(x, y) -> x + y;";

static const char *multi_line_function = "fn add(x, y) ->\n"
                                         "  x + y;";
static const char *multi_line_function_with_braces =
    "fn add(int x, int y) -> int {\n"
    "  return x + y;\n"
    "}";

static const char *single_line_let = "let x := 10;";

static const char *multi_line_let = "let x :=\n"
                                    "  10;";

static const char *punctuation = "{}[](),;";

static const char *single_line_comment = "# {}[](),\n";

static const char *underscore_separated_identifiers =
    "let x_y := 10;\n"
    "fn function_one(float x, float y) -> float {\n"
    "  return x + y;\n"
    "}";

static const char *function_call = "add(10, 20);";

static const char *floating_point_number = "10.0;\n"
                                           "let f float := 20.0;";

static const char *program = "fn add(x, y) -> x + y;\n"
                             "let x := 10;\n"
                             "let y := 20;\n"
                             "let z := x + y;";

static const char *if_else_statement = "if x > 10 {\n"
                                       "  return x;\n"
                                       "} else {\n"
                                       "  return 0;\n"
                                       "}";

static const char *complex_expression = "let result := (a + b) * (c - d) / e;";

static const char *operators = ":= -> = + / * - > < <= >= == ! !=";

void test_single_line_function() {
  struct lexer *l =
      lexer_init(single_line_function, strlen(single_line_function));
  struct token *t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, FUNCTION);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, COMMA);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, FUNCTION_R);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, PLUS);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, END_OF_FILE);
  lexer_free(l);
}

void test_multi_line_function() {
  struct lexer *l =
      lexer_init(multi_line_function, strlen(multi_line_function));
  struct token *t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, FUNCTION);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, COMMA);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, FUNCTION_R);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, PLUS);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, END_OF_FILE);
  lexer_free(l);
}

void test_multi_line_function_with_braces() {
  struct lexer *l = lexer_init(multi_line_function_with_braces,
                               strlen(multi_line_function_with_braces));
  struct token *t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, FUNCTION);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, INT);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, COMMA);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, INT);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, FUNCTION_R);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, INT);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LBRACE);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RETURN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, PLUS);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RBRACE);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, END_OF_FILE);
  lexer_free(l);
}

void test_single_line_let() {
  struct lexer *l = lexer_init(single_line_let, strlen(single_line_let));
  struct token *t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LET);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, ASSIGN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, NUMERICAL);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, END_OF_FILE);
  lexer_free(l);
}

void test_multi_line_let() {
  struct lexer *l = lexer_init(multi_line_let, strlen(multi_line_let));
  struct token *t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LET);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, ASSIGN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, NUMERICAL);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, END_OF_FILE);
  lexer_free(l);
}

void test_punctuation() {
  struct lexer *l = lexer_init(punctuation, strlen(punctuation));
  struct token *t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LBRACE);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RBRACE);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LSQRBRAC);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RSQRBRAC);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, COMMA);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, END_OF_FILE);
  lexer_free(l);
}

void test_single_line_comment() {
  struct lexer *l =
      lexer_init(single_line_comment, strlen(single_line_comment));
  struct token *t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SINGLE_LINE_COMMENT);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, END_OF_FILE);
  lexer_free(l);
}

void test_underscore_separated_identifiers() {
  struct lexer *l = lexer_init(underscore_separated_identifiers,
                               strlen(underscore_separated_identifiers));
  struct token *t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LET);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, ASSIGN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, NUMERICAL);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, FUNCTION);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, FLOAT);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, COMMA);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, FLOAT);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, FUNCTION_R);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, FLOAT);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LBRACE);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RETURN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, PLUS);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RBRACE);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, END_OF_FILE);
  lexer_free(l);
}

void test_function_call() {
  struct lexer *l = lexer_init(function_call, strlen(function_call));
  struct token *t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, NUMERICAL);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, COMMA);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, NUMERICAL);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, END_OF_FILE);
  lexer_free(l);
}

void test_floating_point_number() {
  struct lexer *l =
      lexer_init(floating_point_number, strlen(floating_point_number));
  struct token *t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, NUMERICAL);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LET);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, FLOAT);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, ASSIGN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, NUMERICAL);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, END_OF_FILE);
  lexer_free(l);
}

void test_program() {
  struct lexer *l = lexer_init(program, strlen(program));
  struct token *t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, FUNCTION);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, COMMA);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, FUNCTION_R);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, PLUS);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LET);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, ASSIGN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, NUMERICAL);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LET);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, ASSIGN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, NUMERICAL);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LET);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, ASSIGN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, PLUS);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, END_OF_FILE);
  lexer_free(l);
}

void test_if_else_statement() {
  struct lexer *l = lexer_init(if_else_statement, strlen(if_else_statement));
  struct token *t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IF);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, GT);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, NUMERICAL);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LBRACE);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RETURN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RBRACE);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, ELSE);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LBRACE);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RETURN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, NUMERICAL);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RBRACE);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, END_OF_FILE);
  lexer_free(l);
}

void test_complex_expression() {
  struct lexer *l = lexer_init(complex_expression, strlen(complex_expression));
  struct token *t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LET);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, ASSIGN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, PLUS);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, ASTERISK);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, MINUS);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, RPAREN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SLASH);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, IDENTIFIER);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SEMICOLON);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, END_OF_FILE);
  lexer_free(l);
}

void test_operators() {
  struct lexer *l = lexer_init(operators, strlen(operators));
  struct token *t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, ASSIGN);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, FUNCTION_R);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, EQUAL);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, PLUS);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, SLASH);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, ASTERISK);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, MINUS);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, GT);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LT);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, LT_EQ);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, GT_EQ);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, EQ_EQ);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, BANG);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, NOT_EQ);
  t = lexer_next_token(l);
  ASSERT_TOKEN_TYPE(t, END_OF_FILE);
  lexer_free(l);
}