#include "strtab.hpp"
#include <string.h>

/* Prototype.  Not really a string table. */

string_t
enter (const char *str)
{
  return strdup (str);
}

const char *
get (string_t s)
{
  return s;
}

bool
streq (string_t x, string_t y)
{
  return strcmp (x, y) == 0;
}
