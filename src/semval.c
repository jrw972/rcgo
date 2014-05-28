#include "semval.h"
#include "type.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symtab.h"
#include "debug.h"

void
semval_print (semval_t s)
{
  switch (s.kind)
    {
    case Undefined:
      printf ("Undefined");
      break;
    case Reference:
      reference_print (s.reference);
      break;
    case Value:
      abstract_value_print (s.value);
      break;
    }
}

semval_t
semval_undefined (void)
{
semval_t retval = { kind:Undefined };
  return retval;
}

bool
semval_is_undefined (semval_t s)
{
  return s.kind == Undefined;
}

semval_t
semval_make_reference (reference_t reference)
{
  semval_t retval = { kind: Reference, reference:reference };
  return retval;
}

bool
semval_is_reference (semval_t s)
{
  return s.kind == Reference;
}

semval_t
semval_make_value (abstract_value_t v)
{
semval_t retval = { kind: Value, value:v };
  return retval;
}

bool
semval_is_value (semval_t s)
{
  return s.kind == Value;
}

abstract_value_t
semval_get_value (semval_t s)
{
  assert (s.kind == Value);
  return s.value;
}

semval_t
semval_explicit_dereference (semval_t s)
{
  if (s.kind == Value)
    {
      abstract_value_t av = abstract_value_dereference (s.value);
      bool mutable = abstract_value_is_pointer_to_mutable (s.value);
      return semval_make_reference (reference_make (av, mutable));
    }
  else
    {
      return semval_undefined ();
    }
}

semval_t
semval_implicit_dereference (semval_t s)
{
  if (s.kind == Reference)
    {
      return semval_make_value (reference_value (s.reference));
    }
  else
    {
      return semval_undefined ();
    }
}

static semval_t
semval_unary (semval_t s, abstract_value_t (*func) (abstract_value_t))
{
  if (s.kind == Value)
    {
      return semval_make_value (func (s.value));
    }
  else
    {
      return semval_undefined ();
    }
}

static semval_t
semval_binary (semval_t x,
	       semval_t y,
	       abstract_value_t (*func) (abstract_value_t, abstract_value_t))
{
  if (x.kind == Value && y.kind == Value)
    {
      return semval_make_value (func (x.value, y.value));
    }
  else
    {
      return semval_undefined ();
    }
}

semval_t
semval_logic_not (semval_t s)
{
  return semval_unary (s, abstract_value_logic_not);
}

semval_t
semval_logic_and (semval_t x, semval_t y)
{
  return semval_binary (x, y, abstract_value_logic_and);
}

semval_t
semval_logic_or (semval_t x, semval_t y)
{
  return semval_binary (x, y, abstract_value_logic_or);
}

bool
semval_assignable (semval_t left, semval_t right)
{
  /* TODO:  Check the mutable flag. */
  if (!semval_is_reference (left))
    {
      return false;
    }

  if (!semval_is_value (right))
    {
      return false;
    }

  return reference_assignable (left.reference, right.value);
}

semval_t semval_select (semval_t s, string_t identifier)
{
  if (!semval_is_reference (s)) {
    return semval_undefined ();
  }

  return semval_make_reference (reference_select (s.reference, identifier));
}

bool semval_is_boolean (semval_t s)
{
  if (!semval_is_value (s)) {
    return false;
  }

  return abstract_value_is_boolean (s.value);
}
