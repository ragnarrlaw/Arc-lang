#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

#define ERROR_LOG(msg)                                                         \
  fprintf(stderr, ">>> %s file: %s at: %d in: %s\n", msg, __FILE__, __LINE__,  \
          __func__);

#endif // !ERROR_H
