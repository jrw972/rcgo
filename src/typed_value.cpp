#include "typed_value.hpp"
#include <stdlib.h>
#include "debug.hpp"
#include "symtab.hpp"
#include "type.hpp"
#include "action.hpp"
#include "field.hpp"
#include <error.h>
#include "Callable.hpp"

typed_value_t::typed_value_t (Callable* c)
  : type (c->type ())
  , kind (VALUE)
  , region (CONSTANT)
  , intrinsic_mutability (IMMUTABLE)
  , dereference_mutability (IMMUTABLE)
  , value (c)
  , has_offset (false)
{ }

typed_value_t::typed_value_t (reaction_t* r)
  : type (r->reaction_type)
  , kind (VALUE)
  , region (CONSTANT)
  , intrinsic_mutability (IMMUTABLE)
  , dereference_mutability (IMMUTABLE)
  , value (r)
  , has_offset (false)
{ }

void
typed_value_t::zero ()
{
  assert (kind == VALUE);
  assert (value.present);

  struct visitor : public const_type_visitor_t
  {
    typed_value_t& value;

    visitor (typed_value_t& v) : value (v) { }

    void default_action (const type_t& type)
    {
      type_not_reached (type);
    }

    void visit (const int_type_t& type)
    {
      value.value.ref (type) = 0;
    }

    void visit (const uint_type_t& type)
    {
      value.value.ref (type) = 0;
    }
  };

  visitor v (*this);
  type->accept (v);
}

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
      return out;
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

  value.print (out, type);
  low_value.print (out, type);
  high_value.print (out, type);

  if (has_offset)
    {
      out << " offset=" << offset;
    }

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
  return tv;
}

typed_value_t
typed_value_t::make_range (const typed_value_t& low, const typed_value_t& high, Region region, Mutability intrinsic, Mutability dereference)
{
  assert (low.type == high.type);
  assert (low.kind == VALUE);
  assert (low.value.present);
  assert (high.kind == VALUE);
  assert (high.value.present);

  typed_value_t tv;
  tv.type = low.type;
  tv.kind = VALUE;
  tv.region = region;
  tv.intrinsic_mutability = intrinsic;
  tv.dereference_mutability = dereference;
  tv.low_value = low.value;
  tv.high_value = high.value;
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
  typed_value_t retval = make_value (nil_type_t::instance (), CONSTANT, MUTABLE, MUTABLE);
  retval.value.present = true;
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
      type_not_reached (type);
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
      out.type = f->type;
      out.has_offset = true;
      out.offset = f->offset;
      return out;
    }

  Method* m = type_select_method (in.type, identifier);
  if (m)
    {
      return make_ref (typed_value_t (m));
    }

  Initializer* i = type_select_initializer (in.type, identifier);
  if (i)
    {
      return make_ref (typed_value_t (i));
    }

  Getter* g = type_select_getter (in.type, identifier);
  if (g)
    {
      return make_ref (typed_value_t (g));
    }

  reaction_t* r = type_select_reaction (in.type, identifier);
  if (r)
    {
      return make_ref (typed_value_t (r));
    }

  return typed_value_t ();
}

typed_value_t
typed_value_t::index (const location_t& location, typed_value_t in, typed_value_t index)
{
  assert (in.type != NULL);
  assert (in.kind == REFERENCE);
  assert (index.type != NULL);
  assert (index.kind == VALUE);

  struct visitor : public const_type_visitor_t
  {
    const location_t& location;
    const typed_value_t& index;
    const type_t* result_type;

    visitor (const location_t& loc, const typed_value_t& i) : location (loc), index (i), result_type (NULL) { }

    void default_action (const type_t& type)
    {
      error_at_line (-1, 0, location.file, location.line,
                     "cannot index expression of type %s", type.to_string ().c_str ());
    }

    void visit (const array_type_t& type)
    {
      if (!type_is_integral (index.type))
        {
          error_at_line (-1, 0, location.file, location.line,
                         "cannot index array by value of type %s", index.type->to_string ().c_str ());
        }

      if (index.value.present && index.value.integral_value (index.type) >= type.dimension)
        {
          error_at_line (-1, 0, location.file, location.line,
                         "index out of bounds");
        }

      if (index.low_value.present && index.low_value.integral_value (index.type) < 0)
        {
          error_at_line (-1, 0, location.file, location.line,
                         "index out of bounds");
        }

      if (index.high_value.present && index.high_value.integral_value (index.type) > type.dimension)
        {
          error_at_line (-1, 0, location.file, location.line,
                         "index out of bounds");
        }

      result_type = type.base_type ();
    }
  };
  visitor v (location, index);
  in.type->accept (v);

  in.type = v.result_type;
  return in;
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
      if (out.value.present)
        {
          out.value.ref (type) = !out.value.ref (type);
        }
    }
  };
  visitor v (in);
  in.type->accept (v);

  return v.out;
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

