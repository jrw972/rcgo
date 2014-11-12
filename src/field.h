#ifndef field_h
#define field_h

#include "types.h"
#include "strtab.h"

field_t *field_make (string_t name, type_t * type, ptrdiff_t offset);

string_t field_name (const field_t * field);

type_t *field_type (const field_t * field);

ptrdiff_t field_offset (const field_t * field);

void field_set_offset (field_t* field, ptrdiff_t offset);

#endif /* field_h */
