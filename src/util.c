#include "util.h"
#include <stdlib.h>
#include <string.h>

void *
xmalloc (size_t sz)
{
  void *r = malloc (sz);
  memset (r, 0, sz);
  return r;
}

ptrdiff_t
align_up (ptrdiff_t value, size_t alignment)
{
  return (value + (alignment - 1)) & ~(alignment - 1);
}
