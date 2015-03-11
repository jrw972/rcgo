#include "field.hpp"
#include "util.hpp"

struct field_t
{
  std::string const name;
  const type_t *type;
  ptrdiff_t offset;

  field_t (const std::string& n)
    : name (n)
  { }
};

field_t *
field_make (const std::string& name, const type_t * type, ptrdiff_t offset)
{
  field_t *retval = new field_t (name);
  retval->type = type;
  retval->offset = offset;
  return retval;
}

const std::string&
field_name (const field_t * field)
{
  return field->name;
}

const type_t *
field_type (const field_t * field)
{
  return field->type;
}

ptrdiff_t
field_offset (const field_t * field)
{
  return field->offset;
}

void field_set_offset (field_t* field, ptrdiff_t offset)
{
  field->offset = offset;
}
