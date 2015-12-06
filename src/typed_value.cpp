#include "typed_value.hpp"
#include <stdlib.h>
#include "debug.hpp"
#include "Type.hpp"
using namespace Type;
#include "action.hpp"
#include "field.hpp"
#include <error.h>
#include "Callable.hpp"
#include "Template.hpp"
#include <utility>
using namespace std::rel_ops;
#include "ast.hpp"
using namespace ast;
#include "runtime.hpp"
using namespace runtime;
#include "reaction.hpp"

template <typename TargetType>
struct Helper2
{
  const TargetType& target;
  const typed_value_t& in;
  typed_value_t& out;

  Helper2 (const TargetType& t, const typed_value_t& i, typed_value_t& o) : target (t), in (i), out (o) { }

  template<typename T>
  void operator() (const T& t)
  {
    out.value.ref (target) = in.value.ref (t);
  }

  template<typename T>
  void NotArithmetic (const T& t)
  {
    type_not_reached (t);
  }
};

struct Helper
{
  const typed_value_t& in;
  typed_value_t& out;

  Helper (const typed_value_t& i, typed_value_t& o) : in (i), out (o) { }

  template<typename T>
  void operator() (const T& t)
  {
    Helper2<T> helper2 (t, in, out);
    Type::ArithmeticVisitor<Helper2<T> > visitor (helper2);
    in.type->Accept (visitor);
  }

  template<typename T>
  void NotArithmetic (const T& t)
  {
    type_not_reached (t);
  }
};

static typed_value_t
numericConvert (const typed_value_t tv, const Type::Type* type)
{
  assert (tv.type->IsNumeric ());
  assert (type->IsNumeric ());
  typed_value_t out = tv;
  out.type = type;
  Helper h (tv, out);
  Type::ArithmeticVisitor<Helper> visitor (h);
  type->Accept (visitor);
  return out;
}

static void
RequireIdentical (const Location& location, const char* op, const Type::Type* left, const Type::Type* right)
{
  if (!Type::Identical (left, right))
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "%s cannot be applied to %s and %s (E93)", op, left->ToString ().c_str (), right->ToString ().c_str ());
    }
}

typed_value_t::typed_value_t (Callable* c)
  : type (c->type ())
  , kind (VALUE)
  , intrinsic_mutability (IMMUTABLE)
  , dereference_mutability (IMMUTABLE)
  , component_state (false)
  , value (c)
  , has_offset (false)
{
  fix ();
}

typed_value_t::typed_value_t (::Template* t)
  : type (t->type ())
  , kind (VALUE)
  , intrinsic_mutability (IMMUTABLE)
  , dereference_mutability (IMMUTABLE)
  , component_state (false)
  , value (t)
  , has_offset (false)
{
  fix ();
}

typed_value_t::typed_value_t (reaction_t* r)
  : type (r->reaction_type)
  , kind (VALUE)
  , intrinsic_mutability (IMMUTABLE)
  , dereference_mutability (IMMUTABLE)
  , component_state (false)
  , value (r)
  , has_offset (false)
{
  fix ();
}

