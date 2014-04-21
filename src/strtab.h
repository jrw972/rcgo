#ifndef strtab_h
#define strtab_h

#include <stddef.h>
#include <stdbool.h>

/* String Table */

typedef char *string_t;

string_t enter (const char *str);

const char *get (string_t s);

bool streq (string_t x, string_t y);

#endif /* strtab_h */
