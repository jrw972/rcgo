#include "untyped_value.h"
#include "debug.h"
#include "symtab.h"
#include <string.h>

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
    case UntypedString:
      unimplemented;
    }

  bug ("unhandled case");
}

static untyped_value_t
untyped_value_make_undefined (void)
{
untyped_value_t retval = { kind:UntypedUndefined };
  return retval;
}

untyped_value_t
untyped_value_make_bool (bool b)
{
untyped_value_t retval = { kind: UntypedBool, bool_value:b };
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

untyped_value_t
untyped_value_make_string (rtstring_t b)
{
untyped_value_t retval = { kind: UntypedString, string_value:b };
  return retval;
}
