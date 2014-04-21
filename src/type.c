#include "type.h"
#include <stdlib.h>
#include <string.h>

struct type_t
{
  TypeKind kind;
};

type_t *
type_make_bool (void)
{
  type_t *retval = malloc (sizeof (type_t));
  memset (retval, 0, sizeof (type_t));
  retval->kind = Bool;
  return retval;
}

TypeKind
type_kind (const type_t * type)
{
  return type->kind;
}