// template <typename F, typename T, typename U, typename V>
// static void func4 (const F& f,
//                    typed_value_t& result,
//                    const T& result_type,
//                    const typed_value_t& left,
//                    const U& left_type,
//                    const typed_value_t& right,
//                    const V& right_type)
// {
//   typename T::ValueType result_value;
//   f (result_value, left.get (left_type), right.get (right_type));
//   result.set (result_type, result_value);
// }

// template <typename F, typename T, typename U>
// static void func3 (const F& f,
//                    typed_value_t& result,
//                    const T& result_type,
//                    const typed_value_t& left,
//                    const U& left_type,
//                    const typed_value_t& right)
// {
//   struct visitor : public const_type_visitor_t
//   {
//     const F& f;
//     typed_value_t& result;
//     const T& result_type;
//     const typed_value_t& left;
//     const U& left_type;
//     const typed_value_t& right;

//     visitor (const F& thef,
//              typed_value_t& res,
//              const T& rt,
//              const typed_value_t& l,
//              const U& lt,
//              const typed_value_t& r)
//       : f (thef)
//       , result (res)
//       , result_type (rt)
//       , left (l)
//       , left_type (lt)
//       , right (r)
//     { }

//     void default_action (const type_t& type)
//     {
//       type_not_reached (type);
//     }

//     void visit (const int_type_t& type)
//     {
//       func4 (f, result, result_type, left, left_type, right, type);
//     }

//     void visit (const uint_type_t& type)
//     {
//       func4 (f, result, result_type, left, left_type, right, type);
//     }
//   };

//   visitor v (f, result, result_type, left, left_type, right);
//   right.type->accept (v);
// }

// template <typename F, typename T>
// static void func2 (const F& f,
//                    typed_value_t& result,
//                    const T& result_type,
//                    const typed_value_t& left,
//                    const typed_value_t& right)
// {
//   struct visitor : public const_type_visitor_t
//   {
//     const F& f;
//     typed_value_t& result;
//     const T& result_type;
//     const typed_value_t& left;
//     const typed_value_t& right;

//     visitor (const F& thef,
//              typed_value_t& res,
//              const T& rt,
//              const typed_value_t& l,
//              const typed_value_t& r)
//       : f (thef)
//       , result (res)
//       , result_type (rt)
//       , left (l)
//       , right (r)
//     { }

//     void default_action (const type_t& type)
//     {
//       type_not_reached (type);
//     }

//     void visit (const int_type_t& type)
//     {
//       func3 (f, result, result_type, left, type, right);
//     }
//   };

//   visitor v (f, result, result_type, left, right);
//   left.type->accept (v);
// }

template <typename F>
static void invoke (const F& f,
                    const type_t* type,
                    typed_value_t& result,
                    const typed_value_t& left,
                    const typed_value_t& right)
{
  struct visitor : public const_type_visitor_t
  {
    const F& f;
    typed_value_t& result;
    const typed_value_t& left;
    const typed_value_t& right;

    visitor (const F& thef,
             typed_value_t& res,
             const typed_value_t& l,
             const typed_value_t& r)
      : f (thef)
      , result (res)
      , left (l)
      , right (r)
    { }

    void default_action (const type_t& type)
    {
      type_not_reached (type);
    }

    void visit (const bool_type_t& type)
    {
      f (result, type, left, right);
    }

    void visit (const int_type_t& type)
    {
      f (result, type, left, right);
    }

    void visit (const uint_type_t& type)
    {
      f (result, type, left, right);
    }
  };

  visitor v (f, result, left, right);
  type->accept (v);
}

template<typename T>
typed_value_t check (const T& t, const char* operator_str, typed_value_t left, typed_value_t right)
{
  typed_value_t result;
  result.type = t.check (t, operator_str, left.type, right.type);
  result.kind = typed_value_t::VALUE;
  result.region = typed_value_t::CONSTANT;
  result.intrinsic_mutability = IMMUTABLE;
  result.dereference_mutability = IMMUTABLE;
  result.value.present = t.has_value (left, right);
  if (result.value.present)
    {
      invoke (t, t.dispatch_type (result.type, left.type, right.type), result, left, right);
    }

  return result;
}

struct needs_both
{
  bool has_value (const typed_value_t& left, const typed_value_t& right) const
  {
    return left.value.present && right.value.present;
  }
};

