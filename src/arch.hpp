#ifndef RC_SRC_ARCH_HPP
#define RC_SRC_ARCH_HPP

#include "types.hpp"

namespace arch
{
// Alignment of the stack in bytes.  Typically 4 (32-bits) or 8 (64-bits).
size_t stack_alignment ();
void set_stack_alignment (size_t a);
}

#endif // RC_SRC_ARCH_HPP
