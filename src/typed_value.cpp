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

static void
RequireIdentical (const Location& location, const char* op, const Type::Type* left, const Type::Type* right)
{
  if (!Type::Identitical (left, right))
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "%s cannot be applied to %s and %s (E93)", op, left->ToString ().c_str (), right->ToString ().c_str ());
    }
}

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

template <typename T>
struct SingleDispatchVisitor : public Type::Visitor
{
  T& t;

  SingleDispatchVisitor (T& t_) : t (t_) { }

  void default_action (const Type::Type& type)
  {
    type_not_reached (type);
  }

  void visit (const Type::Bool& type)
  {
    t (type);
  }

  void visit (const Type::Int& type)
  {
    t (type);
  }

  void visit (const Type::Uint& type)
  {
    t (type);
  }

  void visit (const Type::Pointer& type)
  {
    t (type);
  }

  void visit (const Type::Boolean& type)
  {
    t (type);
  }

  void visit (const Type::Integer& type)
  {
    t (type);
  }
};

template <typename T>
static void
singleDispatch (const Type::Type* type, T& t)
{
  SingleDispatchVisitor<T> v (t);
  type->Accept (v);
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

void
typed_value_t::RequireValue (const Location location) const
{
  if (!IsValue ())
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "required type (E24)");
    }
}

template <typename T, typename T1>
struct visitor2 : public Type::Visitor
{
  const T1& type1;
  T& t;

  visitor2 (const T1& t1, T& t_) : type1 (t1), t (t_) { }

  void default_action (const Type::Type& type)
  {
    type_not_reached (type);
  }

  void visit (const Type::Int& type2)
  {
    t (type1, type2);
  }

  void visit (const Type::Uint& type2)
  {
    t (type1, type2);
  }

  void visit (const Type::Boolean& type2)
  {
    t (type1, type2);
  }

  void visit (const Type::Integer& type2)
  {
    t (type1, type2);
  }

  void visit (const Type::Nil& type2)
  {
    t (type1, type2);
  }
};

template <typename T, typename T1>
static void doubleDispatchHelper (const T1& type1, const Type::Type* type2, T& t)
{
  visitor2<T, T1> v (type1, t);
  type2->Accept (v);
}

template <typename T>
struct visitor1 : public Type::Visitor
{
  const Type::Type* type2;
  T& t;
  visitor1 (const Type::Type* t2, T& t_) : type2 (t2), t (t_) { }

  void default_action (const Type::Type& type)
  {
    type_not_reached (type);
  }

  void visit (const Type::Bool& type)
  {
    doubleDispatchHelper (type, type2, t);
  }

  void visit (const Type::Int& type)
  {
    doubleDispatchHelper (type, type2, t);
  }

  void visit (const Type::Uint& type)
  {
    doubleDispatchHelper (type, type2, t);
  }

  void visit (const Type::Pointer& type)
  {
    doubleDispatchHelper (type, type2, t);
  }

  void visit (const Type::Integer& type)
  {
    doubleDispatchHelper (type, type2, t);
  }
};

template <typename T>
static void doubleDispatch (const Type::Type* type1, const Type::Type* type2, T& t)
{
  visitor1<T> v (type2, t);
  type1->Accept (v);
}

struct ConvertImpl
{
  const Location& location;
  typed_value_t& out;
  const typed_value_t& in;

  ConvertImpl (const Location& loc, typed_value_t& o, const typed_value_t& i) : location (loc), out (o), in (i) { }

  void operator() (const Type::Bool& type1, const Type::Boolean& type2)
  {
    out.value.ref (type1) = in.value.ref (type2);
  }
  void operator() (const Type::Int& type1, const Type::Integer& type2)
  {
    out.value.ref (type1) = in.value.ref (type2);
  }
  void operator() (const Type::Uint& type1, const Type::Integer& type2)
  {
    out.value.ref (type1) = in.value.ref (type2);
  }
  void operator() (const Type::Pointer& type1, const Type::Nil& type2)
  {
    out.value.ref (type1) = NULL;
  }

  template <typename T1, typename T2>
  void operator() (const T1& type1, const T2& type2)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   "cannot convert %s to %s (E49)", in.type->ToString ().c_str (), out.type->ToString ().c_str ());
  }

};

typed_value_t
typed_value_t::Convert (const Location& location, const Type::Type* type) const
{
  assert (!IsError ());
  RequireValue (location);
  typed_value_t out = *this;
  out.type = type;
  ConvertImpl c (location, out, *this);
  doubleDispatch (type->UnderlyingType (), this->type, c);
  return out;
}

