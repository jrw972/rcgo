#include "arch.hpp"

namespace arch
{
static size_t stack_alignment_ = 0;

size_t stack_alignment ()
{
  return stack_alignment_;
}

void set_stack_alignment (size_t a)
{
  stack_alignment_ = a;
}
}
