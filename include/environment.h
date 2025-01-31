#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

/**
 * environment tracks active variables/objects
 */

#include "kv.h"
#include <stddef.h>

#define MAX_ROOTS 1024

typedef struct environment environment;

struct environment {
  struct environment *parent; // for global environment, set this to NULL
  struct hash_table *symbols; // k-v store for storing variables and data
};

/**
 * initialize an environment
 */
environment *env_init();

/**
 * define a variable in the environment
 */
void env_define(environment *env, const char *name, void *value);

/**
 * search the environment and it's parents for a key, if found
 * the value in the key is overriden, else a new key entry is
 * added to the current environment
 */
void env_set(environment *env, const char *name, void *value);

/**
 * look up a variable name or a value in the current environment
 * if not found in the current environment, go up the parent heirarchy
 * looking for the key, if not found returns NULL.
 */
struct obj_t *env_look_up(environment *env, char *key);

/**
 * free the environment, release the resources
 */
void env_free(environment *env);

#endif // !ENVIRONMENT_H