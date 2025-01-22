#include "parser_test.h"
#include "ast.h"
#include "parser.h"
#include "test_util.h"
#include "util_repr.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct test_comp {
  const char *input;
  const char *expected;
  int statement_count;
};

void parser_run_all_tests() {
  let_statement_test();
  // return_statement_test();
  // expression_statement_test();
  // expressions_test();
}

void let_statement_test() {
  const struct test_comp tests[] = {
      {"let x := 5;", "let x := 5;", 1},
      {"let x := 5.4;", "let x := 5.4;", 1},
      {"let x := true;", "let x := true;", 1},
      {"let x := \"str\";", "let x := str;", 1},
      {"let x := 10 + 2;", "let x := (10+2);", 1},
      {"let x := 10 + 2 * 40;", "let x := (10+(2*40));", 1},
      {"let x := -10 + 2 * 40;", "let x := ((-10)+(2*40));", 1},
      {"let x := -10 * 2 * 40;", "let x := (((-10)*2)*40);", 1},
      {"let x := --10 * 2 * 40;", "let x := (((--10)*2)*40);", 1},
      {"let x := ++10 * 2 * 40;", "let x := (((++10)*2)*40);", 1},
  };

  for (int i = 0; i < 9; i++) {
    printf("Running test #%d: %s\n", i, tests[i].input);

    struct lexer *l = lexer_init(tests[i].input, strlen(tests[i].input));
    if (!l) {
      printf("Error initializing lexer\n");
      continue;
    }

    struct parser *p = parser_init(l);
    if (!p) {
      printf("Error initializing parser\n");
      lexer_free(l);
      continue;
    }

    struct program *program = parser_parse_program(p);
    if (!program) {
      printf("Error parsing program\n");
      parser_free(p);
      continue;
    }

    if (parser_has_errors(p)) {
      parser_print_errors(p);
    }

    string_t *str = init_string_t(8);
    t_stmt_repr(program->statements[0], str);
    repr_string_t(str);

    assert(program->statement_count == tests[i].statement_count);

    assert(!string_t_cmp(str, (char *)tests[i].expected));

    ast_program_free(program);
    parser_free(p);
    free(str);
  }
}

void return_statement_test() {}

void expression_statement_test() {}
