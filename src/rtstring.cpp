#include "rtstring.hpp"
#include "debug.hpp"
#include <string.h>

rtstring_t
rtstring_make (const char* ptr,
               size_t size)
{
  rtstring_t retval;
  retval.bytes = (char*)malloc (size);
  memcpy (retval.bytes, ptr, size);
  retval.size = size;
  return retval;
}
