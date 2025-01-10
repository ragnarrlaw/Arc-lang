#include "../tests/lexer/lexer_test.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  printf("Running tests...\n");
#ifdef MODULE_LEXER_TEST
  test_single_line_function();
  test_multi_line_function();
  test_multi_line_function_with_braces();
  test_single_line_let();
  test_multi_line_let();
  test_punctuation();
  test_program();
#endif
  printf("Done.\n");
  return EXIT_SUCCESS;
}
