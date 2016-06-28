#ifndef RC_SRC_MEMORY_MODEL_HPP
#define RC_SRC_MEMORY_MODEL_HPP

#include "types.hpp"
#include "util.hpp"
#include "arch.hpp"

namespace runtime
{

/*
 * Utility class for calculate the offsets of arguments and local variables.
 *
 * We assume an x86 calling covention where the caller pushes arguments,
 * the callee then pushes a new base pointer and allocates space for local
 * variables.  This class helps calculate the offset from the base pointer to
 * the arguments and local variables and the size to allocate for local
 * variables at run-time.
 */
class MemoryModel
{
public:
  MemoryModel ();
  bool arguments_empty () const;
  void arguments_push (const type::Type* type);
  bool locals_empty () const;
  void locals_push (const type::Type* type);
  void locals_pop (size_t size);
  ptrdiff_t arguments_offset () const;
  ptrdiff_t locals_offset () const;
  size_t locals_size_on_stack () const;
  void set_receiver_offset ();
  ptrdiff_t receiver_offset () const;

private:
  ptrdiff_t arguments_offset_;
  ptrdiff_t locals_offset_;
  size_t locals_size_on_stack_;
  ptrdiff_t receiver_offset_;
};

}

#endif // RC_SRC_MEMORY_MODEL_HPP
