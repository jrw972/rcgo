#include "value.hpp"

#include <utility>

namespace semantic
{

using namespace type;
using namespace std::rel_ops;

// struct alpha_visitor : public Type::DefaultVisitor
// {
//   bool flag;
//   Complex::ValueType x;

//   alpha_visitor (Complex::ValueType z) : flag (false), x (z) { }

//   void default_action (const Type::Type& type)
//   {
//     TYPE_NOT_REACHED (type);
//   }

//   template <typename T>
//   void doit (const T& type)
//   {
//     typename T::ValueType y;
//     y = x;
//     Complex::ValueType z;
//     z = y;
//     flag = (x == z);
//   }

//   void visit (const Uint8& type)
//   {
//     doit (type);
//   }

//   void visit (const Uint16& type)
//   {
//     doit (type);
//   }

//   void visit (const Uint32& type)
//   {
//     doit (type);
//   }

//   void visit (const Uint64& type)
//   {
//     doit (type);
//   }

//   void visit (const Int8& type)
//   {
//     doit (type);
//   }

//   void visit (const Int16& type)
//   {
//     doit (type);
//   }

//   void visit (const Int32& type)
//   {
//     doit (type);
//   }

//   void visit (const Int64& type)
//   {
//     doit (type);
//   }

//   void visit (const Uint& type)
//   {
//     doit (type);
//   }

//   void visit (const Int& type)
//   {
//     doit (type);
//   }

//   void visit (const Float32& type)
//   {
//     doit (type);
//   }

//   void visit (const Float64& type)
//   {
//     doit (type);
//   }

//   void visit (const Complex64& type)
//   {
//     doit (type);
//   }

//   void visit (const Complex128& type)
//   {
//     doit (type);
//   }

//   void visit (const Integer& type)
//   {
//     doit (type);
//   }
// };

// static bool to_and_back (Complex::ValueType x, const Type::Type* type)
// {
//   alpha_visitor v (x);
//   type->Accept (v);
//   return v.flag;
// }

template <typename S, typename T>
static bool to_and_back_helper (const T& x)
{
  S y;
  y = x;
  T z;
  z = y;
  return x == z;
}

template <typename T>
static bool to_and_back (const T& x, const type::Type* type)
{
  switch (type->underlying_kind ())
    {
    case Uint8_Kind:
      return to_and_back_helper<Uint8::ValueType> (x);
    case Uint16_Kind:
      return to_and_back_helper<Uint16::ValueType> (x);
    case Uint32_Kind:
      return to_and_back_helper<Uint32::ValueType> (x);
    case Uint64_Kind:
      return to_and_back_helper<Uint64::ValueType> (x);
    case Int8_Kind:
      return to_and_back_helper<Int8::ValueType> (x);
    case Int16_Kind:
      return to_and_back_helper<Int16::ValueType> (x);
    case Int32_Kind:
      return to_and_back_helper<Int32::ValueType> (x);
    case Int64_Kind:
      return to_and_back_helper<Int64::ValueType> (x);
    case Float32_Kind:
      return to_and_back_helper<Float32::ValueType> (x);
    case Float64_Kind:
      return to_and_back_helper<Float64::ValueType> (x);
    case Complex64_Kind:
      return to_and_back_helper<Complex64::ValueType> (x);
    case Complex128_Kind:
      return to_and_back_helper<Complex128::ValueType> (x);
    case Uint_Kind:
      return to_and_back_helper<Uint::ValueType> (x);
    case Int_Kind:
      return to_and_back_helper<Int::ValueType> (x);
    case Uintptr_Kind:
      return to_and_back_helper<Uintptr::ValueType> (x);
    default:
      return false;
    }
}

static void convert_numeric (Value& value, UntypedComplex::ValueType x, const type::Type* type)
{
  struct visitor : public type::DefaultVisitor
  {
    Value& value;
    UntypedComplex::ValueType x;

    visitor (Value& v, UntypedComplex::ValueType z) : value (v), x (z) { }

    void default_action (const type::Type& type)
    {
      TYPE_NOT_REACHED (type);
    }

    void visit (const Uint8& type)
    {
      value.uint8_value = x;
    }

    void visit (const Uint16& type)
    {
      value.uint16_value = x;
    }

    void visit (const Uint32& type)
    {
      value.uint32_value = x;
    }

    void visit (const Uint64& type)
    {
      value.uint64_value = x;
    }

    void visit (const Int8& type)
    {
      value.int8_value = x;
    }

    void visit (const Int16& type)
    {
      value.int16_value = x;
    }

    void visit (const Int32& type)
    {
      value.int32_value = x;
    }

    void visit (const Int64& type)
    {
      value.int64_value = x;
    }

    void visit (const Uint& type)
    {
      value.uint_value = x;
    }

    void visit (const Int& type)
    {
      value.int_value = x;
    }

    void visit (const Float32& type)
    {
      value.float32_value = x;
    }

    void visit (const Float64& type)
    {
      value.float64_value = x;
    }

    void visit (const Complex64& type)
    {
      value.complex64_value = x;
    }

    void visit (const Complex128& type)
    {
      value.complex128_value = x;
    }

    void visit (const UntypedInteger& type)
    {
      value.integer_value = x;
    }

    void visit (const UntypedFloat& type)
    {
      value.float_value = x;
    }
  };

  visitor v (value, x);
  type->accept (v);
}

bool
Value::representable (const type::Type* from, const type::Type* to) const
{
  assert (present);
  assert (from->is_untyped ());
  assert (!to->is_untyped ());

  switch (from->underlying_kind ())
    {
    case Untyped_Nil_Kind:
      return
        to->underlying_kind () == Pointer_Kind ||
        to->underlying_kind () == Slice_Kind;
    case Untyped_Boolean_Kind:
      return to->underlying_kind () == Bool_Kind;
    case Untyped_Rune_Kind:
      return to_and_back (rune_value, to);
    case Untyped_Integer_Kind:
      return to_and_back (integer_value, to);
    case Untyped_Float_Kind:
      return to_and_back (float_value, to);
    case Untyped_Complex_Kind:
      return to_and_back (complex_value, to);
    case Untyped_String_Kind:
      return to->underlying_kind () == String_Kind;
    default:
      NOT_REACHED;
    }

  // struct visitor : public type::DefaultVisitor
  // {
  //   const Value& value;
  //   const type::Type* to;
  //   bool flag;

  //   visitor (const Value& v, const type::Type* t) : value (v), to (t), flag (false) { }

  //   void default_action (const type::Type& type)
  //   {
  //     TYPE_NOT_REACHED (type);
  //   }

  //   void visit (const type::Int& type)
  //   {
  //     Complex::ValueType x;
  //     x = value.ref (type);
  //     flag = to_and_back (x, to->UnderlyingType ());
  //   }

  //   void visit (const type::Boolean& type)
  //   {
  //     flag =
  //       type_cast<type::Boolean> (to) != NULL ||
  //       type_cast<type::Bool> (to) != NULL;
  //   }

  //   void visit (const type::Rune& type)
  //   {
  //     Complex::ValueType x;
  //     x = value.ref (type);
  //     flag = to_and_back (x, to->UnderlyingType ());
  //   }

  //   void visit (const type::Integer& type)
  //   {
  //     Complex::ValueType x;
  //     x = value.ref (type);
  //     flag = to_and_back (x, to->UnderlyingType ());
  //   }

  //   void visit (const type::Float& type)
  //   {
  //     Complex::ValueType x;
  //     x = value.ref (type);
  //     flag = to_and_back (x, to->UnderlyingType ());
  //   }

  //   void visit (const type::String& type)
  //   {
  //     if (type::type_cast<type::StringU> (to->UnderlyingType ()))
  //       {
  //         flag = true;
  //       }
  //   }
  // } v (*this, to->UnderlyingType ());
  // from->UnderlyingType ()->Accept (v);
  // return v.flag;
}

void
Value::convert (const type::Type* from, const type::Type* to)
{
  assert (present);

  struct visitor : public type::DefaultVisitor
  {
    Value& value;
    const type::Type* to;

    visitor (Value& v, const type::Type* t) : value (v), to (t) { }

    void default_action (const type::Type& type)
    {
      TYPE_NOT_REACHED (type);
    }

    void visit (const type::UntypedBoolean& type)
    {
      if (type_cast<type::Bool> (to) != NULL)
        {
          value.bool_value = value.boolean_value;
        }
    }

    void visit (const type::UntypedRune& type)
    {
      UntypedComplex::ValueType x;
      x = value.rune_value;
      convert_numeric (value, x, to);
    }

    void visit (const type::UntypedInteger& type)
    {
      UntypedComplex::ValueType x;
      x = value.integer_value;
      convert_numeric (value, x, to);
    }

    void visit (const type::UntypedFloat& type)
    {
      UntypedComplex::ValueType x;
      x = value.float_value;
      convert_numeric (value, x, to);
    }

    void visit (const type::Uint8& type)
    {
      UntypedComplex::ValueType x;
      x = value.uint8_value;
      convert_numeric (value, x, to);
    }

    void visit (const type::Uint16& type)
    {
      UntypedComplex::ValueType x;
      x = value.uint16_value;
      convert_numeric (value, x, to);
    }

    void visit (const type::Uint32& type)
    {
      UntypedComplex::ValueType x;
      x = value.uint32_value;
      convert_numeric (value, x, to);
    }

    void visit (const type::Uint64& type)
    {
      UntypedComplex::ValueType x;
      x = value.uint64_value;
      convert_numeric (value, x, to);
    }

    void visit (const type::Int8& type)
    {
      UntypedComplex::ValueType x;
      x = value.int8_value;
      convert_numeric (value, x, to);
    }

    void visit (const type::Int16& type)
    {
      UntypedComplex::ValueType x;
      x = value.int16_value;
      convert_numeric (value, x, to);
    }

    void visit (const type::Int32& type)
    {
      UntypedComplex::ValueType x;
      x = value.int32_value;
      convert_numeric (value, x, to);
    }

    void visit (const type::Int64& type)
    {
      UntypedComplex::ValueType x;
      x = value.int64_value;
      convert_numeric (value, x, to);
    }

    void visit (const type::Int& type)
    {
      UntypedComplex::ValueType x;
      x = value.int_value;
      convert_numeric (value, x, to);
    }

    void visit (const type::Float32& type)
    {
      UntypedComplex::ValueType x;
      x = value.float32_value;
      convert_numeric (value, x, to);
    }

    void visit (const type::Float64& type)
    {
      UntypedComplex::ValueType x;
      x = value.float64_value;
      convert_numeric (value, x, to);
    }

    void visit (const type::Complex64& type)
    {
      UntypedComplex::ValueType x;
      x = value.complex64_value;
      convert_numeric (value, x, to);
    }

    void visit (const type::Complex128& type)
    {
      UntypedComplex::ValueType x;
      x = value.complex128_value;
      convert_numeric (value, x, to);
    }

    void visit (const type::UntypedString& type)
    {
      if (type::type_cast<type::String> (to))
        {
          value.stringu_value = value.string_value;
          return;
        }

      NOT_REACHED;
    }

    void visit (const type::UntypedNil& type)
    {
      if (type::type_cast<type::Pointer> (to))
        {
          value.pointer_value = NULL;
          return;
        }

      if (type::type_cast<type::Slice> (to))
        {
          value.slice_value = Slice::ValueType ();
          return;
        }

      NOT_REACHED;
    }

  } v (*this, to->underlying_type ());

  from->underlying_type ()->accept (v);
}

type::Int::ValueType
Value::to_int (const type::Type* type) const
{
  assert (is_typed_integer (type));
  switch (type->underlying_kind ())
    {
    case Uint8_Kind:
      return uint8_value;
    case Uint16_Kind:
      return uint16_value;
    case Uint32_Kind:
      return uint32_value;
    case Uint64_Kind:
      return uint64_value;
    case Int8_Kind:
      return int8_value;
    case Int16_Kind:
      return int16_value;
    case Int32_Kind:
      return int32_value;
    case Int64_Kind:
      return int64_value;
    case Uint_Kind:
      return uint_value;
    case Int_Kind:
      return int_value;
    case Uintptr_Kind:
      return uintptr_value;
    default:
      NOT_REACHED;
    }
}

void equal (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Nil_Kind:
      out.boolean_value = true;
      break;
    case Untyped_Boolean_Kind:
      out.boolean_value = left.boolean_value == right.boolean_value;
      break;
    case Untyped_Rune_Kind:
      out.boolean_value = left.rune_value == right.rune_value;
      break;
    case Untyped_Integer_Kind:
      out.boolean_value = left.integer_value == right.integer_value;
      break;
    case Untyped_Float_Kind:
      out.boolean_value = left.float_value == right.float_value;
      break;
    case Untyped_Complex_Kind:
      out.boolean_value = left.complex_value == right.complex_value;
      break;
    case Untyped_String_Kind:
      out.boolean_value = left.string_value == right.string_value;
      break;
    case Bool_Kind:
      out.boolean_value = left.bool_value == right.bool_value;
      break;
    case Uint8_Kind:
      out.boolean_value = left.uint8_value == right.uint8_value;
      break;
    case Uint16_Kind:
      out.boolean_value = left.uint16_value == right.uint16_value;
      break;
    case Uint32_Kind:
      out.boolean_value = left.uint32_value == right.uint32_value;
      break;
    case Uint64_Kind:
      out.boolean_value = left.uint64_value == right.uint64_value;
      break;
    case Int8_Kind:
      out.boolean_value = left.int8_value == right.int8_value;
      break;
    case Int16_Kind:
      out.boolean_value = left.int16_value == right.int16_value;
      break;
    case Int32_Kind:
      out.boolean_value = left.int32_value == right.int32_value;
      break;
    case Int64_Kind:
      out.boolean_value = left.int64_value == right.int64_value;
      break;
    case Float32_Kind:
      out.boolean_value = left.float32_value == right.float32_value;
      break;
    case Float64_Kind:
      out.boolean_value = left.float64_value == right.float64_value;
      break;
    case Complex64_Kind:
      out.boolean_value = left.complex64_value == right.complex64_value;
      break;
    case Complex128_Kind:
      out.boolean_value = left.complex128_value == right.complex128_value;
      break;
    case Uint_Kind:
      out.boolean_value = left.uint_value == right.uint_value;
      break;
    case Int_Kind:
      out.boolean_value = left.int_value == right.int_value;
      break;
    case Uintptr_Kind:
      out.boolean_value = left.uintptr_value == right.uintptr_value;
      break;
    case String_Kind:
      out.boolean_value = left.stringu_value == right.stringu_value;
      break;
    case Pointer_Kind:
      out.boolean_value = left.pointer_value == right.pointer_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void not_equal (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Nil_Kind:
      out.boolean_value = false;
      break;
    case Untyped_Boolean_Kind:
      out.boolean_value = left.boolean_value != right.boolean_value;
      break;
    case Untyped_Rune_Kind:
      out.boolean_value = left.rune_value != right.rune_value;
      break;
    case Untyped_Integer_Kind:
      out.boolean_value = left.integer_value != right.integer_value;
      break;
    case Untyped_Float_Kind:
      out.boolean_value = left.float_value != right.float_value;
      break;
    case Untyped_Complex_Kind:
      out.boolean_value = left.complex_value != right.complex_value;
      break;
    case Untyped_String_Kind:
      out.boolean_value = left.string_value != right.string_value;
      break;
    case Bool_Kind:
      out.boolean_value = left.bool_value != right.bool_value;
      break;
    case Uint8_Kind:
      out.boolean_value = left.uint8_value != right.uint8_value;
      break;
    case Uint16_Kind:
      out.boolean_value = left.uint16_value != right.uint16_value;
      break;
    case Uint32_Kind:
      out.boolean_value = left.uint32_value != right.uint32_value;
      break;
    case Uint64_Kind:
      out.boolean_value = left.uint64_value != right.uint64_value;
      break;
    case Int8_Kind:
      out.boolean_value = left.int8_value != right.int8_value;
      break;
    case Int16_Kind:
      out.boolean_value = left.int16_value != right.int16_value;
      break;
    case Int32_Kind:
      out.boolean_value = left.int32_value != right.int32_value;
      break;
    case Int64_Kind:
      out.boolean_value = left.int64_value != right.int64_value;
      break;
    case Float32_Kind:
      out.boolean_value = left.float32_value != right.float32_value;
      break;
    case Float64_Kind:
      out.boolean_value = left.float64_value != right.float64_value;
      break;
    case Complex64_Kind:
      out.boolean_value = left.complex64_value != right.complex64_value;
      break;
    case Complex128_Kind:
      out.boolean_value = left.complex128_value != right.complex128_value;
      break;
    case Uint_Kind:
      out.boolean_value = left.uint_value != right.uint_value;
      break;
    case Int_Kind:
      out.boolean_value = left.int_value != right.int_value;
      break;
    case Uintptr_Kind:
      out.boolean_value = left.uintptr_value != right.uintptr_value;
      break;
    case String_Kind:
      out.boolean_value = left.stringu_value != right.stringu_value;
      break;
    case Pointer_Kind:
      out.boolean_value = left.pointer_value != right.pointer_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void less_than (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.boolean_value = left.rune_value < right.rune_value;
      break;
    case Untyped_Integer_Kind:
      out.boolean_value = left.integer_value < right.integer_value;
      break;
    case Untyped_Float_Kind:
      out.boolean_value = left.float_value < right.float_value;
      break;
    case Untyped_String_Kind:
      out.boolean_value = left.string_value < right.string_value;
      break;
    case Uint8_Kind:
      out.boolean_value = left.uint8_value < right.uint8_value;
      break;
    case Uint16_Kind:
      out.boolean_value = left.uint16_value < right.uint16_value;
      break;
    case Uint32_Kind:
      out.boolean_value = left.uint32_value < right.uint32_value;
      break;
    case Uint64_Kind:
      out.boolean_value = left.uint64_value < right.uint64_value;
      break;
    case Int8_Kind:
      out.boolean_value = left.int8_value < right.int8_value;
      break;
    case Int16_Kind:
      out.boolean_value = left.int16_value < right.int16_value;
      break;
    case Int32_Kind:
      out.boolean_value = left.int32_value < right.int32_value;
      break;
    case Int64_Kind:
      out.boolean_value = left.int64_value < right.int64_value;
      break;
    case Float32_Kind:
      out.boolean_value = left.float32_value < right.float32_value;
      break;
    case Float64_Kind:
      out.boolean_value = left.float64_value < right.float64_value;
      break;
    case Uint_Kind:
      out.boolean_value = left.uint_value < right.uint_value;
      break;
    case Int_Kind:
      out.boolean_value = left.int_value < right.int_value;
      break;
    case Uintptr_Kind:
      out.boolean_value = left.uintptr_value < right.uintptr_value;
      break;
    case String_Kind:
      out.boolean_value = left.stringu_value < right.stringu_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void less_equal (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.boolean_value = left.rune_value <= right.rune_value;
      break;
    case Untyped_Integer_Kind:
      out.boolean_value = left.integer_value <= right.integer_value;
      break;
    case Untyped_Float_Kind:
      out.boolean_value = left.float_value <= right.float_value;
      break;
    case Untyped_String_Kind:
      out.boolean_value = left.string_value <= right.string_value;
      break;
    case Uint8_Kind:
      out.boolean_value = left.uint8_value <= right.uint8_value;
      break;
    case Uint16_Kind:
      out.boolean_value = left.uint16_value <= right.uint16_value;
      break;
    case Uint32_Kind:
      out.boolean_value = left.uint32_value <= right.uint32_value;
      break;
    case Uint64_Kind:
      out.boolean_value = left.uint64_value <= right.uint64_value;
      break;
    case Int8_Kind:
      out.boolean_value = left.int8_value <= right.int8_value;
      break;
    case Int16_Kind:
      out.boolean_value = left.int16_value <= right.int16_value;
      break;
    case Int32_Kind:
      out.boolean_value = left.int32_value <= right.int32_value;
      break;
    case Int64_Kind:
      out.boolean_value = left.int64_value <= right.int64_value;
      break;
    case Float32_Kind:
      out.boolean_value = left.float32_value <= right.float32_value;
      break;
    case Float64_Kind:
      out.boolean_value = left.float64_value <= right.float64_value;
      break;
    case Uint_Kind:
      out.boolean_value = left.uint_value <= right.uint_value;
      break;
    case Int_Kind:
      out.boolean_value = left.int_value <= right.int_value;
      break;
    case Uintptr_Kind:
      out.boolean_value = left.uintptr_value <= right.uintptr_value;
      break;
    case String_Kind:
      out.boolean_value = left.stringu_value <= right.stringu_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void more_than (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.boolean_value = left.rune_value > right.rune_value;
      break;
    case Untyped_Integer_Kind:
      out.boolean_value = left.integer_value > right.integer_value;
      break;
    case Untyped_Float_Kind:
      out.boolean_value = left.float_value > right.float_value;
      break;
    case Untyped_String_Kind:
      out.boolean_value = left.string_value > right.string_value;
      break;
    case Uint8_Kind:
      out.boolean_value = left.uint8_value > right.uint8_value;
      break;
    case Uint16_Kind:
      out.boolean_value = left.uint16_value > right.uint16_value;
      break;
    case Uint32_Kind:
      out.boolean_value = left.uint32_value > right.uint32_value;
      break;
    case Uint64_Kind:
      out.boolean_value = left.uint64_value > right.uint64_value;
      break;
    case Int8_Kind:
      out.boolean_value = left.int8_value > right.int8_value;
      break;
    case Int16_Kind:
      out.boolean_value = left.int16_value > right.int16_value;
      break;
    case Int32_Kind:
      out.boolean_value = left.int32_value > right.int32_value;
      break;
    case Int64_Kind:
      out.boolean_value = left.int64_value > right.int64_value;
      break;
    case Float32_Kind:
      out.boolean_value = left.float32_value > right.float32_value;
      break;
    case Float64_Kind:
      out.boolean_value = left.float64_value > right.float64_value;
      break;
    case Uint_Kind:
      out.boolean_value = left.uint_value > right.uint_value;
      break;
    case Int_Kind:
      out.boolean_value = left.int_value > right.int_value;
      break;
    case Uintptr_Kind:
      out.boolean_value = left.uintptr_value > right.uintptr_value;
      break;
    case String_Kind:
      out.boolean_value = left.stringu_value > right.stringu_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void more_equal (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.boolean_value = left.rune_value >= right.rune_value;
      break;
    case Untyped_Integer_Kind:
      out.boolean_value = left.integer_value >= right.integer_value;
      break;
    case Untyped_Float_Kind:
      out.boolean_value = left.float_value >= right.float_value;
      break;
    case Untyped_String_Kind:
      out.boolean_value = left.string_value >= right.string_value;
      break;
    case Uint8_Kind:
      out.boolean_value = left.uint8_value >= right.uint8_value;
      break;
    case Uint16_Kind:
      out.boolean_value = left.uint16_value >= right.uint16_value;
      break;
    case Uint32_Kind:
      out.boolean_value = left.uint32_value >= right.uint32_value;
      break;
    case Uint64_Kind:
      out.boolean_value = left.uint64_value >= right.uint64_value;
      break;
    case Int8_Kind:
      out.boolean_value = left.int8_value >= right.int8_value;
      break;
    case Int16_Kind:
      out.boolean_value = left.int16_value >= right.int16_value;
      break;
    case Int32_Kind:
      out.boolean_value = left.int32_value >= right.int32_value;
      break;
    case Int64_Kind:
      out.boolean_value = left.int64_value >= right.int64_value;
      break;
    case Float32_Kind:
      out.boolean_value = left.float32_value >= right.float32_value;
      break;
    case Float64_Kind:
      out.boolean_value = left.float64_value >= right.float64_value;
      break;
    case Uint_Kind:
      out.boolean_value = left.uint_value >= right.uint_value;
      break;
    case Int_Kind:
      out.boolean_value = left.int_value >= right.int_value;
      break;
    case Uintptr_Kind:
      out.boolean_value = left.uintptr_value >= right.uintptr_value;
      break;
    case String_Kind:
      out.boolean_value = left.stringu_value >= right.stringu_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void multiply (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.rune_value = left.rune_value * right.rune_value;
      break;
    case Untyped_Integer_Kind:
      out.integer_value = left.integer_value * right.integer_value;
      break;
    case Untyped_Float_Kind:
      out.float_value = left.float_value * right.float_value;
      break;
    case Untyped_Complex_Kind:
      out.complex_value = left.complex_value * right.complex_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value * right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value * right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value * right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value * right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value * right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value * right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value * right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value * right.int64_value;
      break;
    case Float32_Kind:
      out.float32_value = left.float32_value * right.float32_value;
      break;
    case Float64_Kind:
      out.float64_value = left.float64_value * right.float64_value;
      break;
    case Complex64_Kind:
      out.complex64_value = left.complex64_value * right.complex64_value;
      break;
    case Complex128_Kind:
      out.complex128_value = left.complex128_value * right.complex128_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value * right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value * right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value * right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void divide (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.rune_value = left.rune_value / right.rune_value;
      break;
    case Untyped_Integer_Kind:
      out.integer_value = left.integer_value / right.integer_value;
      break;
    case Untyped_Float_Kind:
      out.float_value = left.float_value / right.float_value;
      break;
    case Untyped_Complex_Kind:
      out.complex_value = left.complex_value / right.complex_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value / right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value / right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value / right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value / right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value / right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value / right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value / right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value / right.int64_value;
      break;
    case Float32_Kind:
      out.float32_value = left.float32_value / right.float32_value;
      break;
    case Float64_Kind:
      out.float64_value = left.float64_value / right.float64_value;
      break;
    case Complex64_Kind:
      out.complex64_value = left.complex64_value / right.complex64_value;
      break;
    case Complex128_Kind:
      out.complex128_value = left.complex128_value / right.complex128_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value / right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value / right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value / right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void modulus (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.rune_value = left.rune_value % right.rune_value;
      break;
    case Untyped_Integer_Kind:
      out.integer_value = left.integer_value % right.integer_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value % right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value % right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value % right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value % right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value % right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value % right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value % right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value % right.int64_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value % right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value % right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value % right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void add (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.rune_value = left.rune_value + right.rune_value;
      break;
    case Untyped_Integer_Kind:
      out.integer_value = left.integer_value + right.integer_value;
      break;
    case Untyped_Float_Kind:
      out.float_value = left.float_value + right.float_value;
      break;
    case Untyped_Complex_Kind:
      out.complex_value = left.complex_value + right.complex_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value + right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value + right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value + right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value + right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value + right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value + right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value + right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value + right.int64_value;
      break;
    case Float32_Kind:
      out.float32_value = left.float32_value + right.float32_value;
      break;
    case Float64_Kind:
      out.float64_value = left.float64_value + right.float64_value;
      break;
    case Complex64_Kind:
      out.complex64_value = left.complex64_value + right.complex64_value;
      break;
    case Complex128_Kind:
      out.complex128_value = left.complex128_value + right.complex128_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value + right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value + right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value + right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void subtract (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.rune_value = left.rune_value - right.rune_value;
      break;
    case Untyped_Integer_Kind:
      out.integer_value = left.integer_value - right.integer_value;
      break;
    case Untyped_Float_Kind:
      out.float_value = left.float_value - right.float_value;
      break;
    case Untyped_Complex_Kind:
      out.complex_value = left.complex_value - right.complex_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value - right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value - right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value - right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value - right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value - right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value - right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value - right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value - right.int64_value;
      break;
    case Float32_Kind:
      out.float32_value = left.float32_value - right.float32_value;
      break;
    case Float64_Kind:
      out.float64_value = left.float64_value - right.float64_value;
      break;
    case Complex64_Kind:
      out.complex64_value = left.complex64_value - right.complex64_value;
      break;
    case Complex128_Kind:
      out.complex128_value = left.complex128_value - right.complex128_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value - right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value - right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value - right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void bit_and (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.rune_value = left.rune_value & right.rune_value;
      break;
    case Untyped_Integer_Kind:
      out.integer_value = left.integer_value & right.integer_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value & right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value & right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value & right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value & right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value & right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value & right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value & right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value & right.int64_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value & right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value & right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value & right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void bit_and_not (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.rune_value = left.rune_value & ~right.rune_value;
      break;
    case Untyped_Integer_Kind:
      out.integer_value = left.integer_value & ~right.integer_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value & ~right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value & ~right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value & ~right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value & ~right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value & ~right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value & ~right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value & ~right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value & ~right.int64_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value & ~right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value & ~right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value & ~right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void bit_xor (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.rune_value = left.rune_value ^ right.rune_value;
      break;
    case Untyped_Integer_Kind:
      out.integer_value = left.integer_value ^ right.integer_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value ^ right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value ^ right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value ^ right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value ^ right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value ^ right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value ^ right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value ^ right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value ^ right.int64_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value ^ right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value ^ right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value ^ right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void bit_or (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.rune_value = left.rune_value | right.rune_value;
      break;
    case Untyped_Integer_Kind:
      out.integer_value = left.integer_value | right.integer_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value | right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value | right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value | right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value | right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value | right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value | right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value | right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value | right.int64_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value | right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value | right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value | right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

ValuePrinter::ValuePrinter (const type::Type* t, const Value& v)
  : type (t)
  , value (v)
{ }

std::ostream& operator<< (std::ostream& out, const ValuePrinter& vp)
{
  if (vp.value.present)
    {
      struct visitor : public type::DefaultVisitor
      {
        const Value& tv;
        std::ostream& out;

        visitor (const Value& v, std::ostream& o)
          : tv (v)
          , out (o)
        { }

        void default_action (const type::Type& type)
        {
          TYPE_NOT_REACHED(type);
        }

        void visit (const type::Bool& type)
        {
          out << " value=" << tv.bool_value;
        }

        void visit (const Uint8& type)
        {
          out << " value=" << (int)tv.uint8_value;
        }

        void visit (const Uint16& type)
        {
          out << " value=" << (int)tv.uint16_value;
        }

        void visit (const Uint32& type)
        {
          out << " value=" << tv.uint32_value;
        }

        void visit (const Uint64& type)
        {
          out << " value=" << tv.uint64_value;
        }

        void visit (const Int8& type)
        {
          out << " value=" << (int)tv.int8_value;
        }

        void visit (const Int16& type)
        {
          out << " value=" << (int)tv.int16_value;
        }

        void visit (const Int32& type)
        {
          out << " value=" << tv.int32_value;
        }

        void visit (const Int64& type)
        {
          out << " value=" << tv.int64_value;
        }

        void visit (const Uint& type)
        {
          out << " value=" << tv.uint_value;
        }

        void visit (const Int& type)
        {
          out << " value=" << tv.int_value;
        }

        void visit (const Uintptr& type)
        {
          out << " value=" << tv.uintptr_value;
        }

        void visit (const Float32& type)
        {
          out << " value=" << tv.float32_value;
        }

        void visit (const Float64& type)
        {
          out << " value=" << tv.float64_value;
        }

        void visit (const Complex64& type)
        {
          out << " value=(" << tv.complex64_value.real << ',' << tv.complex64_value.imag << "i)";
        }

        void visit (const Complex128& type)
        {
          out << " value=(" << tv.complex128_value.real << ',' << tv.complex128_value.imag << "i)";
        }

        void visit (const UntypedNil& type)
        {
          out << " value=" << (void*)NULL;
        }

        void visit (const type::Function& type)
        {
          out << " value=<function>";
        }

        void visit (const type::Method& type)
        {
          out << " value=<method>";
        }

        void visit (const Slice& type)
        {
          const Slice::ValueType& s = tv.slice_value;
          out << " value={" << s.ptr << ',' << s.length << ',' << s.capacity << '}';
        }
        void visit (const String& type)
        {
          const String::ValueType& s = tv.stringu_value;
          out << " value={" << s.ptr << ',' << s.length << '}';
        }

        void visit (const Pointer& type)
        {
          out << " value=" << tv.pointer_value;
        }

        void visit (const UntypedBoolean& type)
        {
          out << " value=" << tv.boolean_value;
        }
        void visit (const UntypedInteger& type)
        {
          out << " value=" << tv.integer_value;
        }
        void visit (const UntypedFloat& type)
        {
          out << " value=" << tv.float_value;
        }
        void visit (const UntypedString& type)
        {
          const UntypedString::ValueType& s = tv.string_value;
          out << " value={" << s.ptr << ',' << s.length << '}';
        }
        void visit (const type::PolymorphicFunction& type)
        {
          out << " value=<polymorphic function>";
        }
      };
      visitor v (vp.value, out);
      type_strip (vp.type)->accept (v);
    }

  return out;
}

}
