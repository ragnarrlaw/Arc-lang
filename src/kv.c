#include "kv.h"
#include "util_error.h"
#include <stdlib.h>
#include <string.h>

static unsigned long hash(const char *str);

hash_table *hash_table_init() {
  hash_table *table = malloc(sizeof(hash_table));
  if (!table) {
    ERROR_LOG("error while allocating memory\n");
    return NULL;
  }

  table->buckets = calloc(table->capacity, sizeof(entry *));
  if (!table->buckets) {
    ERROR_LOG("error while allocating memory\n");
    free(table);
    return NULL;
  }

  table->capacity = HT_INITIAL_CAPACITY;
  table->size = 0;
  return table;
}

void hash_table_free(hash_table *table) {
  if (table) {
    for (size_t i = 0; i < table->capacity; i++) {
      entry *e = table->buckets[i];
      while (e) {
        entry *next = e->next;
        free(e->key);
        free(e);
        e = next;
      }
    }
    free(table->buckets);
    free(table);
  }
  table = NULL;
}

void hash_table_insert(hash_table *table, const char *key,
                       struct obj_t *value) {
  if ((double)table->size / table->capacity > HT_LOAD_FACTOR) {
    size_t new_capacity = table->capacity * 2;
    struct entry **new_buckets = realloc(table->buckets, new_capacity);
    if (!new_buckets)
      return;
    table->buckets = new_buckets;
    table->capacity = new_capacity;
  }

  unsigned long index = hash(key) % table->capacity;
  entry *e = table->buckets[index];

  // check if key already exists
  while (e) {
    if (strcmp(e->key, key) == 0) {
      e->value = value; // update value
      return;
    }
    e = e->next;
  }

  // create new entry
  entry *new_entry = malloc(sizeof(entry));
  new_entry->key = strdup(key);
  new_entry->value = value;
  new_entry->next = table->buckets[index];
  table->buckets[index] = new_entry;
  table->size++;
}

struct obj_t *hash_table_get(hash_table *table, const char *key) {
  unsigned long index = hash(key) % table->capacity;
  entry *e = table->buckets[index];
  while (e) {
    if (strcmp(e->key, key) == 0) {
      return e->value;
    }
    e = e->next;
  }
  return NULL;
}

bool hash_table_remove(hash_table *table, const char *key) {
  unsigned long index = hash(key) % table->capacity;
  entry *e = table->buckets[index];
  entry *prev = NULL;

  while (e) {
    if (strcmp(e->key, key) == 0) {
      if (prev) {
        prev->next = e->next;
      } else {
        table->buckets[index] = e->next;
      }
      free(e->key);
      free(e);
      table->size--;
      return true;
    }
    prev = e;
    e = e->next;
  }
  return false;
}

bool hash_table_has(hash_table *table, const char *key) {
  return hash_table_get(table, key) != NULL;
}

// iterator
hash_table_iterator hash_table_iterate(hash_table *table) {
  hash_table_iterator it;
  it.table = table;
  it.bucket_index = 0;
  it.current_entry = NULL;
  return it;
}

bool hash_table_next(hash_table_iterator *it, const char **key,
                     struct obj_t **value) {
  hash_table *table = it->table;
  while (it->bucket_index < table->capacity) {
    if (it->current_entry) {
      it->current_entry = it->current_entry->next;
    } else {
      it->current_entry = table->buckets[it->bucket_index];
      it->bucket_index++;
    }

    if (it->current_entry) {
      *key = it->current_entry->key;
      *value = it->current_entry->value;
      return true;
    }
  }
  return false;
}

/**
 * djb2 hash function for string hashing
 */
static unsigned long hash(const char *str) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  }
  return hash;
}
