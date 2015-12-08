#include "value.hpp"

#include <utility>

#include "type.hpp"

using namespace Type;
using namespace std::rel_ops;

// struct alpha_visitor : public Type::DefaultVisitor
// {
//   bool flag;
//   Complex::ValueType x;

//   alpha_visitor (Complex::ValueType z) : flag (false), x (z) { }

//   void default_action (const Type::Type& type)
//   {
//     type_not_reached (type);
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
static bool to_and_back (const T& x, const Type::Type* type)
{
  switch (type->underlying_kind ())
    {
    case kUint8:
      return to_and_back_helper<Uint8::ValueType> (x);
    case kUint16:
      return to_and_back_helper<Uint16::ValueType> (x);
    case kUint32:
      return to_and_back_helper<Uint32::ValueType> (x);
    case kUint64:
      return to_and_back_helper<Uint64::ValueType> (x);
    case kInt8:
      return to_and_back_helper<Int8::ValueType> (x);
    case kInt16:
      return to_and_back_helper<Int16::ValueType> (x);
    case kInt32:
      return to_and_back_helper<Int32::ValueType> (x);
    case kInt64:
      return to_and_back_helper<Int64::ValueType> (x);
    case kFloat32:
      return to_and_back_helper<Float32::ValueType> (x);
    case kFloat64:
      return to_and_back_helper<Float64::ValueType> (x);
    case kComplex64:
      return to_and_back_helper<Complex64::ValueType> (x);
    case kComplex128:
      return to_and_back_helper<Complex128::ValueType> (x);
    case kUint:
      return to_and_back_helper<Uint::ValueType> (x);
    case kInt:
      return to_and_back_helper<Int::ValueType> (x);
    case kUintptr:
      return to_and_back_helper<Uintptr::ValueType> (x);
    default:
      return false;
    }
}

static void convert_numeric (value_t& value, Complex::ValueType x, const Type::Type* type)
{
  struct visitor : public Type::DefaultVisitor
  {
    value_t& value;
    Complex::ValueType x;

    visitor (value_t& v, Complex::ValueType z) : value (v), x (z) { }

    void default_action (const Type::Type& type)
    {
      type_not_reached (type);
    }

    void visit (const Uint8& type)
    {
      value.ref (type) = x;
    }

    void visit (const Uint16& type)
    {
      value.ref (type) = x;
    }

    void visit (const Uint32& type)
    {
      value.ref (type) = x;
    }

    void visit (const Uint64& type)
    {
      value.ref (type) = x;
    }

    void visit (const Int8& type)
    {
      value.ref (type) = x;
    }

    void visit (const Int16& type)
    {
      value.ref (type) = x;
    }

    void visit (const Int32& type)
    {
      value.ref (type) = x;
    }

    void visit (const Int64& type)
    {
      value.ref (type) = x;
    }

    void visit (const Uint& type)
    {
      value.ref (type) = x;
    }

    void visit (const Int& type)
    {
      value.ref (type) = x;
    }

    void visit (const Float32& type)
    {
      value.ref (type) = x;
    }

    void visit (const Float64& type)
    {
      value.ref (type) = x;
    }

    void visit (const Complex64& type)
    {
      value.ref (type) = x;
    }

    void visit (const Complex128& type)
    {
      value.ref (type) = x;
    }

    void visit (const Integer& type)
    {
      value.ref (type) = x;
    }
  };

  visitor v (value, x);
  type->Accept (v);
}

bool
value_t::representable (const Type::Type* from, const Type::Type* to) const
{
  assert (present);
  assert (from->IsUntyped ());
  assert (!to->IsUntyped ());

  switch (from->underlying_kind ())
    {
    case kNil:
      return
        to->underlying_kind () == kPointer ||
        to->underlying_kind () == kSlice;
    case kBoolean:
      return to->underlying_kind () == kBool;
    case kRune:
      return to_and_back (rune_value_, to);
    case kInteger:
      return to_and_back (integer_value_, to);
    case kFloat:
      return to_and_back (float_value_, to);
    case kComplex:
      return to_and_back (complex_value_, to);
    case kString:
      return to->underlying_kind () == kStringU;
    default:
      not_reached;
    }

  // struct visitor : public Type::DefaultVisitor
  // {
  //   const value_t& value;
  //   const Type::Type* to;
  //   bool flag;

  //   visitor (const value_t& v, const Type::Type* t) : value (v), to (t), flag (false) { }

  //   void default_action (const Type::Type& type)
  //   {
  //     type_not_reached (type);
  //   }

  //   void visit (const Type::Int& type)
  //   {
  //     Complex::ValueType x;
  //     x = value.ref (type);
  //     flag = to_and_back (x, to->UnderlyingType ());
  //   }

  //   void visit (const Type::Boolean& type)
  //   {
  //     flag =
  //       type_cast<Type::Boolean> (to) != NULL ||
  //       type_cast<Type::Bool> (to) != NULL;
  //   }

  //   void visit (const Type::Rune& type)
  //   {
  //     Complex::ValueType x;
  //     x = value.ref (type);
  //     flag = to_and_back (x, to->UnderlyingType ());
  //   }

  //   void visit (const Type::Integer& type)
  //   {
  //     Complex::ValueType x;
  //     x = value.ref (type);
  //     flag = to_and_back (x, to->UnderlyingType ());
  //   }

  //   void visit (const Type::Float& type)
  //   {
  //     Complex::ValueType x;
  //     x = value.ref (type);
  //     flag = to_and_back (x, to->UnderlyingType ());
  //   }

  //   void visit (const Type::String& type)
  //   {
  //     if (Type::type_cast<Type::StringU> (to->UnderlyingType ()))
  //       {
  //         flag = true;
  //       }
  //   }
  // } v (*this, to->UnderlyingType ());
  // from->UnderlyingType ()->Accept (v);
  // return v.flag;
}

