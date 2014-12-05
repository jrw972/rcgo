#ifndef heap_h
#define heap_h

#include "types.h"

heap_t* heap_make (char* begin, char* end);

void* heap_instance (const heap_t* heap);

void* heap_allocate (heap_t* heap, type_t* type);

void heap_collect_garbage (heap_t* heap);

#endif /* heap_h */
