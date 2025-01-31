#include "environment.h"
#include "kv.h"
#include "util_error.h"
#include <stdio.h>
#include <stdlib.h>

environment *env_init() {
  environment *env = malloc(sizeof(environment));
  if (!env) {
    return NULL;
  }
  struct hash_table *table = hash_table_init();
  if (!table) {
    ERROR_LOG("error while allocating memory\n");
    free(env);
    return NULL;
  }
  env->parent = NULL;
  env->symbols = table;
  return env;
}

void env_free(environment *env) {
  if (env) {
    hash_table_free(env->symbols);
    free(env);
  }
  env = NULL;
}

void env_define(environment *env, const char *name, void *value) {
  hash_table_insert(env->symbols, name, value);
}

void env_set(environment *env, const char *name, void *value) {
  environment *current = env;
  while (current) {
    if (hash_table_has(current->symbols, name)) {
      hash_table_insert(current->symbols, name, value);
      return;
    }
    current = current->parent;
  }
  // if nothing is found then define the key in the current env
  env_define(env, name, value);
}

struct obj_t *env_look_up(environment *env, char *key) {
  struct obj_t *value = hash_table_get(env->symbols, key);
  if (!value && env->parent) {
    return env_look_up(env->parent, key);
  }
  return value;
}