void
value_t::convert (const Type::Type* from, const Type::Type* to)
{
  assert (present);

  struct visitor : public Type::DefaultVisitor
  {
    value_t& value;
    const Type::Type* to;

    visitor (value_t& v, const Type::Type* t) : value (v), to (t) { }

    void default_action (const Type::Type& type)
    {
      type_not_reached (type);
    }

    void visit (const Type::Boolean& type)
    {
      if (type_cast<Type::Bool> (to) != NULL)
        {
          value.ref (*Type::Bool::Instance ()) = value.ref (type);
        }
    }

    void visit (const Type::Rune& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Integer& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Float& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Uint8& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Uint16& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Uint32& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Uint64& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Int8& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Int16& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Int32& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Int64& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Int& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Float32& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Float64& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Complex64& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Complex128& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::String& type)
    {
      if (Type::type_cast<Type::StringU> (to))
        {
          value.stringu_value_ = value.string_value_;
          return;
        }

      not_reached;
    }

    void visit (const Type::Nil& type)
    {
      if (Type::type_cast<Type::Pointer> (to))
        {
          value.pointer_value_ = NULL;
          return;
        }
      not_reached;
    }

  } v (*this, to->UnderlyingType ());

  from->UnderlyingType ()->Accept (v);
}

void
value_t::print (std::ostream& out, const Type::Type* type) const
{
  if (present)
    {
      struct visitor : public Type::DefaultVisitor
      {
        const value_t& tv;
        std::ostream& out;

        visitor (const value_t& v, std::ostream& o)
          : tv (v)
          , out (o)
        { }

        void default_action (const Type::Type& type)
        {
          type_not_reached(type);
        }

        void visit (const Type::Bool& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Uint8& type)
        {
          out << " value=" << (int)tv.ref (type);
        }

        void visit (const Uint16& type)
        {
          out << " value=" << (int)tv.ref (type);
        }

        void visit (const Uint32& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Uint64& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Int8& type)
        {
          out << " value=" << (int)tv.ref (type);
        }

        void visit (const Int16& type)
        {
          out << " value=" << (int)tv.ref (type);
        }

        void visit (const Int32& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Int64& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Uint& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Int& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Uintptr& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Float32& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Float64& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Complex64& type)
        {
          out << " value=(" << tv.ref (type).real << ',' << tv.ref (type).imag << "i)";
        }

        void visit (const Complex128& type)
        {
          out << " value=(" << tv.ref (type).real << ',' << tv.ref (type).imag << "i)";
        }

        void visit (const Nil& type)
        {
          out << " value=" << (void*)NULL;
        }

        void visit (const Type::Function& type)
        {
          out << " value=<function>";
        }

        void visit (const Type::Method& type)
        {
          out << " value=<method>";
        }

        void visit (const Slice& type)
        {
          const Slice::ValueType& s = tv.ref (type);
          out << " value={" << s.ptr << ',' << s.length << ',' << s.capacity << '}';
        }
        void visit (const StringU& type)
        {
          const StringU::ValueType& s = tv.ref (type);
          out << " value={" << s.ptr << ',' << s.length << '}';
        }

        void visit (const Pointer& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Boolean& type)
        {
          out << " value=" << tv.ref (type);
        }
        void visit (const Integer& type)
        {
          out << " value=" << tv.ref (type);
        }
        void visit (const Float& type)
        {
          out << " value=" << tv.ref (type);
        }
        void visit (const String& type)
        {
          const String::ValueType& s = tv.ref (type);
          out << " value={" << s.ptr << ',' << s.length << '}';
        }
        void visit (const Type::Template& type)
        {
          out << " value=<template>";
        }
      };
      visitor v (*this, out);
      type_strip (type)->Accept (v);
    }
}


