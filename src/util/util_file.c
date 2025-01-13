#include "util_file.h"
#include "util_error.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_file(file_info *finfo) {
  FILE *file = fopen(finfo->filename, "r");
  if (!file) {
    ERROR_LOG("error while allocating memory");
    return -1;
  }

  fseek(file, 0, SEEK_END);
  finfo->len = ftell(file);
  fseek(file, 0, SEEK_SET);

  finfo->buffer = malloc(finfo->len + 1);
  if (!finfo->buffer) {
    ERROR_LOG("error while allocating memory");
    fclose(file);
    return -1;
  }

  fread(finfo->buffer, sizeof(char), finfo->len, file);
  finfo->buffer[finfo->len] = '\0';
  fclose(file);

  return 0;
}

file_info *load_file(const char *filename) {
  assert(filename != NULL);
  file_info *finfo = (file_info *)malloc(sizeof(file_info));
  if (!finfo) {
    ERROR_LOG("error while allocating memory");
    return NULL;
  }

  finfo->filename = filename;

  int status = read_file(finfo);
  if (!status) {
    return NULL;
  }

  return finfo;
}
