#include "heap.hpp"

#include <cstring>
#include <pthread.h>

#include "util.hpp"
#include "type.hpp"
#include "debug.hpp"

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

namespace runtime
{

// Element in the free list.
struct Heap::Chunk
{
  Chunk* next;
  size_t size;
};

struct Heap::Block
{
  static Block* make (size_t size)
  {
    size = util::align_up (size, SLOT_SIZE);
    size_t bits_bytes = (size / SLOT_SIZE * BITS_PER_SLOT + BITS_PER_SLOT) / 8;
    void* p = operator new (sizeof (Block) + bits_bytes);
    Block* block = new (p) Block (bits_bytes);
    block->begin_ = operator new (size);
    memset (block->begin_, 0, size);
    block->end_ = static_cast<char*> (block->begin_) + size;
    return block;
  }

  ~Block ()
  {
    if (left_)
      {
        delete left_;
      }
    if (right_)
      {
        delete right_;
      }
    operator delete (begin_);
  }

  static void insert (Block** ptr, Block* block)
  {
    assert (block->left_ == NULL);
    assert (block->right_ == NULL);
    Block* p = *ptr;
    if (p == NULL)
      {
        *ptr = block;
        return;
      }

    if (block->begin_ < p->begin_)
      {
        insert (&(p->left_), block);
      }
    else
      {
        insert (&(p->right_), block);
      }
  }

  static Block* find (Block* block, void* address)
  {
    if (block == NULL)
      {
        return NULL;
      }

    if (address >= block->begin_ && address < block->end_)
      {
        return block;
      }

    if (address < block->begin_)
      {
        return find (block->left_, address);
      }
    else
      {
        return find (block->right_, address);
      }
  }

  size_t slot (void* address) const
  {
    assert (begin_ <= address);
    assert (address < end_);
    return (static_cast<char*> (address) - static_cast<char*> (begin_)) / SLOT_SIZE;
  }

  void set_bits (size_t slot, unsigned char mask)
  {
    if (slot % 2 == 1)
      {
        mask <<= 4;
      }
    slot /= 2;

    bits_[slot] |= mask;
  }

  void reset_bits (size_t slot, unsigned char mask)
  {
    if (slot % 2 == 1)
      {
        mask <<= 4;
      }
    slot /= 2;

    bits_[slot] &= ~mask;
  }

  void clear_bits (size_t slot)
  {
    reset_bits (slot, 0x0F);
  }

  unsigned char get_bits (size_t slot) const
  {
    unsigned char retval = bits_[slot / 2];
    if (slot % 2 == 1)
      {
        retval >>= 4;
      }

    return retval & 0x0F;
  }

  void allocate (void* address, size_t size)
  {
    assert (size % SLOT_SIZE == 0);

    size_t slot = this->slot (address);
    size_t slot_end = slot + size / SLOT_SIZE;

    set_bits (slot, OBJECT);
    for (; slot != slot_end; ++slot)
      {
        set_bits (slot, ALLOCATED);
      }
  }

  void mark (void* address, Block** work_list)
  {
    size_t slot = this->slot (address);

    // Get the bits.
    unsigned char bits = get_bits (slot);

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

    marked_ = true;

    if (next_ == NULL)
      {
        // Not on the work list.
        next_ = *work_list;
        *work_list = this;
        if (next_ == NULL)
          {
            next_ = reinterpret_cast<Block*> (1);
          }
      }

    // Mark backward until we find the beginning of the object.
    size_t s;
    for (s = slot; (get_bits (s) & OBJECT) == 0; --s)
      {
        set_bits (s, MARK);
      }

    // Mark forward until we find the end of the object.
    size_t max_slot = (static_cast<char*> (end_) - static_cast<char*> (begin_)) / SLOT_SIZE;
    for (s = slot; s != max_slot && (get_bits (s) & ALLOCATED) != 0; ++s)
      {
        set_bits (s, MARK);
      }
  }

  void scan (Heap* heap, Block** work_list)
  {
    size_t slot;
    size_t slots = (static_cast<char*> (end_) - static_cast<char*> (begin_)) / SLOT_SIZE;
    for (slot = 0; slot != slots; ++slot)
      {
        unsigned char bits = get_bits (slot);
        if (((bits & MARK) != 0) &&
            ((bits & SCANNED) == 0))
          {
            set_bits (slot, SCANNED);
            heap->scan (static_cast<char*> (begin_) + slot * SLOT_SIZE, static_cast<char*> (begin_) + (slot + 1) * SLOT_SIZE, work_list);
          }
      }
  }