Type::Int::ValueType
value_t::to_int (const Type::Type* type) const
{
  assert (is_typed_integer (type));
  switch (type->underlying_kind ())
    {
    case kUint8:
      return uint8_value_;
    case kUint16:
      return uint16_value_;
    case kUint32:
      return uint32_value_;
    case kUint64:
      return uint64_value_;
    case kInt8:
      return int8_value_;
    case kInt16:
      return int16_value_;
    case kInt32:
      return int32_value_;
    case kInt64:
      return int64_value_;
    case kUint:
      return uint_value_;
    case kInt:
      return int_value_;
    case kUintptr:
      return uintptr_value_;
    default:
      not_reached;
    }
}

void equal (value_t& out, const Type::Type* type, const value_t& left, const value_t& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case kNil:
      out.boolean_value_ = true;
      break;
    case kBoolean:
      out.boolean_value_ = left.boolean_value_ == right.boolean_value_;
      break;
    case kRune:
      out.boolean_value_ = left.rune_value_ == right.rune_value_;
      break;
    case kInteger:
      out.boolean_value_ = left.integer_value_ == right.integer_value_;
      break;
    case kFloat:
      out.boolean_value_ = left.float_value_ == right.float_value_;
      break;
    case kComplex:
      out.boolean_value_ = left.complex_value_ == right.complex_value_;
      break;
    case kString:
      out.boolean_value_ = left.string_value_ == right.string_value_;
      break;
    case kBool:
      out.boolean_value_ = left.bool_value_ == right.bool_value_;
      break;
    case kUint8:
      out.boolean_value_ = left.uint8_value_ == right.uint8_value_;
      break;
    case kUint16:
      out.boolean_value_ = left.uint16_value_ == right.uint16_value_;
      break;
    case kUint32:
      out.boolean_value_ = left.uint32_value_ == right.uint32_value_;
      break;
    case kUint64:
      out.boolean_value_ = left.uint64_value_ == right.uint64_value_;
      break;
    case kInt8:
      out.boolean_value_ = left.int8_value_ == right.int8_value_;
      break;
    case kInt16:
      out.boolean_value_ = left.int16_value_ == right.int16_value_;
      break;
    case kInt32:
      out.boolean_value_ = left.int32_value_ == right.int32_value_;
      break;
    case kInt64:
      out.boolean_value_ = left.int64_value_ == right.int64_value_;
      break;
    case kFloat32:
      out.boolean_value_ = left.float32_value_ == right.float32_value_;
      break;
    case kFloat64:
      out.boolean_value_ = left.float64_value_ == right.float64_value_;
      break;
    case kComplex64:
      out.boolean_value_ = left.complex64_value_ == right.complex64_value_;
      break;
    case kComplex128:
      out.boolean_value_ = left.complex128_value_ == right.complex128_value_;
      break;
    case kUint:
      out.boolean_value_ = left.uint_value_ == right.uint_value_;
      break;
    case kInt:
      out.boolean_value_ = left.int_value_ == right.int_value_;
      break;
    case kUintptr:
      out.boolean_value_ = left.uintptr_value_ == right.uintptr_value_;
      break;
    case kStringU:
      out.boolean_value_ = left.stringu_value_ == right.stringu_value_;
      break;
    case kPointer:
      out.boolean_value_ = left.pointer_value_ == right.pointer_value_;
      break;
    default:
      type_not_reached (*type);
    }
}

void not_equal (value_t& out, const Type::Type* type, const value_t& left, const value_t& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case kNil:
      out.boolean_value_ = false;
      break;
    case kBoolean:
      out.boolean_value_ = left.boolean_value_ != right.boolean_value_;
      break;
    case kRune:
      out.boolean_value_ = left.rune_value_ != right.rune_value_;
      break;
    case kInteger:
      out.boolean_value_ = left.integer_value_ != right.integer_value_;
      break;
    case kFloat:
      out.boolean_value_ = left.float_value_ != right.float_value_;
      break;
    case kComplex:
      out.boolean_value_ = left.complex_value_ != right.complex_value_;
      break;
    case kString:
      out.boolean_value_ = left.string_value_ != right.string_value_;
      break;
    case kBool:
      out.boolean_value_ = left.bool_value_ != right.bool_value_;
      break;
    case kUint8:
      out.boolean_value_ = left.uint8_value_ != right.uint8_value_;
      break;
    case kUint16:
      out.boolean_value_ = left.uint16_value_ != right.uint16_value_;
      break;
    case kUint32:
      out.boolean_value_ = left.uint32_value_ != right.uint32_value_;
      break;
    case kUint64:
      out.boolean_value_ = left.uint64_value_ != right.uint64_value_;
      break;
    case kInt8:
      out.boolean_value_ = left.int8_value_ != right.int8_value_;
      break;
    case kInt16:
      out.boolean_value_ = left.int16_value_ != right.int16_value_;
      break;
    case kInt32:
      out.boolean_value_ = left.int32_value_ != right.int32_value_;
      break;
    case kInt64:
      out.boolean_value_ = left.int64_value_ != right.int64_value_;
      break;
    case kFloat32:
      out.boolean_value_ = left.float32_value_ != right.float32_value_;
      break;
    case kFloat64:
      out.boolean_value_ = left.float64_value_ != right.float64_value_;
      break;
    case kComplex64:
      out.boolean_value_ = left.complex64_value_ != right.complex64_value_;
      break;
    case kComplex128:
      out.boolean_value_ = left.complex128_value_ != right.complex128_value_;
      break;
    case kUint:
      out.boolean_value_ = left.uint_value_ != right.uint_value_;
      break;
    case kInt:
      out.boolean_value_ = left.int_value_ != right.int_value_;
      break;
    case kUintptr:
      out.boolean_value_ = left.uintptr_value_ != right.uintptr_value_;
      break;
    case kStringU:
      out.boolean_value_ = left.stringu_value_ != right.stringu_value_;
      break;
    case kPointer:
      out.boolean_value_ = left.pointer_value_ != right.pointer_value_;
      break;
    default:
      type_not_reached (*type);
    }
}

