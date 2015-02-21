#include "typed_value.hpp"
#include <stdlib.h>
#include "debug.hpp"
#include "symtab.hpp"
#include "type.hpp"
#include "method.hpp"
#include "function.hpp"
#include "action.hpp"

std::ostream&
operator<< (std::ostream& out, const typed_value_t& tv)
{
  if (tv.type)
    {
      out << *tv.type;
    }
  else
    {
      out << "(no type)";
    }

  switch (tv.kind)
    {
    case TV_MUTABLE:
      out << " mutable";
      break;
    case TV_IMMUTABLE:
      out << " immutable";
      break;
    case TV_FOREIGN:
      out << " foreign";
      break;
    case TV_CONSTANT:
      out << " constant";
      break;
    }

  out << " has_value=" << tv.has_value;

  return out;
}


typed_value_t
typed_value_make (const type_t* type, TypedValueKind kind)
{
  typed_value_t retval (type, kind);
  retval.has_value = false;
  return retval;
}

typed_value_t
typed_value_make_bool (const type_t * type, bool v)
{
  // TODO:  Why accept a type?
  assert (dynamic_cast<const bool_type_t*> (type) != NULL);
  typed_value_t retval;
  retval.type = type;
  retval.has_value = true;
  retval.bool_value = v;
  return retval;
}

typed_value_t
typed_value_make_int (const type_t * type, int64_t v)
{
  // TODO:  Why accept a type?
  assert (dynamic_cast<const int_type_t*> (type) != NULL);
  typed_value_t retval;
  retval.type = type;
  retval.has_value = true;
  retval.int_value = v;
  return retval;
}

typed_value_t
typed_value_make_uint (const type_t * type, uint64_t u)
{
  // TODO:  Why accept a type?
  assert (dynamic_cast<const uint_type_t*> (type) != NULL);
  typed_value_t retval;
  retval.type = type;
  retval.has_value = true;
  retval.uint_value = u;
  return retval;
}

typed_value_t
typed_value_make_string (const type_t * type, rtstring_t v)
{
  assert (dynamic_cast<const string_type_t*> (type) != NULL);
  typed_value_t retval;
  retval.type = type;
  retval.has_value = true;
  retval.string_value = v;
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
typed_value_make_reaction (reaction_t* reaction)
{
  typed_value_t retval;
  retval.type = reaction->reaction_type ();
  retval.has_value = true;
  retval.reaction_value = reaction;
  return retval;
}

typed_value_t
typed_value_make_nil (void)
{
  typed_value_t retval (nil_type_t::instance (), TV_CONSTANT);
  retval.has_value = true;
  return retval;
}

int64_t typed_value_to_index (typed_value_t tv)
{
  assert (tv.has_value);
  struct visitor : public const_type_visitor_t
  {
    typed_value_t tv;
    int64_t retval;

    visitor (const typed_value_t& t) : tv (t) { }

    void default_action (const type_t& type)
    {
      not_reached;
    }

    void visit (const int_type_t& type)
    {
      retval = tv.int_value;
    }
  };
  visitor v (tv);
  tv.type->accept (v);
  return v.retval;
}

typed_value_t
typed_value_dereference (const typed_value_t& in)
{
  struct visitor : public const_type_visitor_t
  {
    const typed_value_t& in;
    typed_value_t out;

    visitor (const typed_value_t& i) : in (i) { }

    void default_action (const type_t& type)
    {
      not_reached;
    }

    void visit (const pointer_type_t& type)
    {
      out = typed_value_make (type.base_type (), in.kind);
    }

    void visit (const pointer_to_foreign_type_t& type)
    {
      out = typed_value_make (type.base_type (), TV_FOREIGN);
    }

    void visit (const pointer_to_immutable_type_t& type)
    {
      out = typed_value_make (type.base_type (), std::max (TV_IMMUTABLE, in.kind));
    }
  };
  visitor v (in);
  in.type->accept (v);
  return v.out;
}

typed_value_t
typed_value_address_of (const typed_value_t& in)
{
  switch (in.kind)
    {
    case TV_MUTABLE:
      return typed_value_make (pointer_type_t::make (in.type), in.kind);
    case TV_IMMUTABLE:
      return typed_value_make (pointer_to_immutable_type_t::make (in.type), in.kind);
    case TV_FOREIGN:
      return typed_value_make (pointer_to_foreign_type_t::make (in.type), in.kind);
    case TV_CONSTANT:
      break;
    }

  not_reached;
}

typed_value_t
typed_value_binary_arithmetic (const typed_value_t& left_tv,
                               const typed_value_t& right_tv)
{
  if (!type_is_equivalent (left_tv.type, right_tv.type))
    {
      return typed_value_t ();
    }

  if (!type_is_arithmetic (left_tv.type))
    {
      return typed_value_t ();
    }

  typed_value_t retval = right_tv;
  if (left_tv.type->level () < right_tv.type->level ())
    {
      retval = left_tv;
    }

  // Don't inherit value.
  retval.has_value = false;

  if (left_tv.has_value && right_tv.has_value)
    {
      struct visitor : public const_type_visitor_t
      {
        void default_action (const type_t& type)
        {
          not_reached;
        }
      };
      visitor v;
      retval.type->accept (v);
    }

  return retval;
}
