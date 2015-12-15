#include "stack.hpp"

#include "util.hpp"
#include "type.hpp"

namespace runtime
{

using namespace type;

Stack::Stack (size_t capacity)
  : data_ (new char[capacity])
  , base_pointer_ (NULL)
  , top_ (data_)
  , limit_ (data_ + capacity)
{
  std::memset (data_, 0, capacity);
}

Stack::~Stack ()
{
  delete[] data_;
}

void
Stack::push_pointer (void* pointer)
{
  size_t s = util::AlignUp (sizeof (void*), MemoryModel::StackAlignment);
  assert (top_ + s <= limit_);
  std::memcpy (top_, &pointer, sizeof (void*));
  top_ += s;
}

void*
Stack::pop_pointer ()
{
  void* retval;
  size_t s = util::AlignUp (sizeof (void*), MemoryModel::StackAlignment);
  assert (top_ - s >= data_);
  top_ -= s;
  std::memcpy (&retval, top_, sizeof (void*));
  return retval;
}

void*
Stack::peek_pointer () const
{
  void* retval;
  size_t s = util::AlignUp (sizeof (void*), MemoryModel::StackAlignment);
  assert (top_ - s >= data_);
  std::memcpy (&retval, top_ - s, sizeof (void*));
  return retval;
}

void
Stack::push_address (ptrdiff_t offset)
{
  push_pointer (base_pointer_ + offset);
}

void*
Stack::get_address (ptrdiff_t offset)
{
  return base_pointer_ + offset;
}

void*
Stack::read_pointer (ptrdiff_t offset)
{
  void* retval;
  const size_t size = sizeof (void*);
  size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
  char* source = base_pointer_ + offset;
  assert (source >= data_ && source + size <= top_);
  assert (top_ + s <= limit_);
  std::memcpy (&retval, source, size);
  return retval;
}

void
Stack::reserve (size_t size)
{
  size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
  assert (top_ + s <= limit_);
  std::memset (top_, 0, size);
  top_ += s;
}

void
Stack::load (void* ptr,
             size_t size)
{
  size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
  assert (top_ + s <= limit_);
  std::memcpy (top_, ptr, size);
  top_ += s;
}

void
Stack::store (void* ptr,
              size_t size)
{
  size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
  assert (top_ - s >= data_);
  top_ -= s;
  std::memcpy (ptr, top_, size);
}

void
Stack::move (ptrdiff_t offset,
             size_t size)
{
  size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
  char* ptr = base_pointer_ + offset;
  assert (ptr >= data_ && ptr + size <= top_);
  assert (top_ - s >= data_);
  top_ -= s;
  std::memcpy (ptr, top_, size);
}

void
Stack::clear (ptrdiff_t offset,
              size_t size)
{
  size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
  char* ptr = base_pointer_ + offset;
  assert (ptr >= data_ && ptr + size <= top_);
  std::memset (ptr, 0, s);
}

void
Stack::setup (size_t size)
{
  char* bp = top_;
  push_pointer (base_pointer_);
  base_pointer_ = bp;
  reserve (size);
}

void
Stack::teardown ()
{
  top_ = base_pointer_ + sizeof (void*);
  base_pointer_ = (char*)pop_pointer ();
}

void
Stack::popn (size_t size)
{
  size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
  assert (top_ - s >= data_);
  top_ -= s;
}

void
Stack::dump () const
{
  printf ("size = %td base_pointer = %p\n", top_ - data_, base_pointer_);
  size_t increment = MemoryModel::StackAlignment;
  const char* ptr;
  for (ptr = data_; ptr != top_; ptr += increment)
    {
      size_t x;
      std::memcpy (&x, ptr, sizeof (size_t));
      printf ("%p => %zx %s\n", ptr, x, ptr == base_pointer_ ? "<--" : "");
    }
}

void
Stack::write (ptrdiff_t offset,
              void* ptr,
              size_t size)
{
  assert (base_pointer_ + offset >= data_);
  assert (base_pointer_ + offset + size <= limit_);
  std::memcpy (base_pointer_ + offset, ptr, size);
}

}
