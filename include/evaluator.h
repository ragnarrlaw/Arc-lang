#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "ast.h"
#include "environment.h"
#include "object_t.h"

struct obj_t *evaluate_program(struct environment *env, struct program *);
struct obj_t *evaluate_statement(struct environment *env, struct statement *);
struct obj_t *evaluate_expression(struct environment *env, struct expression *);

#endif // !EVALUATOR_H
