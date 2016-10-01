#ifndef RC_SRC_ARCH_HPP
#define RC_SRC_ARCH_HPP

#include "types.hpp"

namespace arch
{
// Alignment of the stack in bytes.  Typically 4 (32-bits) or 8 (64-bits).
size_t stack_alignment ();
void set_stack_alignment (size_t a);

size_t alignment (const type::Type* type);
size_t size (const type::Type* type);
size_t unit_size (const type::Array* array);
size_t unit_size (const type::Slice* slice);
size_t size_on_stack (const type::Type* type);
size_t size_on_stack (const decl::ParameterList* list);
size_t offset (const decl::Field* field);
}

#endif // RC_SRC_ARCH_HPP
