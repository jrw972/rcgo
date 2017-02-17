#include "semantic.hpp"

#include <utility>
#include <cmath>

#include "type.hpp"
#include "polymorphic_function.hpp"
#include "error_reporter.hpp"
#include "identifier.hpp"

using namespace type;
using namespace std::rel_ops;
using namespace util;

namespace semantic
{

ExpressionValue ExpressionValue::make_error ()
{
  ExpressionValue ev;
  ev.kind = Error;
  return ev;
}

ExpressionValue ExpressionValue::make_void ()
{
  ExpressionValue ev;
  ev.kind = Void;
  return ev;
}

ExpressionValue ExpressionValue::make_variable (const type::Type* type)
{
  ExpressionValue ev;
  ev.kind = Variable;
  ev.type = type;
  return ev;
}

ExpressionValue ExpressionValue::make_value (const type::Type* a_type,
    Mutability a_intrinsic_mutability,
    Mutability a_indirection_mutability,
    bool a_variadic)
{
  ExpressionValue ev;
  ev.kind = Value;
  ev.type = a_type;
  ev.intrinsic_mutability = a_intrinsic_mutability;
  ev.indirection_mutability = a_indirection_mutability;
  ev.variadic = a_variadic;
  return ev;
}

ExpressionValue ExpressionValue::make_bool (const type::Type* type, bool v)
{
  assert (type->underlying_kind () == type::Bool_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.bool_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_uint8 (const type::Type* type, uint8_t v)
{
  assert (type->underlying_kind () == type::Uint8_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.uint8_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_uint16 (const type::Type* type, uint16_t v)
{
  assert (type->underlying_kind () == type::Uint16_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.uint16_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_uint32 (const type::Type* type, uint32_t v)
{
  assert (type->underlying_kind () == type::Uint32_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.uint32_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_uint64 (const type::Type* type, uint64_t v)
{
  assert (type->underlying_kind () == type::Uint64_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.uint64_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_int8 (const type::Type* type, int8_t v)
{
  assert (type->underlying_kind () == type::Int8_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.int8_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_int16 (const type::Type* type, int16_t v)
{
  assert (type->underlying_kind () == type::Int16_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.int16_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_int32 (const type::Type* type, int32_t v)
{
  assert (type->underlying_kind () == type::Int32_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.int32_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_int64 (const type::Type* type, int64_t v)
{
  assert (type->underlying_kind () == type::Int64_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.int64_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_float32 (const type::Type* type, float v)
{
  assert (type->underlying_kind () == type::Float32_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.float32_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_float64 (const type::Type* type, double v)
{
  assert (type->underlying_kind () == type::Float64_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.float64_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_complex64 (const type::Type* type, runtime::Complex64 v)
{
  assert (type->underlying_kind () == type::Complex64_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.complex64_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_complex128 (const type::Type* type, runtime::Complex128 v)
{
  assert (type->underlying_kind () == type::Complex128_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.complex128_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_uint (const type::Type* type, unsigned long v)
{
  assert (type->underlying_kind () == type::Uint_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.uint_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_int (const type::Type* type, long v)
{
  assert (type->underlying_kind () == type::Int_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.int_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_uintptr (const type::Type* type, uintptr_t v)
{
  assert (type->underlying_kind () == type::Uintptr_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.uintptr_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_string (const type::Type* type, const runtime::String& v)
{
  assert (type->underlying_kind () == type::String_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.value.string_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_pointer (const type::Type* type)
{
  assert (type->underlying_kind () == type::Pointer_Kind);
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_nil ()
{
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type::UntypedNil::instance ();
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_boolean (bool v)
{
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type::UntypedBoolean::instance ();
  ev.value.untyped_boolean_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_rune (int32_t v)
{
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type::UntypedRune::instance ();
  ev.value.untyped_rune_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_integer (long long v)
{
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type::UntypedInteger::instance ();
  ev.value.untyped_integer_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_float (double v)
{
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type::UntypedFloat::instance ();
  ev.value.untyped_float_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_complex (semantic::UntypedComplex v)
{
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type::UntypedComplex::instance ();
  ev.value.untyped_complex_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_string_untyped (const runtime::String& v)
{
  ExpressionValue ev;
  ev.kind = Constant;
  ev.type = type::UntypedString::instance ();
  ev.value.untyped_string_value = v;
  ev.intrinsic_mutability = Immutable;
  ev.indirection_mutability = Immutable;
  return ev;
}

ExpressionValue ExpressionValue::make_polymorphic_function (decl::PolymorphicFunction* func)
{
  ExpressionValue ev;
  ev.kind = Polymorphic_Function;
  ev.polymorphic_function = func;
  return ev;
}

ExpressionValue ExpressionValue::make_type (const type::Type* t)
{
  ExpressionValue ev;
  ev.kind = Type;
  ev.type = t;
  return ev;
}

ExpressionValue::ExpressionValue ()
  : kind (Unknown)
  , type (NULL)
  , polymorphic_function (NULL)
  , intrinsic_mutability (Unknown_Mutability)
  , indirection_mutability (Unknown_Mutability)
  , receiver_access (AccessNone)
  , receiver_state (false)
{ }

bool ExpressionValue::is_unknown () const
{
  return kind == Unknown;
}

bool ExpressionValue::is_error () const
{
  return kind == Error;
}

bool ExpressionValue::is_value () const
{
  return kind == Value;
}

bool ExpressionValue::is_variable () const
{
  return kind == Variable;
}

bool ExpressionValue::is_void () const
{
  return kind == Void;
}

bool ExpressionValue::is_type () const
{
  return kind == Type;
}

bool ExpressionValue::is_rvalue () const
{
  return kind == Value || kind == Constant || kind == Variable;
}

bool ExpressionValue::is_rvalue_or_void () const
{
  return kind == Value || kind == Constant || kind == Variable || kind == Void;
}

bool ExpressionValue::is_constant () const
{
  return kind == Constant;
}

bool ExpressionValue::is_polymorphic_function () const
{
  return kind == Polymorphic_Function;
}

bool ExpressionValue::is_variadic () const
{
  return variadic;
}

bool ExpressionValue::is_assignable (const type::Type* to) const
{
  if (to->is_untyped ())
    {
      return false;
    }

  if (are_identical (this->type, to))
    {
      return true;
    }

  if (are_identical (this->type->underlying_type (), to->underlying_type ()) &&
      (!this->type->is_named () || !to->is_named ()))
    {
      return true;
    }

  // TODO:  T is an interface type and x implements T.

  if (this->type->kind () == Untyped_Nil_Kind &&
      (to->underlying_kind () == Pointer_Kind ||
       to->underlying_kind () == Function_Kind ||
       to->underlying_kind () == Slice_Kind ||
       to->underlying_kind () == Map_Kind ||
       to->underlying_kind () == Interface_Kind))
    {
      return true;
    }

  if (this->type->is_untyped () && this->representable (to))
    {
      return true;
    }

  return false;
}

void ExpressionValue::fix_string_indirection_mutability ()
{
  switch (kind)
    {
    case Constant:
    case Value:
    case Variable:
      if (type->underlying_kind () == type::String_Kind)
        {
          indirection_mutability = std::max (indirection_mutability, Immutable);
        }
      break;
    default:
      break;
    }
}

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
      return to_and_back_helper<uint8_t> (x);
    case Uint16_Kind:
      return to_and_back_helper<uint16_t> (x);
    case Uint32_Kind:
      return to_and_back_helper<uint32_t> (x);
    case Uint64_Kind:
      return to_and_back_helper<uint64_t> (x);
    case Int8_Kind:
      return to_and_back_helper<int8_t> (x);
    case Int16_Kind:
      return to_and_back_helper<int16_t> (x);
    case Int32_Kind:
      return to_and_back_helper<int32_t> (x);
    case Int64_Kind:
      return to_and_back_helper<int64_t> (x);
    case Float32_Kind:
      return to_and_back_helper<float> (x);
    case Float64_Kind:
      return to_and_back_helper<double> (x);
    case Complex64_Kind:
      return to_and_back_helper<runtime::Complex64> (x);
    case Complex128_Kind:
      return to_and_back_helper<runtime::Complex128> (x);
    case Uint_Kind:
      return to_and_back_helper<unsigned long> (x);
    case Int_Kind:
      return to_and_back_helper<long> (x);
    case Uintptr_Kind:
      return to_and_back_helper<size_t> (x);
    default:
      return false;
    }
}

bool
ExpressionValue::representable (const type::Type* to) const
{
  assert (kind == Constant);
  assert (type->is_untyped ());
  assert (!to->is_untyped ());

  switch (type->underlying_kind ())
    {
    case Untyped_Nil_Kind:
      return
        to->underlying_kind () == Pointer_Kind ||
        to->underlying_kind () == Slice_Kind;
    case Untyped_Boolean_Kind:
      return to->underlying_kind () == Bool_Kind;
    case Untyped_Rune_Kind:
      return to_and_back (value.untyped_rune_value, to);
    case Untyped_Integer_Kind:
      return to_and_back (value.untyped_integer_value, to);
    case Untyped_Float_Kind:
      return to_and_back (value.untyped_float_value, to);
    case Untyped_Complex_Kind:
      return to_and_back (value.untyped_complex_value, to);
    case Untyped_String_Kind:
      return to->underlying_kind () == String_Kind;
    default:
      NOT_REACHED;
    }
}

static void
convert_numeric (Value& value, UntypedComplex x, const type::Type* type)
{
  if (!type->is_numeric ())
    {
      return;
    }

  switch (type->kind ())
    {
    case Uint8_Kind:
      value.uint8_value = x;
      break;
    case Uint16_Kind:
      value.uint16_value = x;
      break;
    case Uint32_Kind:
      value.uint32_value = x;
      break;
    case Uint64_Kind:
      value.uint64_value = x;
      break;
    case Int8_Kind:
      value.int8_value = x;
      break;
    case Int16_Kind:
      value.int16_value = x;
      break;
    case Int32_Kind:
      value.int32_value = x;
      break;
    case Int64_Kind:
      value.int64_value = x;
      break;
    case Uint_Kind:
      value.uint_value = x;
      break;
    case Int_Kind:
      value.int_value = x;
      break;
    case Uintptr_Kind:
      value.uintptr_value = x;
      break;
    case Float32_Kind:
      value.float32_value = x;
      break;
    case Float64_Kind:
      value.float64_value = x;
      break;
    case Complex64_Kind:
      value.complex64_value = x;
      break;
    case Complex128_Kind:
      value.complex128_value = x;
      break;
    case Untyped_Integer_Kind:
      value.untyped_integer_value = x;
      break;
    case Untyped_Float_Kind:
      value.untyped_float_value = x;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void ExpressionValue::convert (const type::Type* to_type)
{
  if (type != to_type)
    {
      if (kind == Constant)
        {
          const type::Type* from = type->underlying_type ();
          const type::Type* to = to_type->underlying_type ();

          switch (from->kind ())
            {
            case Untyped_Boolean_Kind:
            {
              if (to->kind () == Bool_Kind)
                {
                  value.bool_value = value.untyped_boolean_value;
                }
            }
            break;

            case Untyped_Rune_Kind:
            {
              UntypedComplex x;
              x = value.untyped_rune_value;
              convert_numeric (value, x, to);
            }
            break;

            case Untyped_Integer_Kind:
            {
              UntypedComplex x;
              x = value.untyped_integer_value;
              convert_numeric (value, x, to);
            }
            break;

            case Untyped_Float_Kind:
            {
              UntypedComplex x;
              x = value.untyped_float_value;
              convert_numeric (value, x, to);
            }
            break;

            case Uint8_Kind:
            {
              UntypedComplex x;
              x = value.uint8_value;
              convert_numeric (value, x, to);
            }
            break;

            case Uint16_Kind:
            {
              UntypedComplex x;
              x = value.uint16_value;
              convert_numeric (value, x, to);
            }
            break;

            case Uint32_Kind:
            {
              UntypedComplex x;
              x = value.uint32_value;
              convert_numeric (value, x, to);
            }
            break;

            case Uint64_Kind:
            {
              UntypedComplex x;
              x = value.uint64_value;
              convert_numeric (value, x, to);
            }
            break;

            case Int8_Kind:
            {
              UntypedComplex x;
              x = value.int8_value;
              convert_numeric (value, x, to);
            }
            break;

            case Int16_Kind:
            {
              UntypedComplex x;
              x = value.int16_value;
              convert_numeric (value, x, to);
            }
            break;

            case Int32_Kind:
            {
              UntypedComplex x;
              x = value.int32_value;
              convert_numeric (value, x, to);
            }
            break;

            case Int64_Kind:
            {
              UntypedComplex x;
              x = value.int64_value;
              convert_numeric (value, x, to);
            }
            break;

            case Int_Kind:
            {
              UntypedComplex x;
              x = value.int_value;
              convert_numeric (value, x, to);
            }
            break;

            case Uint_Kind:
            {
              UntypedComplex x;
              x = value.uint_value;
              convert_numeric (value, x, to);
            }
            break;

            case Uintptr_Kind:
            {
              UntypedComplex x;
              x = value.uintptr_value;
              convert_numeric (value, x, to);
            }
            break;

            case Float32_Kind:
            {
              UntypedComplex x;
              x = value.float32_value;
              convert_numeric (value, x, to);
            }
            break;

            case Float64_Kind:
            {
              UntypedComplex x;
              x = value.float64_value;
              convert_numeric (value, x, to);
            }
            break;

            case Complex64_Kind:
            {
              UntypedComplex x;
              x = value.complex64_value;
              convert_numeric (value, x, to);
            }
            break;

            case Complex128_Kind:
            {
              UntypedComplex x;
              x = value.complex128_value;
              convert_numeric (value, x, to);
            }
            break;

            case Untyped_String_Kind:
            {
              if (to->kind () == String_Kind)
                {
                  value.string_value = value.untyped_string_value;
                }
            }
            break;

            case Untyped_Nil_Kind:
            {
              if (to->kind () == Pointer_Kind)
                {
                  // value.pointer_value = NULL;
                }
              else if (to->kind () == Slice_Kind)
                {
                  value.slice_value = runtime::Slice::make ();
                }
            }
            break;

            default:
              TYPE_NOT_REACHED (*from);
            }
        }
      type = to_type;
    }
}

long
ExpressionValue::to_int () const
{
  assert (type->is_typed_integer ());
  switch (type->underlying_kind ())
    {
    case Uint8_Kind:
      return value.uint8_value;
    case Uint16_Kind:
      return value.uint16_value;
    case Uint32_Kind:
      return value.uint32_value;
    case Uint64_Kind:
      return value.uint64_value;
    case Int8_Kind:
      return value.int8_value;
    case Int16_Kind:
      return value.int16_value;
    case Int32_Kind:
      return value.int32_value;
    case Int64_Kind:
      return value.int64_value;
    case Uint_Kind:
      return value.uint_value;
    case Int_Kind:
      return value.int_value;
    case Uintptr_Kind:
      return value.uintptr_value;
    default:
      NOT_REACHED;
    }
}

void ExpressionValue::print (std::ostream& out) const
{
  switch (kind)
    {
    case Unknown:
      out << "Unknown";
      break;
    case Error:
      out << "Error";
      break;
    case Value:
      out << "Value " << *type << ' ' << intrinsic_mutability << ' ' << indirection_mutability;
      break;
    case Constant:
      out << "Constant " << *type << ' ';
      switch (type->underlying_kind ())
        {
        case Untyped_Nil_Kind:
          out << "<nil>";
          break;
        case Untyped_Boolean_Kind:
          out << (value.untyped_boolean_value ? "<true>" : "<false>");
          break;
        case Untyped_Rune_Kind:
          out << '<' << value.untyped_rune_value << '>';
          break;
        case Untyped_Integer_Kind:
          out << '<' << value.untyped_integer_value << '>';
          break;
        case Untyped_Float_Kind:
          out << '<' << value.untyped_float_value << '>';
          break;
        case Untyped_Complex_Kind:
          out << '<' << value.untyped_complex_value << '>';
          break;
        case Untyped_String_Kind:
          out << '<' << value.untyped_string_value << '>';
          break;
        case Bool_Kind:
          out << (value.bool_value ? "<true>" : "<false>");
          break;
        case Uint8_Kind:
          out << '<' << (int)value.uint8_value << '>';
          break;
        case Uint16_Kind:
          out << '<' << value.uint16_value << '>';
          break;
        case Uint32_Kind:
          out << '<' << value.uint32_value << '>';
          break;
        case Uint64_Kind:
          out << '<' << value.uint64_value << '>';
          break;
        case Int8_Kind:
          out << '<' << (int)value.int8_value << '>';
          break;
        case Int16_Kind:
          out << '<' << value.int16_value << '>';
          break;
        case Int32_Kind:
          out << '<' << value.int32_value << '>';
          break;
        case Int64_Kind:
          out << '<' << value.int64_value << '>';
          break;
        case Float32_Kind:
          out << '<' << value.float32_value << '>';
          break;
        case Float64_Kind:
          out << '<' << value.float64_value << '>';
          break;
        case Complex64_Kind:
          out << '<' << value.complex64_value << '>';
          break;
        case Complex128_Kind:
          out << '<' << value.complex128_value << '>';
          break;
        case Uint_Kind:
          out << '<' << value.uint_value << '>';
          break;
        case Int_Kind:
          out << '<' << value.int_value << '>';
          break;
        case Uintptr_Kind:
          out << '<' << value.uintptr_value << '>';
          break;
        case String_Kind:
          out << '<' << value.string_value << '>';
          break;
        case Pointer_Kind:
          out << "<nil>";
          break;
        default:
          TYPE_NOT_REACHED (*type);
        }
      out << ' ' << intrinsic_mutability << ' ' << indirection_mutability;
      break;
    case Variable:
      out << "Variable " << *type << ' ' << intrinsic_mutability << ' ' << indirection_mutability;
      break;
    case Type:
      out << "Type " << *type;
      break;
    case EmptyType:
      out << "EmptyType";
      break;
    case Void:
      out << "Void";
      break;
    case Polymorphic_Function:
      out << "Polymorphic_Function " << polymorphic_function->identifier.identifier ();
      break;
    }
}

std::ostream& operator<< (std::ostream& out, const ExpressionValue& ev)
{
  ev.print (out);
  return out;
}

}