struct symmetric_arithmetic : public needs_both
{
  const type_t*
  check (const location_t& location,
         const char* operator_str,
         const type_t* left,
         const type_t* right) const
  {
    if (!((type_is_integral (left) || type_is_floating (left)) &&
          type_is_equal (left, right)))
      {
        error_at_line (-1, 0, location.file, location.line,
                       "E12: incompatible types (%s) %s (%s)", left->to_string().c_str (), operator_str, right->to_string().c_str ());
      }

    return type_choose (left, right);
  }

  const type_t*
  dispatch_type (const type_t* result_type,
                 const type_t* left_type,
                 const type_t* right_type) const
  {
    return result_type;
  }
};

struct symmetric_integer_arithmetic : public needs_both
{
  const type_t*
  check (const location_t& location,
         const char* operator_str,
         const type_t* left,
         const type_t* right) const
  {
    if (!(type_is_integral (left) &&
          type_is_equal (left, right)))
      {
        error_at_line (-1, 0, location.file, location.line,
                       "E13: incompatible types (%s) %s (%s)", left->to_string().c_str (), operator_str, right->to_string().c_str ());
      }

    return type_choose (left, right);
  }

  const type_t*
  dispatch_type (const type_t* result_type,
                 const type_t* left_type,
                 const type_t* right_type) const
  {
    return result_type;
  }
};

struct shift_arithmetic : public needs_both
{
  const type_t*
  check (const location_t& location,
         const char* operator_str,
         const type_t* left,
         const type_t* right) const
  {
    if (!(type_is_integral (left) &&
          type_is_unsigned_integral (right)))
      {
        error_at_line (-1, 0, location.file, location.line,
                       "E14: incompatible types (%s) %s (%s)", left->to_string().c_str (), operator_str, right->to_string().c_str ());
      }

    return left;
  }

  const type_t*
  dispatch_type (const type_t* result_type,
                 const type_t* left_type,
                 const type_t* right_type) const
  {
    return result_type;
  }
};

struct comparable : public needs_both
{
  const type_t*
  check (const location_t& location,
         const char* operator_str,
         const type_t* left,
         const type_t* right) const
  {
    if (!(type_is_comparable (left) &&
          (type_is_equal (left, right) ||
           type_is_pointer_compare (left, right))))
      {
        error_at_line (-1, 0, location.file, location.line,
                       "E15: incompatible types (%s) %s (%s)", left->to_string().c_str (), operator_str, right->to_string().c_str ());
      }

    return bool_type_t::instance ();
  }

  const type_t*
  dispatch_type (const type_t* result_type,
                 const type_t* left_type,
                 const type_t* right_type) const
  {
    return left_type;
  }
};

struct orderable : public needs_both
{
  const type_t*
  check (const location_t& location,
         const char* operator_str,
         const type_t* left,
         const type_t* right) const
  {
    if (!(type_is_orderable (left) &&
          type_is_equal (left, right)))
      {
        error_at_line (-1, 0, location.file, location.line,
                       "E16: incompatible types (%s) %s (%s)", left->to_string().c_str (), operator_str, right->to_string().c_str ());
      }

    return bool_type_t::instance ();
  }

  const type_t*
  dispatch_type (const type_t* result_type,
                 const type_t* left_type,
                 const type_t* right_type) const
  {
    return left_type;
  }
};

struct symmetric_boolean
{
  const type_t*
  check (const location_t& location,
         const char* operator_str,
         const type_t* left,
         const type_t* right) const
  {
    if (!(type_is_boolean (left) &&
          type_is_equal (left, right)))
      {
        error_at_line (-1, 0, location.file, location.line,
                       "E17: incompatible types (%s) %s (%s)", left->to_string().c_str (), operator_str, right->to_string().c_str ());
      }

    return bool_type_t::instance ();
  }

  const type_t*
  dispatch_type (const type_t* result_type,
                 const type_t* left_type,
                 const type_t* right_type) const
  {
    return result_type;
  }
};