void less_than (value_t& out, const Type::Type* type, const value_t& left, const value_t& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case kRune:
      out.boolean_value_ = left.rune_value_ < right.rune_value_;
      break;
    case kInteger:
      out.boolean_value_ = left.integer_value_ < right.integer_value_;
      break;
    case kFloat:
      out.boolean_value_ = left.float_value_ < right.float_value_;
      break;
    case kString:
      out.boolean_value_ = left.string_value_ < right.string_value_;
      break;
    case kUint8:
      out.boolean_value_ = left.uint8_value_ < right.uint8_value_;
      break;
    case kUint16:
      out.boolean_value_ = left.uint16_value_ < right.uint16_value_;
      break;
    case kUint32:
      out.boolean_value_ = left.uint32_value_ < right.uint32_value_;
      break;
    case kUint64:
      out.boolean_value_ = left.uint64_value_ < right.uint64_value_;
      break;
    case kInt8:
      out.boolean_value_ = left.int8_value_ < right.int8_value_;
      break;
    case kInt16:
      out.boolean_value_ = left.int16_value_ < right.int16_value_;
      break;
    case kInt32:
      out.boolean_value_ = left.int32_value_ < right.int32_value_;
      break;
    case kInt64:
      out.boolean_value_ = left.int64_value_ < right.int64_value_;
      break;
    case kFloat32:
      out.boolean_value_ = left.float32_value_ < right.float32_value_;
      break;
    case kFloat64:
      out.boolean_value_ = left.float64_value_ < right.float64_value_;
      break;
    case kUint:
      out.boolean_value_ = left.uint_value_ < right.uint_value_;
      break;
    case kInt:
      out.boolean_value_ = left.int_value_ < right.int_value_;
      break;
    case kUintptr:
      out.boolean_value_ = left.uintptr_value_ < right.uintptr_value_;
      break;
    case kStringU:
      out.boolean_value_ = left.stringu_value_ < right.stringu_value_;
      break;
    default:
      type_not_reached (*type);
    }
}

void less_equal (value_t& out, const Type::Type* type, const value_t& left, const value_t& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case kRune:
      out.boolean_value_ = left.rune_value_ <= right.rune_value_;
      break;
    case kInteger:
      out.boolean_value_ = left.integer_value_ <= right.integer_value_;
      break;
    case kFloat:
      out.boolean_value_ = left.float_value_ <= right.float_value_;
      break;
    case kString:
      out.boolean_value_ = left.string_value_ <= right.string_value_;
      break;
    case kUint8:
      out.boolean_value_ = left.uint8_value_ <= right.uint8_value_;
      break;
    case kUint16:
      out.boolean_value_ = left.uint16_value_ <= right.uint16_value_;
      break;
    case kUint32:
      out.boolean_value_ = left.uint32_value_ <= right.uint32_value_;
      break;
    case kUint64:
      out.boolean_value_ = left.uint64_value_ <= right.uint64_value_;
      break;
    case kInt8:
      out.boolean_value_ = left.int8_value_ <= right.int8_value_;
      break;
    case kInt16:
      out.boolean_value_ = left.int16_value_ <= right.int16_value_;
      break;
    case kInt32:
      out.boolean_value_ = left.int32_value_ <= right.int32_value_;
      break;
    case kInt64:
      out.boolean_value_ = left.int64_value_ <= right.int64_value_;
      break;
    case kFloat32:
      out.boolean_value_ = left.float32_value_ <= right.float32_value_;
      break;
    case kFloat64:
      out.boolean_value_ = left.float64_value_ <= right.float64_value_;
      break;
    case kUint:
      out.boolean_value_ = left.uint_value_ <= right.uint_value_;
      break;
    case kInt:
      out.boolean_value_ = left.int_value_ <= right.int_value_;
      break;
    case kUintptr:
      out.boolean_value_ = left.uintptr_value_ <= right.uintptr_value_;
      break;
    case kStringU:
      out.boolean_value_ = left.stringu_value_ <= right.stringu_value_;
      break;
    default:
      type_not_reached (*type);
    }
}

