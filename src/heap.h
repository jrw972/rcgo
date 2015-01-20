#ifndef heap_h
#define heap_h

#include "types.h"

heap_t* heap_make (char* begin, char* end);

heap_t* heap_make_size (size_t size_of_root);

void* heap_instance (const heap_t* heap);

void* heap_allocate (heap_t* heap, size_t size);

void heap_collect_garbage (heap_t* heap);

void heap_insert_child (heap_t* parent, heap_t* child);

void heap_merge (heap_t* heap, heap_t* x);

void heap_remove_from_parent (heap_t* child);

void heap_inc (heap_t* heap);

void heap_dec (heap_t* heap);

bool heap_in_use (const heap_t* heap);

#endif /* heap_h */
