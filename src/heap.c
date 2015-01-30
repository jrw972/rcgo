#include "heap.h"
#include "util.h"
#include "type.h"
#include "debug.h"
#include <string.h>
#include <pthread.h>

/*
  A heap consists of a number of blocks.  A block represents a
 contiguous memory region.  The region is divided into equal-sized
 slots.  The block contains a set of bits for each slot.  Free slots
 are coalesced into chunks that are organized into a singly-linked
 list.  Allocations are performed using first-fit.

 A slot must be at least the size of a chunk.
*/

// Size of a slot in bytes.
#define SLOT_SIZE (2 * sizeof (void*))
// Keep this many bits per slot.
#define BITS_PER_SLOT 4

#define OBJECT 0x01
#define ALLOCATED 0x02
#define MARK 0x04
#define SCANNED 0x08

// Element in the free list.
typedef struct chunk_t chunk_t;
struct chunk_t {
  chunk_t* next;
  size_t size;
};

typedef struct block_t block_t;
struct block_t {
  // Blocks are organized into a tree sorted by begin.
  block_t* left;
  block_t* right;
  // Blocks are also organized into a set/list when collecting garbage.
  block_t* next;
  // Beginning and end of the storage for this block.
  void* begin;
  void* end;
  // Indicates that at least one slot is marked.
  bool marked;
  // Status bits.
  unsigned char bits[];
};

