#include "heap.h"
#include "util.h"
#include "type.h"
#include "debug.h"
#include <string.h>

/*
  A heap consists of a number of blocks.  A block represents a
 contiguous memory region.  The region is divided into equal-sized
 slots.  The block contains a set of bits for each slot.  Free slots
 are coalesced into chunks that are organized into a singly-linked
 list.  Allocations are performed using first-fit.

 A slot must be at least the size of a chunk.
*/

// Size of a slot in bytes.
#define SLOT_SIZE 16
// Keep this many bits per slot.
#define BITS_PER_SLOT 4

#define OBJECT 0x01
#define ALLOCATED 0x02
#define MARK 0x04
// TODO:  Maybe we can use the extra bit to say the slot contains pointers and need to be scanned.

// Element in the free list.
typedef struct chunk_t chunk_t;
struct chunk_t {
  chunk_t* next;
  size_t size;
};

#define CHUNK_SIZE sizeof (chunk_t)

typedef struct block_t block_t;
struct block_t {
  // Blocks are organized into a tree.
  block_t* left;
  block_t* right;
  void* begin;
  void* end;
  unsigned char bits[];
};

static block_t* block_make (size_t size)
{
  size = align_up (size, SLOT_SIZE);
  size_t bits_bytes = size / SLOT_SIZE * BITS_PER_SLOT / 8;
  block_t* block = xmalloc (sizeof (block_t) + bits_bytes);
  block->begin = xmalloc (size);
  block->end = block->begin + size;
  return block;
}

static void block_free (block_t* block)
{
  if (block != NULL)
    {
      block_free (block->left);
      block_free (block->right);
      free (block->begin);
      free (block);
    }
}

static void block_insert (block_t** ptr, block_t* block)
{
  block_t* p = *ptr;
  if (p == NULL)
    {
      *ptr = block;
      return;
    }

  if (block->begin < p->begin)
    {
      block_insert (&(p->left), block);
    }
  else
    {
      block_insert (&(p->right), block);
    }
}

static block_t* block_find (block_t* block, void* address)
{
  if (block == NULL)
    {
      return NULL;
    }

  if (address >= block->begin && address < block->end)
    {
      return block;
    }

  if (address < block->begin)
    {
      return block_find (block->left, address);
    }
  else
    {
      return block_find (block->right, address);
    }
}

static void block_set_bits (block_t* block, size_t slot, unsigned char mask)
{
  if (slot % 2 == 1)
    {
      mask <<= 4;
    }
  slot /= 2;

  block->bits[slot] |= mask;
}

static void block_reset_bits (block_t* block, size_t slot, unsigned char mask)
{
  if (slot % 2 == 1)
    {
      mask <<= 4;
    }
  slot /= 2;

  block->bits[slot] &= ~mask;
}

static void block_clear_bits (block_t* block, size_t slot)
{
  block_reset_bits (block, slot, 0x0F);
}

static unsigned char block_get_bits (block_t* block, size_t slot)
{
  unsigned char retval = block->bits[slot / 2];
  if (slot % 2 == 1)
    {
      retval >>= 4;
    }

  return retval & 0x0F;
}

static void block_allocate (block_t* block, void* address, size_t size)
{
  assert (block->begin <= address);
  assert (address < block->end);

  size_t slot = (address - block->begin) / SLOT_SIZE;

  block_set_bits (block, slot, OBJECT);

  size_t slot_end = slot + size / SLOT_SIZE;
  for (; slot != slot_end; ++slot)
    {
      block_set_bits (block, slot, ALLOCATED);
    }
}

static void block_mark (block_t* block, void* address)
{
  assert (block->begin <= address);
  assert (address < block->end);

  size_t slot = (address - block->begin) / SLOT_SIZE;

  // Get the bits.
  unsigned char bits = block_get_bits (block, slot);

  if (bits & MARK)
    {
      // Already marked.
      return;
    }

  if ((bits & ALLOCATED) == 0)
    {
      // Not allocated.
      return;
    }

  // Mark backward until we find the beginning of the object.
  size_t s;
  for (s = slot; (block_get_bits (block, s) & OBJECT) == 0; --s)
    {
      block_set_bits (block, s, MARK);
    }

  // Mark forward until we find the end of the object.
  size_t max_slot = (block->end - block->begin) / SLOT_SIZE;
  for (s = slot; s != max_slot && (block_get_bits (block, s) & ALLOCATED) != 0; ++s)
    {
      block_set_bits (block, s, MARK);
    }
}