void
typed_value_t::zero ()
{
  assert (kind == VALUE);
  assert (value.present);

  struct visitor : public DefaultVisitor
  {
    typed_value_t& value;

    visitor (typed_value_t& v) : value (v) { }

    void default_action (const Type::Type& type)
    {
      type_not_reached (type);
    }

    void visit (const Type::Int& type)
    {
      value.value.ref (type) = 0;
    }

    void visit (const Uint& type)
    {
      value.value.ref (type) = 0;
    }

    void visit (const Integer& type)
    {
      value.value.ref (type) = 0;
    }
  };

  visitor v (*this);
  type->UnderlyingType ()->Accept (v);
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

  out << " comp " << component_state;

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
typed_value_t::make_value (const Type::Type* type, Mutability intrinsic, Mutability dereference, bool component_state)
{
  typed_value_t tv;
  tv.type = type;
  tv.kind = VALUE;
  tv.intrinsic_mutability = intrinsic;
  tv.dereference_mutability = dereference;
  tv.component_state = component_state;
  tv.fix ();
  return tv;
}

typed_value_t
typed_value_t::make_range (const typed_value_t& low, const typed_value_t& high, Mutability intrinsic, Mutability dereference)
{
  assert (low.type == high.type);
  assert (low.kind == VALUE);
  assert (low.value.present);
  assert (high.kind == VALUE);
  assert (high.value.present);

  typed_value_t tv;
  tv.type = low.type;
  tv.kind = VALUE;
  tv.intrinsic_mutability = intrinsic;
  tv.dereference_mutability = dereference;
  tv.low_value = low.value;
  tv.high_value = high.value;
  return tv;
}

typed_value_t
typed_value_t::make_ref (const Type::Type* type, Mutability intrinsic, Mutability dereference, bool component_state)
{
  typed_value_t tv;
  tv.type = type;
  tv.kind = REFERENCE;
  tv.intrinsic_mutability = intrinsic;
  tv.dereference_mutability = dereference;
  tv.component_state = component_state;
  tv.fix ();
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
  typed_value_t retval = make_value (Nil::Instance (), IMMUTABLE, MUTABLE, false);
  retval.value.present = true;
  return retval;
}

typed_value_t
typed_value_t::implicit_dereference (typed_value_t tv)
{
  assert (tv.type != NULL);
  assert (tv.kind == REFERENCE);
  tv.kind = VALUE;
  tv.component_state = tv.component_state && type_contains_pointer (tv.type);
  return tv;
}

typed_value_t
typed_value_t::dereference (typed_value_t in)
{
  assert (in.kind == VALUE);

  struct visitor : public DefaultVisitor
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
      out = typed_value_t::make_ref (type.Base (), in.dereference_mutability, in.dereference_mutability, in.component_state);
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
      out.fix ();
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

  struct visitor : public DefaultVisitor
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
                                          in.intrinsic_mutability,
                                          in.dereference_mutability,
                                          in.component_state);
    }
  };
  visitor v (location, in, low, high);
  in.type->Accept (v);

  return v.result;
}

template <typename T>
struct SingleDispatchVisitor : public Type::DefaultVisitor
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

  void visit (const Type::Uint64& type)
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

  void visit (const Type::String& type)
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
                     "cannot copy components (E175)");
    }

  {
    const Slice* st = type_strip_cast<Slice> (tv.type);
    if (st != NULL)
      {
        if (type_contains_pointer (st->Base ()))
          {
            error_at_line (-1, 0, location.File.c_str (), location.Line,
                           "copy leaks pointers (E222)");

          }
        // We will copy so a dereference can mutate the data.
        tv.intrinsic_mutability = MUTABLE;
        tv.dereference_mutability = MUTABLE;
      }
  }

  {
    const StringU* st = type_strip_cast<StringU> (tv.type);
    if (st != NULL)
      {
        // We will copy so a dereference can mutate the data.
        tv.intrinsic_mutability = MUTABLE;
        tv.dereference_mutability = IMMUTABLE;
      }
  }

  tv.fix ();

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
                     "cannot change expression of type %s (E176)", tv.type->ToString ().c_str ());
    }

  tv.type = root_type;
  return tv;
}

void
typed_value_t::RequireReference (const Location location) const
{
  if (!IsReference ())
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "expected a reference (E14)");
    }
}

void
typed_value_t::RequireValue (const Location location) const
{
  if (!IsValue ())
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "expected a value (E24)");
    }
}

void
typed_value_t::RequireReferenceOrValue (const Location location) const
{
  if (!(IsReference () || IsValue ()))
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "expected reference or value (E143)");
    }
}

struct RepresentableImpl
{
  const typed_value_t& in;
  bool retval;

  RepresentableImpl (const typed_value_t& i) : in (i), retval (false) { }

  void operator() (const Type::Bool& type1, const Type::Boolean& type2)
  {
    retval = true;
  }
  void operator() (const Type::Pointer& type1, const Type::Nil& type2)
  {
    retval = true;
  }
  void operator() (const Type::StringU& type1, const Type::String& type2)
  {
    retval = true;
  }
  void operator() (const Type::Slice& type1, const Type::String& type2)
  {
    if (type1.Base () == &NamedByte)
      {
        retval = true;
      }
    else
      {
        retval = false;
      }
  }

  template <typename T1, typename T2>
  void operator() (const T1& type1, const T2& type2)
  {
    retval = false;
  }
};

