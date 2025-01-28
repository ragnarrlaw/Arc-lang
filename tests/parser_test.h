#ifndef PARSER_TEST_H
#define PARSER_TEST_H

#include "parser.h"

void parser_run_all_tests();

void let_statement_test();
void return_statement_test();
void expression_statement_test();
void expressions_test();
void grouped_expression_test();
void conditional_expression_test();
void function_expression_test();
void function_statement_test();
void function_call_expression_test();
void function_test_boolean_expressions();

#endif // !PARSER_TEST_H
