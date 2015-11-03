#include "typed_value.hpp"
#include <stdlib.h>
#include "debug.hpp"
#include "Type.hpp"
#include "action.hpp"
#include "field.hpp"
#include <error.h>
#include "Callable.hpp"
#include "Template.hpp"

using namespace Type;

typed_value_t::typed_value_t (Callable* c)
  : type (c->type ())
  , kind (VALUE)
  , region (CONSTANT)
  , intrinsic_mutability (IMMUTABLE)
  , dereference_mutability (IMMUTABLE)
  , value (c)
  , has_offset (false)
{ }

typed_value_t::typed_value_t (::Template* t)
  : type (t->type ())
  , kind (VALUE)
  , region (CONSTANT)
  , intrinsic_mutability (IMMUTABLE)
  , dereference_mutability (IMMUTABLE)
  , value (t)
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

  struct visitor : public Visitor
  {
    typed_value_t& value;

    visitor (typed_value_t& v) : value (v) { }

    void default_action (const Type::Type& type)
    {
      type_not_reached (type);
    }

    void visit (const Int& type)
    {
      value.value.ref (type) = 0;
    }

    void visit (const Uint& type)
    {
      value.value.ref (type) = 0;
    }
  };

  visitor v (*this);
  type->Accept (v);
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
    case TYPE:
      out << " TYPE";
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

  out << ' ';
  if (kind == REFERENCE)
    {
      if (value.present)
        {
          out << value.reference_value ();
        }
    }
  else
    {
      value.print (out, type);
      low_value.print (out, type);
      high_value.print (out, type);
    }

  if (has_offset)
    {
      out << " offset=" << offset;
    }

  return out;
}

typed_value_t
typed_value_t::make_value (const Type::Type* type, Region region, Mutability intrinsic, Mutability dereference)
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
typed_value_t::make_ref (const Type::Type* type, Region region, Mutability intrinsic, Mutability dereference)
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
  typed_value_t retval = make_value (Nil::Instance (), CONSTANT, MUTABLE, MUTABLE);
  retval.value.present = true;
  return retval;
}

typed_value_t
typed_value_t::implicit_dereference (typed_value_t tv)
{
  assert (tv.type != NULL);
  tv.kind = VALUE;
  return tv;
}

typed_value_t
typed_value_t::dereference (typed_value_t in)
{
  assert (in.kind == VALUE);

  struct visitor : public Visitor
  {
    const typed_value_t& in;
    typed_value_t out;

    visitor (const typed_value_t& i) : in (i) { }

    void default_action (const Type::Type& type)
    {
      type_not_reached (type);
    }

    void visit (const Pointer& type)
    {
      out = typed_value_t::make_ref (type.Base (), HEAP, in.dereference_mutability, in.dereference_mutability);
    }
  };
  visitor v (in);
  in.type->Accept (v);
  return v.out;
}

typed_value_t
typed_value_t::address_of (typed_value_t in)
{
  assert (in.kind == REFERENCE);
  typed_value_t out = in;
  out.kind = VALUE;
  out.type = in.type->GetPointer ();
  out.dereference_mutability = std::max (in.intrinsic_mutability, in.dereference_mutability);
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

  ::Method* m = type_select_method (in.type, identifier);
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
typed_value_t::index (const Location& location, typed_value_t in, typed_value_t index)
{
  assert (in.type != NULL);
  assert (in.kind == REFERENCE);
  assert (index.type != NULL);
  assert (index.kind == VALUE);

  struct visitor : public Visitor
  {
    const Location& location;
    const typed_value_t& index;
    const Type::Type* result_type;

    visitor (const Location& loc, const typed_value_t& i) : location (loc), index (i), result_type (NULL) { }

    void default_action (const Type::Type& type)
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "cannot index expression of type %s (E74)", type.ToString ().c_str ());
    }

    void visit (const Array& type)
    {
      if (!type_is_integral (index.type))
        {
          error_at_line (-1, 0, location.File.c_str (), location.Line,
                         "cannot index array by value of type %s (E75)", index.type->ToString ().c_str ());
        }

      if (index.value.present && index.integral_value () >= type.dimension)
        {
          error_at_line (-1, 0, location.File.c_str (), location.Line,
                         "index out of bounds (E76)");
        }

      if (index.low_value.present && index.low_integral_value () < 0)
        {
          error_at_line (-1, 0, location.File.c_str (), location.Line,
                         "index out of bounds (E77)");
        }

      if (index.high_value.present && index.high_integral_value () > type.dimension)
        {
          error_at_line (-1, 0, location.File.c_str (), location.Line,
                         "index out of bounds (E78)");
        }

      result_type = type.Base ();
    }
  };
  visitor v (location, index);
  in.type->Accept (v);

  in.type = v.result_type;
  return in;
}

