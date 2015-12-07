#ifndef rc_util_hpp
#define rc_util_hpp

#include <cstddef>
#include <cstdlib>

namespace util
{

inline ptrdiff_t AlignUp (ptrdiff_t value, size_t alignment)
{
  return (value + (alignment - 1)) & ~(alignment - 1);
}

}

#endif /* rc_util_hpp */
