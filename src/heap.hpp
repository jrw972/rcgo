#ifndef RC_SRC_HEAP_HPP
#define RC_SRC_HEAP_HPP

#include "types.hpp"

namespace runtime
{

struct Heap
{
  Heap (void* begin, size_t size);
  Heap (size_t size_of_root);
  void* root () const;
  void* allocate (size_t size);
  bool collect_garbage (bool force = false);
  void merge (Heap* x);
  void insert_child (Heap* child);
  void remove_from_parent ();
  void dump ();
  bool contains (void* ptr);
  bool is_object (void* ptr);
  bool is_allocated (void* ptr);
  bool is_child (Heap* child);

  struct Block;
  struct Chunk;

  // The root block of this heap.
  Block* block;
  // Lock for this heap.
  pthread_mutex_t mutex;

  // List of free chunks.
  Chunk* free_list_head;

  // TODO:  These should be counted in slots, not bytes.
  // Number of slots allocated in this heap.
  size_t allocated_size;
  // Size of the next block to be allocated.
  size_t next_block_size;
  // Size when next collection will be triggered.
  size_t next_collection_size;

  /* The beginning and end of the root of the heap.  For a statically allocated component, they refer to a chunk in the parent component.  For a child heap, they refer to a chunk in the heap. */
  char* begin;
  char* end;

  // The pointers for a tree of heaps.
  Heap* child;
  Heap* next;
  Heap* parent;
  // Flag indicating heap was reachable during garbage collection.
  bool reachable;
};

}

#endif // RC_SRC_HEAP_HPP
