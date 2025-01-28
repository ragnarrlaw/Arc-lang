#include "parser_test.h"
#include "ast.h"
#include "parser.h"
#include "util_repr.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct test_comp {
  const char *input;
  const char *expected;
  int statement_count;
};

void parser_run_all_tests() {
  let_statement_test();
  return_statement_test();
  expression_statement_test();
  grouped_expression_test();
  conditional_expression_test();
  function_expression_test();
  function_statement_test();
  function_call_expression_test();
  function_test_boolean_expressions();
}

#define LET_STATEMENT_TESTS 10
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

  for (int i = 0; i < LET_STATEMENT_TESTS; i++) {
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

    assert(!parser_has_errors(p));

    assert(program->statement_count == tests[i].statement_count);

    assert(!string_t_cmp(str, (char *)tests[i].expected));

    ast_program_free(program);
    parser_free(p);
    free_string_t(str);
  }
}

#define RETURN_STATEMENT_TESTS 10
void return_statement_test() {
  const struct test_comp tests[] = {
      {"return 5;", "return 5;", 1},
      {"return 5.4;", "return 5.4;", 1},
      {"return  true;", "return true;", 1},
      {"return \"str\";", "return str;", 1},
      {"return 10 + 2;", "return (10+2);", 1},
      {"return 10 + 2 * 40;", "return (10+(2*40));", 1},
      {"return -10 + 2 * 40;", "return ((-10)+(2*40));", 1},
      {"return -10 * 2 * 40;", "return (((-10)*2)*40);", 1},
      {"return --10 * 2 * 40;", "return (((--10)*2)*40);", 1},
      {"return ++10 * 2 * 40;", "return (((++10)*2)*40);", 1},
  };

  for (int i = 0; i < LET_STATEMENT_TESTS; i++) {
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

    assert(!parser_has_errors(p));

    assert(program->statement_count == tests[i].statement_count);

    assert(!string_t_cmp(str, (char *)tests[i].expected));

    ast_program_free(program);
    parser_free(p);
    free_string_t(str);
  }
}

#define EXPRESSIONS_TESTS 30
void expression_statement_test() {
  const struct test_comp tests[] = {
      // with the terminating semi colon
      {"5;", "5;", 1},
      {"5.4;", "5.4;", 1},
      {"true;", "true;", 1},
      {"\"str\";", "str;", 1},
      {"10 + 2;", "(10+2);", 1},
      {"10 + 2 * 40;", "(10+(2*40));", 1},
      {"10 % 2;", "(10%2);", 1},
      {"-10 + 2 * 40;", "((-10)+(2*40));", 1},
      {"-10 * 2 * 40;", "(((-10)*2)*40);", 1},
      {"--10 * 2 * 40;", "(((--10)*2)*40);", 1},
      {"++10 * 2 * 40;", "(((++10)*2)*40);", 1},
      {"5++;", "(5++);", 1},
      {"5--;", "(5--);", 1},

      // without the terminating semicolon
      {"5", "5", 1},
      {"5.4", "5.4", 1},
      {"\"str\";", "str", 1},
      {"10 + 2", "(10+2)", 1},
      {"10 % 2", "(10%2)", 1},

      // boolean expressions without semicolons
      {"true", "true", 1},
      {"false", "false", 1},
      {"true == true", "(true==true)", 1},
      {"false == false", "(false==false)", 1},
      {"false != true", "(false!=true)", 1},
      {"true != false", "(true!=false)", 1},
      {"10 == 2", "(10==2)", 1},
      {"10 == 2", "(10==2)", 1},
      {"1 > 2 == false", "((1>2)==false)", 1},
      {"1 > 2 != true", "((1>2)!=true)", 1},
      {"2 < 5 == true", "((2<5)==true)", 1},
      {"2 < 5 != false", "((2<5)!=false)", 1},
  };

  for (int i = 0; i < EXPRESSIONS_TESTS; i++) {
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

    assert(!parser_has_errors(p));

    assert(program->statement_count == tests[i].statement_count);

    assert(!string_t_cmp(str, (char *)tests[i].expected));

    ast_program_free(program);
    parser_free(p);
    free_string_t(str);
  }
}

#define GROUPED_EXPRESSIONS_TESTS 3