typed_value_t
typed_value_t::slice (const Location& location,
                      typed_value_t in,
                      typed_value_t low,
                      typed_value_t high)
{
  assert (in.type != NULL);
  assert (in.kind == REFERENCE);
  assert (low.type != NULL);
  assert (low.kind == VALUE);
  assert (high.type != NULL);
  assert (high.kind == VALUE);

  struct visitor : public Visitor
  {
    const Location& location;
    const typed_value_t& in;
    const typed_value_t& low;
    const typed_value_t& high;
    typed_value_t result;

    visitor (const Location& loc,
             const typed_value_t& i,
             const typed_value_t& l,
             const typed_value_t& h)
      : location (loc)
      , in (i)
      , low (l)
      , high (h)
    { }

    void
    default_action (const Type::Type& type)
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "cannot slice expression of type %s (E79)", type.ToString ().c_str ());
    }

    void
    visit (const Array& type)
    {
      if (!type_is_integral (low.type))
        {
          error_at_line (-1, 0, location.File.c_str (), location.Line,
                         "lower bound of slice expression is not integral (E80)");
        }

      if (!type_is_integral (high.type))
        {
          error_at_line (-1, 0, location.File.c_str (), location.Line,
                         "upper bound of slice expression is not integral (E81)");
        }

      if (low.value.present &&
          (low.integral_value () < 0 ||
           low.integral_value () >= type.dimension))
        {
          error_at_line (-1, 0, location.File.c_str (), location.Line,
                         "lower bound of slice expression is out of bounds (E82)");
        }

      if (high.value.present &&
          (high.integral_value () < 0 ||
           high.integral_value () > type.dimension))
        {
          error_at_line (-1, 0, location.File.c_str (), location.Line,
                         "upper bound of slice expression is out of bounds (E83)");
        }

      if (low.value.present &&
          high.value.present &&
          low.integral_value () > high.integral_value ())
        {
          error_at_line (-1, 0, location.File.c_str (), location.Line,
                         "lower bound of slice expression exceeds upper bound (E84)");
        }

      result = typed_value_t::make_value (type.Base ()->GetSlice (),
                                          in.region,
                                          in.intrinsic_mutability,
                                          in.dereference_mutability);
    }
  };
  visitor v (location, in, low, high);
  in.type->Accept (v);

  return v.result;
}

typed_value_t
typed_value_t::logic_not (typed_value_t in)
{
  assert (in.type != NULL);
  assert (in.kind == VALUE);

  struct visitor : public Visitor
  {
    const typed_value_t in;
    typed_value_t out;

    visitor (typed_value_t i) : in (i) { }

    void visit (const NamedType& type)
    {
      type.UnderlyingType ()->Accept (*this);
    }

    void visit (const Bool& type)
    {
      out = in;
      if (out.value.present)
        {
          out.value.ref (type) = !out.value.ref (type);
        }
    }
  };
  visitor v (in);
  in.type->Accept (v);

  return v.out;
}

typed_value_t
typed_value_t::merge (typed_value_t in)
{
  assert (in.type != NULL);
  assert (in.kind == typed_value_t::VALUE);

  typed_value_t out;
  const Type::Type* type = type_merge (in.type);
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
  const Type::Type* type = type_move (in.type);
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
//   struct visitor : public Visitor
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

//     void default_action (const Type& type)
//     {
//       type_not_reached (type);
//     }

//     void visit (const Int& type)
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
//   struct visitor : public Visitor
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

//     void default_action (const Type& type)
//     {
//       type_not_reached (type);
//     }

//     void visit (const Int& type)
//     {
//       func3 (f, result, result_type, left, type, right);
//     }
//   };

//   visitor v (f, result, result_type, left, right);
//   left.type->accept (v);
// }

template <typename F>
static void invoke (const F& f,
                    const Type::Type* type,
                    typed_value_t& result,
                    const typed_value_t& left,
                    const typed_value_t& right)
{
  struct visitor : public Visitor
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

    void default_action (const Type::Type& type)
    {
      type_not_reached (type);
    }

    void visit (const Bool& type)
    {
      f (result, type, left, right);
    }

    void visit (const Int& type)
    {
      f (result, type, left, right);
    }

    void visit (const Uint& type)
    {
      f (result, type, left, right);
    }
  };

  visitor v (f, result, left, right);
  type->Accept (v);
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
  const Type::Type*
  check (const Location& location,
         const char* operator_str,
         const Type::Type* left,
         const Type::Type* right) const
  {
    if (!((type_is_integral (left) || type_is_floating (left)) &&
          type_is_equal (left, right)))
      {
        error_at_line (-1, 0, location.File.c_str (), location.Line,
                       "incompatible types (%s) %s (%s) (E85)", left->ToString().c_str (), operator_str, right->ToString().c_str ());
      }

    return type_choose (left, right);
  }

  const Type::Type*
  dispatch_type (const Type::Type* result_type,
                 const Type::Type* left_type,
                 const Type::Type* right_type) const
  {
    return result_type;
  }
};

