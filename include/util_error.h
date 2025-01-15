#ifndef UTIL_ERROR_H
#define UTIL_ERROR_H

#include <stdio.h>

#define ERROR_LOG(msg)                                                         \
  fprintf(stderr, ">>> %s file: %s at: %d in: %s\n", msg, __FILE__, __LINE__,  \
          __func__);

#define TODO                                                                   \
  ERROR_LOG("TODO: implement this");                                           \
  exit(1);

#endif // !UTIL_ERROR_H
