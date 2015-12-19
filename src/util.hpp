#ifndef RC_SRC_UTIL_HPP
#define RC_SRC_UTIL_HPP

#include <cstddef>
#include <cstdlib>

namespace util
{

inline ptrdiff_t align_up (ptrdiff_t value, size_t alignment)
{
  return (value + (alignment - 1)) & ~(alignment - 1);
}

}

#endif // RC_SRC_UTIL_HPP
