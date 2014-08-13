#ifndef util_h
#define util_h

#include <stddef.h>

#define VECTOR_INIT(array, type) do { \
  array = malloc (sizeof (type));                     \
  array##_size = 0;                                           \
  array##_capacity = 1;                                       \
} while (0);

#define VECTOR_ENSURE_ONE(array, type) do { \
  if (array##_size == array##_capacity) \
    { \
      array##_capacity *= 2;                                     \
      array = realloc (array, array##_capacity * sizeof (type)); \
    } \
  } while (0);

#define VECTOR_PUSH(array, type, value) do {  \
    VECTOR_ENSURE_ONE(array, type);                           \
    array[array##_size++] = value;                                    \
  } while (0);

#define VECTOR_FOREACH(ptr, limit, array, type)                         \
  type *ptr, *limit;                                                    \
  for (ptr = array, limit = array + array##_size; ptr != limit; ++ptr)

#define VECTOR_BEGIN(array) (array)
#define VECTOR_END(array) (array + array##_size)

void*
xmalloc (size_t size);

#endif /* util_h */
