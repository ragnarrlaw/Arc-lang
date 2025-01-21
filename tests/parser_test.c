#include "parser_test.h"
#include "ast.h"
#include "parser.h"
#include "test_util.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct test_comp {
  const char input[20];
  const char expected[20];
};

void parser_run_all_tests() {
  let_statement_test();
  // return_statement_test();
  // expression_statement_test();
  // expressions_test();
}

void let_statement_test() {
  const struct test_comp tests[] = {
      // (struct test_comp){"let x := 5;", "letx:=5"},
      // {"let x := 5.4;", "letx:=5.4"},
      // {"let x := \"string\";", "letx:=\"string\""},
      // {"let x := true;", "letx:=true"},
      {"let x := 10 + 2;", "letx:=(10+2)"},
  };
  for (int i = 0; i < 1; i++) {
    struct lexer *l = lexer_init(tests[i].input, strlen(tests[i].input));
    struct parser *p = parser_init(l);
    struct program *program = parser_parse_program(p);
    if (program == NULL) {
      printf("error while parsing program\n");
    }
    char *repr = t_stmt_repr(program->statements[0]);

    assert(program->statement_count == 1);

    if (parser_has_errors(p)) {
      parser_print_errors(p);
    }

    if (strcmp(repr, tests[i].expected) != 0) {
      printf("expected: %s, got: %s\n", tests[i].expected, repr);
    }
    free(repr);

    ast_program_free(program);
    parser_free(p);
  }
}

void return_statement_test() {
  const char *int_literal = "return 5;";
  const char *float_literal = "return 5.4;";
  const char *string_literal = "return \"string\";";
  const char *bool_literal = "return true;";

  const char *return_stmts[] = {int_literal, float_literal, string_literal,
                                bool_literal};

  for (int i = 0; i < 4; i++) {
    struct lexer *l = lexer_init(return_stmts[i], strlen(return_stmts[i]));
    struct parser *p = parser_init(l);
    struct program *program = parser_parse_program(p);
    if (program == NULL) {
      printf("error while parsing program\n");
    }
  }
}

void expression_statement_test() {
  const char *int_literal = "5";
  const char *int_literal_with_semicolon = "5;";
  const char *float_literal = "5.4";
  const char *float_literal_with_semicolon = "5.4;";
  const char *string_literal = "\"string\"";
  const char *string_literal_with_semicolon = "\"string\";";
  const char *bool_literal = "true";
  const char *bool_literal_with_semicolon = "true;";

  const char *expression_stmts[] = {
      int_literal,    int_literal_with_semicolon,
      float_literal,  float_literal_with_semicolon,
      string_literal, string_literal_with_semicolon,
      bool_literal,   bool_literal_with_semicolon};

  for (int i = 0; i < 4; i++) {
    struct lexer *l =
        lexer_init(expression_stmts[i], strlen(expression_stmts[i]));
    struct parser *p = parser_init(l);
    struct program *program = parser_parse_program(p);
    if (program == NULL) {
      printf("error while parsing program\n");
    }
  }
}
