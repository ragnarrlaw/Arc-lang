
#ifndef KV_H
#define KV_H

#include "object_t.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define HT_INITIAL_CAPACITY 16
#define HT_LOAD_FACTOR 0.75

// clang-format off

// hash table entry (key-value pair)
typedef struct entry {
  char *key;
  struct obj_t *value;
  struct entry *next; // for collisions
} entry;

// hash table structure
typedef struct hash_table {
  entry **buckets; // array of pointers to entries
  size_t size;     // number of entries
  size_t capacity; // number of buckets
} hash_table;

// iterator for GC marking
typedef struct hash_table_iterator {
  hash_table *table;
  size_t bucket_index;
  entry *current_entry;
} hash_table_iterator;

hash_table *hash_table_init();
void hash_table_free(hash_table *table);
void hash_table_insert(hash_table *table, const char *key, struct obj_t *value);
struct obj_t *hash_table_get(hash_table *table, const char *key);
bool hash_table_remove(hash_table *table, const char *key);
bool hash_table_has(hash_table *table, const char *key);

hash_table_iterator hash_table_iterate(hash_table *table);
bool hash_table_next(hash_table_iterator *it, const char **key, struct obj_t **value);

#endif // !KV_H

// clang-format on