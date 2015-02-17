#include "typed_value.hpp"
#include <stdlib.h>
#include "debug.hpp"
#include "symtab.hpp"
#include "type.hpp"
#include "method.hpp"
#include "function.hpp"

typed_value_t
typed_value_make (const type_t* type)
{
  typed_value_t retval = { type: type, has_value: false };
  return retval;
}

typed_value_t
typed_value_make_bool (const type_t * type, bool v)
{
  assert (dynamic_cast<const bool_type_t*> (type) != NULL ||
          dynamic_cast<const untyped_boolean_type_t*> (type) != NULL);
  typed_value_t retval;
  retval.type = type;
  retval.has_value = true;
  retval.bool_value = v;
  return retval;
}

typed_value_t
typed_value_make_uint (type_t * type, uint64_t u)
{
  assert (dynamic_cast<const untyped_integer_type_t*> (type) != NULL);
  typed_value_t retval;
  retval.type = type;
  retval.has_value = true;
  retval.uint_value = u;
  return retval;
}

typed_value_t
typed_value_make_string (const type_t * type, rtstring_t v)
{
  assert (dynamic_cast<const string_type_t*> (type) != NULL ||
          dynamic_cast<const untyped_string_type_t*> (type) != NULL);
  typed_value_t retval;
  retval.type = type;
  retval.has_value = true;
  retval.string_value = v;
  return retval;
}

typed_value_t
typed_value_make_integer (int64_t i)
{
  typed_value_t retval;
  retval.type = untyped_integer_type_t::instance ();
  retval.has_value = true;
  retval.integer_value = i;
  return retval;
}

typed_value_t
typed_value_make_method (method_t* method)
{
  typed_value_t retval;
  retval.type = method_type (method);
  retval.has_value = true;
  retval.method_value = method;
  return retval;
}

typed_value_t
typed_value_make_function (function_t* function)
{
  typed_value_t retval;
  retval.type = function_type (function);
  retval.has_value = true;
  retval.function_value = function;
  return retval;
}

typed_value_t
typed_value_make_nil (void)
{
  typed_value_t retval = { type: untyped_nil_type_t::instance (), has_value: true};
  return retval;
}

typed_value_t
typed_value_iota (void)
{
  typed_value_t retval = { type: untyped_integer_type_t::instance (), has_value: false };
  return retval;
}

void
typed_value_convert (typed_value_t& from, const typed_value_t& to)
{
  if (!type_is_equal (from.type, to.type) &&
      type_is_convertible (from.type, to.type))
    {
      // Conversion is necessary and possible.
      // Convert the value.
      if (from.has_value)
        {
          struct visitor : public const_type_visitor_t
          {
            typed_value_t& tv;
            visitor (typed_value_t& t) : tv (t) { }

            void visit (const named_type_t& type)
            {
              type.subtype ()->accept (*this);
            }

            void visit (const immutable_type_t& type)
            {
              type.base_type ()->accept (*this);
            }

            void visit (const bool_type_t& type)
            {
              // Must be an untyped boolean.
              // Value is already set.
            }

            void visit (const uint_type_t& type)
            {
              // Must be an untyped integer.
              tv.uint_value = tv.integer_value;
            }

            void visit (const pointer_type_t& type)
            {
              tv.pointer_value = NULL;
            }

            void visit (const pointer_to_immutable_type_t& type)
            {
              tv.pointer_value = NULL;
            }

            void default_action (const type_t& type)
            {
              not_reached;
            }
          };
          visitor v (from);
          to.type->accept (v);
        }

      from.type = to.type;
    }
}