struct Multiply : public location_t, public symmetric_arithmetic
{
  Multiply (const location_t& loc)
    : location_t (loc)
  { }

  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) * right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct Divide : public location_t, public symmetric_arithmetic
{
  Divide (const location_t& loc)
    : location_t (loc)
  { }

  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    if (right.value.ref (type) == 0)
      {
        error_at_line (-1, 0, file, line,
                       "division by zero");
      }

    result.value.ref (type) = left.value.ref (type) / right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct Modulus : public location_t, public symmetric_integer_arithmetic
{
  Modulus (const location_t& l)
    : location_t (l)
  { }

  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    if (right.value.ref (type) == 0)
      {
        error_at_line (-1, 0, file, line,
                       "division by zero");
      }

    result.value.ref (type) = left.value.ref (type) % right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct Add : public location_t, public symmetric_arithmetic
{
  Add (const location_t& loc)
    : location_t (loc)
  { }

  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) + right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct Subtract : public location_t, public symmetric_arithmetic
{
  Subtract (const location_t& loc)
    : location_t (loc)
  { }

  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) - right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const uint_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) - right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct LeftShift : public location_t, public shift_arithmetic
{
  LeftShift (const location_t& l)
    : location_t (l)
  { }

  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) << right.value.integral_value (right.type);
  }

  void
  operator() (typed_value_t& result,
              const uint_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) << right.value.integral_value (right.type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct RightShift : public location_t, public shift_arithmetic
{
  RightShift (const location_t& l)
    : location_t (l)
  { }

  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) >> right.value.integral_value (right.type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct BitAnd : public location_t, public symmetric_integer_arithmetic
{
  BitAnd (const location_t& l)
    : location_t (l)
  { }

  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) & right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct BitAndNot : public location_t, public symmetric_integer_arithmetic
{
  BitAndNot (const location_t& l)
    : location_t (l)
  { }

  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) & ~(right.value.ref (type));
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct BitOr : public location_t, public symmetric_integer_arithmetic
{
  BitOr (const location_t& l)
    : location_t (l)
  { }

  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) | right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct BitXor : public location_t, public symmetric_integer_arithmetic
{
  BitXor (const location_t& l)
    : location_t (l)
  { }


  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) ^ right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct Equal : public location_t, public comparable
{
  Equal (const location_t& l)
    : location_t (l)
  { }


  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (*bool_type_t::instance ()) = left.value.ref (type) == right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct NotEqual : public location_t, public comparable
{
  NotEqual (const location_t& l)
    : location_t (l)
  { }


  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (*bool_type_t::instance ()) = left.value.ref (type) != right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct LessThan : public location_t, public orderable
{
  LessThan (const location_t& l)
    : location_t (l)
  { }


  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (*bool_type_t::instance ()) = left.value.ref (type) < right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct LessEqual : public location_t, public orderable
{
  LessEqual (const location_t& l)
    : location_t (l)
  { }


  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (*bool_type_t::instance ()) = left.value.ref (type) <= right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct MoreThan : public location_t, public orderable
{
  MoreThan (const location_t& l)
    : location_t (l)
  { }


  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (*bool_type_t::instance ()) = left.value.ref (type) > right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct MoreEqual : public location_t, public orderable
{
  MoreEqual (const location_t& l)
    : location_t (l)
  { }


  void
  operator() (typed_value_t& result,
              const int_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (*bool_type_t::instance ()) = left.value.ref (type) >= right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct LogicOr : public location_t, public symmetric_boolean
{
  LogicOr (const location_t& l)
    : location_t (l)
  { }

  bool
  has_value (const typed_value_t& left,
             const typed_value_t& right) const
  {
    return left.value.present && (left.value.ref (*bool_type_t::instance ()) == true || right.value.present);
  }

  void
  operator() (typed_value_t& result,
              const bool_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) || right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct LogicAnd : public location_t, public symmetric_boolean
{
  LogicAnd (const location_t& l)
    : location_t (l)
  { }

  bool
  has_value (const typed_value_t& left,
             const typed_value_t& right) const
  {
    return left.value.present && (left.value.ref (*bool_type_t::instance ()) == false || right.value.present);
  }

  void
  operator() (typed_value_t& result,
              const bool_type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) && right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const type_t& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

typed_value_t
typed_value_t::binary (const location_t& location,
                       BinaryArithmetic arithmetic,
                       typed_value_t left,
                       typed_value_t right)
{
  assert (left.type != NULL);
  assert (left.kind == typed_value_t::VALUE);
  assert (right.type != NULL);
  assert (right.kind == typed_value_t::VALUE);

  const char* operator_str = binary_arithmetic_symbol (arithmetic);

  switch (arithmetic)
    {
    case MULTIPLY:
      return check (Multiply (location), operator_str, left, right);
    case DIVIDE:
      return check (Divide (location), operator_str, left, right);
    case MODULUS:
      return check (Modulus (location), operator_str, left, right);
    case LEFT_SHIFT:
      return check (LeftShift (location), operator_str, left, right);
    case RIGHT_SHIFT:
      return check (RightShift (location), operator_str, left, right);
    case BIT_AND:
      return check (BitAnd (location), operator_str, left, right);
    case BIT_AND_NOT:
      return check (BitAndNot (location), operator_str, left, right);

    case ADD:
      return check (Add (location), operator_str, left, right);
    case SUBTRACT:
      return check (Subtract (location), operator_str, left, right);
    case BIT_OR:
      return check (BitOr (location), operator_str, left, right);
    case BIT_XOR:
      return check (BitXor (location), operator_str, left, right);

    case EQUAL:
      return check (Equal (location), operator_str, left, right);
    case NOT_EQUAL:
      return check (NotEqual (location), operator_str, left, right);
    case LESS_THAN:
      return check (LessThan (location), operator_str, left, right);
    case LESS_EQUAL:
      return check (LessEqual (location), operator_str, left, right);
    case MORE_THAN:
      return check (MoreThan (location), operator_str, left, right);
    case MORE_EQUAL:
      return check (MoreEqual (location), operator_str, left, right);

    case LOGIC_OR:
      return check (LogicOr (location), operator_str, left, right);

    case LOGIC_AND:
      return check (LogicAnd (location), operator_str, left, right);
    }

  not_reached;
}

template<typename Action, typename DefaultAction>
struct cast_visitor : public const_type_visitor_t {
  Action& action;
  DefaultAction& defaultaction;

  cast_visitor (Action& a, DefaultAction& da) : action (a), defaultaction (da) { }

  virtual void visit (const int_type_t& type) { action (type); }
  virtual void visit (const int8_type_t& type) { action (type); }
  virtual void visit (const uint_type_t& type) { action (type); }
  virtual void visit (const uint8_type_t& type) { action (type); }
  virtual void visit (const uint32_type_t& type) { action (type); }
  virtual void visit (const uint64_type_t& type) { action (type); }
  virtual void visit (const uint128_type_t& type) { action (type); }
  virtual void visit (const float64_type_t& type) { action (type); }
  virtual void default_action (const type_t& type) { defaultaction (type); }
};

template <typename Type1, typename F>
struct DoubleDispatchAction2 {
  const Type1& type1;
  F& f;

  DoubleDispatchAction2 (const Type1& t1, F& func) : type1 (t1), f(func) { }

  template <typename Type2>
  void operator() (const Type2& type2)
  {
    f (type1, type2);
  }
};

struct DoubleDispatchDefaultAction2 {
  void operator() (const type_t& type)
  {
    type_not_reached (type);
  }
};

template <template <typename, typename> class Visitor2, typename F>
struct DoubleDispatchAction1 {
  const type_t* type2;
  F& f;

  DoubleDispatchAction1 (const type_t* t2, F& func)
    : type2 (t2)
    , f (func)
  { }

  template <typename Type1>
  void operator() (const Type1& type1)
  {
    DoubleDispatchAction2<Type1, F> a (type1, f);
    DoubleDispatchDefaultAction2 da;
    Visitor2<DoubleDispatchAction2<Type1, F>, DoubleDispatchDefaultAction2> visitor2 (a, da);
    type2->accept (visitor2);
  }
};

struct DoubleDispatchDefaultAction1 {
  void operator() (const type_t& type)
  {
    type_not_reached (type);
  }
};

template <template <typename, typename> class Visitor1,
          template <typename, typename> class Visitor2,
          typename F>
static void double_dispatch (const type_t* type1,
                             const type_t* type2,
                             F& f)
{
  DoubleDispatchAction1<Visitor2, F> a (type2, f);
  DoubleDispatchDefaultAction1 da;
  Visitor1<DoubleDispatchAction1<Visitor2, F>, DoubleDispatchDefaultAction1> visitor1 (a, da);
  type1->accept (visitor1);
}

struct castor {
  typed_value_t& tv;

  castor (typed_value_t& t) : tv (t) { }

  template<typename Type1, typename Type2>
  void operator() (const Type1& type1, const Type2& type2)
  {
    tv.value.ref (type1) = tv.value.ref (type2);
  }
};

typed_value_t
typed_value_t::cast (const location_t& location, const type_t* type, const typed_value_t tv)
{
  if (!type_is_castable (tv.type, type))
    {
      error_at_line (-1, 0, location.file, location.line,
                     "E20: cannot cast expression of type %s to type %s", tv.type->to_string().c_str(), type->to_string().c_str());
    }

  return cast_exec (type, tv);
}

typed_value_t
typed_value_t::cast_exec (const type_t* type, const typed_value_t tv)
{
  typed_value_t retval = tv;
  retval.type = type;
  if (tv.value.present)
    {
      castor c (retval);
      double_dispatch<cast_visitor, cast_visitor> (type_strip (type), type_strip (tv.type), c);
    }

  return retval;
}