void more_than (value_t& out, const Type::Type* type, const value_t& left, const value_t& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case kRune:
      out.boolean_value_ = left.rune_value_ > right.rune_value_;
      break;
    case kInteger:
      out.boolean_value_ = left.integer_value_ > right.integer_value_;
      break;
    case kFloat:
      out.boolean_value_ = left.float_value_ > right.float_value_;
      break;
    case kString:
      out.boolean_value_ = left.string_value_ > right.string_value_;
      break;
    case kUint8:
      out.boolean_value_ = left.uint8_value_ > right.uint8_value_;
      break;
    case kUint16:
      out.boolean_value_ = left.uint16_value_ > right.uint16_value_;
      break;
    case kUint32:
      out.boolean_value_ = left.uint32_value_ > right.uint32_value_;
      break;
    case kUint64:
      out.boolean_value_ = left.uint64_value_ > right.uint64_value_;
      break;
    case kInt8:
      out.boolean_value_ = left.int8_value_ > right.int8_value_;
      break;
    case kInt16:
      out.boolean_value_ = left.int16_value_ > right.int16_value_;
      break;
    case kInt32:
      out.boolean_value_ = left.int32_value_ > right.int32_value_;
      break;
    case kInt64:
      out.boolean_value_ = left.int64_value_ > right.int64_value_;
      break;
    case kFloat32:
      out.boolean_value_ = left.float32_value_ > right.float32_value_;
      break;
    case kFloat64:
      out.boolean_value_ = left.float64_value_ > right.float64_value_;
      break;
    case kUint:
      out.boolean_value_ = left.uint_value_ > right.uint_value_;
      break;
    case kInt:
      out.boolean_value_ = left.int_value_ > right.int_value_;
      break;
    case kUintptr:
      out.boolean_value_ = left.uintptr_value_ > right.uintptr_value_;
      break;
    case kStringU:
      out.boolean_value_ = left.stringu_value_ > right.stringu_value_;
      break;
    default:
      type_not_reached (*type);
    }
}

void more_equal (value_t& out, const Type::Type* type, const value_t& left, const value_t& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case kRune:
      out.boolean_value_ = left.rune_value_ >= right.rune_value_;
      break;
    case kInteger:
      out.boolean_value_ = left.integer_value_ >= right.integer_value_;
      break;
    case kFloat:
      out.boolean_value_ = left.float_value_ >= right.float_value_;
      break;
    case kString:
      out.boolean_value_ = left.string_value_ >= right.string_value_;
      break;
    case kUint8:
      out.boolean_value_ = left.uint8_value_ >= right.uint8_value_;
      break;
    case kUint16:
      out.boolean_value_ = left.uint16_value_ >= right.uint16_value_;
      break;
    case kUint32:
      out.boolean_value_ = left.uint32_value_ >= right.uint32_value_;
      break;
    case kUint64:
      out.boolean_value_ = left.uint64_value_ >= right.uint64_value_;
      break;
    case kInt8:
      out.boolean_value_ = left.int8_value_ >= right.int8_value_;
      break;
    case kInt16:
      out.boolean_value_ = left.int16_value_ >= right.int16_value_;
      break;
    case kInt32:
      out.boolean_value_ = left.int32_value_ >= right.int32_value_;
      break;
    case kInt64:
      out.boolean_value_ = left.int64_value_ >= right.int64_value_;
      break;
    case kFloat32:
      out.boolean_value_ = left.float32_value_ >= right.float32_value_;
      break;
    case kFloat64:
      out.boolean_value_ = left.float64_value_ >= right.float64_value_;
      break;
    case kUint:
      out.boolean_value_ = left.uint_value_ >= right.uint_value_;
      break;
    case kInt:
      out.boolean_value_ = left.int_value_ >= right.int_value_;
      break;
    case kUintptr:
      out.boolean_value_ = left.uintptr_value_ >= right.uintptr_value_;
      break;
    case kStringU:
      out.boolean_value_ = left.stringu_value_ >= right.stringu_value_;
      break;
    default:
      type_not_reached (*type);
    }
}

