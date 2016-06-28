#include "memory_model.hpp"

namespace runtime
{

MemoryModel::MemoryModel ()
  : arguments_offset_ (-(ptrdiff_t)sizeof (void*))
  , locals_offset_ (sizeof (void*))
  , locals_size_on_stack_ (0)
{ }

bool MemoryModel::arguments_empty () const
{
  return arguments_offset_ == -(ptrdiff_t)sizeof (void*);
}

void MemoryModel::arguments_push (const type::Type* type)
{
  arguments_offset_ -= arch::size_on_stack (type);
}

bool MemoryModel::locals_empty () const
{
  return locals_offset_ == sizeof (void*);
}

void MemoryModel::locals_push (const type::Type* type)
{
  size_t size = arch::size_on_stack (type);
  locals_offset_ += size;
  locals_size_on_stack_ += size;
}

void MemoryModel::locals_pop (size_t size)
{
  locals_offset_ -= util::align_up (size, arch::stack_alignment ());
}

ptrdiff_t MemoryModel::arguments_offset () const
{
  return arguments_offset_;
}

ptrdiff_t MemoryModel::locals_offset () const
{
  return locals_offset_;
}

size_t MemoryModel::locals_size_on_stack () const
{
  return locals_size_on_stack_;
}

void MemoryModel::set_receiver_offset ()
{
  receiver_offset_ = arguments_offset ();
}

ptrdiff_t MemoryModel::receiver_offset () const
{
  return receiver_offset_;
}
}