void grouped_expression_test() {
  const struct test_comp tests[] = {
      {"(1 + 3) * 2", "((1+3)*2)", 1},
      {"(((0)))", "0", 1},
      {"(0 + 1) > 2 != true", "(((0+1)>2)!=true)", 1},
  };

  for (int i = 0; i < GROUPED_EXPRESSIONS_TESTS; i++) {
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

    assert(!parser_has_errors(p));

    assert(program->statement_count == tests[i].statement_count);

    assert(!string_t_cmp(str, (char *)tests[i].expected));

    ast_program_free(program);
    parser_free(p);
    free_string_t(str);
  }
}

#define CONDITIONALS_TESTS 5
void conditional_expression_test() {
  const struct test_comp tests[] = {
      {"if 1 < 2 { x }", "if((1<2)){x}", 1},
      {"if 1 < 2 { x } else { y }", "if((1<2)){x} else {y}", 1},
      {"if 1 < 2 { let x := 10; let y := 20; x + y } else { 20 }",
       "if((1<2)){let x := 10;let y := 20;(x+y)} else {20}", 1},

      {"if 1 < 2 { true } else { false }", "if((1<2)){true} else {false}", 1},
      {"if 1 < 2 { true != false } else { false == false }",
       "if((1<2)){(true!=false)} else {(false==false)}", 1},
  };

  for (int i = 0; i < CONDITIONALS_TESTS; i++) {
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

    assert(!parser_has_errors(p));

    assert(program->statement_count == tests[i].statement_count);

    assert(!string_t_cmp(str, (char *)tests[i].expected));

    ast_program_free(program);
    parser_free(p);
    free_string_t(str);
  }
}

#define FUNCTION_EXPRESSION_TESTS 1
void function_expression_test() {
  const struct test_comp tests[] = {
      {"let r := fn (x , y) { x + y; }", "let r := fn(x,y,){(x+y)};", 1},
  };

  for (int i = 0; i < FUNCTION_EXPRESSION_TESTS; i++) {
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

    assert(!parser_has_errors(p));

    assert(program->statement_count == tests[i].statement_count);

    assert(!string_t_cmp(str, (char *)tests[i].expected));

    ast_program_free(program);
    parser_free(p);
    free_string_t(str);
  }
}

#define FUNCTION_STATEMENT_TESTS 3
void function_statement_test() {
  const struct test_comp tests[] = {
      {"fn add(x , y) { x + y; }", "fn add (x,y,) {(x+y)}", 1},
      {"fn add(a, b, c, d, x , y, z) { x - y + 1 + a * b / 7 * c - (c + d); }",
       "fn add (a,b,c,d,x,y,z,) {((((x-y)+1)+(((a*b)/7)*c))-(c+d))}", 1},
      {"fn ret(x , y) { if (x > y) { true } else { false } }",
       "fn ret (x,y,) {if((x>y)){true} else {false}}", 1},
  };

  for (int i = 0; i < FUNCTION_STATEMENT_TESTS; i++) {
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

    assert(!parser_has_errors(p));

    assert(program->statement_count == tests[i].statement_count);

    assert(!string_t_cmp(str, (char *)tests[i].expected));

    ast_program_free(program);
    parser_free(p);
    free_string_t(str);
  }
}

#define FUNCTION_CALL_EXPRESSION_TESTS 2
void function_call_expression_test() {
  const struct test_comp tests[] = {
      {"add(10, 90)", "(add(10,90,))", 1},
      // {"add(10, --90)", "(add(10,(--90),))", 1},
      {"add(10, 90++)", "(add(10,(90++),))", 1},
  };

  for (int i = 0; i < FUNCTION_CALL_EXPRESSION_TESTS; i++) {
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

    assert(!parser_has_errors(p));

    assert(program->statement_count == tests[i].statement_count);

    assert(!string_t_cmp(str, (char *)tests[i].expected));

    ast_program_free(program);
    parser_free(p);
    free_string_t(str);
  }
}

#define BOOLEAN_EXPRESSION_TESTS 2
void function_test_boolean_expressions() {
  const struct test_comp tests[] = {
      {"!true", "", 1},
      {"!!true", "", 1},
  };

  for (int i = 0; i < BOOLEAN_EXPRESSION_TESTS; i++) {
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

    assert(!parser_has_errors(p));

    assert(program->statement_count == tests[i].statement_count);

    assert(!string_t_cmp(str, (char *)tests[i].expected));

    ast_program_free(program);
    parser_free(p);
    free_string_t(str);
  }
}
