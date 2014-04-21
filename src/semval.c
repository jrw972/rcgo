#include "semval.h"
#include "type.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symtab.h"
#include "debug.h"

bool
typed_value_is_undefined (typed_value_t t)
{
  return t.type == NULL;
}

typed_value_t
typed_value_make_bool (type_t * type, bool v)
{
typed_value_t retval = { type: type, bool_value:v };
  return retval;
}

untyped_value_t
untyped_value_make_undefined (void)
{
untyped_value_t retval = { kind:UntypedUndefined };
  return retval;
}

bool
untyped_value_is_undefined (untyped_value_t u)
{
  return u.kind == UntypedUndefined;
}

untyped_value_t
untyped_value_make_bool (bool b)
{
untyped_value_t retval = { kind: UntypedBool, bool_value:b };
  return retval;
}

typed_value_t
untyped_value_to_typed (untyped_value_t u)
{
  typed_value_t retval;

  switch (u.kind)
    {
    case UntypedUndefined:
      unimplemented;

    case UntypedBool:
      {
	/* Look up bool.  This doesn't feel quite right as I am looking up something by name, but I'm not sure exactly what I need or even if this function will survive. */
	symbol_t *symbol = symtab_find (symtab_get_top (), "bool");
	assert (symbol != NULL);
	semval_t semval = symbol_get_semval (symbol);
	assert (semval_is_type (semval));
	retval =
	  typed_value_make_bool (semval_get_type (semval), u.bool_value);
      }
    }

  return retval;
}

untyped_value_t
untyped_value_logic_not (untyped_value_t u)
{
  if (u.kind == UntypedBool)
    {
      return untyped_value_make_bool (!u.bool_value);
    }
  else
    {
      return untyped_value_make_undefined ();
    }
}

abstract_value_t
abstract_value_make_undefined (void)
{
abstract_value_t retval = { kind:UndefinedValue };
  return retval;
}

abstract_value_t
abstract_value_make_untyped_value (untyped_value_t u)
{
  if (untyped_value_is_undefined (u))
    {
      return abstract_value_make_undefined ();
    }
  else
    {
    abstract_value_t retval = { kind: UntypedValue, untyped_value:u };
      return retval;
    }
}

abstract_value_t
abstract_value_make_typed_value (typed_value_t v)
{
  if (typed_value_is_undefined (v))
    {
      return abstract_value_make_undefined ();
    }
  else
    {
    abstract_value_t retval = { kind: TypedValue, typed_value:v };
      return retval;
    }
}

typed_value_t
abstract_value_get_typed_value (abstract_value_t a)
{
  assert (a.kind == TypedValue);
  return a.typed_value;
}

bool
abstract_value_is_typed_value (abstract_value_t a)
{
  return a.kind == TypedValue;
}

abstract_value_t
abstract_value_to_typed (abstract_value_t v)
{
  if (v.kind == UntypedValue)
    {
      v =
	abstract_value_make_typed_value (untyped_value_to_typed
					 (v.untyped_value));
    }
  return v;
}

abstract_value_t
abstract_value_logic_not (abstract_value_t v)
{
  switch (v.kind)
    {
    case UndefinedValue:
      /* Do nothing. */
      break;
    case UntypedValue:
      v =
	abstract_value_make_untyped_value (untyped_value_logic_not
					   (v.untyped_value));
      break;
    case TypedValue:
      unimplemented;
    case Typed:
      unimplemented;
    }
  return v;
}

static reference_t
reference_make (abstract_value_t value)
{
reference_t retval = { value:value };
  return retval;
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
semval_make_reference (semval_t s)
{
  assert (s.kind == Value);
semval_t retval = { kind: Reference, reference:reference_make (s.value) };
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
semval_make_type (type_t * type)
{
semval_t retval = { kind: Type, type:type };
  return retval;
}

bool
semval_is_type (semval_t s)
{
  return s.kind == Type;
}

type_t *
semval_get_type (semval_t s)
{
  assert (s.kind == Type);
  return s.type;
}

semval_t
semval_dereference (semval_t s)
{
  if (s.kind == Reference)
    {
      return semval_make_value (s.reference.value);
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

semval_t
semval_to_typed_value (semval_t s)
{
  return semval_unary (s, abstract_value_to_typed);
}

semval_t
semval_logic_not (semval_t s)
{
  return semval_unary (s, abstract_value_logic_not);
}
