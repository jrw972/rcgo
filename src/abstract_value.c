#include "abstract_value.h"
#include "debug.h"

void
abstract_value_print (abstract_value_t a)
{
  switch (a.kind) {
  case UntypedValue:
    printf ("UntypedValue");
    break;
  case TypedValue:
    printf ("TypedValue");
    break;
  case Typed:
    printf ("Typed");
    break;
  case UndefinedValue:
    printf ("UndefinedValue");
    break;
  }
}
AbstractValueKind
abstract_value_kind (abstract_value_t a)
{
  return a.kind;
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

untyped_value_t
abstract_value_get_untyped_value (abstract_value_t a)
{
  assert (a.kind == UntypedValue);
  return a.untyped_value;
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
abstract_value_make_typed (const type_t* type)
{
  abstract_value_t retval = { kind: Typed, typed:type };
  return retval;
}

const type_t*
abstract_value_get_typed (abstract_value_t a)
{
  assert (a.kind == Typed);
  return a.typed;
}

bool abstract_value_is_typed (abstract_value_t a)
{
  return a.kind == Typed;
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
      v = abstract_value_make_typed (type_logic_not (v.typed));
      break;
    }
  return v;
}

/* Modify x and y to have the same type. */
static void
abstract_value_homogenize (abstract_value_t* x,
                           abstract_value_t* y)
{
  if (x->kind == y->kind) {
    /* Done. */
    return;
  }
  else if (x->kind < y->kind) {
    /* Change x->kind. */
    switch (x->kind) {
    case UntypedValue:
      switch (y->kind) {
      case UntypedValue:
        bug("precondition violated");
        break;
      case TypedValue:
        *x = abstract_value_make_typed_value (typed_value_from_untyped (abstract_value_get_untyped_value (*x),
                                                                            typed_value_get_type (abstract_value_get_typed_value (*y))));
        break;
      case Typed:
        *x = abstract_value_make_typed_value (typed_value_from_untyped (abstract_value_get_untyped_value (*x),
                                                                            abstract_value_get_typed (*y)));
        break;
      case UndefinedValue:
        *x = abstract_value_make_undefined ();
        break;
      }
      break;
    case TypedValue:
      *x = abstract_value_make_typed (typed_value_get_type (abstract_value_get_typed_value (*x)));
      break;
    case Typed:
      *x = abstract_value_make_undefined ();
      break;
    case UndefinedValue:
      bug ("precondition violated");
      break;
    }
    abstract_value_homogenize (x, y);
  }
  else {
    /* Swap. */
    abstract_value_homogenize (y, x);
  }
}

abstract_value_t
abstract_value_logic_and (abstract_value_t x,
                          abstract_value_t y)
{
  abstract_value_homogenize (&x, &y);

  abstract_value_t retval;
  switch (x.kind)
    {
    case UndefinedValue:
      retval = abstract_value_make_undefined ();
      break;
    case UntypedValue:
      retval = abstract_value_make_untyped_value (untyped_value_logic_and (x.untyped_value, y.untyped_value));
      break;
    case TypedValue:
      unimplemented;
    case Typed:
      retval = abstract_value_make_typed (type_logic_and (x.typed, y.typed));
      break;
    }

  return retval;
}

abstract_value_t
abstract_value_logic_or (abstract_value_t x,
                          abstract_value_t y)
{
  abstract_value_homogenize (&x, &y);

  abstract_value_t retval;
  switch (x.kind)
    {
    case UndefinedValue:
      retval = abstract_value_make_undefined ();
      break;
    case UntypedValue:
      retval = abstract_value_make_untyped_value (untyped_value_logic_or (x.untyped_value, y.untyped_value));
      break;
    case TypedValue:
      unimplemented;
    case Typed:
      unimplemented;
    }

  return retval;
}

bool
abstract_value_assignable (abstract_value_t target,
                           abstract_value_t source)
{
  /*
     The target must be representable by the machine, therefore, it must have a type.
     If it has a value, then it was on the RHS which is not a valid target.
   */

  if (target.kind != Typed) {
    return false;
  }

  switch (source.kind) {
  case UntypedValue:
    return type_can_represent (target.typed, source.untyped_value);
  case TypedValue:
    unimplemented;
    break;
  case Typed:
    return type_assignable (target.typed, source.typed);
  case UndefinedValue:
    unimplemented;
    break;
  }

  bug ("unhandled case");
}
