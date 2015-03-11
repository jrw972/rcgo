#include "typed_value.hpp"
#include <stdlib.h>
#include "debug.hpp"
#include "symtab.hpp"
#include "type.hpp"
#include "method.hpp"
#include "function.hpp"
#include "action.hpp"
#include "field.hpp"

std::ostream&
typed_value_t::print (std::ostream& out) const
{
  if (type)
    {
      out << *type;
    }
  else
    {
      out << "(no type)";
    }

  switch (kind)
    {
    case VALUE:
      out << " VALUE";
      break;
    case REFERENCE:
      out << " REFERENCE";
      break;
    }

  switch (region)
    {
    case CONSTANT:
      out << " constant";
      break;
    case STACK:
      out << " stack";
      break;
    case HEAP:
      out << " heap";
      break;
    }

  switch (intrinsic_mutability)
    {
    case MUTABLE:
      out << " mutable";
      break;
    case IMMUTABLE:
      out << " immutable";
      break;
    case FOREIGN:
      out << " foreign";
      break;
    }

  switch (dereference_mutability)
    {
    case MUTABLE:
      out << " mutable";
      break;
    case IMMUTABLE:
      out << " immutable";
      break;
    case FOREIGN:
      out << " foreign";
      break;
    }

  out << " has_value=" << has_value;
  out << " has_offset=" << has_offset;

  return out;
}

typed_value_t
typed_value_t::make_value (const type_t* type, Region region, Mutability intrinsic, Mutability dereference)
{
  typed_value_t tv;
  tv.type = type;
  tv.kind = VALUE;
  tv.region = region;
  tv.intrinsic_mutability = intrinsic;
  tv.dereference_mutability = dereference;
  tv.has_value = false;
  return tv;
}

typed_value_t
typed_value_t::make_ref (const type_t* type, Region region, Mutability intrinsic, Mutability dereference)
{
  typed_value_t tv;
  tv.type = type;
  tv.kind = REFERENCE;
  tv.region = region;
  tv.intrinsic_mutability = intrinsic;
  tv.dereference_mutability = dereference;
  tv.has_value = false;
  return tv;
}

typed_value_t
typed_value_t::make_ref (typed_value_t tv)
{
  assert (tv.type != NULL);
  assert (tv.kind == VALUE);
  tv.kind = REFERENCE;
  return tv;
}

typed_value_t
typed_value_t::nil (void)
{
  typed_value_t retval = make_value (nil_type_t::instance (), CONSTANT, IMMUTABLE, IMMUTABLE);
  retval.has_value = true;
  return retval;
}

typed_value_t
typed_value_t::implicit_dereference (typed_value_t tv)
{
  assert (tv.type != NULL);
  assert (tv.kind == REFERENCE);
  tv.kind = VALUE;
  return tv;
}

typed_value_t
typed_value_t::dereference (typed_value_t in)
{
  assert (in.kind == VALUE);

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
      out = typed_value_t::make_ref (type.base_type (), HEAP, in.dereference_mutability, in.dereference_mutability);
    }
  };
  visitor v (in);
  in.type->accept (v);
  return v.out;
}

typed_value_t
typed_value_t::address_of (typed_value_t in)
{
  assert (in.kind == REFERENCE);
  typed_value_t out = in;
  out.kind = VALUE;
  out.type = pointer_type_t::make (in.type);
  return out;
}

typed_value_t
typed_value_t::select (typed_value_t in, const std::string& identifier)
{
  assert (in.kind == REFERENCE);
  field_t* f = type_select_field (in.type, identifier);
  if (f)
    {
      typed_value_t out = in;
      out.type = field_type (f);
      out.has_offset = true;
      out.offset = field_offset (f);
      return out;
    }

  method_t* m = type_select_method (in.type, identifier);
  if (m)
    {
      return make_ref (typed_value_t (m));
    }

  reaction_t* r = type_select_reaction (in.type, identifier);
  if (r)
    {
      return make_ref (typed_value_t (r));
    }

  return typed_value_t ();
}

typed_value_t
typed_value_t::index (typed_value_t in, typed_value_t index)
{
  assert (in.type != NULL);
  assert (in.kind == REFERENCE);
  assert (index.type != NULL);
  assert (index.kind == VALUE);

  typed_value_t out;

  const type_t* t = type_index (in.type, index.type);
  if (t == NULL)
    {
      return out;
    }

  if (index.has_value)
    {
      struct visitor : public const_type_visitor_t
      {
        const type_t* type;
        typed_value_t index;
        bool flag;

        visitor (const type_t* t, typed_value_t i) : type (t), index (i), flag (false) { }

        void default_action (const type_t&)
        {
          not_reached;
        }

        void visit (const int_type_t&)
        {
          flag = type_is_index (type, index.int_value);
        }
      };
      visitor v (in.type, index);
      index.type->accept (v);

      if (!v.flag)
        {
          return out;
        }
    }

  out = in;
  out.type = t;
  return out;
}