  static Block* remove_right_most (Block** b)
  {
    Block* block = *b;
    assert (block != NULL);

    if (block->right_ == NULL)
      {
        // We are the right-most.
        // Promote the left.
        *b = block->left_;
        block->left_ = NULL;
        return block;
      }
    else
      {
        return remove_right_most (&block->right_);
      }
  }

  void dump () const
  {
    printf ("%zd block=%p begin=%p end=%p size=%zd left=%p right=%p\n", pthread_self(), this, begin_, end_, (char*)end_ - (char*)begin_, left_, right_);
    size_t slot;
    size_t slots = ((char*)end_ - (char*)begin_) / SLOT_SIZE;
    for (slot = 0; slot != slots; ++slot)
      {
        unsigned char bits = get_bits (slot);
        printf ("%zd slot %zd bits=%x scanned=%d mark=%d object=%d allocated=%d\n", pthread_self(), slot, bits, (bits & SCANNED) != 0, (bits & MARK) != 0, (bits & OBJECT) != 0, (bits & ALLOCATED) != 0);
      }
    char** begin = (char**)begin_;
    char** end = (char**)end_;
    for (; begin < end; ++begin)
      {
        printf ("%zd %p => %p\n", pthread_self(), begin, *begin);
      }
    if (left_)
      {
        left_->dump ();
      }
    if (right_)
      {
        right_->dump ();
      }
  }

  // Return the number of allocated slots.
  static size_t sweep (Block** b, Heap::Chunk** head)
  {
    Block* block = *b;

    if (block == NULL)
      {
        return 0;
      }

    size_t retval = 0;

    retval += sweep (&block->left_, head);
    retval += sweep (&block->right_, head);

    if (block->marked_)
      {
        size_t slot = 0;
        size_t max_slot = ((char*)block->end_ - (char*)block->begin_) / SLOT_SIZE;
        while (slot != max_slot)
          {
            unsigned char bits = block->get_bits (slot);
            if ((bits & MARK) != 0)
              {
                // Marked.
                block->reset_bits (slot, SCANNED | MARK);
                ++slot;
                ++retval;
              }
            else
              {
                // Not marked.
                size_t slot_begin = slot;
                for (; slot != max_slot && (block->get_bits (slot) & MARK) == 0; ++slot)
                  {
                    block->clear_bits (slot);
                  }
                Chunk* c = reinterpret_cast<Chunk*>((char*)block->begin_ + slot_begin * SLOT_SIZE);
                c->size = (slot - slot_begin) * SLOT_SIZE;
                c->next = *head;
                *head = c;
              }
          }
        block->marked_ = false;
      }
    else
      {
        Block* left = block->left_;
        Block* right = block->right_;

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
            Block* r = Block::remove_right_most (&block->left_);
            r->left_ = block->left_;
            r->right_ = block->right_;
            *b = r;
          }

        // Free the data.
        block->left_ = NULL;
        block->right_ = NULL;
        delete block;
      }

    return retval;
  }

  void* begin () const
  {
    return begin_;
  }

  static void scan_worklist (Block* work_list, Heap* heap)
  {
    while (work_list != NULL &&
           work_list != reinterpret_cast<Block*> (1))
      {
        Block* b = work_list;
        work_list = b->next_;
        b->next_ = NULL;
        b->scan (heap, &work_list);
      }
  }

  static void merge (Block** root, Block* block)
  {
    if (block != NULL)
      {
        merge (root, block->left_);
        block->left_ = NULL;
        merge (root, block->right_);
        block->right_ = NULL;
        Block::insert (root, block);
      }
  }

private:
  // Blocks are organized into a tree sorted by begin.
  Block* left_;
  Block* right_;
  // Blocks are also organized into a set/list when collecting garbage.
  Block* next_;
  // Beginning and end of the storage for this block.
  void* begin_;
  void* end_;
  // Indicates that at least one slot is marked.
  bool marked_;
  // Status bits.
  unsigned char bits_[];

  Block (size_t bits_bytes)
    : left_ (NULL)
    , right_ (NULL)
    , next_ (NULL)
    , begin_ (NULL)
    , end_ (NULL)
    , marked_ (false)
  {
    memset (bits_, 0, bits_bytes);
  }
};

Heap::Heap (void* b, size_t size)
  : block_ (NULL)
  , free_list_head_ (NULL)
  , allocated_size_ (0)
  , next_block_size_ (0)
  , next_collection_size_ (0)
  , begin_ (static_cast<char*> (b))
  , end_ (begin_ + size)
  , child_ (NULL)
  , next_ (NULL)
  , parent_ (NULL)
  , reachable_ (false)
{
  pthread_mutex_init (&mutex_, NULL);
}

