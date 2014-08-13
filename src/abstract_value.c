#include "abstract_value.h"
#include "debug.h"

const char* abstract_value_to_string (abstract_value_t a)
{
  switch (a.kind)
    {
    case UntypedValue:
      return untyped_value_to_string (a.untyped_value);
    case TypedValue:
      unimplemented;
    case Typed:
      return type_to_string (a.typed);
    case UndefinedValue:
      return "undefined";
    }

  bug ("unhandled case");
}

AbstractValueKind
abstract_value_kind (abstract_value_t a)
{
  return a.kind;
}

static abstract_value_t
abstract_value_make_undefined (void)
{
abstract_value_t retval = { kind:UndefinedValue };
  return retval;
}

bool abstract_value_is_undefined (abstract_value_t a)
{
  return a.kind == UndefinedValue;
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
abstract_value_make_typed (const type_t * type,
                           select_result_t selected_field)
{
  if (type_is_undefined (type)) {
    return abstract_value_make_undefined ();
  }
  else {
    abstract_value_t retval = { kind: Typed, typed:type, selected_field: selected_field };
    return retval;
  }
}

const type_t *
abstract_value_get_typed (abstract_value_t a)
{
  assert (a.kind == Typed);
  return a.typed;
}

bool
abstract_value_is_typed (abstract_value_t a)
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
      v = abstract_value_make_typed (type_logic_not (v.typed), select_result_make_undefined ());
      break;
    }
  return v;
}

/* Modify x and y to have the same type. */
static void
abstract_value_homogenize (abstract_value_t * x, abstract_value_t * y)
{
  if (x->kind == y->kind)
    {
      /* Done. */
      return;
    }
  else if (x->kind < y->kind)
    {
      /* Change x->kind. */
      switch (x->kind)
	{
	case UntypedValue:
	  switch (y->kind)
	    {
	    case UntypedValue:
	      bug ("precondition violated");
	      break;
	    case TypedValue:
	      *x =
		abstract_value_make_typed_value (typed_value_from_untyped
						 (abstract_value_get_untyped_value
						  (*x),
						  typed_value_get_type
						  (abstract_value_get_typed_value
						   (*y))));
	      break;
	    case Typed:
	      *x =
		abstract_value_make_typed_value (typed_value_from_untyped
						 (abstract_value_get_untyped_value
						  (*x),
						  abstract_value_get_typed
						  (*y)));
	      break;
	    case UndefinedValue:
	      *x = abstract_value_make_undefined ();
	      break;
	    }
	  break;
	case TypedValue:
	  *x =
	    abstract_value_make_typed (typed_value_get_type
				       (abstract_value_get_typed_value (*x)), select_result_make_undefined ());
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
  else
    {
      /* Swap. */
      abstract_value_homogenize (y, x);
    }
}

static abstract_value_t
abstract_value_binary (abstract_value_t x, abstract_value_t y,
		       untyped_value_t (*untyped_func) (untyped_value_t,
							untyped_value_t),
		       const type_t * (*typed_func) (const type_t *,
						     const type_t *))
{
  abstract_value_homogenize (&x, &y);

  abstract_value_t retval;
  switch (x.kind)
    {
    case UndefinedValue:
      retval = abstract_value_make_undefined ();
      break;
    case UntypedValue:
      retval =
	abstract_value_make_untyped_value (untyped_func
					   (x.untyped_value,
					    y.untyped_value));
      break;
    case TypedValue:
      unimplemented;
    case Typed:
      retval = abstract_value_make_typed (typed_func (x.typed, y.typed), select_result_make_undefined ());
      break;
    }

  return retval;
}

abstract_value_t
abstract_value_logic_and (abstract_value_t x, abstract_value_t y)
{
  return abstract_value_binary (x, y, untyped_value_logic_and,
				type_logic_and);
}

abstract_value_t
abstract_value_logic_or (abstract_value_t x, abstract_value_t y)
{
  return abstract_value_binary (x, y, untyped_value_logic_or, type_logic_or);
}

bool
abstract_value_assignable (abstract_value_t target, abstract_value_t source)
{
  /*
     The target must be representable by the machine, therefore, it must have a type.
     If it has a value, then it was on the RHS which is not a valid target.
   */

  if (target.kind != Typed)
    {
      return false;
    }

  switch (source.kind)
    {
    case UntypedValue:
      return type_can_represent (target.typed, source.untyped_value);
    case TypedValue:
      unimplemented;
      break;
    case Typed:
      return type_assignable (target.typed, source.typed);
    case UndefinedValue:
      return false;
      break;
    }

  bug ("unhandled case");
}

abstract_value_t
abstract_value_dereference (abstract_value_t x)
{
  switch (x.kind)
    {
    case UntypedValue:
      unimplemented;
    case TypedValue:
      unimplemented;
    case Typed:
      return abstract_value_make_typed (type_dereference (x.typed), select_result_make_undefined ());
    case UndefinedValue:
      unimplemented;
    }

  bug ("unhandled case");
}

abstract_value_t
abstract_value_select (abstract_value_t v, string_t identifier)
{
  switch (v.kind)
    {
    case UntypedValue:
      unimplemented;
    case TypedValue:
      unimplemented;
    case Typed:
      {
        select_result_t result = type_select (v.typed, identifier);
        switch (result.kind)
          {
          case SelectResultField:
            return abstract_value_make_typed (field_type (result.field), result);
          case SelectResultReaction:
            return abstract_value_make_typed (reaction_type (result.reaction), result);
          case SelectResultUndefined:
            return abstract_value_make_undefined ();
          }
      }
      break;
    case UndefinedValue:
      unimplemented;
    }

  bug ("unhandled case");
}

bool
abstract_value_is_boolean (abstract_value_t v)
{
  switch (v.kind)
    {
    case UntypedValue:
      return untyped_value_is_bool (v.untyped_value);
    case TypedValue:
      unimplemented;
    case Typed:
      return type_is_boolean (v.typed);
    case UndefinedValue:
      unimplemented;
    }

  bug ("unhandled case");
}

bool abstract_value_is_bindable (abstract_value_t output,
                                 abstract_value_t input)
{
  return abstract_value_is_typed (output) && abstract_value_is_typed (input) && type_is_bindable (output.typed, input.typed);
}

action_t* abstract_value_get_reaction (abstract_value_t v)
{
  if (abstract_value_is_typed (v)) {
    return type_reaction_get_reaction (v.typed);
  }
  else {
    return NULL;
  }
}

bool abstract_value_check_arg (abstract_value_t v, size_t idx, abstract_value_t arg)
{
  if (v.kind != Typed)
    {
      return false;
    }

  switch (arg.kind)
    {
    case UntypedValue:
      unimplemented;
    case TypedValue:
      unimplemented;
    case Typed:
      return type_check_arg (v.typed, idx, arg.typed);
    case UndefinedValue:
      return false;
    }

  bug ("unhandled case");
}

abstract_value_t abstract_value_return_value (abstract_value_t v)
{
  if (v.kind != Typed)
    {
      return abstract_value_make_undefined ();
    }

  return abstract_value_make_typed (type_return_value (v.typed), select_result_make_undefined ());
}

select_result_t abstract_value_selected_field (abstract_value_t v)
{
  switch (v.kind)
    {
    case UntypedValue:
      unimplemented;
    case TypedValue:
      unimplemented;
    case Typed:
      return v.selected_field;
    case UndefinedValue:
      unimplemented;
    }

  bug ("unhandled case");
}