template <typename T>
typed_value_t
comparison (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  assert (!left.IsError ());
  assert (!right.IsError());
  left.RequireValue (location);
  right.RequireValue (location);
  typed_value_t out = typed_value_t::make_value (Type::Boolean::Instance (), typed_value_t::STACK, IMMUTABLE, IMMUTABLE);
  T c (location, out, left, right);
  RequireIdentical (location, c.Op (), left.type, right.type);
  singleDispatch (left.type->UnderlyingType (), c);
  return out;
}

template <typename O>
struct Compare
{
  const Location& location;
  typed_value_t& out;
  const typed_value_t& left;
  const typed_value_t& right;
  Compare (const Location& loc, typed_value_t& o, const typed_value_t& l, const typed_value_t& r) : location (loc), out (o), left (l), right (r) { }

  const char* Op () const
  {
    return O::Op ();
  }

  template<typename T>
  void doit (const T& type)
  {
    out.value.ref (*Type::Boolean::Instance ()) = O () (left.value.ref (type), right.value.ref (type));
    out.value.present = left.value.present && right.value.present;
  }

  void operator() (const Type::Bool& type)
  {
    doit (type);
  }
  void operator() (const Type::Int& type)
  {
    doit (type);
  }
  void operator() (const Type::Uint& type)
  {
    doit (type);
  }
  void operator() (const Type::Pointer& type)
  {
    doit (type);
  }
  void operator() (const Type::Boolean& type)
  {
    doit (type);
  }
  void operator() (const Type::Integer& type)
  {
    doit (type);
  }

  template <typename T>
  void operator() (const T& type)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   "%s cannot be applied to %s (E25)", Op (), type.ToString ().c_str ());
  }
};

struct EqualOp {
  static const char* Op ()
  {
    return "==";
  }

  template <typename T>
  bool operator() (const T& t1, const T& t2) const
  {
    return t1 == t2;
  }
};

typed_value_t
typed_value_t::Equal (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return comparison<Compare <EqualOp> > (location, left, right);
}

struct NotEqualOp {
  static const char* Op ()
  {
    return "!=";
  }

  template <typename T>
  bool operator() (const T& t1, const T& t2) const
  {
    return t1 != t2;
  }
};

typed_value_t
typed_value_t::NotEqual (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return comparison<Compare <NotEqualOp> > (location, left, right);
}

template <typename O>
struct Order
{
  const Location& location;
  typed_value_t& out;
  const typed_value_t& left;
  const typed_value_t& right;
  Order (const Location& loc, typed_value_t& o, const typed_value_t& l, const typed_value_t& r) : location (loc), out (o), left (l), right (r) { }

  const char* Op () const
  {
    return O::Op ();
  }

  template<typename T>
  void doit (const T& type)
  {
    out.value.ref (*Type::Boolean::Instance ()) = O () (left.value.ref (type), right.value.ref (type));
    out.value.present = left.value.present && right.value.present;
  }

  void operator() (const Type::Int& type)
  {
    doit (type);
  }
  void operator() (const Type::Integer& type)
  {
    doit (type);
  }

  template <typename T>
  void operator() (const T& type)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   "%s cannot be applied to %s (E91)", Op (), type.ToString ().c_str ());

  }
};

struct LessThanOp {
  static const char* Op ()
  {
    return "<";
  }

  template <typename T>
  bool operator() (const T& t1, const T& t2) const
  {
    return t1 < t2;
  }
};

typed_value_t
typed_value_t::LessThan (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return comparison<Order<LessThanOp> > (location, left, right);
}

struct LessEqualOp {
  static const char* Op ()
  {
    return "<=";
  }

  template <typename T>
  bool operator() (const T& t1, const T& t2) const
  {
    return t1 <= t2;
  }
};

typed_value_t
typed_value_t::LessEqual (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return comparison<Order<LessEqualOp> > (location, left, right);
}

struct MoreThanOp {
  static const char* Op ()
  {
    return ">";
  }

  template <typename T>
  bool operator() (const T& t1, const T& t2) const
  {
    return t1 > t2;
  }
};

typed_value_t
typed_value_t::MoreThan (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return comparison<Order <MoreThanOp> > (location, left, right);
}

struct MoreEqualOp {
  static const char* Op ()
  {
    return ">=";
  }