struct symmetric_integer_arithmetic : public needs_both
{
  const Type::Type*
  check (const Location& location,
         const char* operator_str,
         const Type::Type* left,
         const Type::Type* right) const
  {
    if (!(type_is_integral (left) &&
          type_is_equal (left, right)))
      {
        error_at_line (-1, 0, location.File.c_str (), location.Line,
                       "incompatible types (%s) %s (%s) (E86)", left->ToString().c_str (), operator_str, right->ToString().c_str ());
      }

    return type_choose (left, right);
  }

  const Type::Type*
  dispatch_type (const Type::Type* result_type,
                 const Type::Type* left_type,
                 const Type::Type* right_type) const
  {
    return result_type;
  }
};

struct shift_arithmetic : public needs_both
{
  const Type::Type*
  check (const Location& location,
         const char* operator_str,
         const Type::Type* left,
         const Type::Type* right) const
  {
    if (!(type_is_integral (left) &&
          type_is_unsigned_integral (right)))
      {
        error_at_line (-1, 0, location.File.c_str (), location.Line,
                       "incompatible types (%s) %s (%s) (E87)", left->ToString().c_str (), operator_str, right->ToString().c_str ());
      }

    return left;
  }

  const Type::Type*
  dispatch_type (const Type::Type* result_type,
                 const Type::Type* left_type,
                 const Type::Type* right_type) const
  {
    return result_type;
  }
};

struct comparable : public needs_both
{
  const Type::Type*
  check (const Location& location,
         const char* operator_str,
         const Type::Type* left,
         const Type::Type* right) const
  {
    if (!(type_is_comparable (left) &&
          (type_is_equal (left, right) ||
           type_is_pointer_compare (left, right))))
      {
        error_at_line (-1, 0, location.File.c_str (), location.Line,
                       "incompatible types (%s) %s (%s) (E88)", left->ToString().c_str (), operator_str, right->ToString().c_str ());
      }

    return Bool::Instance ();
  }

  const Type::Type*
  dispatch_type (const Type::Type* result_type,
                 const Type::Type* left_type,
                 const Type::Type* right_type) const
  {
    return left_type;
  }
};

struct orderable : public needs_both
{
  const Type::Type*
  check (const Location& location,
         const char* operator_str,
         const Type::Type* left,
         const Type::Type* right) const
  {
    if (!(type_is_orderable (left) &&
          type_is_equal (left, right)))
      {
        error_at_line (-1, 0, location.File.c_str (), location.Line,
                       "incompatible types (%s) %s (%s) (E89)", left->ToString().c_str (), operator_str, right->ToString().c_str ());
      }

    return Bool::Instance ();
  }

  const Type::Type*
  dispatch_type (const Type::Type* result_type,
                 const Type::Type* left_type,
                 const Type::Type* right_type) const
  {
    return left_type;
  }
};

struct symmetric_boolean
{
  const Type::Type*
  check (const Location& location,
         const char* operator_str,
         const Type::Type* left,
         const Type::Type* right) const
  {
    if (!(type_is_boolean (left) &&
          type_is_equal (left, right)))
      {
        error_at_line (-1, 0, location.File.c_str (), location.Line,
                       "incompatible types (%s) %s (%s) (E90)", left->ToString().c_str (), operator_str, right->ToString().c_str ());
      }

    return Bool::Instance ();
  }

  const Type::Type*
  dispatch_type (const Type::Type* result_type,
                 const Type::Type* left_type,
                 const Type::Type* right_type) const
  {
    return result_type;
  }
};

