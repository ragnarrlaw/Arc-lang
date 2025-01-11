#ifndef REPR_H
#define REPR_H

#define PRINT_FIELD_INT(structure, field) printf(#field ": %d\n", structure->field)
#define PRINT_FIELD_STR(structure, field) printf(#field ": %s\n", structure->field)
#define PRINT_FIELD_CHAR_ARRAY(structure, field, len) \
  do { \
    printf(#field ": "); \
    for (size_t i = 0; i < len; i++) { \
      printf("%c", structure->field[i]); \
    } \
    printf("\n"); \
  } while (0)

#define PRINT_CHAR_ARRAY(arr, len) \
  do { \
    for (size_t i = 0; i < len; i++) { \
      printf("%c", arr[i]); \
    } \
    printf("\n"); \
  } while (0)

#endif // !REPR_H
