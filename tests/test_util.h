#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include "ast.h"
#include "token.h"
#include "util_repr.h"
#include <stdio.h>

static int tests_run = 0;
static int tests_passed = 0;

#define RUN_TEST(test)                                                         \
  do {                                                                         \
    printf("Running %s...\n", #test);                                          \
    tests_run++;                                                               \
    test();                                                                    \
    tests_passed++;                                                            \
    printf("%s passed.\n", #test);                                             \
  } while (0)

#define ASSERT(condition)                                                      \
  do {                                                                         \
    if (!(condition)) {                                                        \
      fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", #condition,  \
              __FILE__, __LINE__);                                             \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

#define PRINT_TEST_RESULTS()                                                   \
  do {                                                                         \
    printf("Tests run: %d\n", tests_run);                                      \
    printf("Tests passed: %d\n", tests_passed);                                \
    printf("Tests failed: %d\n", tests_run - tests_passed);                    \
  } while (0)

void t_stmt_repr(struct statement *, string_t *str);
void t_expr_repr(struct expression *expr, string_t *str);
void test_repr();

#endif // !TEST_UTIL_H