struct Multiply : public Location, public symmetric_arithmetic
{
  Multiply (const Location& loc)
    : Location (loc)
  { }

  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) * right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct Divide : public Location, public symmetric_arithmetic
{
  Divide (const Location& loc)
    : Location (loc)
  { }

  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    if (right.value.ref (type) == 0)
      {
        error_at_line (-1, 0, File.c_str (), Line,
                       "division by zero (E91)");
      }

    result.value.ref (type) = left.value.ref (type) / right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct Modulus : public Location, public symmetric_integer_arithmetic
{
  Modulus (const Location& l)
    : Location (l)
  { }

  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    if (right.value.ref (type) == 0)
      {
        error_at_line (-1, 0, File.c_str (), Line,
                       "division by zero (E92)");
      }

    result.value.ref (type) = left.value.ref (type) % right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct Add : public Location, public symmetric_arithmetic
{
  Add (const Location& loc)
    : Location (loc)
  { }

  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) + right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct Subtract : public Location, public symmetric_arithmetic
{
  Subtract (const Location& loc)
    : Location (loc)
  { }

  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) - right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Uint& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) - right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct LeftShift : public Location, public shift_arithmetic
{
  LeftShift (const Location& l)
    : Location (l)
  { }

  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) << right.integral_value ();
  }

  void
  operator() (typed_value_t& result,
              const Uint& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) << right.integral_value ();
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct RightShift : public Location, public shift_arithmetic
{
  RightShift (const Location& l)
    : Location (l)
  { }

  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) >> right.integral_value ();
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct BitAnd : public Location, public symmetric_integer_arithmetic
{
  BitAnd (const Location& l)
    : Location (l)
  { }

  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) & right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct BitAndNot : public Location, public symmetric_integer_arithmetic
{
  BitAndNot (const Location& l)
    : Location (l)
  { }

  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) & ~(right.value.ref (type));
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct BitOr : public Location, public symmetric_integer_arithmetic
{
  BitOr (const Location& l)
    : Location (l)
  { }

  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) | right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct BitXor : public Location, public symmetric_integer_arithmetic
{
  BitXor (const Location& l)
    : Location (l)
  { }


  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) ^ right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct Equal : public Location, public comparable
{
  Equal (const Location& l)
    : Location (l)
  { }


  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (*Bool::Instance ()) = left.value.ref (type) == right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Bool& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (*Bool::Instance ()) = left.value.ref (type) == right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct NotEqual : public Location, public comparable
{
  NotEqual (const Location& l)
    : Location (l)
  { }

  void
  operator() (typed_value_t& result,
              const Bool& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (*Bool::Instance ()) = left.value.ref (type) != right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (*Bool::Instance ()) = left.value.ref (type) != right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct LessThan : public Location, public orderable
{
  LessThan (const Location& l)
    : Location (l)
  { }


  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (*Bool::Instance ()) = left.value.ref (type) < right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct LessEqual : public Location, public orderable
{
  LessEqual (const Location& l)
    : Location (l)
  { }


  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (*Bool::Instance ()) = left.value.ref (type) <= right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct MoreThan : public Location, public orderable
{
  MoreThan (const Location& l)
    : Location (l)
  { }


  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (*Bool::Instance ()) = left.value.ref (type) > right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct MoreEqual : public Location, public orderable
{
  MoreEqual (const Location& l)
    : Location (l)
  { }


  void
  operator() (typed_value_t& result,
              const Int& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (*Bool::Instance ()) = left.value.ref (type) >= right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct LogicOr : public Location, public symmetric_boolean
{
  LogicOr (const Location& l)
    : Location (l)
  { }

  bool
  has_value (const typed_value_t& left,
             const typed_value_t& right) const
  {
    return left.value.present && (left.value.ref (*Bool::Instance ()) == true || right.value.present);
  }

  void
  operator() (typed_value_t& result,
              const Bool& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) || right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

struct LogicAnd : public Location, public symmetric_boolean
{
  LogicAnd (const Location& l)
    : Location (l)
  { }

  bool
  has_value (const typed_value_t& left,
             const typed_value_t& right) const
  {
    return left.value.present && (left.value.ref (*Bool::Instance ()) == false || right.value.present);
  }

  void
  operator() (typed_value_t& result,
              const Bool& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    result.value.ref (type) = left.value.ref (type) && right.value.ref (type);
  }

  void
  operator() (typed_value_t& result,
              const Type::Type& type,
              const typed_value_t& left,
              const typed_value_t& right) const
  {
    type_not_reached (type);
  }
};

typed_value_t
typed_value_t::binary (const Location& location,
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
struct cast_visitor : public Visitor
{
  Action& action;
  DefaultAction& defaultaction;

  cast_visitor (Action& a, DefaultAction& da) : action (a), defaultaction (da) { }

  virtual void visit (const Int& type)
  {
    action (type);
  }
  virtual void visit (const Int8& type)
  {
    action (type);
  }
  virtual void visit (const Uint& type)
  {
    action (type);
  }
  virtual void visit (const Uint8& type)
  {
    action (type);
  }
  virtual void visit (const Uint32& type)
  {
    action (type);
  }
  virtual void visit (const Uint64& type)
  {
    action (type);
  }
  virtual void visit (const Uint128& type)
  {
    action (type);
  }
  virtual void visit (const Float64& type)
  {
    action (type);
  }
  virtual void default_action (const Type::Type& type)
  {
    defaultaction (type);
  }
};

template <typename Type1, typename F>
struct DoubleDispatchAction2
{
  const Type1& type1;
  F& f;

  DoubleDispatchAction2 (const Type1& t1, F& func) : type1 (t1), f(func) { }

  template <typename Type2>
  void operator() (const Type2& type2)
  {
    f (type1, type2);
  }
};

struct DoubleDispatchDefaultAction2
{
  void operator() (const Type::Type& type)
  {
    type_not_reached (type);
  }
};

template <template <typename, typename> class Visitor2, typename F>
struct DoubleDispatchAction1
{
  const Type::Type* type2;
  F& f;

  DoubleDispatchAction1 (const Type::Type* t2, F& func)
    : type2 (t2)
    , f (func)
  { }

  template <typename Type1>
  void operator() (const Type1& type1)
  {
    DoubleDispatchAction2<Type1, F> a (type1, f);
    DoubleDispatchDefaultAction2 da;
    Visitor2<DoubleDispatchAction2<Type1, F>, DoubleDispatchDefaultAction2> visitor2 (a, da);
    type2->Accept (visitor2);
  }
};

struct DoubleDispatchDefaultAction1
{
  void operator() (const Type::Type& type)
  {
    type_not_reached (type);
  }
};

template <template <typename, typename> class Visitor1,
         template <typename, typename> class Visitor2,
         typename F>
static void double_dispatch (const Type::Type* type1,
                             const Type::Type* type2,
                             F& f)
{
  DoubleDispatchAction1<Visitor2, F> a (type2, f);
  DoubleDispatchDefaultAction1 da;
  Visitor1<DoubleDispatchAction1<Visitor2, F>, DoubleDispatchDefaultAction1> visitor1 (a, da);
  type1->Accept (visitor1);
}

struct castor
{
  typed_value_t& tv;

  castor (typed_value_t& t) : tv (t) { }

  template<typename Type1, typename Type2>
  void operator() (const Type1& type1, const Type2& type2)
  {
    tv.value.ref (type1) = tv.value.ref (type2);
  }
};

typed_value_t
typed_value_t::cast (const Location& location, const Type::Type* type, const typed_value_t tv)
{
  if (!type_is_castable (tv.type, type))
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "cannot cast expression of type %s to type %s (E93)", tv.type->ToString().c_str(), type->ToString().c_str());
    }

  return cast_exec (type, tv);
}

typed_value_t
typed_value_t::cast_exec (const Type::Type* type, const typed_value_t tv)
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

typed_value_t
typed_value_t::copy (const Location& location, typed_value_t tv)
{
  // TODO:  This function may no longer be necessary.
  if (type_strip_cast<Component> (tv.type) != NULL)
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "cannot copy components (E94)");
    }

  const Slice* st = type_strip_cast<Slice> (tv.type);
  if (st != NULL)
    {
      if (type_contains_pointer (st->Base ()))
        {
          error_at_line (-1, 0, location.File.c_str (), location.Line,
                         "copy leaks pointers (E95)");

        }
      // We will copy so a dereference can mutate the data.
      tv.intrinsic_mutability = MUTABLE;
      tv.dereference_mutability = MUTABLE;
    }

  return tv;
}

typed_value_t
typed_value_t::copy_exec (typed_value_t tv)
{
  const Slice* st = type_strip_cast<Slice> (tv.type);
  if (st!= NULL)
    {
      Slice::ValueType& slice = tv.value.ref (*st);
      size_t sz = st->Base ()->Size () * slice.length;
      void* ptr = malloc (sz);
      memcpy (ptr, slice.ptr, sz);
      slice.ptr = ptr;
    }

  return tv;
}

typed_value_t
typed_value_t::change (const Location& location, typed_value_t tv)
{
  const Type::Type* root_type = type_change (tv.type);
  if (root_type == NULL)
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "cannot change expression of type %s (E96)", tv.type->ToString ().c_str ());
    }

  tv.type = root_type;
  return tv;
}
