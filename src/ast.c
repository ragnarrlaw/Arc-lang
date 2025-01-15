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