bool
typed_value_t::RepresentableBy (const Type::Type* target) const
{
  assert (this->value.present);
  const Type::Type* source = this->type;
  if (source->IsNumeric () && target->IsNumeric ())
    {
      // Assume that Complex can represent any number.
      // Convert to a Complex.
      typed_value_t c1 = numericConvert (*this, Type::Complex::Instance ());
      // Convert to the target type.
      typed_value_t v = numericConvert (c1, target->UnderlyingType ());
      // Convert back to Complex.
      typed_value_t c2 = numericConvert (v, Type::Complex::Instance ());
      // Compare.
      return c1.value.ref (*Type::Complex::Instance ()) == c2.value.ref (*Type::Complex::Instance ());
    }
  else
    {
      RepresentableImpl ri (*this);
      DoubleDispatch (target->UnderlyingType (), source, ri);
      return ri.retval;
    }
}

bool
typed_value_t::AssignableTo (const Type::Type* target) const
{
  const Type::Type* source = this->type;

  if (Identical (source, target))
    {
      return true;
    }

  if (Identical (source->UnderlyingType (), target->UnderlyingType ()) &&
      (type_cast<NamedType> (source) == NULL ||
       type_cast<NamedType> (target) == NULL))
    {
      return true;
    }

  // TODO: target is interface and source implements it

  if (type_cast<Nil> (source))
    {
      if (type_cast<Pointer> (target->UnderlyingType ()) != NULL ||
          type_cast<Type::Function> (target->UnderlyingType ()) != NULL ||
          type_cast<Slice> (target->UnderlyingType ()) != NULL
          //type_cast<Map> (target->UnderlyingType ()) != NULL ||
          //type_cast<Interface> (target->UnderlyingType ()) != NULL ||
         )
        {
          return true;
        }
    }

  if (source->IsUntyped () && this->RepresentableBy (target))
    {
      return true;
    }

  return false;
}

struct ConvertImpl
{
  const Location& location;
  typed_value_t& out;
  const typed_value_t& in;
  Node& node;

  ConvertImpl (const Location& loc, typed_value_t& o, const typed_value_t& i, Node& n) : location (loc), out (o), in (i), node (n) { }

  // Caused by untyped bool being convert to named bool.
  // The untyped bool comes from comparison operators.
  void operator() (const Type::Bool& type1, const Type::Bool& type2)
  {
    out.value.ref (type1) = in.value.ref (type2);
  }
  void operator() (const Type::Bool& type1, const Type::Boolean& type2)
  {
    out.value.ref (type1) = in.value.ref (type2);
  }

  void operator() (const Type::Pointer& type1, const Type::Nil& type2)
  {
    out.value.ref (type1) = NULL;
  }
  void operator() (const Type::StringU& type1, const Type::String& type2)
  {
    out.value.ref (type1) = in.value.ref (type2);
  }
  void operator() (const Type::Slice& type1, const Type::String& type2)
  {
    if (type1.Base () == &NamedByte)
      {
        // Dynamic conversion.
        out.intrinsic_mutability = MUTABLE;
        out.dereference_mutability = MUTABLE;
        out.value.present = false;
        unimplemented;
        //node.operation = &ConvertStringToSliceOfBytes::instance;
      }
    else
      {
        error (type1, type2);
      }
  }

  template <typename T1, typename T2>
  void operator() (const T1& type1, const T2& type2)
  {
    error (type1, type2);
  }

  template <typename T1, typename T2>
  void error (const T1& type1, const T2& type2)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   "cannot convert %s to %s (E49)", in.type->ToString ().c_str (), out.type->ToString ().c_str ());
  }

};

static bool
pointerConversion (const Type::Type* x, const Type::Type* t)
{
  unimplemented;
}

