#ifndef heap_h
#define heap_h

#include "types.hpp"

heap_t* heap_make (void* begin, size_t size);

heap_t* heap_make_size (size_t size_of_root);

void* heap_instance (const heap_t* heap);

void* heap_allocate (heap_t* heap, size_t size);

void heap_collect_garbage (heap_t* heap);

void heap_merge (heap_t* heap, heap_t* x);

void heap_insert_child (heap_t* parent, heap_t* child);

void heap_remove_from_parent (heap_t* child);

void heap_dump (heap_t* heap);

#endif /* heap_h */
