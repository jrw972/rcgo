#ifndef field_h
#define field_h

#include "types.h"
#include "strtab.h"

field_t *
field_make (string_t name, type_t * type, ptrdiff_t offset, size_t number);

string_t field_name (const field_t * field);

type_t *field_type (const field_t * field);

size_t field_number (const field_t * field);

ptrdiff_t field_offset (const field_t * field);

#endif /* field_h */
