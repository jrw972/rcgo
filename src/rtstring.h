#ifndef rtstring_h
#define rtstring_h

#include <stddef.h>

typedef struct {
  char* bytes;
  size_t size;
} rtstring_t;

rtstring_t
rtstring_make (const char* ptr,
               size_t size);

#endif /* rtstring_h */
