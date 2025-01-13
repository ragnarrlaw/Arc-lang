#ifndef UTIL_FILE_H
#define UTIL_FILE_H

typedef struct file_info {
  const char *filename;
  long len;
  char *buffer;
} file_info;

file_info *load_file(const char *filename);

#endif // !UTIL_FILE_H