typed_value_t
typed_value_t::logic_not (typed_value_t in)
{
  assert (in.type != NULL);
  assert (in.kind == VALUE);

  struct visitor : public const_type_visitor_t
  {
    const typed_value_t in;
    typed_value_t out;

    visitor (typed_value_t i) : in (i) { }

    void visit (const named_type_t& type)
    {
      type.subtype ()->accept (*this);
    }

    void visit (const bool_type_t& type)
    {
      out = in;
      if (out.has_value)
        {
          out.bool_value = !out.bool_value;
        }
    }
  };
  visitor v (in);
  in.type->accept (v);

  return v.out;
}

static typed_value_t
binary_logic (typed_value_t left, typed_value_t right, bool (*func) (bool, bool))
{
  assert (left.type != NULL);
  assert (left.kind == typed_value_t::VALUE);
  assert (right.type != NULL);
  assert (right.kind == typed_value_t::VALUE);

  typed_value_t result;

  if (!type_is_equivalent (left.type, right.type))
    {
      return result;
    }

  if (!type_is_boolean (left.type))
    {
      return result;
    }

  result.type = type_choose (left.type, right.type);
  result.kind = typed_value_t::VALUE;
  result.region = typed_value_t::CONSTANT;
  result.intrinsic_mutability = IMMUTABLE;
  result.dereference_mutability = IMMUTABLE;
  result.has_value = left.has_value && right.has_value;
  if (result.has_value)
    {
      result.bool_value = func (left.bool_value, right.bool_value);
    }

  return result;
}

static bool logic_or_func (bool x, bool y) { return x || y; }

typed_value_t
typed_value_t::logic_or (typed_value_t left, typed_value_t right)
{
  return binary_logic (left, right, logic_or_func);
}

static bool logic_and_func (bool x, bool y) { return x && y; }

typed_value_t
typed_value_t::logic_and (typed_value_t left, typed_value_t right)
{
  return binary_logic (left, right, logic_and_func);
}

static typed_value_t
compare (typed_value_t left, typed_value_t right)
{
  assert (left.type != NULL);
  assert (left.kind == typed_value_t::VALUE);
  assert (right.type != NULL);
  assert (right.kind == typed_value_t::VALUE);

  typed_value_t result;

  if (!type_is_equivalent (left.type, right.type))
    {
      return result;
    }

  const type_t* t = type_choose (left.type, right.type);
  if (!type_is_comparable (t))
    {
      return result;
    }

  result.type = bool_type_t::instance ();
  result.kind = typed_value_t::VALUE;
  result.region = typed_value_t::CONSTANT;
  result.intrinsic_mutability = IMMUTABLE;
  result.dereference_mutability = IMMUTABLE;

  if (left.has_value && right.has_value)
    {
      result.has_value = true;
      unimplemented;
      // result.bool_value = ??
    }

  return result;
}

typed_value_t
typed_value_t::equal (typed_value_t left, typed_value_t right)
{
  return compare (left, right);
}

typed_value_t
typed_value_t::not_equal (typed_value_t left, typed_value_t right)
{
  return compare (left, right);
}

typed_value_t
typed_value_t::merge (typed_value_t in)
{
  assert (in.type != NULL);
  assert (in.kind == typed_value_t::VALUE);

  typed_value_t out;
  const type_t* type = type_merge (in.type);
  if (type == NULL)
    {
      return out;
    }

  out = in;
  out.type = type;
  out.dereference_mutability = MUTABLE;

  return out;
}

typed_value_t
typed_value_t::move (typed_value_t in)
{
  assert (in.type != NULL);
  assert (in.kind == typed_value_t::VALUE);

  typed_value_t out;
  const type_t* type = type_move (in.type);
  if (type == NULL)
    {
      return out;
    }

  out = in;
  out.type = type;
  out.dereference_mutability = MUTABLE;

  return out;
}

typed_value_t
typed_value_t::add (typed_value_t left, typed_value_t right)
{
  assert (left.type != NULL);
  assert (left.kind == typed_value_t::VALUE);
  assert (right.type != NULL);
  assert (right.kind == typed_value_t::VALUE);

  typed_value_t result;

  if (!type_is_equivalent (left.type, right.type))
    {
      return result;
    }

  if (!type_is_arithmetic (left.type))
    {
      return result;
    }

  result.type = type_choose (left.type, right.type);
  result.kind = typed_value_t::VALUE;
  result.region = typed_value_t::CONSTANT;
  result.intrinsic_mutability = IMMUTABLE;
  result.dereference_mutability = IMMUTABLE;
  result.has_value = left.has_value && right.has_value;

  if (result.has_value)
    {
      unimplemented;
    }

  return result;
}