static block_t* block_make (size_t size)
{
  size = align_up (size, SLOT_SIZE);
  size_t bits_bytes = (size / SLOT_SIZE * BITS_PER_SLOT + BITS_PER_SLOT) / 8;
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

static size_t block_slot (const block_t* block, void* address)
{
  assert (block->begin <= address);
  assert (address < block->end);
  return (address - block->begin) / SLOT_SIZE;
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

static unsigned char block_get_bits (const block_t* block, size_t slot)
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
  assert (size % SLOT_SIZE == 0);

  size_t slot = block_slot (block, address);
  size_t slot_end = slot + size / SLOT_SIZE;

  block_set_bits (block, slot, OBJECT);
  for (; slot != slot_end; ++slot)
    {
      block_set_bits (block, slot, ALLOCATED);
    }
}

static void block_mark (block_t* block, void* address, block_t** work_list)
{
  size_t slot = block_slot (block, address);

  // Get the bits.
  unsigned char bits = block_get_bits (block, slot);

  if ((bits & MARK) != 0)
    {
      // Already marked.
      return;
    }

  if ((bits & ALLOCATED) == 0)
    {
      // Not allocated.
      return;
    }

  block->marked = true;

  if (block->next == NULL)
    {
      // Not on the work list.
      block->next = *work_list;
      *work_list = block;
      if (block->next == NULL)
        {
          block->next = (block_t*)1;
        }
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

static void scan (heap_t* heap, void* begin, void* end, block_t** work_list);

static void block_scan (heap_t* heap, block_t* block, block_t** work_list)
{
  size_t slot;
  size_t slots = (block->end - block->begin) / SLOT_SIZE;
  for (slot = 0; slot != slots; ++slot)
    {
      unsigned char bits = block_get_bits (block, slot);
      if (((bits & MARK) != 0) &&
          ((bits & SCANNED) == 0))
        {
          block_set_bits (block, slot, SCANNED);
          scan (heap, block->begin + slot * SLOT_SIZE, block->begin + (slot + 1) * SLOT_SIZE, work_list);
        }
    }
}

static block_t* block_remove_right_most (block_t** b)
{
  block_t* block = *b;
  assert (block != NULL);

  if (block->right == NULL)
    {
      // We are the right-most.
      // Promote the left.
      *b = block->left;
      block->left = NULL;
      return block;
    }
  else
    {
      return block_remove_right_most (&block->right);
    }
}

static void block_dump (const block_t* block)
{
  if (block != NULL)
    {
      printf ("%zd block=%p begin=%p end=%p size=%zd left=%p right=%p\n", pthread_self(), block, block->begin, block->end, block->end - block->begin, block->left, block->right);
      size_t slot;
      size_t slots = (block->end - block->begin) / SLOT_SIZE;
      for (slot = 0; slot != slots; ++slot)
        {
          unsigned char bits = block_get_bits (block, slot);
          printf ("%zd slot %zd bits=%x scanned=%d mark=%d object=%d allocated=%d\n", pthread_self(), slot, bits, (bits & SCANNED) != 0, (bits & MARK) != 0, (bits & OBJECT) != 0, (bits & ALLOCATED) != 0);
        }
      char** begin = block->begin;
      char** end = block->end;
      for (; begin < end; ++begin)
        {
          printf ("%zd %p => %p\n", pthread_self(), begin, *begin);
        }
      block_dump (block->left);
      block_dump (block->right);
    }
}

static void block_sweep (block_t** b, chunk_t** head)
{
  block_t* block = *b;

  if (block == NULL)
    {
      return;
    }

  block_sweep (&block->left, head);
  block_sweep (&block->right, head);

  if (block->marked)
    {
      size_t slot = 0;
      size_t max_slot = (block->end - block->begin) / SLOT_SIZE;
      while (slot != max_slot)
        {
          unsigned char bits = block_get_bits (block, slot);
          if ((bits & MARK) != 0)
            {
              // Marked.
              block_reset_bits (block, slot, SCANNED | MARK);
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
      block->marked = false;
    }
  else
    {
      block_t* left = block->left;
      block_t* right = block->right;

      if (left == NULL && right == NULL)
        {
          *b = NULL;
        }
      else if (left == NULL && right != NULL)
        {
          *b = right;
        }
      else if (left != NULL && right == NULL)
        {
          *b = left;
        }
      else
        {
          block_t* r = block_remove_right_most (&block->left);
          r->left = block->left;
          r->right = block->right;
          *b = r;
        }

      // Free the data.
      free (block->begin);
      free (block);
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
  heap_t* parent;
  bool reachable;
  pthread_mutex_t mutex;
};

heap_t* heap_make (char* begin, char* end)
{
  heap_t* h = xmalloc (sizeof (heap_t));
  h->begin = begin;
  h->end = end;
  pthread_mutex_init (&h->mutex, NULL);
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

static void heap_dump_i (heap_t* heap)
{
  printf ("%zd heap=%p begin=%p end=%p next_sz=%zd dirty=%d reachable=%d parent=%p next=%p\n", pthread_self(), heap, heap->begin, heap->end, heap->next_allocation_size, heap->dirty, heap->reachable, heap->parent, heap->next);

  if (block_find (heap->block, heap->begin) == NULL)
    {
      char** begin = (char**)heap->begin;
      char** end = (char**)heap->end;
      for (; begin < end; ++begin)
        {
          printf ("%zd %p => %p\n", pthread_self(), begin, *begin);
        }
    }

  block_dump (heap->block);

  chunk_t* ch;
  for (ch = heap->free_list_head; ch != NULL; ch = ch->next)
    {
      printf ("%zd chunk=%p size=%zd next=%p\n", pthread_self(), ch, ch->size, ch->next);
    }

  heap_t* c;
  for (c = heap->child; c != NULL; c = c->next)
    {
      heap_dump (c);
    }
}

void heap_dump (heap_t* heap)
{
  pthread_mutex_lock (&heap->mutex);
  heap_dump_i (heap);
  pthread_mutex_unlock (&heap->mutex);
}

void* heap_allocate (heap_t* heap, size_t size)
{
  if (size == 0)
    {
      return NULL;
    }

  pthread_mutex_lock (&heap->mutex);

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

  pthread_mutex_unlock (&heap->mutex);

  return c;
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

static void mark_slot_for_address (heap_t* heap, void* p, block_t** work_list)
{
  block_t* block = block_find (heap->block, p);
  if (block != NULL)
    {
      block_mark (block, p, work_list);
    }
  else
    {
      heap_t* ptr;
      for (ptr = heap->child; ptr != NULL; ptr = ptr->next)
        {
          if (ptr == (heap_t*)p)
            {
              ptr->reachable = true;
              break;
            }
        }
    }
}

static void scan (heap_t* heap, void* begin, void* end, block_t** work_list)
{
  char** b;
  char** e;

  b = (char**)align_up ((size_t)begin, sizeof (void*));
  e = end;

  for (; b < e; ++b)
    {
      char* p = *b;
      mark_slot_for_address (heap, p, work_list);
    }
}

void heap_collect_garbage (heap_t* heap)
{
  block_t* work_list = NULL;
  pthread_mutex_lock (&heap->mutex);
  if (heap->dirty)
    {
      block_t* b = block_find (heap->block, heap->begin);
      if (b != NULL)
        {
          block_mark (b, heap->begin, &work_list);
        }
      scan (heap, heap->begin, heap->end, &work_list);

      while (work_list != NULL &&
             work_list != (block_t*)1)
        {
          block_t* b = work_list;
          work_list = b->next;
          b->next = NULL;
          block_scan (heap, b, &work_list);
        }

      // Sweep the heap (reconstruct the free list).
      heap->free_list_head = NULL;
      block_sweep (&heap->block, &heap->free_list_head);
      if (heap->block == NULL)
        {
          heap->next_allocation_size = 0;
        }
      heap->dirty = false;

      heap_t** child = &(heap->child);
      while (*child != NULL)
        {
          if ((*child)->reachable)
            {
              // Recur on reachable children.
              heap_collect_garbage (*child);
              (*child)->reachable = false;
              child = &(*child)->next;
            }
          else
            {
              // Free unreachable children.
              heap_t* h = *child;
              *child = h->next;
              heap_free (h);
            }
        }
    }
  pthread_mutex_unlock (&heap->mutex);
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
  pthread_mutex_lock (&heap->mutex);
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

  heap->dirty = true;

  // Free the heap.
  free (x);

  pthread_mutex_unlock (&heap->mutex);
}

void heap_insert_child (heap_t* parent, heap_t* child)
{
  pthread_mutex_lock (&parent->mutex);
  child->parent = parent;
  child->next = parent->child;
  parent->child = child;
  parent->dirty = true;
  pthread_mutex_unlock (&parent->mutex);
}

void heap_remove_from_parent (heap_t* child)
{
  heap_t* parent = child->parent;

  if (parent != NULL)
    {
      pthread_mutex_lock (&parent->mutex);
      heap_t** ptr = &(parent->child);
      while (*ptr != child)
        {
          ptr = &(*ptr)->next;
        }

      // Remove from parent's list.
      *ptr = child->next;

      // Make the child forget the parent.
      child->parent = NULL;

      parent->dirty = true;
      pthread_mutex_unlock (&parent->mutex);
    }
}
