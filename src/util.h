#ifndef util_h
#define util_h

#include <stddef.h>
#include <stdlib.h>

#define VECTOR_DECL(array, type) type* array;   \
  size_t array##_size;                          \
  size_t array##_capacity

#define VECTOR_INIT(array, type, size, x) do {                         \
    array = malloc (((size == 0) ? 1 : size) * sizeof (type)); \
    array##_size = size;                                                  \
    array##_capacity = ((size == 0) ? 1 : size);             \
    size_t idx;                                              \
    for (idx = 0; idx != size; ++idx) { \
      array[idx] = x;                               \
    }                                               \
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

#define VECTOR_POP(array) do {                  \
  if (array##_size != 0)                        \
    {                                           \
  --array##_size;                               \
    }                                           \
  }    while (0);

#define VECTOR_FOREACH(ptr, limit, array, type)                         \
  type *ptr; \
  type *limit; \
  for (ptr = array, limit = array + array##_size; ptr != limit; ++ptr)

#define VECTOR_BEGIN(array) (array)
#define VECTOR_END(array) (array + array##_size)
#define VECTOR_NEXT(pos) (pos + 1)
#define VECTOR_SIZE(array) array##_size
#define VECTOR_EMPTY(array) (array##_size == 0)
#define VECTOR_AT(array, idx) array[idx]
#define VECTOR_SET(array, idx, x) array[idx] = x
#define VECTOR_MOVE(array_to, array_from) do { array_to = array_from; array_to##_size = array_from##_size; array_to##_capacity = array_from##_capacity; } while (0);

void*
xmalloc (size_t size);

ptrdiff_t
align_up (ptrdiff_t value,
          size_t alignment);

#endif /* util_h */
