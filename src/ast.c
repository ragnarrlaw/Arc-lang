#include "ast.h"
#include "util_error.h"
#include <stdlib.h>

struct statement *ast_statement_init(enum STATEMENT_TYPE type) {
  struct statement *s = (struct statement *)malloc(sizeof(struct statement));
  s->type = type;
  return s;
}

void ast_statement_free(struct statement *s) {
  if (s != NULL) {
    switch (s->type) { TODO }
    free(s);
  }
  s = NULL;
}

struct expression *ast_expression_init(enum EXPRESSION_TYPE e) {
  struct expression *expr =
      (struct expression *)malloc(sizeof(struct expression));
  if (expr == NULL) {
    ERROR_LOG("error while allocating memory");
    return NULL;
  }
  expr->type = e;
  return expr;
}

void ast_expression_free(struct expression *e) {
  if (e != NULL) {
    switch (e->type) { TODO }
  }
  e = NULL;
}

struct literal *ast_literal_init(enum LITERAL_TYPE type) {
  struct literal *l = (struct literal *)malloc(sizeof(struct literal));
  if (l == NULL) {
    ERROR_LOG("error while allocating memory");
    return NULL;
  }
  l->literal_type = type;
  return l;
}

void free_literal(struct literal *literal) {
  if (!literal)
    return;

  switch (literal->literal_type) {
  case LITERAL_STRING:
    free(literal->value.string_literal->value);
    free(literal->value.string_literal);
    break;
  case LITERAL_ARRAY:
    for (size_t i = 0; i < literal->value.array_literal->count; i++) {
      free_literal(&literal->value.array_literal->elements[i]);
    }
    free(literal->value.array_literal->elements);
    free(literal->value.array_literal);
    break;
  case LITERAL_STRUCT:
    for (size_t i = 0; i < literal->value.struct_literal->field_count; i++) {
      free_literal(literal->value.struct_literal->fields[i]);
      free(literal->value.struct_literal->fieldnames[i]);
    }
    free(literal->value.struct_literal->fields);
    free(literal->value.struct_literal->fieldnames);
    free(literal->value.struct_literal);
    break;
  default:
    break;
  }
  free(literal);
}

struct program *ast_program_init() {
  struct program *p = (struct program *)malloc(sizeof(struct program));
  if (p == NULL) {
    ERROR_LOG("error while allocating memory");
    return NULL;
  }
  p->statements = (struct statement **)malloc(sizeof(struct statement *));
  if (p->statements == NULL) {
    ERROR_LOG("error while allocating memory");
    return NULL;
  }
  p->statement_count = 0;
  return p;
}

void ast_program_push_statement(struct program *p, struct statement *s) {
  p->statements = (struct statement **)realloc(
      p->statements, sizeof(struct statement *) * (p->statement_count + 1));
  if (p->statements == NULL) {
    ERROR_LOG("error while allocating memory");
    return;
  }
  p->statements[p->statement_count] = s;
  p->statement_count++;
}

void ast_program_free(struct program *p) {
  for (size_t i = 0; i < p->statement_count; i++) {
    ast_statement_free(p->statements[i]);
  }
  free(p->statements);
  free(p);
}