Heap::Heap (size_t size_of_root)
  : block_ (NULL)
  , free_list_head_ (NULL)
  , allocated_size_ (0)
  , next_block_size_ (0)
  , next_collection_size_ (0)
  , begin_ (NULL)
  , end_ (NULL)
  , child_ (NULL)
  , next_ (NULL)
  , parent_ (NULL)
  , reachable_ (false)
{
  pthread_mutex_init (&mutex_, NULL);
  begin_ = static_cast<char*> (allocate (size_of_root));
  end_ = begin_ + size_of_root;
}

void*
Heap::root () const
{
  return begin_;
}

void
Heap::dump_i () const
{
  printf ("%zd heap=%p begin=%p end=%p next_sz=%zd reachable=%d parent=%p next=%p\n", pthread_self(), this, this->begin_, this->end_, this->next_block_size_, this->reachable_, this->parent_, this->next_);

  if (Heap::Block::find (this->block_, this->begin_) == NULL)
    {
      char** begin = (char**)this->begin_;
      char** end = (char**)this->end_;
      for (; begin < end; ++begin)
        {
          printf ("%zd %p => %p\n", pthread_self(), begin, *begin);
        }
    }

  if (this->block_)
    {
      this->block_->dump ();
    }

  Heap::Chunk* ch;
  for (ch = this->free_list_head_; ch != NULL; ch = ch->next)
    {
      printf ("%zd chunk=%p size=%zd next=%p\n", pthread_self(), ch, ch->size, ch->next);
    }

  Heap* c;
  for (c = this->child_; c != NULL; c = c->next_)
    {
      c->dump ();
    }
}

void
Heap::dump ()
{
  pthread_mutex_lock (&mutex_);
  dump_i ();
  pthread_mutex_unlock (&mutex_);
}

void*
Heap::allocate (size_t size)
{
  if (size == 0)
    {
      return NULL;
    }

  pthread_mutex_lock (&mutex_);

  // Must be a multiple of the slot size.
  size = util::align_up (size, SLOT_SIZE);

  // Find a chunk.
  Heap::Chunk** chunk;
  for (chunk = &free_list_head_;
       *chunk != NULL && (*chunk)->size < size;
       chunk = &(*chunk)->next)
    ;;

  Heap::Block* block = NULL;

  if (*chunk == NULL)
    {
      // We need to allocate a block.
      if (size > next_block_size_)
        {
          next_block_size_ = size;
        }
      // Allocate the block.
      block = Heap::Block::make (next_block_size_);
      // Insert the block into the heap.
      Heap::Block::insert (&(this->block_), block);
      // Insert the chunk at the end of the free list.
      *chunk = reinterpret_cast<Heap::Chunk*> (block->begin ());
      (*chunk)->size = next_block_size_;
      (*chunk)->next = NULL;

      next_block_size_ *= 2;
    }

  // Remove the chunk from the free list.
  Heap::Chunk* c = *chunk;
  *chunk = c->next;

  // Split the chunk and reinsert.
  if (c->size - size > 0)
    {
      Heap::Chunk* d = reinterpret_cast<Heap::Chunk*> ((char*)c + size);
      d->size = c->size - size;
      d->next = free_list_head_;
      free_list_head_ = d;
    }

  // Find the block containing this chunk.
  if (block == NULL)
    {
      block = Heap::Block::find (this->block_, c);
    }

  block->allocate (c, size);

  // Clear the memory.
  memset (c, 0, size);

  allocated_size_ += size;

  pthread_mutex_unlock (&mutex_);

  return c;
}

Heap::~Heap ()
{
  // Free all the blocks.
  if (block_)
    {
      delete block_;
    }

  // Free the child heaps.
  while (child_ != NULL)
    {
      Heap* c = child_;
      child_ = c->next_;
      delete c;
    }
}

void
Heap::mark_slot_for_address (void* p, Heap::Block** work_list)
{
  Heap::Block* block = Heap::Block::find (this->block_, p);
  if (block != NULL)
    {
      block->mark (p, work_list);
    }
  else
    {
      Heap* ptr;
      for (ptr = this->child_; ptr != NULL; ptr = ptr->next_)
        {
          if (ptr == (Heap*)p)
            {
              ptr->reachable_ = true;
              break;
            }
        }
    }
}

void
Heap::scan (void* begin, void* end, Heap::Block** work_list)
{
  char** b;
  char** e;

  b = (char**)util::align_up ((size_t)begin, sizeof (void*));
  e = (char**)end;

  for (; b < e; ++b)
    {
      char* p = *b;
      mark_slot_for_address (p, work_list);
    }
}

