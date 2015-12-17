#ifndef RC_SRC_HEAP_HPP
#define RC_SRC_HEAP_HPP

#include "types.hpp"

namespace runtime
{

struct Heap
{
  // Constructor for external root of the given size.
  // Used for statically allocated components.
  Heap (void* begin, size_t size);
  // Constructor for internal root of the given size.
  // Used for dynamically allocated heaps.
  Heap (size_t size_of_root);
  ~Heap ();
  // Return the root of the heap.
  void* root () const;
  // Allocate size bytes.
  void* allocate (size_t size);
  bool collect_garbage (bool force = false);
  // Merge x into this heap.
  void merge (Heap* x);
  void insert_child (Heap* child);
  void remove_from_parent ();
  void dump ();
  bool contains (void* ptr);
  bool is_object (void* ptr);
  bool is_allocated (void* ptr);
  bool is_child (Heap* child);

private:
  struct Block;
  struct Chunk;

  // The root block of this heap.
  Block* block_;

  // Lock for this heap.
  pthread_mutex_t mutex_;

  // List of free chunks.
  Chunk* free_list_head_;

  // Number of bytes allocated in this heap.
  size_t allocated_size_;
  // Size of the next block to be allocated (bytes).
  size_t next_block_size_;
  // Size when next collection will be triggered (bytes).
  size_t next_collection_size_;

  // The beginning and end of the root of the heap.  For a statically
  // allocated component, they refer to a chunk in the parent
  // component.  For a child heap, they refer to a chunk in the
  // heap.
  char* begin_;
  char* end_;

  // The pointers for a tree of heaps.
  Heap* child_;
  Heap* next_;
  Heap* parent_;
  // Flag indicating heap was reachable during garbage collection.
  bool reachable_;

  void scan (void* begin, void* end, Block** work_list);
  void mark_slot_for_address (void* p, Block** work_list);
  void dump_i () const;
};

}

#endif // RC_SRC_HEAP_HPP
