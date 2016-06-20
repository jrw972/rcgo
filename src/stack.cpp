#include "stack.hpp"

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
  size_t s = util::align_up (sizeof (void*), arch::stack_alignment ());
  assert (top_ + s <= limit_);
  std::memcpy (top_, &pointer, sizeof (void*));
  top_ += s;
}

void*
Stack::pop_pointer ()
{
  void* retval;
  size_t s = util::align_up (sizeof (void*), arch::stack_alignment ());
  assert (top_ - s >= data_);
  top_ -= s;
  std::memcpy (&retval, top_, sizeof (void*));
  return retval;
}

void*
Stack::peek_pointer () const
{
  void* retval;
  size_t s = util::align_up (sizeof (void*), arch::stack_alignment ());
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
  size_t s = util::align_up (size, arch::stack_alignment ());
  char* source = base_pointer_ + offset;
  assert (source >= data_ && source + size <= top_);
  assert (top_ + s <= limit_);
  std::memcpy (&retval, source, size);
  return retval;
}

void
Stack::reserve (size_t size)
{
  size_t s = util::align_up (size, arch::stack_alignment ());
  assert (top_ + s <= limit_);
  std::memset (top_, 0, size);
  top_ += s;
}

void
Stack::load (const void* ptr,
             size_t size)
{
  size_t s = util::align_up (size, arch::stack_alignment ());
  assert (top_ + s <= limit_);
  std::memcpy (top_, ptr, size);
  top_ += s;
}

void
Stack::store (void* ptr,
              size_t size)
{
  size_t s = util::align_up (size, arch::stack_alignment ());
  assert (top_ - s >= data_);
  top_ -= s;
  std::memcpy (ptr, top_, size);
}

void
Stack::move (ptrdiff_t offset,
             size_t size)
{
  size_t s = util::align_up (size, arch::stack_alignment ());
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
  size_t s = util::align_up (size, arch::stack_alignment ());
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
  size_t s = util::align_up (size, arch::stack_alignment ());
  assert (top_ - s >= data_);
  top_ -= s;
}

void
Stack::print (std::ostream& out) const
{
  size_t const increment = arch::stack_alignment ();
  assert (sizeof (size_t) == increment);

  out << "size = " << size () << " base_pointer = " << (void*)base_pointer_ << '\n';
  for (const char* ptr = data_; ptr != top_; ptr += increment)
    {
      size_t x;
      std::memcpy (&x, ptr, sizeof (size_t));
      std::cout << (void*)ptr << " => " << x << ' ' << (ptr == base_pointer_ ? "<--" : "") << '\n';
    }
}

void
Stack::write (ptrdiff_t offset,
              const void* ptr,
              size_t size)
{
  assert (base_pointer_ + offset >= data_);
  assert (base_pointer_ + offset + size <= limit_);
  std::memcpy (base_pointer_ + offset, ptr, size);
}

void
Stack::read (ptrdiff_t offset,
             void* ptr,
             size_t size) const
{
  assert (base_pointer_ + offset >= data_);
  assert (base_pointer_ + offset + size <= limit_);
  std::memcpy (ptr, base_pointer_ + offset, size);
}

char* Stack::base_pointer () const
{
  return base_pointer_;
}

void Stack::base_pointer (char* base_pointer)
{
  base_pointer_ = base_pointer;
}

char* Stack::top () const
{
  return top_;
}

bool Stack::empty () const
{
  return data_ == top_;
}

void* Stack::pointer_to_instruction_pointer () const
{
  return base_pointer_ - sizeof (void*);
}

size_t Stack::size () const
{
  return top_ - data_;
}
}
