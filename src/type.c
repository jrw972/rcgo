#include "type.h"
#include <stdlib.h>
#include <string.h>
#include "debug.h"

struct type_t
{
  TypeKind kind;
};

static type_t*
make (TypeKind kind)
{
  type_t *retval = malloc (sizeof (type_t));
  memset (retval, 0, sizeof (type_t));
  retval->kind = kind;
  return retval;
}

static type_t *
type_make_undefined (void)
{
  static type_t * retval = NULL;
  if (retval == NULL) {
    retval = make (TypeUndefined);
  }
  return retval;
}

type_t *
type_make_bool (void)
{
  return make (TypeBool);
}

TypeKind
type_kind (const type_t * type)
{
  return type->kind;
}

bool type_can_represent (const type_t* type,
                         untyped_value_t u)
{
  switch (type->kind) {
  case TypeUndefined:
    return false;
  case TypeBool:
    return untyped_value_is_bool (u);
  }
  bug ("unhandled case");
}

bool type_assignable (const type_t* target,
                      const type_t* source)
{
  switch (target->kind) {
  case TypeUndefined:
    return false;
  case TypeBool:
    return target == source;
  }
  bug ("unhandled case");
}

const type_t * type_logic_not (const type_t * type)
{
  if (type->kind == TypeBool) {
    return type;
  }
  else {
    return type_make_undefined ();
  }
}

const type_t * type_logic_and (const type_t * x,
                               const type_t * y)
{
  if (x->kind == TypeBool && x == y) {
    return x;
  }
  else {
    return type_make_undefined ();
  }
}