  template <typename T>
  bool operator() (const T& t1, const T& t2) const
  {
    return t1 >= t2;
  }
};

typed_value_t
typed_value_t::MoreEqual (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return comparison<Order <MoreEqualOp> > (location, left, right);
}

template <typename T>
typed_value_t
symmetricBinary (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  assert (!left.IsError ());
  assert (!right.IsError());
  left.RequireValue (location);
  right.RequireValue (location);
  typed_value_t out = left;
  T c (location, out, left, right);
  RequireIdentical (location, c.Op (), left.type, right.type);
  singleDispatch (left.type->UnderlyingType (), c);
  return out;
}

struct LogicNotImpl
{
  const Location& location;
  typed_value_t& out;
  const typed_value_t& in;
  LogicNotImpl (const Location& loc, typed_value_t& o, const typed_value_t& i) : location (loc), out (o), in (i) { }

  void operator() (const Type::Bool& type)
  {
    out.value.ref (type) = !in.value.ref (type);
  }
  void operator() (const Type::Boolean& type)
  {
    out.value.ref (type) = !in.value.ref (type);
  }

  template <typename T>
  void operator() (const T& type)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   "! cannot be applied to %s (E87)", type.ToString ().c_str ());

  }
};

typed_value_t
typed_value_t::LogicNot (const Location& location) const
{
  assert (!IsError ());
  RequireValue (location);
  typed_value_t out = *this;
  LogicNotImpl c (location, out, *this);
  singleDispatch (this->type->UnderlyingType (), c);
  return out;
}

struct LogicOrImpl
{
  const Location& location;
  typed_value_t& out;
  const typed_value_t& left;
  const typed_value_t& right;
  LogicOrImpl (const Location& loc, typed_value_t& o, const typed_value_t& l, const typed_value_t& r) : location (loc), out (o), left (l), right (r) { }

  const char* Op () const
  {
    return "||";
  }

  void operator() (const Type::Bool& type)
  {
    out.value.ref (type) = left.value.ref (type) || right.value.ref (type);
    out.value.present = left.value.present && (left.value.ref (type) == true || right.value.present);
  }

  void operator() (const Type::Boolean& type)
  {
    out.value.ref (type) = left.value.ref (type) || right.value.ref (type);
    out.value.present = left.value.present && (left.value.ref (type) == true || right.value.present);
  }

  template <typename T>
  void operator() (const T& type)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   "|| cannot be applied to %s (E86)", type.ToString ().c_str ());

  }
};

typed_value_t
typed_value_t::LogicOr (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return symmetricBinary<LogicOrImpl> (location, left, right);
}

struct LogicAndImpl
{
  const Location& location;
  typed_value_t& out;
  const typed_value_t& left;
  const typed_value_t& right;
  LogicAndImpl (const Location& loc, typed_value_t& o, const typed_value_t& l, const typed_value_t& r) : location (loc), out (o), left (l), right (r) { }

  const char* Op () const
  {
    return "&&";
  }

  void operator() (const Type::Bool& type)
  {
    out.value.ref (type) = left.value.ref (type) && right.value.ref (type);
    out.value.present = left.value.present && (left.value.ref (type) == false || right.value.present);
  }

  void operator() (const Type::Boolean& type)
  {
    out.value.ref (type) = left.value.ref (type) && right.value.ref (type);
    out.value.present = left.value.present && (left.value.ref (type) == false || right.value.present);
  }

  template <typename T>
  void operator() (const T& type)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   "&& cannot be applied to %s (E85)", type.ToString ().c_str ());

  }
};

typed_value_t
typed_value_t::LogicAnd (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return symmetricBinary<LogicAndImpl> (location, left, right);
}

template <typename O>
struct SymmetricBinary
{
  const Location& location;
  typed_value_t& out;
  const typed_value_t& left;
  const typed_value_t& right;
  SymmetricBinary (const Location& loc, typed_value_t& o, const typed_value_t& l, const typed_value_t& r) : location (loc), out (o), left (l), right (r) { }

  const char* Op () const
  {
    return O::Op ();
  }

  template <typename T>
  void doit (const T& type)
  {
    out.value.ref (type) = O () (left.value.ref (type), right.value.ref (type));
    out.value.present = left.value.present && right.value.present;
  }

  void operator() (const Type::Int& type)
  {
    doit (type);
  }

  void operator() (const Type::Integer& type)
  {
    doit (type);
  }

