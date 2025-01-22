#include "lexer_test.h"
#include "parser_test.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  printf("Running lexer tests...\n");
  lexer_run_all_tests();
  printf("Done.\n");

  printf("Running parser tests...\n");
  parser_run_all_tests();
  printf("Done.\n");
  return EXIT_SUCCESS;
}
