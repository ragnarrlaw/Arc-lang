#ifndef LEXER_TEST_H
#define LEXER_TEST_H

#include "../../src/lexer/lexer.h"

void assert_token_type(struct token *, enum TOKEN_TYPE);
void test_single_line_function();
void test_multi_line_function();
void test_multi_line_function_with_braces();
void test_single_line_let();
void test_multi_line_let();
void test_punctuation();
void test_program();

#endif // !LEXER_TEST_H