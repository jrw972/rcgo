#include "untyped_value.h"
#include "debug.h"
#include "symtab.h"

const char *
untyped_value_to_string (untyped_value_t u)
{
  switch (u.kind)
    {
    case UntypedUndefined:
      return "undefined";
    case UntypedBool:
      if (u.bool_value)
	{
	  return "true";
	}
      else
	{
	  return "false";
	}
    }

  bug ("unhandled case");
}

static untyped_value_t
untyped_value_make_undefined (void)
{
untyped_value_t retval = { kind:UntypedUndefined };
  return retval;
}

UntypedValueKind
untyped_value_kind (untyped_value_t u)
{
  return u.kind;
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

bool
untyped_value_bool_value (untyped_value_t u)
{
  assert (u.kind == UntypedBool);
  return u.bool_value;
}

bool
untyped_value_is_bool (untyped_value_t u)
{
  return u.kind == UntypedBool;
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

untyped_value_t
untyped_value_logic_and (untyped_value_t x, untyped_value_t y)
{
  if (x.kind == UntypedBool && y.kind == UntypedBool)
    {
      return untyped_value_make_bool (x.bool_value && y.bool_value);
    }
  else
    {
      return untyped_value_make_undefined ();
    }
}

untyped_value_t
untyped_value_logic_or (untyped_value_t x, untyped_value_t y)
{
  if (x.kind == UntypedBool && y.kind == UntypedBool)
    {
      return untyped_value_make_bool (x.bool_value || y.bool_value);
    }
  else
    {
      return untyped_value_make_undefined ();
    }
}

size_t
untyped_value_size (untyped_value_t x)
{
  switch (x.kind)
    {
    case UntypedUndefined:
      not_reached;
    case UntypedBool:
      return 1;
    }

  not_reached;
}
