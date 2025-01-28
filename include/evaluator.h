#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "ast.h"
#include "object_t.h"

struct obj_t *evaluate_program(struct program *);
struct obj_t *evaluate_statement(struct statement *);
struct obj_t *evaluate_expression(struct expression *);

#endif // !EVALUATOR_H
