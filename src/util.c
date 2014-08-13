#include "util.h"
#include <stdlib.h>
#include <string.h>

void*
xmalloc (size_t sz)
{
  void* r = malloc (sz);
  memset (r, 0, sz);
  return r;
}