void multiply (value_t& out, const Type::Type* type, const value_t& left, const value_t& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case kRune:
      out.rune_value_ = left.rune_value_ * right.rune_value_;
      break;
    case kInteger:
      out.integer_value_ = left.integer_value_ * right.integer_value_;
      break;
    case kFloat:
      out.float_value_ = left.float_value_ * right.float_value_;
      break;
    case kComplex:
      out.complex_value_ = left.complex_value_ * right.complex_value_;
      break;
    case kUint8:
      out.uint8_value_ = left.uint8_value_ * right.uint8_value_;
      break;
    case kUint16:
      out.uint16_value_ = left.uint16_value_ * right.uint16_value_;
      break;
    case kUint32:
      out.uint32_value_ = left.uint32_value_ * right.uint32_value_;
      break;
    case kUint64:
      out.uint64_value_ = left.uint64_value_ * right.uint64_value_;
      break;
    case kInt8:
      out.int8_value_ = left.int8_value_ * right.int8_value_;
      break;
    case kInt16:
      out.int16_value_ = left.int16_value_ * right.int16_value_;
      break;
    case kInt32:
      out.int32_value_ = left.int32_value_ * right.int32_value_;
      break;
    case kInt64:
      out.int64_value_ = left.int64_value_ * right.int64_value_;
      break;
    case kFloat32:
      out.float32_value_ = left.float32_value_ * right.float32_value_;
      break;
    case kFloat64:
      out.float64_value_ = left.float64_value_ * right.float64_value_;
      break;
    case kComplex64:
      out.complex64_value_ = left.complex64_value_ * right.complex64_value_;
      break;
    case kComplex128:
      out.complex128_value_ = left.complex128_value_ * right.complex128_value_;
      break;
    case kUint:
      out.uint_value_ = left.uint_value_ * right.uint_value_;
      break;
    case kInt:
      out.int_value_ = left.int_value_ * right.int_value_;
      break;
    case kUintptr:
      out.uintptr_value_ = left.uintptr_value_ * right.uintptr_value_;
      break;
    default:
      type_not_reached (*type);
    }
}

void divide (value_t& out, const Type::Type* type, const value_t& left, const value_t& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case kRune:
      out.rune_value_ = left.rune_value_ / right.rune_value_;
      break;
    case kInteger:
      out.integer_value_ = left.integer_value_ / right.integer_value_;
      break;
    case kFloat:
      out.float_value_ = left.float_value_ / right.float_value_;
      break;
    case kComplex:
      out.complex_value_ = left.complex_value_ / right.complex_value_;
      break;
    case kUint8:
      out.uint8_value_ = left.uint8_value_ / right.uint8_value_;
      break;
    case kUint16:
      out.uint16_value_ = left.uint16_value_ / right.uint16_value_;
      break;
    case kUint32:
      out.uint32_value_ = left.uint32_value_ / right.uint32_value_;
      break;
    case kUint64:
      out.uint64_value_ = left.uint64_value_ / right.uint64_value_;
      break;
    case kInt8:
      out.int8_value_ = left.int8_value_ / right.int8_value_;
      break;
    case kInt16:
      out.int16_value_ = left.int16_value_ / right.int16_value_;
      break;
    case kInt32:
      out.int32_value_ = left.int32_value_ / right.int32_value_;
      break;
    case kInt64:
      out.int64_value_ = left.int64_value_ / right.int64_value_;
      break;
    case kFloat32:
      out.float32_value_ = left.float32_value_ / right.float32_value_;
      break;
    case kFloat64:
      out.float64_value_ = left.float64_value_ / right.float64_value_;
      break;
    case kComplex64:
      out.complex64_value_ = left.complex64_value_ / right.complex64_value_;
      break;
    case kComplex128:
      out.complex128_value_ = left.complex128_value_ / right.complex128_value_;
      break;
    case kUint:
      out.uint_value_ = left.uint_value_ / right.uint_value_;
      break;
    case kInt:
      out.int_value_ = left.int_value_ / right.int_value_;
      break;
    case kUintptr:
      out.uintptr_value_ = left.uintptr_value_ / right.uintptr_value_;
      break;
    default:
      type_not_reached (*type);
    }
}

void modulus (value_t& out, const Type::Type* type, const value_t& left, const value_t& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case kRune:
      out.rune_value_ = left.rune_value_ % right.rune_value_;
      break;
    case kInteger:
      out.integer_value_ = left.integer_value_ % right.integer_value_;
      break;
    case kUint8:
      out.uint8_value_ = left.uint8_value_ % right.uint8_value_;
      break;
    case kUint16:
      out.uint16_value_ = left.uint16_value_ % right.uint16_value_;
      break;
    case kUint32:
      out.uint32_value_ = left.uint32_value_ % right.uint32_value_;
      break;
    case kUint64:
      out.uint64_value_ = left.uint64_value_ % right.uint64_value_;
      break;
    case kInt8:
      out.int8_value_ = left.int8_value_ % right.int8_value_;
      break;
    case kInt16:
      out.int16_value_ = left.int16_value_ % right.int16_value_;
      break;
    case kInt32:
      out.int32_value_ = left.int32_value_ % right.int32_value_;
      break;
    case kInt64:
      out.int64_value_ = left.int64_value_ % right.int64_value_;
      break;
    case kUint:
      out.uint_value_ = left.uint_value_ % right.uint_value_;
      break;
    case kInt:
      out.int_value_ = left.int_value_ % right.int_value_;
      break;
    case kUintptr:
      out.uintptr_value_ = left.uintptr_value_ % right.uintptr_value_;
      break;
    default:
      type_not_reached (*type);
    }
}

