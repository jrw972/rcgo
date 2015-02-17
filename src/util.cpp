#include "util.hpp"

ptrdiff_t
align_up (ptrdiff_t value, size_t alignment)
{
  return (value + (alignment - 1)) & ~(alignment - 1);
}