static void scan (heap_t* heap, block_t* root_block, void* begin, void* end);

static void block_scan (heap_t* heap, block_t* root_block, block_t* block)
{
  if (block == NULL)
    {
      return;
    }

  block_scan (heap, root_block, block->left);
  block_scan (heap, root_block, block->right);
  scan (heap, root_block, block->begin, block->end);
}

static void block_sweep (block_t* block, chunk_t** head)
{
  if (block == NULL)
    {
      return;
    }

  block_sweep (block->left, head);
  block_sweep (block->right, head);

  size_t slot = 0;
  size_t max_slot = (block->end - block->begin) / SLOT_SIZE;
  while (slot != max_slot)
    {
      unsigned char bits = block_get_bits (block, slot);
      if ((bits & MARK) != 0)
        {
          // Marked.
          block_reset_bits (block, slot, MARK);
          ++slot;
        }
      else
        {
          // Not marked.
          size_t slot_begin = slot;
          for (; slot != max_slot && (block_get_bits (block, slot) & MARK) == 0; ++slot)
            {
              block_clear_bits (block, slot);
            }
          chunk_t* c = block->begin + slot_begin * SLOT_SIZE;
          c->size = (slot - slot_begin) * SLOT_SIZE;
          c->next = *head;
          *head = c;
        }
    }
}

struct heap_t {
  block_t* block;
  char* begin; // This part of the component is allocated in its parent.
  char* end;   // It is the root.
  size_t next_allocation_size;
  chunk_t* free_list_head;
  bool dirty;  // The heap is dirty and should be collected.
  heap_t* child;
  heap_t* next;
  size_t count; // Increment when changing to, decremented when changing from.
  heap_t* parent;
  bool reachable;
};

heap_t* heap_make (char* begin, char* end)
{
  heap_t* h = xmalloc (sizeof (heap_t));
  h->begin = begin;
  h->end = end;
  return h;
}

heap_t* heap_make_size (size_t size_of_root)
{
  heap_t* h = heap_make (NULL, NULL);
  h->begin = heap_allocate (h, size_of_root);
  h->end = h->begin + size_of_root;
  return h;

}

void* heap_instance (const heap_t* heap)
{
  return heap->begin;
}

static void block_dump (const block_t* block)
{
  if (block != NULL)
    {
      printf ("block=%p begin=%p end=%p left=%p right=%p\n", block, block->begin, block->end, block->left, block->right);
      block_dump (block->left);
      block_dump (block->right);
    }
}

void heap_dump (const heap_t* heap)
{
  printf ("heap=%p begin=%p end=%p next_sz=%zd dirty=%d count=%zd reachable=%d parent=%p next=%p\n", heap, heap->begin, heap->end, heap->next_allocation_size, heap->dirty, heap->count, heap->reachable, heap->parent, heap->next);

  block_dump (heap->block);

  chunk_t* ch;
  for (ch = heap->free_list_head; ch != NULL; ch = ch->next)
    {
      printf ("chunk=%p size=%zd next=%p\n", ch, ch->size, ch->next);
    }

  heap_t* c;
  for (c = heap->child; c != NULL; c = c->next)
    {
      heap_dump (c);
    }
}

