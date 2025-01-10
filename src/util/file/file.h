#ifndef FILE_H
#define FILE_H

typedef struct file_info {
  const char *filename;
  long len;
  char *buffer;
} file_info;

file_info *load_file(const char *filename);

#endif // !FILE_H
