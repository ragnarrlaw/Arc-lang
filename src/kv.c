#include "kv.h"
#include <stdio.h>

static unsigned long hash(const char *str);
static void resize(hash_table *table);

hash_table *hash_table_init() {
  hash_table *table = malloc(sizeof(hash_table));
  if (!table) {
    ERROR_LOG("Failed to allocate hash table\n");
    return NULL;
  }

  table->capacity = HT_INITIAL_CAPACITY;
  table->size = 0;

  table->buckets = calloc(table->capacity, sizeof(entry *));
  if (!table->buckets) {
    ERROR_LOG("Failed to allocate buckets\n");
    free(table);
    return NULL;
  }

  return table;
}

void hash_table_free(hash_table *table) {
  if (!table)
    return;

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

void hash_table_insert(hash_table *table, const char *key,
                       struct obj_t *value) {
  // resize
  if ((double)table->size / table->capacity >= HT_LOAD_FACTOR) {
    resize(table);
  }

  unsigned long index = hash(key) % table->capacity;
  entry *e = table->buckets[index];

  // check for existing key -> chaining
  while (e) {
    if (strcmp(e->key, key) == 0) {
      e->value = value;
      return;
    }
    e = e->next;
  }

  entry *new_entry = malloc(sizeof(entry));
  new_entry->key = strdup(key);
  new_entry->value = value;
  new_entry->next = table->buckets[index];
  table->buckets[index] = new_entry;
  table->size++;
}

static void resize(hash_table *table) {
  size_t new_capacity = table->capacity * 2;
  entry **new_buckets = calloc(new_capacity, sizeof(entry *));
  if (!new_buckets) {
    ERROR_LOG("Resize failed - out of memory\n");
    return;
  }

  // rehash all entries
  for (size_t i = 0; i < table->capacity; i++) {
    entry *e = table->buckets[i];
    while (e) {
      entry *next = e->next;
      unsigned long new_index = hash(e->key) % new_capacity;

      // insert at head of new bucket
      e->next = new_buckets[new_index];
      new_buckets[new_index] = e;

      e = next;
    }
  }

  free(table->buckets);
  table->buckets = new_buckets;
  table->capacity = new_capacity;
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
  hash_table_iterator it = {
      .table = table, .bucket_index = 0, .current_entry = NULL};
  return it;
}

bool hash_table_next(hash_table_iterator *it, const char **key,
                     struct obj_t **value) {
  while (it->bucket_index < it->table->capacity) {
    if (!it->current_entry) {
      // next bucket
      it->current_entry = it->table->buckets[it->bucket_index];
      it->bucket_index++;
    }

    if (it->current_entry) {
      *key = it->current_entry->key;
      *value = it->current_entry->value;
      it->current_entry = it->current_entry->next;
      return true;
    }
  }
  return false;
}

// djb2 hash function
static unsigned long hash(const char *str) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  }
  return hash;
}