void* heap_allocate (heap_t* heap, size_t size)
{
  if (size == 0)
    {
      return NULL;
    }

  // Must be a multiple of the slot size.
  size = align_up (size, SLOT_SIZE);

  // Find a chunk.
  chunk_t** chunk;
  for (chunk = &(heap->free_list_head);
       *chunk != NULL && (*chunk)->size < size;
       chunk = &(*chunk)->next)
    ;;

  block_t* block = NULL;

  if (*chunk == NULL)
    {
      // We need to allocate a block.
      if (size > heap->next_allocation_size)
        {
          heap->next_allocation_size = size;
        }
      // Allocate the block.
      block = block_make (heap->next_allocation_size);
      // Insert the block into the heap.
      block_insert (&(heap->block), block);
      // Insert the chunk at the end of the free list.
      *chunk = (chunk_t*)block->begin;
      (*chunk)->size = heap->next_allocation_size;
      (*chunk)->next = NULL;

      heap->next_allocation_size *= 2;
    }

  // Remove the chunk from the free list.
  chunk_t* c = *chunk;
  *chunk = c->next;

  // Split the chunk and reinsert.
  if (c->size - size > 0)
    {
      chunk_t*d = (chunk_t*)((char*)c + size);
      d->size = c->size - size;
      d->next = heap->free_list_head;
      heap->free_list_head = d;
    }

  // Find the block containing this chunk.
  if (block == NULL)
    {
      block = block_find (heap->block, c);
    }

  block_allocate (block, c, size);

  // Clear the memory.
  memset (c, 0, size);

  heap->dirty = true;

  return c;
}

static void scan (heap_t* heap, block_t* root_block, void* begin, void* end)
{
  char** b;
  char** e;

  b = (char**)align_up ((size_t)begin, sizeof (void*));
  e = end;

  for (; b < e; ++b)
    {
      char* p = *b;
      block_t* block = block_find (root_block, p);
      if (block != NULL)
        {
          block_mark (block, p);
        }
      else
        {
          heap_t* ptr;
          for (ptr = heap->child; ptr != NULL; ptr = ptr->next)
            {
              if (ptr == (heap_t*)p)
                {
                  heap->reachable = true;
                }
            }
        }
    }
}

static void heap_free (heap_t* heap)
{
  // Free all the blocks.
  block_free (heap->block);

  // Free the child heaps.
  while (heap->child != NULL)
    {
      heap_t* c = heap->child;
      heap->child = c->next;
      heap_free (c);
    }

  free (heap);
}

void heap_collect_garbage (heap_t* heap)
{
  if (heap->dirty)
    {
      // Scan the root.
      scan (heap, heap->block, heap->begin, heap->end);
      // Scan the heap.
      block_scan (heap, heap->block, heap->block);
      // Sweep the heap.
      heap->free_list_head = NULL;
      block_sweep (heap->block, &heap->free_list_head);
      heap->dirty = false;

      heap_t** child = &(heap->child);
      while (*child != NULL)
        {
          if ((*child)->reachable)
            {
              heap_collect_garbage (*child);
              (*child)->reachable = false;
              child = &(*child)->next;
            }
          else
            {
              heap_t* h = *child;
              *child = h->next;
              heap_free (h);
            }
        }
    }
}

void heap_insert_child (heap_t* parent, heap_t* child)
{
  child->parent = parent;
  child->next = parent->child;
  parent->child = child;
}

static void merge_blocks (heap_t* heap, block_t* block)
{
  if (block != NULL)
    {
      merge_blocks (heap, block->left);
      merge_blocks (heap, block->right);
      block_insert (&(heap->block), block);
    }
}

void heap_merge (heap_t* heap, heap_t* x)
{
  // Merge the blocks.
  merge_blocks (heap, x->block);

  // Merge the free list.
  chunk_t** ch;
  for (ch = &(heap->free_list_head); *ch != NULL; ch = &(*ch)->next)
    ;;
  *ch = x->free_list_head;

  // Merge the children.
  heap_t** h;
  for (h = &(heap->child); *h != NULL; h = &(*h)->next)
    ;;
  *h = x->child;

  // Calculate dirty.
  heap->dirty = heap->dirty || x->dirty;

  // Free the heap.
  free (x);
}

void heap_remove_from_parent (heap_t* child)
{
  if (child->parent != NULL)
    {
      heap_t** ptr = &(child->parent->child);
      while (*ptr != child)
        {
          ptr = &(*ptr)->next;
        }

      // Remove from parent's list.
      *ptr = child->next;

      // Make the child forget the parent.
      child->parent = NULL;
    }
}

void heap_inc (heap_t* heap)
{
  ++heap->count;
}

void heap_dec (heap_t* heap)
{
  --heap->count;
}

bool heap_in_use (const heap_t* heap)
{
  return heap->count != 0;
}
