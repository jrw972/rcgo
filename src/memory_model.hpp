#ifndef memory_model_hpp
#define memory_model_hpp

#include "types.hpp"
#include "util.hpp"

class memory_model_t
{
public:
  static size_t stack_alignment;

  memory_model_t ()
    : arguments_offset_ (-(ptrdiff_t)sizeof (void*))
    , locals_offset_ (sizeof (void*))
    , locals_size_ (0)
  { }

  bool arguments_empty () const
  {
    return arguments_offset_ == -(ptrdiff_t)sizeof (void*);
  }

  void arguments_push (size_t size)
  {
    arguments_offset_ -= util::AlignUp (size, stack_alignment);
  }

  bool locals_empty () const
  {
    return locals_offset_ == sizeof (void*);
  }

  void locals_push (size_t size)
  {
    size = util::AlignUp (size, stack_alignment);
    locals_offset_ += size;
    locals_size_ += size;
  }

  void locals_pop (size_t size)
  {
    locals_offset_ -= util::AlignUp (size, stack_alignment);
  }

  ptrdiff_t arguments_offset () const { return arguments_offset_; }

  ptrdiff_t locals_offset () const { return locals_offset_; }

  size_t locals_size () const { return locals_size_; }

private:
  ptrdiff_t arguments_offset_;
  ptrdiff_t locals_offset_;
  size_t locals_size_;
};

#endif /* memory_model_hpp */