bool
Heap::collect_garbage (bool force)
{
  bool retval = false;
  pthread_mutex_lock (&mutex_);
  // Strictly greater.  This avoids continuous collection if both are zero.
  if (force || allocated_size_ > next_collection_size_)
    {
      // Full collection.
      Heap::Block* work_list = NULL;
      Heap::Block* b = Heap::Block::find (block_, begin_);
      if (b != NULL)
        {
          b->mark (begin_, &work_list);
        }
      scan (begin_, end_, &work_list);

      Heap::Block::scan_worklist (work_list, this);

      // Sweep the heap (reconstruct the free list).
      free_list_head_ = NULL;
      allocated_size_ = Heap::Block::sweep (&block_, &free_list_head_) * SLOT_SIZE;
      next_collection_size_ = allocated_size_ * 2;

      if (block_ == NULL)
        {
          next_block_size_ = 0;
        }

      Heap** child = &this->child_;
      while (*child != NULL)
        {
          if ((*child)->reachable_)
            {
              // Recur on reachable children.
              (*child)->collect_garbage ();
              (*child)->reachable_ = false;
              child = &(*child)->next_;
            }
          else
            {
              // Free unreachable children.
              Heap* h = *child;
              *child = h->next_;
              delete h;
            }
        }
      retval = true;
    }
  else
    {
      // Just process children.
      for (Heap* child = this->child_; child != NULL; child = child->next_)
        {
          retval |= child->collect_garbage ();
        }
    }
  pthread_mutex_unlock (&mutex_);
  return retval;
}


void
Heap::merge (Heap* x)
{
  pthread_mutex_lock (&mutex_);
  // Merge the blocks.
  Heap::Block::merge (&block_, x->block_);
  x->block_ = NULL;

  // Merge the free list.
  Heap::Chunk** ch;
  for (ch = &free_list_head_; *ch != NULL; ch = &(*ch)->next)
    ;;
  *ch = x->free_list_head_;

  allocated_size_ += x->allocated_size_;
  // Not quite sure how to combine next_collection_size.
  // Doing nothing should work but may not be optimal.

  // Merge the children.
  Heap** h;
  for (h = &child_; *h != NULL; h = &(*h)->next_)
    ;;
  *h = x->child_;
  x->child_ = NULL;

  // Free the heap.
  delete x;

  pthread_mutex_unlock (&mutex_);
}

void
Heap::insert_child (Heap* child)
{
  pthread_mutex_lock (&this->mutex_);
  child->parent_ = this;
  child->next_ = this->child_;
  this->child_ = child;
  // Force garbage collection.
  this->next_collection_size_ = 0;
  pthread_mutex_unlock (&this->mutex_);
}

void
Heap::remove_from_parent ()
{
  Heap* parent = this->parent_;

  if (parent != NULL)
    {
      pthread_mutex_lock (&parent->mutex_);
      Heap** ptr = &(parent->child_);
      while (*ptr != this)
        {
          ptr = &(*ptr)->next_;
        }

      // Remove from parent's list.
      *ptr = this->next_;

      // Make the child forget the parent.
      this->parent_ = NULL;

      // Force garbage collection.
      parent->next_collection_size_ = 0;

      pthread_mutex_unlock (&parent->mutex_);
    }
}

bool
Heap::contains (void* ptr)
{
  pthread_mutex_lock (&mutex_);
  bool retval = Heap::Block::find (block_, ptr) != NULL;
  pthread_mutex_unlock (&mutex_);
  return retval;
}

bool
Heap::is_object (void* ptr)
{
  pthread_mutex_lock (&mutex_);
  Heap::Block* block = Heap::Block::find (this->block_, ptr);
  if (block == NULL)
    {
      pthread_mutex_unlock (&mutex_);
      return false;
    }

  size_t slot = block->slot (ptr);
  unsigned char bits = block->get_bits (slot);
  pthread_mutex_unlock (&mutex_);

  return bits & OBJECT;
}

bool
Heap::is_allocated (void* ptr)
{
  pthread_mutex_lock (&mutex_);
  Heap::Block* block = Heap::Block::find (this->block_, ptr);
  if (block == NULL)
    {
      pthread_mutex_unlock (&mutex_);
      return false;
    }

  size_t slot = block->slot (ptr);
  unsigned char bits = block->get_bits (slot);
  pthread_mutex_unlock (&mutex_);

  return bits & ALLOCATED;
}

bool
Heap::is_child (Heap* child)
{
  pthread_mutex_lock (&mutex_);
  for (Heap* h = this->child_; h != NULL; h = h->next_)
    {
      if (h == child)
        {
          pthread_mutex_unlock (&mutex_);
          return true;
        }
    }
  pthread_mutex_unlock (&mutex_);
  return false;
}
}