void add (value_t& out, const Type::Type* type, const value_t& left, const value_t& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case kRune:
      out.rune_value_ = left.rune_value_ + right.rune_value_;
      break;
    case kInteger:
      out.integer_value_ = left.integer_value_ + right.integer_value_;
      break;
    case kFloat:
      out.float_value_ = left.float_value_ + right.float_value_;
      break;
    case kComplex:
      out.complex_value_ = left.complex_value_ + right.complex_value_;
      break;
    case kUint8:
      out.uint8_value_ = left.uint8_value_ + right.uint8_value_;
      break;
    case kUint16:
      out.uint16_value_ = left.uint16_value_ + right.uint16_value_;
      break;
    case kUint32:
      out.uint32_value_ = left.uint32_value_ + right.uint32_value_;
      break;
    case kUint64:
      out.uint64_value_ = left.uint64_value_ + right.uint64_value_;
      break;
    case kInt8:
      out.int8_value_ = left.int8_value_ + right.int8_value_;
      break;
    case kInt16:
      out.int16_value_ = left.int16_value_ + right.int16_value_;
      break;
    case kInt32:
      out.int32_value_ = left.int32_value_ + right.int32_value_;
      break;
    case kInt64:
      out.int64_value_ = left.int64_value_ + right.int64_value_;
      break;
    case kFloat32:
      out.float32_value_ = left.float32_value_ + right.float32_value_;
      break;
    case kFloat64:
      out.float64_value_ = left.float64_value_ + right.float64_value_;
      break;
    case kComplex64:
      out.complex64_value_ = left.complex64_value_ + right.complex64_value_;
      break;
    case kComplex128:
      out.complex128_value_ = left.complex128_value_ + right.complex128_value_;
      break;
    case kUint:
      out.uint_value_ = left.uint_value_ + right.uint_value_;
      break;
    case kInt:
      out.int_value_ = left.int_value_ + right.int_value_;
      break;
    case kUintptr:
      out.uintptr_value_ = left.uintptr_value_ + right.uintptr_value_;
      break;
    default:
      type_not_reached (*type);
    }
}

void subtract (value_t& out, const Type::Type* type, const value_t& left, const value_t& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case kRune:
      out.rune_value_ = left.rune_value_ - right.rune_value_;
      break;
    case kInteger:
      out.integer_value_ = left.integer_value_ - right.integer_value_;
      break;
    case kFloat:
      out.float_value_ = left.float_value_ - right.float_value_;
      break;
    case kComplex:
      out.complex_value_ = left.complex_value_ - right.complex_value_;
      break;
    case kUint8:
      out.uint8_value_ = left.uint8_value_ - right.uint8_value_;
      break;
    case kUint16:
      out.uint16_value_ = left.uint16_value_ - right.uint16_value_;
      break;
    case kUint32:
      out.uint32_value_ = left.uint32_value_ - right.uint32_value_;
      break;
    case kUint64:
      out.uint64_value_ = left.uint64_value_ - right.uint64_value_;
      break;
    case kInt8:
      out.int8_value_ = left.int8_value_ - right.int8_value_;
      break;
    case kInt16:
      out.int16_value_ = left.int16_value_ - right.int16_value_;
      break;
    case kInt32:
      out.int32_value_ = left.int32_value_ - right.int32_value_;
      break;
    case kInt64:
      out.int64_value_ = left.int64_value_ - right.int64_value_;
      break;
    case kFloat32:
      out.float32_value_ = left.float32_value_ - right.float32_value_;
      break;
    case kFloat64:
      out.float64_value_ = left.float64_value_ - right.float64_value_;
      break;
    case kComplex64:
      out.complex64_value_ = left.complex64_value_ - right.complex64_value_;
      break;
    case kComplex128:
      out.complex128_value_ = left.complex128_value_ - right.complex128_value_;
      break;
    case kUint:
      out.uint_value_ = left.uint_value_ - right.uint_value_;
      break;
    case kInt:
      out.int_value_ = left.int_value_ - right.int_value_;
      break;
    case kUintptr:
      out.uintptr_value_ = left.uintptr_value_ - right.uintptr_value_;
      break;
    default:
      type_not_reached (*type);
    }
}

void bit_and (value_t& out, const Type::Type* type, const value_t& left, const value_t& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case kRune:
      out.rune_value_ = left.rune_value_ & right.rune_value_;
      break;
    case kInteger:
      out.integer_value_ = left.integer_value_ & right.integer_value_;
      break;
    case kUint8:
      out.uint8_value_ = left.uint8_value_ & right.uint8_value_;
      break;
    case kUint16:
      out.uint16_value_ = left.uint16_value_ & right.uint16_value_;
      break;
    case kUint32:
      out.uint32_value_ = left.uint32_value_ & right.uint32_value_;
      break;
    case kUint64:
      out.uint64_value_ = left.uint64_value_ & right.uint64_value_;
      break;
    case kInt8:
      out.int8_value_ = left.int8_value_ & right.int8_value_;
      break;
    case kInt16:
      out.int16_value_ = left.int16_value_ & right.int16_value_;
      break;
    case kInt32:
      out.int32_value_ = left.int32_value_ & right.int32_value_;
      break;
    case kInt64:
      out.int64_value_ = left.int64_value_ & right.int64_value_;
      break;
    case kUint:
      out.uint_value_ = left.uint_value_ & right.uint_value_;
      break;
    case kInt:
      out.int_value_ = left.int_value_ & right.int_value_;
      break;
    case kUintptr:
      out.uintptr_value_ = left.uintptr_value_ & right.uintptr_value_;
      break;
    default:
      type_not_reached (*type);
    }
}