  template <typename T>
  void operator() (const T& type)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   "%s cannot be applied to %s (E133)", Op (), type.ToString ().c_str ());
  }
};

struct MultiplyOp {
  static const char* Op ()
  {
    return "*";
  }

  template <typename T>
  T operator() (const T& t1, const T& t2) const
  {
    return t1 * t2;
  }
};

typed_value_t
typed_value_t::Multiply (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return symmetricBinary<SymmetricBinary <MultiplyOp> > (location, left, right);
}

struct DivideOp {
  static const char* Op ()
  {
    return "/";
  }

  template <typename T>
  T operator() (const T& t1, const T& t2) const
  {
    return t1 / t2;
  }
};

typed_value_t
typed_value_t::Divide (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return symmetricBinary<SymmetricBinary <DivideOp> > (location, left, right);
}

struct ModulusOp {
  static const char* Op ()
  {
    return "%";
  }

  template <typename T>
  T operator() (const T& t1, const T& t2) const
  {
    return t1 % t2;
  }
};

typed_value_t
typed_value_t::Modulus (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return symmetricBinary<SymmetricBinary <ModulusOp> > (location, left, right);
}

struct AddOp {
  static const char* Op ()
  {
    return "+";
  }

  template <typename T>
  T operator() (const T& t1, const T& t2) const
  {
    return t1 + t2;
  }
};

typed_value_t
typed_value_t::Add (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return symmetricBinary<SymmetricBinary <AddOp> > (location, left, right);
}

struct SubtractOp {
  static const char* Op ()
  {
    return "-";
  }

  template <typename T>
  T operator() (const T& t1, const T& t2) const
  {
    return t1 - t2;
  }
};

typed_value_t
typed_value_t::Subtract (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return symmetricBinary<SymmetricBinary <SubtractOp> > (location, left, right);
}

struct LeftShiftImpl
{
  const Location& location;
  typed_value_t& out;
  const typed_value_t& left;
  const typed_value_t& right;

  LeftShiftImpl (const Location& loc, typed_value_t& o, const typed_value_t& l, const typed_value_t& r) : location (loc), out (o), left (l), right (r) { }

  void operator() (const Type::Int& type1, const Type::Int& type2)
  {
    out.value.ref (type1) = left.value.ref (type1) << right.value.ref (type2);
    out.value.present = left.value.present && right.value.present;
  }
  void operator() (const Type::Int& type1, const Type::Uint& type2)
  {
    out.value.ref (type1) = left.value.ref (type1) << right.value.ref (type2);
    out.value.present = left.value.present && right.value.present;
  }
  void operator() (const Type::Int& type1, const Type::Integer& type2)
  {
    out.value.ref (type1) = left.value.ref (type1) << right.value.ref (type2);
    out.value.present = left.value.present && right.value.present;
  }
  void operator() (const Type::Integer& type1, const Type::Uint& type2)
  {
    out.value.ref (type1) = left.value.ref (type1) << right.value.ref (type2);
    out.value.present = left.value.present && right.value.present;
  }
  void operator() (const Type::Integer& type1, const Type::Integer& type2)
  {
    out.value.ref (type1) = left.value.ref (type1) << right.value.ref (type2);
    out.value.present = left.value.present && right.value.present;
  }

  template <typename T1, typename T2>
  void operator() (const T1& type1, const T2& type2)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   "<< cannot be applied to %s and %s (E140)", type1.ToString ().c_str (), type2.ToString ().c_str ());

  }
};

typed_value_t
typed_value_t::LeftShift (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  assert (!left.IsError ());
  assert (!right.IsError());
  left.RequireValue (location);
  right.RequireValue (location);
  typed_value_t out = left;
  LeftShiftImpl c (location, out, left, right);
  doubleDispatch (left.type->UnderlyingType (), right.type->UnderlyingType (), c);
  return out;
}

struct RightShiftImpl
{
  const Location& location;
  typed_value_t& out;
  const typed_value_t& left;
  const typed_value_t& right;

  RightShiftImpl (const Location& loc, typed_value_t& o, const typed_value_t& l, const typed_value_t& r) : location (loc), out (o), left (l), right (r) { }

