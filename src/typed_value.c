#include "typed_value.h"
#include <stdlib.h>
#include "debug.h"
#include "symtab.h"
#include "type.h"
#include "method.h"

typed_value_t typed_value_make (type_t * type)
{
  typed_value_t retval = { type: type, has_value: false };
  return retval;
}

typed_value_t
typed_value_make_bool (type_t * type, bool v)
{
  assert (type_kind (type) == TypeBool || type_kind (type) == UntypedBool);
  typed_value_t retval = { type: type, has_value: true, bool_value:v };
  return retval;
}

typed_value_t typed_value_make_uint (type_t * type, uint64_t u)
{
  assert (type_kind (type) == TypeUint);
  typed_value_t retval = { type: type, has_value: true, uint_value:u };
  return retval;
}

typed_value_t
typed_value_make_string (type_t * type, rtstring_t v)
{
  assert (type_kind (type) == TypeString || type_kind (type) == UntypedString);
  typed_value_t retval = { type: type, has_value: true, string_value:v };
  return retval;
}

typed_value_t typed_value_make_integer (int64_t i)
{
  typed_value_t retval = { type: type_make_untyped_integer (), has_value: true, integer_value:i };
  return retval;
}

typed_value_t typed_value_make_method (method_t* method)
{
  typed_value_t retval = { type: method_type (method), has_value: true, method_value:method };
  return retval;
}

typed_value_t typed_value_make_nil (void)
{
  typed_value_t retval = { type: type_make_untyped_nil (), has_value: true};
  return retval;
}

bool typed_value_is_boolean (typed_value_t t)
{
  return type_kind (t.type) == TypeBool || type_kind (t.type) == UntypedBool;
}

typed_value_t typed_value_logic_not (typed_value_t t)
{
  assert (typed_value_is_boolean (t));
  if (t.has_value)
    {
      return typed_value_make_bool (t.type, !t.bool_value);
    }
  else
    {
      return typed_value_make (t.type);
    }
}

bool typed_value_can_dereference (typed_value_t t)
{
  return type_is_pointer (t.type);
}

typed_value_t typed_value_dereference (typed_value_t t)
{
  assert (typed_value_can_dereference (t));
  return typed_value_make (type_pointer_base_type (t.type));
}

bool typed_value_can_select (typed_value_t tv, string_t name)
{
  return type_select (tv.type, name) != NULL;
}

typed_value_t typed_value_select (typed_value_t tv, string_t name)
{
  // TODO:  If selected a method, func, etc. remember what was selected.
  return typed_value_make (type_select (tv.type, name));
}

bool typed_value_is_untyped (typed_value_t tv)
{
  return type_is_untyped (tv.type);
}

bool typed_value_can_convert (typed_value_t tv, type_t* type)
{
  assert (typed_value_is_untyped (tv));
  assert (!type_is_untyped (type));

  switch (type_kind (type))
    {
    case TypeBool:
      // Converting to bool.
      return type_kind (tv.type) == UntypedBool;

    case TypeComponent:
      unimplemented;
    case TypeFieldList:
      unimplemented;
    case TypeFunc:
      unimplemented;
        case TypeImmutable:
          unimplemented;

    case TypePointer:
      return type_kind (tv.type) == UntypedNil;

    case TypePort:
      unimplemented;
    case TypeReaction:
      unimplemented;
    case TypeSignature:
      unimplemented;

    case TypeString:
      // Converting to string.
      return type_kind (tv.type) == UntypedString;

    case TypeStruct:
      unimplemented;

    case TypeUint:
      // Converting to uint.
      // TODO:  Check that tv.integer_value can be represented by a uint.
      return type_kind (tv.type) == UntypedInteger;

    case TypeVoid:
      unimplemented;
    case TypeUndefined:
      unimplemented;

    case UntypedUndefined:
      unimplemented;
    case UntypedNil:
      unimplemented;

    case UntypedBool:
      unimplemented;
    case UntypedInteger:
      unimplemented;
    case UntypedString:
      unimplemented;
    }

  not_reached;
}

typed_value_t typed_value_convert (typed_value_t tv, type_t* type)
{
  assert (typed_value_can_convert (tv, type));
  if (tv.has_value)
    {
      switch (type_kind (type))
        {
        case TypeBool:
          // Value is already in bool_value.
          break;

        case TypeComponent:
          unimplemented;
        case TypeFieldList:
          unimplemented;
        case TypeFunc:
          unimplemented;
        case TypeImmutable:
          unimplemented;

        case TypePointer:
          tv.pointer_value = NULL;
          break;

        case TypePort:
          unimplemented;
        case TypeReaction:
          unimplemented;
        case TypeSignature:
          unimplemented;

        case TypeString:
          // Value is already in string_value.
          break;

        case TypeStruct:
          unimplemented;

        case TypeUint:
          tv.uint_value = tv.integer_value;
          break;

        case TypeVoid:
          unimplemented;
        case TypeUndefined:
          unimplemented;

        case UntypedUndefined:
          unimplemented;
    case UntypedNil:
      unimplemented;
        case UntypedBool:
          unimplemented;
        case UntypedInteger:
          unimplemented;
        case UntypedString:
          unimplemented;
        }
    }

  tv.type = type;
  return tv;
}

bool typed_value_convertible (typed_value_t target, typed_value_t source)
{
  return type_convertible (target.type, source.type);
}

bool typed_value_is_arithmetic (typed_value_t tv)
{
  return type_is_arithmetic (tv.type);
}
