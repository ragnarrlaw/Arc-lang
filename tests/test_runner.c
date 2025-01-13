#include "lexer_test.h"
#include "test_util.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  printf("Running tests...\n");
  lexer_run_all_tests();
  PRINT_TEST_RESULTS();
  printf("Done.\n");
  return EXIT_SUCCESS;
}
