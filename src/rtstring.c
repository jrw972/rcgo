#include "rtstring.h"
#include "debug.h"
#include <string.h>

rtstring_t
rtstring_make (const char* ptr,
               size_t size)
{
  rtstring_t retval;
  retval.bytes = malloc (size);
  memcpy (retval.bytes, ptr, size);
  retval.size = size;
  return retval;
}
