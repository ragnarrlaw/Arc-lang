#include "lexer_test.h"
#include "lexer.h"
#include "test_util.h"
#include "token.h"
#include <assert.h>
#include <string.h>

void lexer_run_all_tests() {
  RUN_TEST(test_single_line_function);
  RUN_TEST(test_multi_line_function);
  RUN_TEST(test_multi_line_function_with_braces);
  RUN_TEST(test_single_line_let);
  RUN_TEST(test_multi_line_let);
  RUN_TEST(test_punctuation);
  RUN_TEST(test_single_line_comment);
  RUN_TEST(test_underscore_separated_identifiers);
  RUN_TEST(test_function_call);
  RUN_TEST(test_floating_point_number);
  RUN_TEST(test_if_else_statement);
  RUN_TEST(test_complex_expression);
  RUN_TEST(test_operators);
  RUN_TEST(test_program);
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
  if (t->type != FUNCTION) {
    printf("expected: %d, got: %d\n", FUNCTION, t->type);
    token_repr(t);
    assert(t->type == FUNCTION);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != LPAREN) {
    printf("expected: %d, got: %d\n", LPAREN, t->type);
    token_repr(t);
    assert(t->type == LPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != COMMA) {
    printf("expected: %d, got: %d\n", COMMA, t->type);
    token_repr(t);
    assert(t->type == COMMA);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != RPAREN) {
    printf("expected: %d, got: %d\n", RPAREN, t->type);
    token_repr(t);
    assert(t->type == RPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != FUNCTION_R) {
    printf("expected: %d, got: %d\n", FUNCTION_R, t->type);
    token_repr(t);
    assert(t->type == FUNCTION_R);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != PLUS) {
    printf("expected: %d, got: %d\n", PLUS, t->type);
    token_repr(t);
    assert(t->type == PLUS);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != END_OF_FILE) {
    printf("expected: %d, got: %d\n", END_OF_FILE, t->type);
    token_repr(t);
    assert(t->type == END_OF_FILE);
  }
  lexer_free(l);
}

void test_multi_line_function() {
  struct lexer *l =
      lexer_init(multi_line_function, strlen(multi_line_function));
  struct token *t = lexer_next_token(l);
  if (t->type != FUNCTION) {
    printf("expected: %d, got: %d\n", FUNCTION, t->type);
    token_repr(t);
    assert(t->type == FUNCTION);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != LPAREN) {
    printf("expected: %d, got: %d\n", LPAREN, t->type);
    token_repr(t);
    assert(t->type == LPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != COMMA) {
    printf("expected: %d, got: %d\n", COMMA, t->type);
    token_repr(t);
    assert(t->type == COMMA);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != RPAREN) {
    printf("expected: %d, got: %d\n", RPAREN, t->type);
    token_repr(t);
    assert(t->type == RPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != FUNCTION_R) {
    printf("expected: %d, got: %d\n", FUNCTION_R, t->type);
    token_repr(t);
    assert(t->type == FUNCTION_R);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != PLUS) {
    printf("expected: %d, got: %d\n", PLUS, t->type);
    token_repr(t);
    assert(t->type == PLUS);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != END_OF_FILE) {
    printf("expected: %d, got: %d\n", END_OF_FILE, t->type);
    token_repr(t);
    assert(t->type == END_OF_FILE);
  }
  lexer_free(l);
}

void test_multi_line_function_with_braces() {
  struct lexer *l = lexer_init(multi_line_function_with_braces,
                               strlen(multi_line_function_with_braces));
  struct token *t = lexer_next_token(l);
  if (t->type != FUNCTION) {
    printf("expected: %d, got: %d\n", FUNCTION, t->type);
    token_repr(t);
    assert(t->type == FUNCTION);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != LPAREN) {
    printf("expected: %d, got: %d\n", LPAREN, t->type);
    token_repr(t);
    assert(t->type == LPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != INT) {
    printf("expected: %d, got: %d\n", INT, t->type);
    token_repr(t);
    assert(t->type == INT);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != COMMA) {
    printf("expected: %d, got: %d\n", COMMA, t->type);
    token_repr(t);
    assert(t->type == COMMA);
  }
  t = lexer_next_token(l);
  if (t->type != INT) {
    printf("expected: %d, got: %d\n", INT, t->type);
    token_repr(t);
    assert(t->type == INT);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != RPAREN) {
    printf("expected: %d, got: %d\n", RPAREN, t->type);
    token_repr(t);
    assert(t->type == RPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != FUNCTION_R) {
    printf("expected: %d, got: %d\n", FUNCTION_R, t->type);
    token_repr(t);
    assert(t->type == FUNCTION_R);
  }
  t = lexer_next_token(l);
  if (t->type != INT) {
    printf("expected: %d, got: %d\n", INT, t->type);
    token_repr(t);
    assert(t->type == INT);
  }
  t = lexer_next_token(l);
  if (t->type != LBRACE) {
    printf("expected: %d, got: %d\n", LBRACE, t->type);
    token_repr(t);
    assert(t->type == LBRACE);
  }
  t = lexer_next_token(l);
  if (t->type != RETURN) {
    printf("expected: %d, got: %d\n", RETURN, t->type);
    token_repr(t);
    assert(t->type == RETURN);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != PLUS) {
    printf("expected: %d, got: %d\n", PLUS, t->type);
    token_repr(t);
    assert(t->type == PLUS);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != RBRACE) {
    printf("expected: %d, got: %d\n", RBRACE, t->type);
    token_repr(t);
    assert(t->type == RBRACE);
  }
  t = lexer_next_token(l);
  if (t->type != END_OF_FILE) {
    printf("expected: %d, got: %d\n", END_OF_FILE, t->type);
    token_repr(t);
    assert(t->type == END_OF_FILE);
  }
  lexer_free(l);
}

void test_single_line_let() {
  struct lexer *l = lexer_init(single_line_let, strlen(single_line_let));
  struct token *t = lexer_next_token(l);
  if (t->type != LET) {
    printf("expected: %d, got: %d\n", LET, t->type);
    token_repr(t);
    assert(t->type == LET);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != ASSIGN) {
    printf("expected: %d, got: %d\n", ASSIGN, t->type);
    token_repr(t);
    assert(t->type == ASSIGN);
  }
  t = lexer_next_token(l);
  if (t->type != INT) {
    printf("expected: %d, got: %d\n", INT, t->type);
    token_repr(t);
    assert(t->type == INT);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != END_OF_FILE) {
    printf("expected: %d, got: %d\n", END_OF_FILE, t->type);
    token_repr(t);
    assert(t->type == END_OF_FILE);
  }
  lexer_free(l);
}

void test_multi_line_let() {
  struct lexer *l = lexer_init(multi_line_let, strlen(multi_line_let));
  struct token *t = lexer_next_token(l);
  if (t->type != LET) {
    printf("expected: %d, got: %d\n", LET, t->type);
    token_repr(t);
    assert(t->type == LET);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != ASSIGN) {
    printf("expected: %d, got: %d\n", ASSIGN, t->type);
    token_repr(t);
    assert(t->type == ASSIGN);
  }
  t = lexer_next_token(l);
  if (t->type != INT) {
    printf("expected: %d, got: %d\n", INT, t->type);
    token_repr(t);
    assert(t->type == INT);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != END_OF_FILE) {
    printf("expected: %d, got: %d\n", END_OF_FILE, t->type);
    token_repr(t);
    assert(t->type == END_OF_FILE);
  }
  lexer_free(l);
}

void test_punctuation() {
  struct lexer *l = lexer_init(punctuation, strlen(punctuation));
  struct token *t = lexer_next_token(l);
  if (t->type != LBRACE) {
    printf("expected: %d, got: %d\n", LBRACE, t->type);
    token_repr(t);
    assert(t->type == LBRACE);
  }
  t = lexer_next_token(l);
  if (t->type != RBRACE) {
    printf("expected: %d, got: %d\n", RBRACE, t->type);
    token_repr(t);
    assert(t->type == RBRACE);
  }
  t = lexer_next_token(l);
  if (t->type != LSQRBRAC) {
    printf("expected: %d, got: %d\n", LSQRBRAC, t->type);
    token_repr(t);
    assert(t->type == LSQRBRAC);
  }
  t = lexer_next_token(l);
  if (t->type != RSQRBRAC) {
    printf("expected: %d, got: %d\n", RSQRBRAC, t->type);
    token_repr(t);
    assert(t->type == RSQRBRAC);
  }
  t = lexer_next_token(l);
  if (t->type != LPAREN) {
    printf("expected: %d, got: %d\n", LPAREN, t->type);
    token_repr(t);
    assert(t->type == LPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != RPAREN) {
    printf("expected: %d, got: %d\n", RPAREN, t->type);
    token_repr(t);
    assert(t->type == RPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != COMMA) {
    printf("expected: %d, got: %d\n", COMMA, t->type);
    token_repr(t);
    assert(t->type == COMMA);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != END_OF_FILE) {
    printf("expected: %d, got: %d\n", END_OF_FILE, t->type);
    token_repr(t);
    assert(t->type == END_OF_FILE);
  }
  lexer_free(l);
}

void test_single_line_comment() {
  struct lexer *l =
      lexer_init(single_line_comment, strlen(single_line_comment));
  struct token *t = lexer_next_token(l);
  if (t->type != SINGLE_LINE_COMMENT) {
    printf("expected: %d, got: %d\n", SINGLE_LINE_COMMENT, t->type);
    token_repr(t);
    assert(t->type == SINGLE_LINE_COMMENT);
  }
  t = lexer_next_token(l);
  if (t->type != END_OF_FILE) {
    printf("expected: %d, got: %d\n", END_OF_FILE, t->type);
    token_repr(t);
    assert(t->type == END_OF_FILE);
  }
  lexer_free(l);
}

void test_underscore_separated_identifiers() {
  struct lexer *l = lexer_init(underscore_separated_identifiers,
                               strlen(underscore_separated_identifiers));
  struct token *t = lexer_next_token(l);
  if (t->type != LET) {
    printf("expected: %d, got: %d\n", LET, t->type);
    token_repr(t);
    assert(t->type == LET);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != ASSIGN) {
    printf("expected: %d, got: %d\n", ASSIGN, t->type);
    token_repr(t);
    assert(t->type == ASSIGN);
  }
  t = lexer_next_token(l);
  if (t->type != INT) {
    printf("expected: %d, got: %d\n", INT, t->type);
    token_repr(t);
    assert(t->type == INT);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != FUNCTION) {
    printf("expected: %d, got: %d\n", FUNCTION, t->type);
    token_repr(t);
    assert(t->type == FUNCTION);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != LPAREN) {
    printf("expected: %d, got: %d\n", LPAREN, t->type);
    token_repr(t);
    assert(t->type == LPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != FLOAT) {
    printf("expected: %d, got: %d\n", FLOAT, t->type);
    token_repr(t);
    assert(t->type == FLOAT);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != COMMA) {
    printf("expected: %d, got: %d\n", COMMA, t->type);
    token_repr(t);
    assert(t->type == COMMA);
  }
  t = lexer_next_token(l);
  if (t->type != FLOAT) {
    printf("expected: %d, got: %d\n", FLOAT, t->type);
    token_repr(t);
    assert(t->type == FLOAT);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != RPAREN) {
    printf("expected: %d, got: %d\n", RPAREN, t->type);
    token_repr(t);
    assert(t->type == RPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != FUNCTION_R) {
    printf("expected: %d, got: %d\n", FUNCTION_R, t->type);
    token_repr(t);
    assert(t->type == FUNCTION_R);
  }
  t = lexer_next_token(l);
  if (t->type != FLOAT) {
    printf("expected: %d, got: %d\n", FLOAT, t->type);
    token_repr(t);
    assert(t->type == FLOAT);
  }
  t = lexer_next_token(l);
  if (t->type != LBRACE) {
    printf("expected: %d, got: %d\n", LBRACE, t->type);
    token_repr(t);
    assert(t->type == LBRACE);
  }
  t = lexer_next_token(l);
  if (t->type != RETURN) {
    printf("expected: %d, got: %d\n", RETURN, t->type);
    token_repr(t);
    assert(t->type == RETURN);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != PLUS) {
    printf("expected: %d, got: %d\n", PLUS, t->type);
    token_repr(t);
    assert(t->type == PLUS);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != RBRACE) {
    printf("expected: %d, got: %d\n", RBRACE, t->type);
    token_repr(t);
    assert(t->type == RBRACE);
  }
  t = lexer_next_token(l);
  if (t->type != END_OF_FILE) {
    printf("expected: %d, got: %d\n", END_OF_FILE, t->type);
    token_repr(t);
    assert(t->type == END_OF_FILE);
  }
  lexer_free(l);
}

void test_function_call() {
  struct lexer *l = lexer_init(function_call, strlen(function_call));
  struct token *t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != LPAREN) {
    printf("expected: %d, got: %d\n", LPAREN, t->type);
    token_repr(t);
    assert(t->type == LPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != INT) {
    printf("expected: %d, got: %d\n", INT, t->type);
    token_repr(t);
    assert(t->type == INT);
  }
  t = lexer_next_token(l);
  if (t->type != COMMA) {
    printf("expected: %d, got: %d\n", COMMA, t->type);
    token_repr(t);
    assert(t->type == COMMA);
  }
  t = lexer_next_token(l);
  if (t->type != INT) {
    printf("expected: %d, got: %d\n", INT, t->type);
    token_repr(t);
    assert(t->type == INT);
  }
  t = lexer_next_token(l);
  if (t->type != RPAREN) {
    printf("expected: %d, got: %d\n", RPAREN, t->type);
    token_repr(t);
    assert(t->type == RPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != END_OF_FILE) {
    printf("expected: %d, got: %d\n", END_OF_FILE, t->type);
    token_repr(t);
    assert(t->type == END_OF_FILE);
  }
  lexer_free(l);
}

void test_floating_point_number() {
  struct lexer *l =
      lexer_init(floating_point_number, strlen(floating_point_number));
  struct token *t = lexer_next_token(l);
  if (t->type != FLOAT) {
    printf("expected: %d, got: %d\n", FLOAT, t->type);
    token_repr(t);
    assert(t->type == FLOAT);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != LET) {
    printf("expected: %d, got: %d\n", LET, t->type);
    token_repr(t);
    assert(t->type == LET);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != FLOAT) {
    printf("expected: %d, got: %d\n", FLOAT, t->type);
    token_repr(t);
    assert(t->type == FLOAT);
  }
  t = lexer_next_token(l);
  if (t->type != ASSIGN) {
    printf("expected: %d, got: %d\n", ASSIGN, t->type);
    token_repr(t);
    assert(t->type == ASSIGN);
  }
  t = lexer_next_token(l);
  if (t->type != FLOAT) {
    printf("expected: %d, got: %d\n", FLOAT, t->type);
    token_repr(t);
    assert(t->type == FLOAT);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != END_OF_FILE) {
    printf("expected: %d, got: %d\n", END_OF_FILE, t->type);
    token_repr(t);
    assert(t->type == END_OF_FILE);
  }
  lexer_free(l);
}

void test_program() {
  struct lexer *l = lexer_init(program, strlen(program));
  struct token *t = lexer_next_token(l);
  if (t->type != FUNCTION) {
    printf("expected: %d, got: %d\n", FUNCTION, t->type);
    token_repr(t);
    assert(t->type == FUNCTION);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != LPAREN) {
    printf("expected: %d, got: %d\n", LPAREN, t->type);
    token_repr(t);
    assert(t->type == LPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != COMMA) {
    printf("expected: %d, got: %d\n", COMMA, t->type);
    token_repr(t);
    assert(t->type == COMMA);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != RPAREN) {
    printf("expected: %d, got: %d\n", RPAREN, t->type);
    token_repr(t);
    assert(t->type == RPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != FUNCTION_R) {
    printf("expected: %d, got: %d\n", FUNCTION_R, t->type);
    token_repr(t);
    assert(t->type == FUNCTION_R);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != PLUS) {
    printf("expected: %d, got: %d\n", PLUS, t->type);
    token_repr(t);
    assert(t->type == PLUS);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != LET) {
    printf("expected: %d, got: %d\n", LET, t->type);
    token_repr(t);
    assert(t->type == LET);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != ASSIGN) {
    printf("expected: %d, got: %d\n", ASSIGN, t->type);
    token_repr(t);
    assert(t->type == ASSIGN);
  }
  t = lexer_next_token(l);
  if (t->type != INT) {
    printf("expected: %d, got: %d\n", INT, t->type);
    token_repr(t);
    assert(t->type == INT);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != LET) {
    printf("expected: %d, got: %d\n", LET, t->type);
    token_repr(t);
    assert(t->type == LET);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != ASSIGN) {
    printf("expected: %d, got: %d\n", ASSIGN, t->type);
    token_repr(t);
    assert(t->type == ASSIGN);
  }
  t = lexer_next_token(l);
  if (t->type != INT) {
    printf("expected: %d, got: %d\n", INT, t->type);
    token_repr(t);
    assert(t->type == INT);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != LET) {
    printf("expected: %d, got: %d\n", LET, t->type);
    token_repr(t);
    assert(t->type == LET);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != ASSIGN) {
    printf("expected: %d, got: %d\n", ASSIGN, t->type);
    token_repr(t);
    assert(t->type == ASSIGN);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != PLUS) {
    printf("expected: %d, got: %d\n", PLUS, t->type);
    token_repr(t);
    assert(t->type == PLUS);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != END_OF_FILE) {
    printf("expected: %d, got: %d\n", END_OF_FILE, t->type);
    token_repr(t);
    assert(t->type == END_OF_FILE);
  }
  lexer_free(l);
}

void test_if_else_statement() {
  struct lexer *l = lexer_init(if_else_statement, strlen(if_else_statement));
  struct token *t = lexer_next_token(l);
  if (t->type != IF) {
    printf("expected: %d, got: %d\n", IF, t->type);
    token_repr(t);
    assert(t->type == IF);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != GT) {
    printf("expected: %d, got: %d\n", GT, t->type);
    token_repr(t);
    assert(t->type == GT);
  }
  t = lexer_next_token(l);
  if (t->type != INT) {
    printf("expected: %d, got: %d\n", INT, t->type);
    token_repr(t);
    assert(t->type == INT);
  }
  t = lexer_next_token(l);
  if (t->type != LBRACE) {
    printf("expected: %d, got: %d\n", LBRACE, t->type);
    token_repr(t);
    assert(t->type == LBRACE);
  }
  t = lexer_next_token(l);
  if (t->type != RETURN) {
    printf("expected: %d, got: %d\n", RETURN, t->type);
    token_repr(t);
    assert(t->type == RETURN);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != RBRACE) {
    printf("expected: %d, got: %d\n", RBRACE, t->type);
    token_repr(t);
    assert(t->type == RBRACE);
  }
  t = lexer_next_token(l);
  if (t->type != ELSE) {
    printf("expected: %d, got: %d\n", ELSE, t->type);
    token_repr(t);
    assert(t->type == ELSE);
  }
  t = lexer_next_token(l);
  if (t->type != LBRACE) {
    printf("expected: %d, got: %d\n", LBRACE, t->type);
    token_repr(t);
    assert(t->type == LBRACE);
  }
  t = lexer_next_token(l);
  if (t->type != RETURN) {
    printf("expected: %d, got: %d\n", RETURN, t->type);
    token_repr(t);
    assert(t->type == RETURN);
  }
  t = lexer_next_token(l);
  if (t->type != INT) {
    printf("expected: %d, got: %d\n", INT, t->type);
    token_repr(t);
    assert(t->type == INT);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != RBRACE) {
    printf("expected: %d, got: %d\n", RBRACE, t->type);
    token_repr(t);
    assert(t->type == RBRACE);
  }
  t = lexer_next_token(l);
  if (t->type != END_OF_FILE) {
    printf("expected: %d, got: %d\n", END_OF_FILE, t->type);
    token_repr(t);
    assert(t->type == END_OF_FILE);
  }
  lexer_free(l);
}

void test_complex_expression() {
  struct lexer *l = lexer_init(complex_expression, strlen(complex_expression));
  struct token *t = lexer_next_token(l);
  if (t->type != LET) {
    printf("expected: %d, got: %d\n", LET, t->type);
    token_repr(t);
    assert(t->type == LET);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != ASSIGN) {
    printf("expected: %d, got: %d\n", ASSIGN, t->type);
    token_repr(t);
    assert(t->type == ASSIGN);
  }
  t = lexer_next_token(l);
  if (t->type != LPAREN) {
    printf("expected: %d, got: %d\n", LPAREN, t->type);
    token_repr(t);
    assert(t->type == LPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != PLUS) {
    printf("expected: %d, got: %d\n", PLUS, t->type);
    token_repr(t);
    assert(t->type == PLUS);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != RPAREN) {
    printf("expected: %d, got: %d\n", RPAREN, t->type);
    token_repr(t);
    assert(t->type == RPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != ASTERISK) {
    printf("expected: %d, got: %d\n", ASTERISK, t->type);
    token_repr(t);
    assert(t->type == ASTERISK);
  }
  t = lexer_next_token(l);
  if (t->type != LPAREN) {
    printf("expected: %d, got: %d\n", LPAREN, t->type);
    token_repr(t);
    assert(t->type == LPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != MINUS) {
    printf("expected: %d, got: %d\n", MINUS, t->type);
    token_repr(t);
    assert(t->type == MINUS);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != RPAREN) {
    printf("expected: %d, got: %d\n", RPAREN, t->type);
    token_repr(t);
    assert(t->type == RPAREN);
  }
  t = lexer_next_token(l);
  if (t->type != SLASH) {
    printf("expected: %d, got: %d\n", SLASH, t->type);
    token_repr(t);
    assert(t->type == SLASH);
  }
  t = lexer_next_token(l);
  if (t->type != IDENTIFIER) {
    printf("expected: %d, got: %d\n", IDENTIFIER, t->type);
    token_repr(t);
    assert(t->type == IDENTIFIER);
  }
  t = lexer_next_token(l);
  if (t->type != SEMICOLON) {
    printf("expected: %d, got: %d\n", SEMICOLON, t->type);
    token_repr(t);
    assert(t->type == SEMICOLON);
  }
  t = lexer_next_token(l);
  if (t->type != END_OF_FILE) {
    printf("expected: %d, got: %d\n", END_OF_FILE, t->type);
    token_repr(t);
    assert(t->type == END_OF_FILE);
  }
  lexer_free(l);
}

void test_operators() {
  struct lexer *l = lexer_init(operators, strlen(operators));
  struct token *t = lexer_next_token(l);
  if (t->type != ASSIGN) {
    printf("expected: %d, got: %d\n", ASSIGN, t->type);
    token_repr(t);
    assert(t->type == ASSIGN);
  }
  t = lexer_next_token(l);
  if (t->type != FUNCTION_R) {
    printf("expected: %d, got: %d\n", FUNCTION_R, t->type);
    token_repr(t);
    assert(t->type == FUNCTION_R);
  }
  t = lexer_next_token(l);
  if (t->type != EQUAL) {
    printf("expected: %d, got: %d\n", EQUAL, t->type);
    token_repr(t);
    assert(t->type == EQUAL);
  }
  t = lexer_next_token(l);
  if (t->type != PLUS) {
    printf("expected: %d, got: %d\n", PLUS, t->type);
    token_repr(t);
    assert(t->type == PLUS);
  }
  t = lexer_next_token(l);
  if (t->type != SLASH) {
    printf("expected: %d, got: %d\n", SLASH, t->type);
    token_repr(t);
    assert(t->type == SLASH);
  }
  t = lexer_next_token(l);
  if (t->type != ASTERISK) {
    printf("expected: %d, got: %d\n", ASTERISK, t->type);
    token_repr(t);
    assert(t->type == ASTERISK);
  }
  t = lexer_next_token(l);
  if (t->type != MINUS) {
    printf("expected: %d, got: %d\n", MINUS, t->type);
    token_repr(t);
    assert(t->type == MINUS);
  }
  t = lexer_next_token(l);
  if (t->type != GT) {
    printf("expected: %d, got: %d\n", GT, t->type);
    token_repr(t);
    assert(t->type == GT);
  }
  t = lexer_next_token(l);
  if (t->type != LT) {
    printf("expected: %d, got: %d\n", LT, t->type);
    token_repr(t);
    assert(t->type == LT);
  }
  t = lexer_next_token(l);
  if (t->type != LT_EQ) {
    printf("expected: %d, got: %d\n", LT_EQ, t->type);
    token_repr(t);
    assert(t->type == LT_EQ);
  }
  t = lexer_next_token(l);
  if (t->type != GT_EQ) {
    printf("expected: %d, got: %d\n", GT_EQ, t->type);
    token_repr(t);
    assert(t->type == GT_EQ);
  }
  t = lexer_next_token(l);
  if (t->type != EQ_EQ) {
    printf("expected: %d, got: %d\n", EQ_EQ, t->type);
    token_repr(t);
    assert(t->type == EQ_EQ);
  }
  t = lexer_next_token(l);
  if (t->type != BANG) {
    printf("expected: %d, got: %d\n", BANG, t->type);
    token_repr(t);
    assert(t->type == BANG);
  }
  t = lexer_next_token(l);
  if (t->type != NOT_EQ) {
    printf("expected: %d, got: %d\n", NOT_EQ, t->type);
    token_repr(t);
    assert(t->type == NOT_EQ);
  }
  t = lexer_next_token(l);
  if (t->type != END_OF_FILE) {
    printf("expected: %d, got: %d\n", END_OF_FILE, t->type);
    token_repr(t);
    assert(t->type == END_OF_FILE);
  }
  lexer_free(l);
}