  void operator() (const Type::Int& type1, const Type::Int& type2)
  {
    out.value.ref (type1) = left.value.ref (type1) >> right.value.ref (type2);
    out.value.present = left.value.present && right.value.present;
  }
  void operator() (const Type::Int& type1, const Type::Uint& type2)
  {
    out.value.ref (type1) = left.value.ref (type1) >> right.value.ref (type2);
    out.value.present = left.value.present && right.value.present;
  }
  void operator() (const Type::Int& type1, const Type::Integer& type2)
  {
    out.value.ref (type1) = left.value.ref (type1) >> right.value.ref (type2);
    out.value.present = left.value.present && right.value.present;
  }
  void operator() (const Type::Integer& type1, const Type::Uint& type2)
  {
    out.value.ref (type1) = left.value.ref (type1) >> right.value.ref (type2);
    out.value.present = left.value.present && right.value.present;
  }
  void operator() (const Type::Integer& type1, const Type::Integer& type2)
  {
    out.value.ref (type1) = left.value.ref (type1) >> right.value.ref (type2);
    out.value.present = left.value.present && right.value.present;
  }

  template <typename T1, typename T2>
  void operator() (const T1& type1, const T2& type2)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   ">> cannot be applied to %s and %s (E139)", type1.ToString ().c_str (), type2.ToString ().c_str ());

  }
};

typed_value_t
typed_value_t::RightShift (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  assert (!left.IsError ());
  assert (!right.IsError());
  left.RequireValue (location);
  right.RequireValue (location);
  typed_value_t out = left;
  RightShiftImpl c (location, out, left, right);
  doubleDispatch (left.type->UnderlyingType (), right.type->UnderlyingType (), c);
  return out;
}

struct BitAndOp {
  static const char* Op ()
  {
    return "&";
  }

  template <typename T>
  T operator() (const T& t1, const T& t2) const
  {
    return t1 & t2;
  }
};

typed_value_t
typed_value_t::BitAnd (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return symmetricBinary<SymmetricBinary<BitAndOp> > (location, left, right);
}

struct BitAndNotOp {
  static const char* Op ()
  {
    return "&^";
  }

  template <typename T>
  T operator() (const T& t1, const T& t2) const
  {
    return t1 & ~t2;
  }
};

typed_value_t
typed_value_t::BitAndNot (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return symmetricBinary<SymmetricBinary<BitAndNotOp> > (location, left, right);
}

struct BitOrOp {
  static const char* Op ()
  {
    return "|";
  }

  template <typename T>
  T operator() (const T& t1, const T& t2) const
  {
    return t1 | t2;
  }
};

typed_value_t
typed_value_t::BitOr (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return symmetricBinary<SymmetricBinary<BitOrOp> > (location, left, right);
}

struct BitXorOp {
  static const char* Op ()
  {
    return "^";
  }

  template <typename T>
  T operator() (const T& t1, const T& t2) const
  {
    return t1 ^ t2;
  }
};

typed_value_t
typed_value_t::BitXor (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  return symmetricBinary<SymmetricBinary<BitXorOp> > (location, left, right);
}

struct IntegralValueImpl
{
  Type::Int::ValueType retval;
  const typed_value_t& in;

  IntegralValueImpl (const typed_value_t& i) : in (i) { }

  void operator() (const Type::Uint& type)
  {
    retval = in.value.ref (type);
  }

  void operator() (const Type::Integer& type)
  {
    retval = in.value.ref (type);
  }

  template <typename T>
  void operator() (const T& type)
  {
    type_not_reached (type);
  }
};

Type::Int::ValueType
typed_value_t::integral_value () const
{
  Location loc;
  IntegralValueImpl c (*this);
  singleDispatch (this->type->UnderlyingType (), c);
  return c.retval;
}

struct ArrayDimensionImpl
{
  const Location& location;
  const typed_value_t& in;

  ArrayDimensionImpl (const Location& loc, const typed_value_t& i) : location (loc), in (i) { }

  template<typename T>
  void doit (const T& type)
  {
    if (in.value.ref (type) < 0)
      {
        error_at_line (-1, 0, location.File.c_str (), location.Line,
                       "array dimension is negative (E105)");
      }
  }

  void operator() (const Type::Uint& type)
  {
    doit (type);
  }

  void operator() (const Type::Integer& type)
  {
    doit (type);
  }

  template <typename T>
  void operator() (const T& type)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   "%s cannot be used as an array dimension (E103)", type.ToString ().c_str ());
  }
};

void
typed_value_t::ArrayDimension (const Location& location) const
{
  if (!this->value.present)
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "array dimension is not constant (E104)");
    }

  ArrayDimensionImpl c (location, *this);
  singleDispatch (this->type->UnderlyingType (), c);
}