typed_value_t
typed_value_t::Convert (const Location& location, const Type::Type* type, Node& node) const
{
  assert (!IsError ());
  RequireValue (location);
  if (this->value.present)
    {
      if (this->RepresentableBy (type))
        {
          if (this->type->IsNumeric ())
            {
              return numericConvert (*this, type);
            }
          else
            {
              typed_value_t out = *this;
              out.type = type;
              ConvertImpl c (location, out, *this, node);
              DoubleDispatch (type->UnderlyingType (), this->type, c);
              out.fix ();
              return out;
            }
        }
      else if (this->type->IsFloatingPoint () && type->IsFloatingPoint ())
        {
          unimplemented;
        }
      else if (this->type->IsInteger () && type->IsString ())
        {
          unimplemented;
        }
      else
        {
          // Error.
          unimplemented;
        }
    }
  else
    {
      if (this->AssignableTo (type))
        {
          typed_value_t out = *this;
          out.type = type;
          out.fix ();
          return out;
        }
      else if (Type::Identical (this->type->UnderlyingType (), type->UnderlyingType ()))
        {
          unimplemented;
        }
      else if (pointerConversion (this->type, type))
        {
          unimplemented;
        }
      else if ((this->type->IsInteger () || this->type->IsInteger ()) &&
               (type->IsInteger () || type->IsFloatingPoint ()))
        {
          unimplemented;
        }
      else if (this->type->IsComplex () && type->IsComplex ())
        {
          unimplemented;
        }
      else if (this->type->IsInteger () && type->IsString ())
        {
          unimplemented;
        }
      else if (this->type->IsSliceOfBytes () && type->IsString ())
        {
          unimplemented;
        }
      else if (this->type->IsSliceOfRunes () && type->IsString ())
        {
          unimplemented;
        }
      else if (this->type->IsString () && (type->IsSliceOfBytes () || type->IsSliceOfRunes ()))
        {
          unimplemented;
        }
      else
        {
          // Error
          unimplemented;
        }
    }

  not_reached;
  // typed_value_t out = *this;
  // out.type = type;
  // ConvertImpl c (location, out, *this);
  // DoubleDispatch (type->UnderlyingType (), this->type, c);
  // return out;
}

template <typename VisitorType>
typed_value_t
comparison (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  assert (!left.IsError ());
  assert (!right.IsError());
  left.RequireValue (location);
  right.RequireValue (location);
  typed_value_t out;
  if (left.value.present && right.value.present)
    {
      out = typed_value_t::make_value (Type::Boolean::Instance (), IMMUTABLE, IMMUTABLE, false);
    }
  else
    {
      out = typed_value_t::make_value (Type::Bool::Instance (), IMMUTABLE, IMMUTABLE, false);
    }
  typename VisitorType::DispatchType c (location, left, right);
  RequireIdentical (location, c.Op (), left.type, right.type);
  VisitorType visitor (c);
  left.type->UnderlyingType ()->Accept (visitor);
  if (left.value.present && right.value.present)
    {
      out.value.ref (*Type::Boolean::Instance ()) = c.retval;
    }
  else
    {
      out.value.ref (*Type::Bool::Instance ()) = c.retval;
    }
  out.value.present = left.value.present && right.value.present;
  return out;
}

template <typename O>
struct BooleanApply
{
  const Location& location;
  bool retval;
  const typed_value_t& left;
  const typed_value_t& right;
  BooleanApply (const Location& loc, const typed_value_t& l, const typed_value_t& r) : location (loc), retval (false), left (l), right (r) { }

  const char* Op () const
  {
    return O::Op ();
  }

  template<typename T>
  void operator() (const T& type)
  {
    retval = O () (left.value.ref (type), right.value.ref (type));
  }

  template <typename T>
  void NotComparable (const T& type)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   "%s cannot be applied to %s (E25)", Op (), type.ToString ().c_str ());
  }

  template <typename T>
  void NotOrderable (const T& type)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   "%s cannot be applied to %s (E91)", Op (), type.ToString ().c_str ());

  }
};

struct EqualOp
{
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
  return comparison<Type::ComparableVisitor<BooleanApply <EqualOp> > > (location, left, right);
}

struct NotEqualOp
{
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
  return comparison<Type::ComparableVisitor<BooleanApply <NotEqualOp> > > (location, left, right);
}

struct LessThanOp
{
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
  return comparison<Type::OrderableVisitor<BooleanApply<LessThanOp> > > (location, left, right);
}

struct LessEqualOp
{
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
  return comparison<Type::OrderableVisitor<BooleanApply<LessEqualOp> > > (location, left, right);
}

struct MoreThanOp
{
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
  return comparison<Type::OrderableVisitor<BooleanApply <MoreThanOp> > > (location, left, right);
}

struct MoreEqualOp
{
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
  return comparison<Type::OrderableVisitor<BooleanApply <MoreEqualOp> > > (location, left, right);
}

template <typename VisitorType>
typed_value_t
symmetricBinary (const Location& location, const typed_value_t& left, const typed_value_t& right)
{
  assert (!left.IsError ());
  assert (!right.IsError());
  left.RequireValue (location);
  right.RequireValue (location);
  typed_value_t out = left;
  typename VisitorType::DispatchType c (location, out, left, right);
  RequireIdentical (location, c.Op (), left.type, right.type);
  VisitorType visitor (c);
  left.type->UnderlyingType ()->Accept (visitor);
  return out;
}