void bit_and_not (value_t& out, const Type::Type* type, const value_t& left, const value_t& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case kRune:
      out.rune_value_ = left.rune_value_ & ~right.rune_value_;
      break;
    case kInteger:
      out.integer_value_ = left.integer_value_ & ~right.integer_value_;
      break;
    case kUint8:
      out.uint8_value_ = left.uint8_value_ & ~right.uint8_value_;
      break;
    case kUint16:
      out.uint16_value_ = left.uint16_value_ & ~right.uint16_value_;
      break;
    case kUint32:
      out.uint32_value_ = left.uint32_value_ & ~right.uint32_value_;
      break;
    case kUint64:
      out.uint64_value_ = left.uint64_value_ & ~right.uint64_value_;
      break;
    case kInt8:
      out.int8_value_ = left.int8_value_ & ~right.int8_value_;
      break;
    case kInt16:
      out.int16_value_ = left.int16_value_ & ~right.int16_value_;
      break;
    case kInt32:
      out.int32_value_ = left.int32_value_ & ~right.int32_value_;
      break;
    case kInt64:
      out.int64_value_ = left.int64_value_ & ~right.int64_value_;
      break;
    case kUint:
      out.uint_value_ = left.uint_value_ & ~right.uint_value_;
      break;
    case kInt:
      out.int_value_ = left.int_value_ & ~right.int_value_;
      break;
    case kUintptr:
      out.uintptr_value_ = left.uintptr_value_ & ~right.uintptr_value_;
      break;
    default:
      type_not_reached (*type);
    }
}

void bit_xor (value_t& out, const Type::Type* type, const value_t& left, const value_t& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case kRune:
      out.rune_value_ = left.rune_value_ ^ right.rune_value_;
      break;
    case kInteger:
      out.integer_value_ = left.integer_value_ ^ right.integer_value_;
      break;
    case kUint8:
      out.uint8_value_ = left.uint8_value_ ^ right.uint8_value_;
      break;
    case kUint16:
      out.uint16_value_ = left.uint16_value_ ^ right.uint16_value_;
      break;
    case kUint32:
      out.uint32_value_ = left.uint32_value_ ^ right.uint32_value_;
      break;
    case kUint64:
      out.uint64_value_ = left.uint64_value_ ^ right.uint64_value_;
      break;
    case kInt8:
      out.int8_value_ = left.int8_value_ ^ right.int8_value_;
      break;
    case kInt16:
      out.int16_value_ = left.int16_value_ ^ right.int16_value_;
      break;
    case kInt32:
      out.int32_value_ = left.int32_value_ ^ right.int32_value_;
      break;
    case kInt64:
      out.int64_value_ = left.int64_value_ ^ right.int64_value_;
      break;
    case kUint:
      out.uint_value_ = left.uint_value_ ^ right.uint_value_;
      break;
    case kInt:
      out.int_value_ = left.int_value_ ^ right.int_value_;
      break;
    case kUintptr:
      out.uintptr_value_ = left.uintptr_value_ ^ right.uintptr_value_;
      break;
    default:
      type_not_reached (*type);
    }
}

void bit_or (value_t& out, const Type::Type* type, const value_t& left, const value_t& right)
{
  out.present = true;
  switch (type->underlying_kind ())
    {
    case kRune:
      out.rune_value_ = left.rune_value_ | right.rune_value_;
      break;
    case kInteger:
      out.integer_value_ = left.integer_value_ | right.integer_value_;
      break;
    case kUint8:
      out.uint8_value_ = left.uint8_value_ | right.uint8_value_;
      break;
    case kUint16:
      out.uint16_value_ = left.uint16_value_ | right.uint16_value_;
      break;
    case kUint32:
      out.uint32_value_ = left.uint32_value_ | right.uint32_value_;
      break;
    case kUint64:
      out.uint64_value_ = left.uint64_value_ | right.uint64_value_;
      break;
    case kInt8:
      out.int8_value_ = left.int8_value_ | right.int8_value_;
      break;
    case kInt16:
      out.int16_value_ = left.int16_value_ | right.int16_value_;
      break;
    case kInt32:
      out.int32_value_ = left.int32_value_ | right.int32_value_;
      break;
    case kInt64:
      out.int64_value_ = left.int64_value_ | right.int64_value_;
      break;
    case kUint:
      out.uint_value_ = left.uint_value_ | right.uint_value_;
      break;
    case kInt:
      out.int_value_ = left.int_value_ | right.int_value_;
      break;
    case kUintptr:
      out.uintptr_value_ = left.uintptr_value_ | right.uintptr_value_;
      break;
    default:
      type_not_reached (*type);
    }
}
