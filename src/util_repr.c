#include "util_repr.h"
#include "util_error.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct string_t *init_string_t(size_t capacity) {
  struct string_t *s = malloc(sizeof(struct string_t));
  if (!s) {
    ERROR_LOG("error memory allocation failed for string_t\n");
    return NULL;
  }
  if (capacity > 0) {
    char *mem = malloc(sizeof(char) * capacity);
    if (!mem) {
      ERROR_LOG("error memory allocation failed for string_t - string\n");
      free(s);
      return NULL;
    }
    s->str = mem;
    s->len = 0;
    s->cap = capacity;
  } else {
    s->cap = 0;
    s->len = 0;
    s->str = NULL;
  }
  return s;
}

size_t string_t_cat(struct string_t *dst, char *src) {
  if (dst && src) {
    size_t src_len = strlen(src);
    if (dst->len + src_len >= dst->cap) {
      size_t size = src_len + dst->len + dst->cap * 2;
      char *ptr = realloc(dst->str, size);
      if (ptr) {
        dst->str = ptr;
        char *start = ptr + dst->len;
        memcpy(start, src, src_len);
        dst->len += src_len;
        dst->cap = size;
      } else {
        ERROR_LOG("error while reallocating str data\n")
        return 0;
      }
    } else {
      char *start = dst->str + dst->len;
      memcpy(start, src, src_len);
      dst->len += src_len;
    }
    return src_len;
  }
  return 0;
}

size_t string_t_ncat(struct string_t *dst, char *src, size_t len) {
  if (dst && src) {
    if (len + dst->len >= dst->cap) {
      size_t size = len + dst->len + dst->cap * 2;
      char *ptr = realloc(dst->str, size);
      if (ptr) {
        dst->str = ptr;
        char *start = dst->str + dst->len;
        memcpy(start, src, len);
        dst->len += len;
        dst->cap = size;
      } else {
        ERROR_LOG("error while reallocating str data\n")
        return 0;
      }
    } else {
      char *start = dst->str + dst->len;
      memcpy(start, src, len);
      dst->len += len;
    }
    return len;
  }
  return 0;
}

size_t string_t_cat_char(struct string_t *s, char c) {
  if (s) {
    if (s->len + 1 >= s->cap) {
      size_t size = s->len + s->cap * 2;
      char *ptr = realloc(s->str, size);
      if (ptr) {
        s->str = ptr;
        s->str[s->len] = c;
        s->len++;
        s->cap = size;
      } else {
        ERROR_LOG("error while reallocating str data\n")
        return 0;
      }
    } else {
      s->str[s->len] = c;
      s->len++;
    }
    return 1;
  }
  return 0;
}

int string_t_cmp(struct string_t *lhs, char *rhs) {
  if (lhs && rhs) {
    size_t len = strlen(rhs);
    if (len >= lhs->len) {
      return strncmp(lhs->str, rhs, lhs->len);
    } else {
      return strncmp(lhs->str, rhs, len);
    }
  }
  return -1;
}

int string_t_ncmp(struct string_t *lhs, char *rhs, size_t len) {
  if (lhs && rhs) {
    if (len >= lhs->len) {
      return strncmp(lhs->str, rhs, lhs->len);
    } else {
      return strncmp(lhs->str, rhs, len);
    }
  }
  return -1;
}

void repr_string_t(struct string_t *s) {
  if (s && s->str) {
    printf("\n");
    printf("length: %ld capacity: %ld str: ", s->len, s->cap);
    for (size_t i = 0; i < s->len; i++) {
      printf("%c", s->str[i]);
    }
    printf("\n");
  }
}

size_t len_string_t(struct string_t *s) {
  if (s) {
    return s->len;
  }
  return 0;
}

size_t cap_string_t(struct string_t *s) {
  if (s) {
    return s->cap;
  }
  return 0;
}

void free_string_t(struct string_t *s) {
  if (s) {
    if (s->str) {
      free(s->str);
      s->str = NULL;
    }
    free(s);
  }
  s = NULL;
}
