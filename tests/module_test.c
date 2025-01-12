#include "lexer/lexer_test.h"
#include "utils/util.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  printf("Running tests...\n");
#ifdef MODULE_LEXER_TEST
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
#endif
  PRINT_TEST_RESULTS();
  printf("Done.\n");
  return EXIT_SUCCESS;
}