struct LogicNotImpl
{
  const Location& location;
  typed_value_t& out;
  const typed_value_t& in;
  LogicNotImpl (const Location& loc, typed_value_t& o, const typed_value_t& i) : location (loc), out (o), in (i) { }

  template <typename T>
  void operator() (const T& type)
  {
    out.value.ref (type) = !in.value.ref (type);
  }

  template <typename T>
  void NotLogical (const T& type)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   "! cannot be applied to %s (E87)", in.type->ToString ().c_str ());

  }
};

typed_value_t
typed_value_t::LogicNot (const Location& location) const
{
  assert (!IsError ());
  RequireValue (location);
  typed_value_t out = *this;
  LogicNotImpl c (location, out, *this);
  Type::LogicalVisitor<LogicNotImpl> visitor (c);
  this->type->UnderlyingType ()->Accept (visitor);
  return out;
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
  void operator() (const T& type)
  {
    out.value.present = left.value.present && right.value.present;
    if (out.value.present)
      {
        out.value.ref (type) = O () (left.value.ref (type), right.value.ref (type));
      }
  }

  template <typename T>
  void NotArithmetic (const T& type)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   "%s cannot be applied to %s (E133)", Op (), type.ToString ().c_str ());
  }

  template <typename T>
  void NotIntegral (const T& type)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   "%s cannot be applied to %s (E70)", Op (), type.ToString ().c_str ());
  }
};

struct NegateImpl
{
  const Location& location;
  typed_value_t& out;
  const typed_value_t& in;
  NegateImpl (const Location& loc, typed_value_t& o, const typed_value_t& i) : location (loc), out (o), in (i) { }

  template <typename T>
  void operator() (const T& type)
  {
    out.value.ref (type) = -in.value.ref (type);
  }

  template <typename T>
  void NotArithmetic (const T& type)
  {
    error_at_line (-1, 0, location.File.c_str (), location.Line,
                   "- cannot be applied to %s (E67)", in.type->ToString ().c_str ());

  }
};

typed_value_t
typed_value_t::Negate (const Location& location) const
{
  assert (!IsError ());
  RequireValue (location);
  typed_value_t out = *this;
  NegateImpl c (location, out, *this);
  Type::ArithmeticVisitor<NegateImpl> visitor (c);
  this->type->UnderlyingType ()->Accept (visitor);
  return out;
}

struct AddOp
{
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
  return symmetricBinary<Type::ArithmeticVisitor<SymmetricBinary <AddOp> > > (location, left, right);
}

struct SubtractOp
{
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
  return symmetricBinary<Type::ArithmeticVisitor<SymmetricBinary <SubtractOp> > > (location, left, right);
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
  DoubleDispatch (left.type->UnderlyingType (), right.type->UnderlyingType (), c);
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
  DoubleDispatch (left.type->UnderlyingType (), right.type->UnderlyingType (), c);
  return out;
}

struct BitAndOp
{
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
  return symmetricBinary<Type::IntegralVisitor<SymmetricBinary<BitAndOp> > > (location, left, right);
}

struct BitAndNotOp
{
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
  return symmetricBinary<Type::IntegralVisitor<SymmetricBinary<BitAndNotOp> > > (location, left, right);
}

struct BitOrOp
{
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
  return symmetricBinary<Type::IntegralVisitor<SymmetricBinary<BitOrOp> > > (location, left, right);
}

struct BitXorOp
{
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
  return symmetricBinary<Type::IntegralVisitor<SymmetricBinary<BitXorOp> > > (location, left, right);
}

struct IntegralValueImpl
{
  Type::Int::ValueType retval;
  const value_t& in;

  IntegralValueImpl (const value_t& i) : in (i) { }

  void operator() (const Type::Int& type)
  {
    retval = in.ref (type);
  }

  void operator() (const Type::Uint& type)
  {
    retval = in.ref (type);
  }

  void operator() (const Type::Integer& type)
  {
    retval = in.ref (type);
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
  IntegralValueImpl c (this->value);
  singleDispatch (this->type->UnderlyingType (), c);
  return c.retval;
}

Type::Int::ValueType
typed_value_t::low_integral_value () const
{
  Location loc;
  IntegralValueImpl c (this->low_value);
  singleDispatch (this->type->UnderlyingType (), c);
  return c.retval;
}

Type::Int::ValueType
typed_value_t::high_integral_value () const
{
  Location loc;
  IntegralValueImpl c (this->high_value);
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

  void operator() (const Type::Int& type)
  {
    doit (type);
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
