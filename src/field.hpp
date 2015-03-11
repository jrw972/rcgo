#ifndef field_h
#define field_h

#include "types.hpp"

field_t *field_make (const std::string& name, const type_t * type, ptrdiff_t offset);

const std::string& field_name (const field_t * field);

const type_t *field_type (const field_t * field);

ptrdiff_t field_offset (const field_t * field);

void field_set_offset (field_t* field, ptrdiff_t offset);

#endif /* field_h */
