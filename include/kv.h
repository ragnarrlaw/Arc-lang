
#ifndef KV_H
#define KV_H

// clang-format off

#include "object_t.h"
#include "util_error.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define HT_INITIAL_CAPACITY 16
#define HT_LOAD_FACTOR 0.75

typedef struct entry {
    char *key;
    struct obj_t *value;
    struct entry *next;
} entry;

typedef struct hash_table {
    entry **buckets;
    size_t size;
    size_t capacity;
} hash_table;

typedef struct hash_table_iterator {
    hash_table *table;
    size_t bucket_index;
    entry *current_entry;
} hash_table_iterator;

// hash table interface
hash_table *hash_table_init();
void hash_table_free(hash_table *table);
void hash_table_insert(hash_table *table, const char *key, struct obj_t *value);
struct obj_t *hash_table_get(hash_table *table, const char *key);
bool hash_table_remove(hash_table *table, const char *key);
bool hash_table_has(hash_table *table, const char *key);

// iterator interface
hash_table_iterator hash_table_iterate(hash_table *table);
bool hash_table_next(hash_table_iterator *it, const char **key, struct obj_t **value);


#endif // !KV_H

// clang-format on