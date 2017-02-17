#include "semantic.hpp"

#include <error.h>
#include <utility>

#include "node.hpp"
#include "node_visitor.hpp"
#include "node_cast.hpp"
#include "polymorphic_function.hpp"
#include "callable.hpp"
#include "symbol_visitor.hpp"
#include "symbol.hpp"
#include "symbol.hpp"
#include "parameter_list.hpp"
#include "error_reporter.hpp"
#include "process_type.hpp"
#include "scope.hpp"
#include "polymorphic_function_visitor.hpp"
#include "symbol_table.hpp"
#include "identifier.hpp"
#include "source_file.hpp"

namespace semantic
{
using namespace util;
using namespace ast;
using namespace type;
using namespace decl;
using namespace std::rel_ops;
using namespace source;

ExpressionValueList collect_evals (ast::Node* node)
{
  struct visitor : public DefaultNodeVisitor
  {
    ExpressionValueList list;

    virtual void default_action (Node& node)
    {
      list.push_back (node.eval);
    }
  };

  visitor v;
  node->visit_children (v);
  return v.list;
}

source::LocationList collect_locations (ast::Node* node)
{
  struct visitor : public DefaultNodeVisitor
  {
    source::LocationList list;

    virtual void default_action (Node& node)
    {
      list.push_back (node.location);
    }
  };

  visitor v;
  node->visit_children (v);
  return v.list;
}

void distribute_evals (const ExpressionValueList& list, ast::Node* node)
{
  struct visitor : public DefaultNodeVisitor
  {
    ExpressionValueList::const_iterator pos;

    visitor (ExpressionValueList::const_iterator a_pos)
      : pos (a_pos)
    { }

    virtual void default_action (Node& node)
    {
      node.eval = *pos++;
    }
  };

  visitor v (list.begin ());
  node->visit_children (v);
}

struct PassThroughPicker
{
  static const type::Type* pick (const type::Type* input_type,
                                 const semantic::ExpressionValue& left,
                                 const semantic::ExpressionValue& right)
  {
    return input_type;
  }
};

struct BooleanPicker
{
  static const type::Type* pick (const type::Type* input_type,
                                 const semantic::ExpressionValue& left,
                                 const semantic::ExpressionValue& right)
  {
    if (left.is_constant () && right.is_constant ())
      {
        return UntypedBoolean::instance ();
      }
    else
      {
        return Bool::instance ();
      }
  }
};

template <typename Op>
struct BinaryValueComputer
{
  static void
  compute (semantic::ExpressionValue& result,
           const type::Type* in_type,
           const semantic::ExpressionValue& left,
           const semantic::ExpressionValue& right)
  {
    if (left.is_constant () && right.is_constant ())
      {
        Op() (result, in_type, left, right);
      }
  }
};

struct SMultiply
{
  void operator() (ExpressionValue& out, const type::Type* type, const ExpressionValue& left, const ExpressionValue& right) const
  {
    assert (left.is_constant ());
    assert (right.is_constant ());
    out.kind = ExpressionValue::Constant;
    switch (type->underlying_kind ())
      {
      case Untyped_Rune_Kind:
        out.value.untyped_rune_value = left.value.untyped_rune_value * right.value.untyped_rune_value;
        break;
      case Untyped_Integer_Kind:
        out.value.untyped_integer_value = left.value.untyped_integer_value * right.value.untyped_integer_value;
        break;
      case Untyped_Float_Kind:
        out.value.untyped_float_value = left.value.untyped_float_value * right.value.untyped_float_value;
        break;
      case Untyped_Complex_Kind:
        out.value.untyped_complex_value = left.value.untyped_complex_value * right.value.untyped_complex_value;
        break;
      case Uint8_Kind:
        out.value.uint8_value = left.value.uint8_value * right.value.uint8_value;
        break;
      case Uint16_Kind:
        out.value.uint16_value = left.value.uint16_value * right.value.uint16_value;
        break;
      case Uint32_Kind:
        out.value.uint32_value = left.value.uint32_value * right.value.uint32_value;
        break;
      case Uint64_Kind:
        out.value.uint64_value = left.value.uint64_value * right.value.uint64_value;
        break;
      case Int8_Kind:
        out.value.int8_value = left.value.int8_value * right.value.int8_value;
        break;
      case Int16_Kind:
        out.value.int16_value = left.value.int16_value * right.value.int16_value;
        break;
      case Int32_Kind:
        out.value.int32_value = left.value.int32_value * right.value.int32_value;
        break;
      case Int64_Kind:
        out.value.int64_value = left.value.int64_value * right.value.int64_value;
        break;
      case Float32_Kind:
        out.value.float32_value = left.value.float32_value * right.value.float32_value;
        break;
      case Float64_Kind:
        out.value.float64_value = left.value.float64_value * right.value.float64_value;
        break;
      case Complex64_Kind:
        out.value.complex64_value = left.value.complex64_value * right.value.complex64_value;
        break;
      case Complex128_Kind:
        out.value.complex128_value = left.value.complex128_value * right.value.complex128_value;
        break;
      case Uint_Kind:
        out.value.uint_value = left.value.uint_value * right.value.uint_value;
        break;
      case Int_Kind:
        out.value.int_value = left.value.int_value * right.value.int_value;
        break;
      case Uintptr_Kind:
        out.value.uintptr_value = left.value.uintptr_value * right.value.uintptr_value;
        break;
      default:
        TYPE_NOT_REACHED (*type);
      }
  }
};

struct SDivide
{
  void operator() (ExpressionValue& out, const type::Type* type, const ExpressionValue& left, const ExpressionValue& right) const
  {
    assert (left.is_constant ());
    assert (right.is_constant ());
    out.kind = ExpressionValue::Constant;
    switch (type->underlying_kind ())
      {
      case Untyped_Rune_Kind:
        out.value.untyped_rune_value = left.value.untyped_rune_value / right.value.untyped_rune_value;
        break;
      case Untyped_Integer_Kind:
        out.value.untyped_integer_value = left.value.untyped_integer_value / right.value.untyped_integer_value;
        break;
      case Untyped_Float_Kind:
        out.value.untyped_float_value = left.value.untyped_float_value / right.value.untyped_float_value;
        break;
      case Untyped_Complex_Kind:
        out.value.untyped_complex_value = left.value.untyped_complex_value / right.value.untyped_complex_value;
        break;
      case Uint8_Kind:
        out.value.uint8_value = left.value.uint8_value / right.value.uint8_value;
        break;
      case Uint16_Kind:
        out.value.uint16_value = left.value.uint16_value / right.value.uint16_value;
        break;
      case Uint32_Kind:
        out.value.uint32_value = left.value.uint32_value / right.value.uint32_value;
        break;
      case Uint64_Kind:
        out.value.uint64_value = left.value.uint64_value / right.value.uint64_value;
        break;
      case Int8_Kind:
        out.value.int8_value = left.value.int8_value / right.value.int8_value;
        break;
      case Int16_Kind:
        out.value.int16_value = left.value.int16_value / right.value.int16_value;
        break;
      case Int32_Kind:
        out.value.int32_value = left.value.int32_value / right.value.int32_value;
        break;
      case Int64_Kind:
        out.value.int64_value = left.value.int64_value / right.value.int64_value;
        break;
      case Float32_Kind:
        out.value.float32_value = left.value.float32_value / right.value.float32_value;
        break;
      case Float64_Kind:
        out.value.float64_value = left.value.float64_value / right.value.float64_value;
        break;
      case Complex64_Kind:
        out.value.complex64_value = left.value.complex64_value / right.value.complex64_value;
        break;
      case Complex128_Kind:
        out.value.complex128_value = left.value.complex128_value / right.value.complex128_value;
        break;
      case Uint_Kind:
        out.value.uint_value = left.value.uint_value / right.value.uint_value;
        break;
      case Int_Kind:
        out.value.int_value = left.value.int_value / right.value.int_value;
        break;
      case Uintptr_Kind:
        out.value.uintptr_value = left.value.uintptr_value / right.value.uintptr_value;
        break;
      default:
        TYPE_NOT_REACHED (*type);
      }
  }
};

struct SModulus
{
  void operator() (ExpressionValue& out, const type::Type* type, const ExpressionValue& left, const ExpressionValue& right) const
  {
    assert (left.is_constant ());
    assert (right.is_constant ());
    out.kind = ExpressionValue::Constant;
    switch (type->underlying_kind ())
      {
      case Untyped_Rune_Kind:
        out.value.untyped_rune_value = left.value.untyped_rune_value % right.value.untyped_rune_value;
        break;
      case Untyped_Integer_Kind:
        out.value.untyped_integer_value = left.value.untyped_integer_value % right.value.untyped_integer_value;
        break;
      case Uint8_Kind:
        out.value.uint8_value = left.value.uint8_value % right.value.uint8_value;
        break;
      case Uint16_Kind:
        out.value.uint16_value = left.value.uint16_value % right.value.uint16_value;
        break;
      case Uint32_Kind:
        out.value.uint32_value = left.value.uint32_value % right.value.uint32_value;
        break;
      case Uint64_Kind:
        out.value.uint64_value = left.value.uint64_value % right.value.uint64_value;
        break;
      case Int8_Kind:
        out.value.int8_value = left.value.int8_value % right.value.int8_value;
        break;
      case Int16_Kind:
        out.value.int16_value = left.value.int16_value % right.value.int16_value;
        break;
      case Int32_Kind:
        out.value.int32_value = left.value.int32_value % right.value.int32_value;
        break;
      case Int64_Kind:
        out.value.int64_value = left.value.int64_value % right.value.int64_value;
        break;
      case Uint_Kind:
        out.value.uint_value = left.value.uint_value % right.value.uint_value;
        break;
      case Int_Kind:
        out.value.int_value = left.value.int_value % right.value.int_value;
        break;
      case Uintptr_Kind:
        out.value.uintptr_value = left.value.uintptr_value % right.value.uintptr_value;
        break;
      default:
        TYPE_NOT_REACHED (*type);
      }
  }
};

struct SBitAnd
{
  void operator() (ExpressionValue& out, const type::Type* type, const ExpressionValue& left, const ExpressionValue& right) const
  {
    assert (left.is_constant ());
    assert (right.is_constant ());
    out.kind = ExpressionValue::Constant;
    switch (type->underlying_kind ())
      {
      case Untyped_Rune_Kind:
        out.value.untyped_rune_value = left.value.untyped_rune_value & right.value.untyped_rune_value;
        break;
      case Untyped_Integer_Kind:
        out.value.untyped_integer_value = left.value.untyped_integer_value & right.value.untyped_integer_value;
        break;
      case Uint8_Kind:
        out.value.uint8_value = left.value.uint8_value & right.value.uint8_value;
        break;
      case Uint16_Kind:
        out.value.uint16_value = left.value.uint16_value & right.value.uint16_value;
        break;
      case Uint32_Kind:
        out.value.uint32_value = left.value.uint32_value & right.value.uint32_value;
        break;
      case Uint64_Kind:
        out.value.uint64_value = left.value.uint64_value & right.value.uint64_value;
        break;
      case Int8_Kind:
        out.value.int8_value = left.value.int8_value & right.value.int8_value;
        break;
      case Int16_Kind:
        out.value.int16_value = left.value.int16_value & right.value.int16_value;
        break;
      case Int32_Kind:
        out.value.int32_value = left.value.int32_value & right.value.int32_value;
        break;
      case Int64_Kind:
        out.value.int64_value = left.value.int64_value & right.value.int64_value;
        break;
      case Uint_Kind:
        out.value.uint_value = left.value.uint_value & right.value.uint_value;
        break;
      case Int_Kind:
        out.value.int_value = left.value.int_value & right.value.int_value;
        break;
      case Uintptr_Kind:
        out.value.uintptr_value = left.value.uintptr_value & right.value.uintptr_value;
        break;
      default:
        TYPE_NOT_REACHED (*type);
      }
  }
};

struct SBitAndNot
{
  void operator() (ExpressionValue& out, const type::Type* type, const ExpressionValue& left, const ExpressionValue& right) const
  {
    assert (left.is_constant ());
    assert (right.is_constant ());
    out.kind = ExpressionValue::Constant;
    switch (type->underlying_kind ())
      {
      case Untyped_Rune_Kind:
        out.value.untyped_rune_value = left.value.untyped_rune_value & ~right.value.untyped_rune_value;
        break;
      case Untyped_Integer_Kind:
        out.value.untyped_integer_value = left.value.untyped_integer_value & ~right.value.untyped_integer_value;
        break;
      case Uint8_Kind:
        out.value.uint8_value = left.value.uint8_value & ~right.value.uint8_value;
        break;
      case Uint16_Kind:
        out.value.uint16_value = left.value.uint16_value & ~right.value.uint16_value;
        break;
      case Uint32_Kind:
        out.value.uint32_value = left.value.uint32_value & ~right.value.uint32_value;
        break;
      case Uint64_Kind:
        out.value.uint64_value = left.value.uint64_value & ~right.value.uint64_value;
        break;
      case Int8_Kind:
        out.value.int8_value = left.value.int8_value & ~right.value.int8_value;
        break;
      case Int16_Kind:
        out.value.int16_value = left.value.int16_value & ~right.value.int16_value;
        break;
      case Int32_Kind:
        out.value.int32_value = left.value.int32_value & ~right.value.int32_value;
        break;
      case Int64_Kind:
        out.value.int64_value = left.value.int64_value & ~right.value.int64_value;
        break;
      case Uint_Kind:
        out.value.uint_value = left.value.uint_value & ~right.value.uint_value;
        break;
      case Int_Kind:
        out.value.int_value = left.value.int_value & ~right.value.int_value;
        break;
      case Uintptr_Kind:
        out.value.uintptr_value = left.value.uintptr_value & ~right.value.uintptr_value;
        break;
      default:
        TYPE_NOT_REACHED (*type);
      }
  }
};

struct SAdd
{
  void operator() (ExpressionValue& out, const type::Type* type, const ExpressionValue& left, const ExpressionValue& right) const
  {
    assert (left.is_constant ());
    assert (right.is_constant ());
    out.kind = ExpressionValue::Constant;
    switch (type->underlying_kind ())
      {
      case Untyped_Rune_Kind:
        out.value.untyped_rune_value = left.value.untyped_rune_value + right.value.untyped_rune_value;
        break;
      case Untyped_Integer_Kind:
        out.value.untyped_integer_value = left.value.untyped_integer_value + right.value.untyped_integer_value;
        break;
      case Untyped_Float_Kind:
        out.value.untyped_float_value = left.value.untyped_float_value + right.value.untyped_float_value;
        break;
      case Untyped_Complex_Kind:
        out.value.untyped_complex_value = left.value.untyped_complex_value + right.value.untyped_complex_value;
        break;
      case Uint8_Kind:
        out.value.uint8_value = left.value.uint8_value + right.value.uint8_value;
        break;
      case Uint16_Kind:
        out.value.uint16_value = left.value.uint16_value + right.value.uint16_value;
        break;
      case Uint32_Kind:
        out.value.uint32_value = left.value.uint32_value + right.value.uint32_value;
        break;
      case Uint64_Kind:
        out.value.uint64_value = left.value.uint64_value + right.value.uint64_value;
        break;
      case Int8_Kind:
        out.value.int8_value = left.value.int8_value + right.value.int8_value;
        break;
      case Int16_Kind:
        out.value.int16_value = left.value.int16_value + right.value.int16_value;
        break;
      case Int32_Kind:
        out.value.int32_value = left.value.int32_value + right.value.int32_value;
        break;
      case Int64_Kind:
        out.value.int64_value = left.value.int64_value + right.value.int64_value;
        break;
      case Float32_Kind:
        out.value.float32_value = left.value.float32_value + right.value.float32_value;
        break;
      case Float64_Kind:
        out.value.float64_value = left.value.float64_value + right.value.float64_value;
        break;
      case Complex64_Kind:
        out.value.complex64_value = left.value.complex64_value + right.value.complex64_value;
        break;
      case Complex128_Kind:
        out.value.complex128_value = left.value.complex128_value + right.value.complex128_value;
        break;
      case Uint_Kind:
        out.value.uint_value = left.value.uint_value + right.value.uint_value;
        break;
      case Int_Kind:
        out.value.int_value = left.value.int_value + right.value.int_value;
        break;
      case Uintptr_Kind:
        out.value.uintptr_value = left.value.uintptr_value + right.value.uintptr_value;
        break;
      default:
        TYPE_NOT_REACHED (*type);
      }
  }
};

struct SSubtract
{
  void operator() (ExpressionValue& out, const type::Type* type, const ExpressionValue& left, const ExpressionValue& right) const
  {
    assert (left.is_constant ());
    assert (right.is_constant ());
    out.kind = ExpressionValue::Constant;
    switch (type->underlying_kind ())
      {
      case Untyped_Rune_Kind:
        out.value.untyped_rune_value = left.value.untyped_rune_value - right.value.untyped_rune_value;
        break;
      case Untyped_Integer_Kind:
        out.value.untyped_integer_value = left.value.untyped_integer_value - right.value.untyped_integer_value;
        break;
      case Untyped_Float_Kind:
        out.value.untyped_float_value = left.value.untyped_float_value - right.value.untyped_float_value;
        break;
      case Untyped_Complex_Kind:
        out.value.untyped_complex_value = left.value.untyped_complex_value - right.value.untyped_complex_value;
        break;
      case Uint8_Kind:
        out.value.uint8_value = left.value.uint8_value - right.value.uint8_value;
        break;
      case Uint16_Kind:
        out.value.uint16_value = left.value.uint16_value - right.value.uint16_value;
        break;
      case Uint32_Kind:
        out.value.uint32_value = left.value.uint32_value - right.value.uint32_value;
        break;
      case Uint64_Kind:
        out.value.uint64_value = left.value.uint64_value - right.value.uint64_value;
        break;
      case Int8_Kind:
        out.value.int8_value = left.value.int8_value - right.value.int8_value;
        break;
      case Int16_Kind:
        out.value.int16_value = left.value.int16_value - right.value.int16_value;
        break;
      case Int32_Kind:
        out.value.int32_value = left.value.int32_value - right.value.int32_value;
        break;
      case Int64_Kind:
        out.value.int64_value = left.value.int64_value - right.value.int64_value;
        break;
      case Float32_Kind:
        out.value.float32_value = left.value.float32_value - right.value.float32_value;
        break;
      case Float64_Kind:
        out.value.float64_value = left.value.float64_value - right.value.float64_value;
        break;
      case Complex64_Kind:
        out.value.complex64_value = left.value.complex64_value - right.value.complex64_value;
        break;
      case Complex128_Kind:
        out.value.complex128_value = left.value.complex128_value - right.value.complex128_value;
        break;
      case Uint_Kind:
        out.value.uint_value = left.value.uint_value - right.value.uint_value;
        break;
      case Int_Kind:
        out.value.int_value = left.value.int_value - right.value.int_value;
        break;
      case Uintptr_Kind:
        out.value.uintptr_value = left.value.uintptr_value - right.value.uintptr_value;
        break;
      default:
        TYPE_NOT_REACHED (*type);
      }
  }
};

struct SBitOr
{
  void operator() (ExpressionValue& out, const type::Type* type, const ExpressionValue& left, const ExpressionValue& right) const
  {
    assert (left.is_constant ());
    assert (right.is_constant ());
    out.kind = ExpressionValue::Constant;
    switch (type->underlying_kind ())
      {
      case Untyped_Rune_Kind:
        out.value.untyped_rune_value = left.value.untyped_rune_value | right.value.untyped_rune_value;
        break;
      case Untyped_Integer_Kind:
        out.value.untyped_integer_value = left.value.untyped_integer_value | right.value.untyped_integer_value;
        break;
      case Uint8_Kind:
        out.value.uint8_value = left.value.uint8_value | right.value.uint8_value;
        break;
      case Uint16_Kind:
        out.value.uint16_value = left.value.uint16_value | right.value.uint16_value;
        break;
      case Uint32_Kind:
        out.value.uint32_value = left.value.uint32_value | right.value.uint32_value;
        break;
      case Uint64_Kind:
        out.value.uint64_value = left.value.uint64_value | right.value.uint64_value;
        break;
      case Int8_Kind:
        out.value.int8_value = left.value.int8_value | right.value.int8_value;
        break;
      case Int16_Kind:
        out.value.int16_value = left.value.int16_value | right.value.int16_value;
        break;
      case Int32_Kind:
        out.value.int32_value = left.value.int32_value | right.value.int32_value;
        break;
      case Int64_Kind:
        out.value.int64_value = left.value.int64_value | right.value.int64_value;
        break;
      case Uint_Kind:
        out.value.uint_value = left.value.uint_value | right.value.uint_value;
        break;
      case Int_Kind:
        out.value.int_value = left.value.int_value | right.value.int_value;
        break;
      case Uintptr_Kind:
        out.value.uintptr_value = left.value.uintptr_value | right.value.uintptr_value;
        break;
      default:
        TYPE_NOT_REACHED (*type);
      }
  }
};

struct SBitXor
{
  void operator() (ExpressionValue& out, const type::Type* type, const ExpressionValue& left, const ExpressionValue& right) const
  {
    assert (left.is_constant ());
    assert (right.is_constant ());
    out.kind = ExpressionValue::Constant;
    switch (type->underlying_kind ())
      {
      case Untyped_Rune_Kind:
        out.value.untyped_rune_value = left.value.untyped_rune_value ^ right.value.untyped_rune_value;
        break;
      case Untyped_Integer_Kind:
        out.value.untyped_integer_value = left.value.untyped_integer_value ^ right.value.untyped_integer_value;
        break;
      case Uint8_Kind:
        out.value.uint8_value = left.value.uint8_value ^ right.value.uint8_value;
        break;
      case Uint16_Kind:
        out.value.uint16_value = left.value.uint16_value ^ right.value.uint16_value;
        break;
      case Uint32_Kind:
        out.value.uint32_value = left.value.uint32_value ^ right.value.uint32_value;
        break;
      case Uint64_Kind:
        out.value.uint64_value = left.value.uint64_value ^ right.value.uint64_value;
        break;
      case Int8_Kind:
        out.value.int8_value = left.value.int8_value ^ right.value.int8_value;
        break;
      case Int16_Kind:
        out.value.int16_value = left.value.int16_value ^ right.value.int16_value;
        break;
      case Int32_Kind:
        out.value.int32_value = left.value.int32_value ^ right.value.int32_value;
        break;
      case Int64_Kind:
        out.value.int64_value = left.value.int64_value ^ right.value.int64_value;
        break;
      case Uint_Kind:
        out.value.uint_value = left.value.uint_value ^ right.value.uint_value;
        break;
      case Int_Kind:
        out.value.int_value = left.value.int_value ^ right.value.int_value;
        break;
      case Uintptr_Kind:
        out.value.uintptr_value = left.value.uintptr_value ^ right.value.uintptr_value;
        break;
      default:
        TYPE_NOT_REACHED (*type);
      }
  }
};

struct SEqual
{
  void operator() (ExpressionValue& out, const type::Type* type, const ExpressionValue& left, const ExpressionValue& right) const
  {
    assert (left.is_constant ());
    assert (right.is_constant ());
    out.kind = ExpressionValue::Constant;
    switch (type->underlying_kind ())
      {
      case Untyped_Nil_Kind:
        out.value.untyped_boolean_value = true;
        break;
      case Untyped_Boolean_Kind:
        out.value.untyped_boolean_value = left.value.untyped_boolean_value == right.value.untyped_boolean_value;
        break;
      case Untyped_Rune_Kind:
        out.value.untyped_boolean_value = left.value.untyped_rune_value == right.value.untyped_rune_value;
        break;
      case Untyped_Integer_Kind:
        out.value.untyped_boolean_value = left.value.untyped_integer_value == right.value.untyped_integer_value;
        break;
      case Untyped_Float_Kind:
        out.value.untyped_boolean_value = left.value.untyped_float_value == right.value.untyped_float_value;
        break;
      case Untyped_Complex_Kind:
        out.value.untyped_boolean_value = left.value.untyped_complex_value == right.value.untyped_complex_value;
        break;
      case Untyped_String_Kind:
        out.value.untyped_boolean_value = left.value.untyped_string_value == right.value.untyped_string_value;
        break;
      case Bool_Kind:
        out.value.untyped_boolean_value = left.value.bool_value == right.value.bool_value;
        break;
      case Uint8_Kind:
        out.value.untyped_boolean_value = left.value.uint8_value == right.value.uint8_value;
        break;
      case Uint16_Kind:
        out.value.untyped_boolean_value = left.value.uint16_value == right.value.uint16_value;
        break;
      case Uint32_Kind:
        out.value.untyped_boolean_value = left.value.uint32_value == right.value.uint32_value;
        break;
      case Uint64_Kind:
        out.value.untyped_boolean_value = left.value.uint64_value == right.value.uint64_value;
        break;
      case Int8_Kind:
        out.value.untyped_boolean_value = left.value.int8_value == right.value.int8_value;
        break;
      case Int16_Kind:
        out.value.untyped_boolean_value = left.value.int16_value == right.value.int16_value;
        break;
      case Int32_Kind:
        out.value.untyped_boolean_value = left.value.int32_value == right.value.int32_value;
        break;
      case Int64_Kind:
        out.value.untyped_boolean_value = left.value.int64_value == right.value.int64_value;
        break;
      case Float32_Kind:
        out.value.untyped_boolean_value = left.value.float32_value == right.value.float32_value;
        break;
      case Float64_Kind:
        out.value.untyped_boolean_value = left.value.float64_value == right.value.float64_value;
        break;
      case Complex64_Kind:
        out.value.untyped_boolean_value = left.value.complex64_value == right.value.complex64_value;
        break;
      case Complex128_Kind:
        out.value.untyped_boolean_value = left.value.complex128_value == right.value.complex128_value;
        break;
      case Uint_Kind:
        out.value.untyped_boolean_value = left.value.uint_value == right.value.uint_value;
        break;
      case Int_Kind:
        out.value.untyped_boolean_value = left.value.int_value == right.value.int_value;
        break;
      case Uintptr_Kind:
        out.value.untyped_boolean_value = left.value.uintptr_value == right.value.uintptr_value;
        break;
      case String_Kind:
        out.value.untyped_boolean_value = left.value.string_value == right.value.string_value;
        break;
      case Pointer_Kind:
        // Always nil.
        out.value.untyped_boolean_value = true;
        break;
      default:
        TYPE_NOT_REACHED (*type);
      }
  }
};

struct SNotEqual
{
  void operator() (ExpressionValue& out, const type::Type* type, const ExpressionValue& left, const ExpressionValue& right) const
  {
    assert (left.is_constant ());
    assert (right.is_constant ());
    out.kind = ExpressionValue::Constant;
    switch (type->underlying_kind ())
      {
      case Untyped_Nil_Kind:
        out.value.untyped_boolean_value = false;
        break;
      case Untyped_Boolean_Kind:
        out.value.untyped_boolean_value = left.value.untyped_boolean_value != right.value.untyped_boolean_value;
        break;
      case Untyped_Rune_Kind:
        out.value.untyped_boolean_value = left.value.untyped_rune_value != right.value.untyped_rune_value;
        break;
      case Untyped_Integer_Kind:
        out.value.untyped_boolean_value = left.value.untyped_integer_value != right.value.untyped_integer_value;
        break;
      case Untyped_Float_Kind:
        out.value.untyped_boolean_value = left.value.untyped_float_value != right.value.untyped_float_value;
        break;
      case Untyped_Complex_Kind:
        out.value.untyped_boolean_value = left.value.untyped_complex_value != right.value.untyped_complex_value;
        break;
      case Untyped_String_Kind:
        out.value.untyped_boolean_value = left.value.untyped_string_value != right.value.untyped_string_value;
        break;
      case Bool_Kind:
        out.value.untyped_boolean_value = left.value.bool_value != right.value.bool_value;
        break;
      case Uint8_Kind:
        out.value.untyped_boolean_value = left.value.uint8_value != right.value.uint8_value;
        break;
      case Uint16_Kind:
        out.value.untyped_boolean_value = left.value.uint16_value != right.value.uint16_value;
        break;
      case Uint32_Kind:
        out.value.untyped_boolean_value = left.value.uint32_value != right.value.uint32_value;
        break;
      case Uint64_Kind:
        out.value.untyped_boolean_value = left.value.uint64_value != right.value.uint64_value;
        break;
      case Int8_Kind:
        out.value.untyped_boolean_value = left.value.int8_value != right.value.int8_value;
        break;
      case Int16_Kind:
        out.value.untyped_boolean_value = left.value.int16_value != right.value.int16_value;
        break;
      case Int32_Kind:
        out.value.untyped_boolean_value = left.value.int32_value != right.value.int32_value;
        break;
      case Int64_Kind:
        out.value.untyped_boolean_value = left.value.int64_value != right.value.int64_value;
        break;
      case Float32_Kind:
        out.value.untyped_boolean_value = left.value.float32_value != right.value.float32_value;
        break;
      case Float64_Kind:
        out.value.untyped_boolean_value = left.value.float64_value != right.value.float64_value;
        break;
      case Complex64_Kind:
        out.value.untyped_boolean_value = left.value.complex64_value != right.value.complex64_value;
        break;
      case Complex128_Kind:
        out.value.untyped_boolean_value = left.value.complex128_value != right.value.complex128_value;
        break;
      case Uint_Kind:
        out.value.untyped_boolean_value = left.value.uint_value != right.value.uint_value;
        break;
      case Int_Kind:
        out.value.untyped_boolean_value = left.value.int_value != right.value.int_value;
        break;
      case Uintptr_Kind:
        out.value.untyped_boolean_value = left.value.uintptr_value != right.value.uintptr_value;
        break;
      case String_Kind:
        out.value.untyped_boolean_value = left.value.string_value != right.value.string_value;
        break;
      case Pointer_Kind:
        out.value.untyped_boolean_value = false;
        break;
      default:
        TYPE_NOT_REACHED (*type);
      }
  }
};

struct SLessThan
{
  void operator() (ExpressionValue& out, const type::Type* type, const ExpressionValue& left, const ExpressionValue& right) const
  {
    assert (left.is_constant ());
    assert (right.is_constant ());
    out.kind = ExpressionValue::Constant;
    switch (type->underlying_kind ())
      {
      case Untyped_Rune_Kind:
        out.value.untyped_boolean_value = left.value.untyped_rune_value < right.value.untyped_rune_value;
        break;
      case Untyped_Integer_Kind:
        out.value.untyped_boolean_value = left.value.untyped_integer_value < right.value.untyped_integer_value;
        break;
      case Untyped_Float_Kind:
        out.value.untyped_boolean_value = left.value.untyped_float_value < right.value.untyped_float_value;
        break;
      case Untyped_String_Kind:
        out.value.untyped_boolean_value = left.value.untyped_string_value < right.value.untyped_string_value;
        break;
      case Uint8_Kind:
        out.value.untyped_boolean_value = left.value.uint8_value < right.value.uint8_value;
        break;
      case Uint16_Kind:
        out.value.untyped_boolean_value = left.value.uint16_value < right.value.uint16_value;
        break;
      case Uint32_Kind:
        out.value.untyped_boolean_value = left.value.uint32_value < right.value.uint32_value;
        break;
      case Uint64_Kind:
        out.value.untyped_boolean_value = left.value.uint64_value < right.value.uint64_value;
        break;
      case Int8_Kind:
        out.value.untyped_boolean_value = left.value.int8_value < right.value.int8_value;
        break;
      case Int16_Kind:
        out.value.untyped_boolean_value = left.value.int16_value < right.value.int16_value;
        break;
      case Int32_Kind:
        out.value.untyped_boolean_value = left.value.int32_value < right.value.int32_value;
        break;
      case Int64_Kind:
        out.value.untyped_boolean_value = left.value.int64_value < right.value.int64_value;
        break;
      case Float32_Kind:
        out.value.untyped_boolean_value = left.value.float32_value < right.value.float32_value;
        break;
      case Float64_Kind:
        out.value.untyped_boolean_value = left.value.float64_value < right.value.float64_value;
        break;
      case Uint_Kind:
        out.value.untyped_boolean_value = left.value.uint_value < right.value.uint_value;
        break;
      case Int_Kind:
        out.value.untyped_boolean_value = left.value.int_value < right.value.int_value;
        break;
      case Uintptr_Kind:
        out.value.untyped_boolean_value = left.value.uintptr_value < right.value.uintptr_value;
        break;
      case String_Kind:
        out.value.untyped_boolean_value = left.value.string_value < right.value.string_value;
        break;
      default:
        TYPE_NOT_REACHED (*type);
      }
  }
};

struct SLessEqual
{
  void operator() (ExpressionValue& out, const type::Type* type, const ExpressionValue& left, const ExpressionValue& right) const
  {
    assert (left.is_constant ());
    assert (right.is_constant ());
    out.kind = ExpressionValue::Constant;
    switch (type->underlying_kind ())
      {
      case Untyped_Rune_Kind:
        out.value.untyped_boolean_value = left.value.untyped_rune_value <= right.value.untyped_rune_value;
        break;
      case Untyped_Integer_Kind:
        out.value.untyped_boolean_value = left.value.untyped_integer_value <= right.value.untyped_integer_value;
        break;
      case Untyped_Float_Kind:
        out.value.untyped_boolean_value = left.value.untyped_float_value <= right.value.untyped_float_value;
        break;
      case Untyped_String_Kind:
        out.value.untyped_boolean_value = left.value.untyped_string_value <= right.value.untyped_string_value;
        break;
      case Uint8_Kind:
        out.value.untyped_boolean_value = left.value.uint8_value <= right.value.uint8_value;
        break;
      case Uint16_Kind:
        out.value.untyped_boolean_value = left.value.uint16_value <= right.value.uint16_value;
        break;
      case Uint32_Kind:
        out.value.untyped_boolean_value = left.value.uint32_value <= right.value.uint32_value;
        break;
      case Uint64_Kind:
        out.value.untyped_boolean_value = left.value.uint64_value <= right.value.uint64_value;
        break;
      case Int8_Kind:
        out.value.untyped_boolean_value = left.value.int8_value <= right.value.int8_value;
        break;
      case Int16_Kind:
        out.value.untyped_boolean_value = left.value.int16_value <= right.value.int16_value;
        break;
      case Int32_Kind:
        out.value.untyped_boolean_value = left.value.int32_value <= right.value.int32_value;
        break;
      case Int64_Kind:
        out.value.untyped_boolean_value = left.value.int64_value <= right.value.int64_value;
        break;
      case Float32_Kind:
        out.value.untyped_boolean_value = left.value.float32_value <= right.value.float32_value;
        break;
      case Float64_Kind:
        out.value.untyped_boolean_value = left.value.float64_value <= right.value.float64_value;
        break;
      case Uint_Kind:
        out.value.untyped_boolean_value = left.value.uint_value <= right.value.uint_value;
        break;
      case Int_Kind:
        out.value.untyped_boolean_value = left.value.int_value <= right.value.int_value;
        break;
      case Uintptr_Kind:
        out.value.untyped_boolean_value = left.value.uintptr_value <= right.value.uintptr_value;
        break;
      case String_Kind:
        out.value.untyped_boolean_value = left.value.string_value <= right.value.string_value;
        break;
      default:
        TYPE_NOT_REACHED (*type);
      }
  }
};

struct SMoreThan
{
  void operator() (ExpressionValue& out, const type::Type* type, const ExpressionValue& left, const ExpressionValue& right) const
  {
    assert (left.is_constant ());
    assert (right.is_constant ());
    out.kind = ExpressionValue::Constant;
    switch (type->underlying_kind ())
      {
      case Untyped_Rune_Kind:
        out.value.untyped_boolean_value = left.value.untyped_rune_value > right.value.untyped_rune_value;
        break;
      case Untyped_Integer_Kind:
        out.value.untyped_boolean_value = left.value.untyped_integer_value > right.value.untyped_integer_value;
        break;
      case Untyped_Float_Kind:
        out.value.untyped_boolean_value = left.value.untyped_float_value > right.value.untyped_float_value;
        break;
      case Untyped_String_Kind:
        out.value.untyped_boolean_value = left.value.untyped_string_value > right.value.untyped_string_value;
        break;
      case Uint8_Kind:
        out.value.untyped_boolean_value = left.value.uint8_value > right.value.uint8_value;
        break;
      case Uint16_Kind:
        out.value.untyped_boolean_value = left.value.uint16_value > right.value.uint16_value;
        break;
      case Uint32_Kind:
        out.value.untyped_boolean_value = left.value.uint32_value > right.value.uint32_value;
        break;
      case Uint64_Kind:
        out.value.untyped_boolean_value = left.value.uint64_value > right.value.uint64_value;
        break;
      case Int8_Kind:
        out.value.untyped_boolean_value = left.value.int8_value > right.value.int8_value;
        break;
      case Int16_Kind:
        out.value.untyped_boolean_value = left.value.int16_value > right.value.int16_value;
        break;
      case Int32_Kind:
        out.value.untyped_boolean_value = left.value.int32_value > right.value.int32_value;
        break;
      case Int64_Kind:
        out.value.untyped_boolean_value = left.value.int64_value > right.value.int64_value;
        break;
      case Float32_Kind:
        out.value.untyped_boolean_value = left.value.float32_value > right.value.float32_value;
        break;
      case Float64_Kind:
        out.value.untyped_boolean_value = left.value.float64_value > right.value.float64_value;
        break;
      case Uint_Kind:
        out.value.untyped_boolean_value = left.value.uint_value > right.value.uint_value;
        break;
      case Int_Kind:
        out.value.untyped_boolean_value = left.value.int_value > right.value.int_value;
        break;
      case Uintptr_Kind:
        out.value.untyped_boolean_value = left.value.uintptr_value > right.value.uintptr_value;
        break;
      case String_Kind:
        out.value.untyped_boolean_value = left.value.string_value > right.value.string_value;
        break;
      default:
        TYPE_NOT_REACHED (*type);
      }
  }
};

struct SMoreEqual
{
  void operator() (ExpressionValue& out, const type::Type* type, const ExpressionValue& left, const ExpressionValue& right) const
  {
    assert (left.is_constant ());
    assert (right.is_constant ());
    out.kind = ExpressionValue::Constant;
    switch (type->underlying_kind ())
      {
      case Untyped_Rune_Kind:
        out.value.untyped_boolean_value = left.value.untyped_rune_value >= right.value.untyped_rune_value;
        break;
      case Untyped_Integer_Kind:
        out.value.untyped_boolean_value = left.value.untyped_integer_value >= right.value.untyped_integer_value;
        break;
      case Untyped_Float_Kind:
        out.value.untyped_boolean_value = left.value.untyped_float_value >= right.value.untyped_float_value;
        break;
      case Untyped_String_Kind:
        out.value.untyped_boolean_value = left.value.untyped_string_value >= right.value.untyped_string_value;
        break;
      case Uint8_Kind:
        out.value.untyped_boolean_value = left.value.uint8_value >= right.value.uint8_value;
        break;
      case Uint16_Kind:
        out.value.untyped_boolean_value = left.value.uint16_value >= right.value.uint16_value;
        break;
      case Uint32_Kind:
        out.value.untyped_boolean_value = left.value.uint32_value >= right.value.uint32_value;
        break;
      case Uint64_Kind:
        out.value.untyped_boolean_value = left.value.uint64_value >= right.value.uint64_value;
        break;
      case Int8_Kind:
        out.value.untyped_boolean_value = left.value.int8_value >= right.value.int8_value;
        break;
      case Int16_Kind:
        out.value.untyped_boolean_value = left.value.int16_value >= right.value.int16_value;
        break;
      case Int32_Kind:
        out.value.untyped_boolean_value = left.value.int32_value >= right.value.int32_value;
        break;
      case Int64_Kind:
        out.value.untyped_boolean_value = left.value.int64_value >= right.value.int64_value;
        break;
      case Float32_Kind:
        out.value.untyped_boolean_value = left.value.float32_value >= right.value.float32_value;
        break;
      case Float64_Kind:
        out.value.untyped_boolean_value = left.value.float64_value >= right.value.float64_value;
        break;
      case Uint_Kind:
        out.value.untyped_boolean_value = left.value.uint_value >= right.value.uint_value;
        break;
      case Int_Kind:
        out.value.untyped_boolean_value = left.value.int_value >= right.value.int_value;
        break;
      case Uintptr_Kind:
        out.value.untyped_boolean_value = left.value.uintptr_value >= right.value.uintptr_value;
        break;
      case String_Kind:
        out.value.untyped_boolean_value = left.value.string_value >= right.value.string_value;
        break;
      default:
        TYPE_NOT_REACHED (*type);
      }
  }
};

template <typename InputPicker, typename OutputPicker, typename Computer>
void
binary_check (ErrorReporter& er,
              const source::Location& location,
              const std::string& name,
              ExpressionValue& result,
              ExpressionValueList& arguments,
              const source::LocationList& locations)
{
  assert (arguments.size () == 2);
  ExpressionValue& left = arguments[0];
  ExpressionValue& right = arguments[1];

  if (left.is_error () || right.is_error ())
    {
      result = ExpressionValue::make_error ();
      return;
    }

  if (!left.is_rvalue ())
    {
      er.expected_an_rvalue (locations[0]);
      result = ExpressionValue::make_error ();
      return;
    }

  if (!right.is_rvalue ())
    {
      er.expected_an_rvalue (locations[1]);
      result = ExpressionValue::make_error ();
      return;
    }

  if (left.type->is_untyped () &&
      right.type->is_untyped ())
    {
      const type::Type* t = InputPicker::pick (left.type, right.type);
      if (t == NULL)
        {
          er.cannot_be_applied (location, name, left.type, right.type);
          result = ExpressionValue::make_error ();
          return;
        }
      left.convert (t);
      right.convert (t);

      result.kind = ExpressionValue::Constant;
      result.type = OutputPicker::pick (t, left, right);
      Computer::compute (result, t, left, right);
      result.intrinsic_mutability = Immutable;
      result.indirection_mutability = Immutable;

      return;
    }

  if (!(left.is_assignable (right.type) ||
        right.is_assignable (left.type)))
    {
      er.cannot_be_applied (location, name, left.type, right.type);
      result = ExpressionValue::make_error ();
      return;
    }

  const type::Type* in_type = choose (left.type, right.type);
  left.convert (in_type);
  right.convert (in_type);

  if (InputPicker::pick (in_type, in_type) == NULL)
    {
      er.cannot_be_applied (location, name, left.type, right.type);
      result.kind = ExpressionValue::Error;
      return;
    }

  result.kind = ExpressionValue::Value;
  result.type = OutputPicker::pick (in_type, left, right);
  result.intrinsic_mutability = Immutable;
  result.indirection_mutability = Immutable;

  Computer::compute (result, in_type, left, right);
}

template <typename B>
void shift_check (ErrorReporter& er,
                  const source::Location& location,
                  const std::string& name,
                  ExpressionValue& result,
                  ExpressionValueList& arguments,
                  const source::LocationList& locations)
{
  assert (arguments.size () == 2);
  ExpressionValue& left = arguments[0];
  ExpressionValue& right = arguments[1];

  if (left.is_error () || right.is_error ())
    {
      result = ExpressionValue::make_error ();
      return;
    }

  if (!left.is_rvalue ())
    {
      er.expected_an_rvalue (locations[0]);
      result = ExpressionValue::make_error ();
      return;
    }

  if (!right.is_rvalue ())
    {
      er.expected_an_rvalue (locations[1]);
      result = ExpressionValue::make_error ();
      return;
    }

  if (!(right.type->is_typed_unsigned_integer () || right.type->is_untyped_numeric ()))
    {
      er.cannot_be_applied (location, name, left.type, right.type);
      result = ExpressionValue::make_error ();
      return;
    }

  if (right.is_constant ())
    {
      if (!right.type->is_typed_unsigned_integer () && !right.representable (type::Uint::instance ()))
        {
          er.cannot_be_applied (location, name, left.type, right.type);
          result = ExpressionValue::make_error ();
          return;
        }
      right.convert (type::Uint::instance ());
      right.type = type::Uint::instance ();
    }

  if (!left.type->is_integral ())
    {
      er.cannot_be_applied (location, name, left.type, right.type);
      result = ExpressionValue::make_error ();
      return;
    }

  if (left.is_constant () &&
      right.is_constant ())
    {
      switch (left.type->underlying_kind ())
        {
        case Uint8_Kind:
          result = ExpressionValue::make_uint8 (left.type, B () (left.value.uint8_value, right.value.uint_value));
          break;
        case Uint16_Kind:
          result = ExpressionValue::make_uint16 (left.type, B () (left.value.uint16_value, right.value.uint_value));
          break;
        case Uint32_Kind:
          result = ExpressionValue::make_uint32 (left.type, B () (left.value.uint32_value, right.value.uint_value));
          break;
        case Uint64_Kind:
          result = ExpressionValue::make_uint64 (left.type, B () (left.value.uint64_value, right.value.uint_value));
          break;
        case Int8_Kind:
          result = ExpressionValue::make_int8 (left.type, B () (left.value.int8_value, right.value.uint_value));
          break;
        case Int16_Kind:
          result = ExpressionValue::make_int16 (left.type, B () (left.value.int16_value, right.value.uint_value));
          break;
        case Int32_Kind:
          result = ExpressionValue::make_int32 (left.type, B () (left.value.int32_value, right.value.uint_value));
          break;
        case Int64_Kind:
          result = ExpressionValue::make_int64 (left.type, B () (left.value.int64_value, right.value.uint_value));
          break;
        case Uint_Kind:
          result = ExpressionValue::make_uint (left.type, B () (left.value.uint_value, right.value.uint_value));
          break;
        case Int_Kind:
          result = ExpressionValue::make_int (left.type, B () (left.value.int_value, right.value.uint_value));
          break;
        case Uintptr_Kind:
          result = ExpressionValue::make_uintptr (left.type, B () (left.value.uintptr_value, right.value.uint_value));
          break;
        case Untyped_Rune_Kind:
          result = ExpressionValue::make_rune (B () (left.value.untyped_rune_value, right.value.uint_value));
          break;
        case Untyped_Integer_Kind:
          result = ExpressionValue::make_integer (B () (left.value.untyped_integer_value, right.value.uint_value));
          break;
        default:
          TYPE_NOT_REACHED (*left.type);
        }
    }
  else
    {
      result = ExpressionValue::make_value (left.type, Immutable, Immutable);
    }
}

struct LogicAndComputer
{
  static void compute (semantic::ExpressionValue& result,
                       const type::Type* in_type,
                       const semantic::ExpressionValue& left,
                       const semantic::ExpressionValue& right)
  {
    if (left.is_constant ())
      {
        if (!left.value.bool_value)
          {
            result.kind = ExpressionValue::Constant;
            result.value.untyped_boolean_value = false;
          }
        else if (right.is_constant ())
          {
            result.kind = ExpressionValue::Constant;
            result.value.untyped_boolean_value = right.value.bool_value;
          }
      }
  }
};

struct LogicOrComputer
{
  static void compute (semantic::ExpressionValue& result,
                       const type::Type* in_type,
                       const semantic::ExpressionValue& left,
                       const semantic::ExpressionValue& right)
  {
    if (left.is_constant ())
      {
        if (left.value.bool_value)
          {
            result.kind = ExpressionValue::Constant;
            result.value.untyped_boolean_value = true;
          }
        else if (right.is_constant ())
          {
            result.kind = ExpressionValue::Constant;
            result.value.untyped_boolean_value = right.value.bool_value;
          }
      }
  }
};

static bool check_dimension (ErrorReporter& er,
                             const source::Location& location,
                             ExpressionValue& arg)
{
  if (!arg.is_rvalue ())
    {
      er.expected_an_rvalue (location);
      return false;
    }

  if (arg.is_constant ())
    {
      // Convert to an int.
      if (!arg.representable (&named_int))
        {
          er.cannot_convert (location, arg.type, &named_int);
          return false;
        }

      arg.convert (&named_int);
      arg.type = &named_int;
      long dim = arg.value.int_value;
      if (dim < 0)
        {
          er.negative_array_dimension (location, dim);
          return false;
        }
    }
  else
    {
      if (!arg.type->is_integral ())
        {
          er.non_integer_array_dimension (location, arg.type);
          return false;
        }
    }

  return true;
}


void check_polymorphic_function_call (ErrorReporter& er,
                                      const PolymorphicFunction* pf,
                                      const source::Location& loc,
                                      ExpressionValue& result,
                                      ExpressionValueList& arguments,
                                      const source::LocationList& locations)
{
  struct Visitor : public PolymorphicFunctionVisitor
  {
    ErrorReporter& er;
    const source::Location& location;
    ExpressionValue& result;
    ExpressionValueList& arguments;
    const source::LocationList& locations;

    Visitor (ErrorReporter& a_er,
             const source::Location& a_location,
             ExpressionValue& a_result,
             ExpressionValueList& a_arguments,
             const source::LocationList& a_locations)
      : er (a_er)
      , location (a_location)
      , result (a_result)
      , arguments (a_arguments)
      , locations (a_locations)
    { }

    virtual void visit (const LogicNot& pf)
    {
      assert (arguments.size () == 1);
      const ExpressionValue& arg = arguments.front ();

      if (arg.is_error ())
        {
          result = ExpressionValue::make_error ();
          return;
        }

      if (!arg.is_rvalue ())
        {
          er.expected_an_rvalue (location);
          result = ExpressionValue::make_error ();
          return;
        }

      if (!arg.type->is_any_boolean ())
        {
          er.cannot_be_applied (location, pf.identifier.identifier (), arg.type);
          result = ExpressionValue::make_error ();
          return;
        }

      if (arg.is_constant ())
        {
          switch (arg.type->underlying_kind ())
            {
            case Bool_Kind:
              result = ExpressionValue::make_bool (arg.type, !arg.value.bool_value);
              break;
            case Untyped_Boolean_Kind:
              result = ExpressionValue::make_boolean (!arg.value.untyped_boolean_value);
              break;
            default:
              NOT_REACHED;
            }
        }
      else
        {
          result = ExpressionValue::make_value (arg.type, Immutable, Immutable);
        }
    }
    virtual void visit (const Negate& pf)
    {
      assert (arguments.size () == 1);
      const ExpressionValue& arg = arguments.front ();

      if (arg.is_error ())
        {
          result = ExpressionValue::make_error ();
          return;
        }

      if (!arg.is_rvalue ())
        {
          er.expected_an_rvalue (location);
          result = ExpressionValue::make_error ();
          return;
        }

      if (!arg.type->is_numeric ())
        {
          er.cannot_be_applied (location, pf.identifier.identifier (), arg.type);
          result = ExpressionValue::make_error ();
          return;
        }

      if (arg.is_constant ())
        {
          switch (arg.type->underlying_kind ())
            {
            case Uint8_Kind:
              result = ExpressionValue::make_uint8 (arg.type, -arg.value.uint8_value);
              break;
            case Uint16_Kind:
              result = ExpressionValue::make_uint16 (arg.type, -arg.value.uint16_value);
              break;
            case Uint32_Kind:
              result = ExpressionValue::make_uint32 (arg.type, -arg.value.uint32_value);
              break;
            case Uint64_Kind:
              result = ExpressionValue::make_uint64 (arg.type, -arg.value.uint64_value);
              break;
            case Int8_Kind:
              result = ExpressionValue::make_int8 (arg.type, -arg.value.int8_value);
              break;
            case Int16_Kind:
              result = ExpressionValue::make_int16 (arg.type, -arg.value.int16_value);
              break;
            case Int32_Kind:
              result = ExpressionValue::make_int32 (arg.type, -arg.value.int32_value);
              break;
            case Int64_Kind:
              result = ExpressionValue::make_int64 (arg.type, -arg.value.int64_value);
              break;
            case Float32_Kind:
              result = ExpressionValue::make_float32 (arg.type, -arg.value.float32_value);
              break;
            case Float64_Kind:
              result = ExpressionValue::make_float64 (arg.type, -arg.value.float64_value);
              break;
            case Complex64_Kind:
              result = ExpressionValue::make_complex64 (arg.type, -arg.value.complex64_value);
              break;
            case Complex128_Kind:
              result = ExpressionValue::make_complex128 (arg.type, -arg.value.complex128_value);
              break;
            case Uint_Kind:
              result = ExpressionValue::make_uint (arg.type, -arg.value.uint_value);
              break;
            case Int_Kind:
              result = ExpressionValue::make_int (arg.type, -arg.value.int_value);
              break;
            case Uintptr_Kind:
              result = ExpressionValue::make_uintptr (arg.type, -arg.value.uintptr_value);
              break;
            case Untyped_Rune_Kind:
              result = ExpressionValue::make_rune (-arg.value.untyped_rune_value);
              break;
            case Untyped_Integer_Kind:
              result = ExpressionValue::make_integer (-arg.value.untyped_integer_value);
              break;
            case Untyped_Float_Kind:
              result = ExpressionValue::make_float (-arg.value.untyped_float_value);
              break;
            case Untyped_Complex_Kind:
              result = ExpressionValue::make_complex (-arg.value.untyped_complex_value);
              break;
            default:
              NOT_REACHED;
            }
        }
      else
        {
          result = ExpressionValue::make_value (arg.type, Immutable, Immutable);
        }
    }
    virtual void visit (const Posate& pf)
    {
      assert (arguments.size () == 1);
      const ExpressionValue& arg = arguments.front ();

      if (arg.is_error ())
        {
          result = ExpressionValue::make_error ();
          return;
        }

      if (!arg.is_rvalue ())
        {
          er.expected_an_rvalue (location);
          result = ExpressionValue::make_error ();
          return;
        }

      if (!arg.type->is_numeric ())
        {
          er.cannot_be_applied (location, pf.identifier.identifier (), arg.type);
          result = ExpressionValue::make_error ();
          return;
        }

      if (arg.is_constant ())
        {
          result = arg;
        }
      else
        {
          result = ExpressionValue::make_value (arg.type, Immutable, Immutable);
        }
    }
    virtual void visit (const Complement& pf)
    {
      assert (arguments.size () == 1);
      const ExpressionValue& arg = arguments.front ();

      if (arg.is_error ())
        {
          result = ExpressionValue::make_error ();
          return;
        }

      if (!arg.is_rvalue ())
        {
          er.expected_an_rvalue (location);
          result = ExpressionValue::make_error ();
          return;
        }

      if (!(arg.type->is_integral ()))
        {
          er.cannot_be_applied (location, pf.identifier.identifier (), arg.type);
          result = ExpressionValue::make_error ();
          return;
        }

      if (arg.is_constant ())
        {
          switch (arg.type->underlying_kind ())
            {
            case Uint8_Kind:
              result = ExpressionValue::make_uint8 (arg.type, ~arg.value.uint8_value);
              break;
            case Uint16_Kind:
              result = ExpressionValue::make_uint16 (arg.type, ~arg.value.uint16_value);
              break;
            case Uint32_Kind:
              result = ExpressionValue::make_uint32 (arg.type, ~arg.value.uint32_value);
              break;
            case Uint64_Kind:
              result = ExpressionValue::make_uint64 (arg.type, ~arg.value.uint64_value);
              break;
            case Int8_Kind:
              result = ExpressionValue::make_int8 (arg.type, ~arg.value.int8_value);
              break;
            case Int16_Kind:
              result = ExpressionValue::make_int16 (arg.type, ~arg.value.int16_value);
              break;
            case Int32_Kind:
              result = ExpressionValue::make_int32 (arg.type, ~arg.value.int32_value);
              break;
            case Int64_Kind:
              result = ExpressionValue::make_int64 (arg.type, ~arg.value.int64_value);
              break;
            case Uint_Kind:
              result = ExpressionValue::make_uint (arg.type, ~arg.value.uint_value);
              break;
            case Int_Kind:
              result = ExpressionValue::make_int (arg.type, ~arg.value.int_value);
              break;
            case Uintptr_Kind:
              result = ExpressionValue::make_uintptr (arg.type, ~arg.value.uintptr_value);
              break;
            case Untyped_Rune_Kind:
              result = ExpressionValue::make_rune (~arg.value.untyped_rune_value);
              break;
            case Untyped_Integer_Kind:
              result = ExpressionValue::make_integer (~arg.value.untyped_integer_value);
              break;
            default:
              NOT_REACHED;
            }
        }
      else
        {
          result = ExpressionValue::make_value (arg.type, Immutable, Immutable);
        }
    }

    virtual void visit (const Multiply& pf)
    {
      binary_check<type::Arithmetic, PassThroughPicker, BinaryValueComputer<SMultiply> > (er, location, pf.identifier.identifier (), result, arguments, locations);
    }
    virtual void visit (const Divide& pf)
    {
      binary_check<type::Arithmetic, PassThroughPicker, BinaryValueComputer<SDivide> > (er, location, pf.identifier.identifier (), result, arguments, locations);
    }
    virtual void visit (const Modulus& pf)
    {
      binary_check<type::Integral, PassThroughPicker, BinaryValueComputer<SModulus> > (er, location, pf.identifier.identifier (), result, arguments, locations);
    }
    virtual void visit (const Add& pf)
    {
      binary_check<type::Arithmetic, PassThroughPicker, BinaryValueComputer<SAdd> > (er, location, pf.identifier.identifier (), result, arguments, locations);
    }
    virtual void visit (const Subtract& pf)
    {
      binary_check<type::Arithmetic, PassThroughPicker, BinaryValueComputer<SSubtract> > (er, location, pf.identifier.identifier (), result, arguments, locations);
    }

    virtual void visit (const Equal& pf)
    {
      binary_check<type::Comparable, BooleanPicker, BinaryValueComputer<SEqual> > (er, location, pf.identifier.identifier (), result, arguments, locations);
    }
    virtual void visit (const NotEqual& pf)
    {
      binary_check<type::Comparable, BooleanPicker, BinaryValueComputer<SNotEqual> > (er, location, pf.identifier.identifier (), result, arguments, locations);
    }
    virtual void visit (const LessThan& pf)
    {
      binary_check<type::Orderable, BooleanPicker, BinaryValueComputer<SLessThan> > (er, location, pf.identifier.identifier (), result, arguments, locations);
    }
    virtual void visit (const LessEqual& pf)
    {
      binary_check<type::Orderable, BooleanPicker, BinaryValueComputer<SLessEqual> > (er, location, pf.identifier.identifier (), result, arguments, locations);
    }
    virtual void visit (const MoreThan& pf)
    {
      binary_check<type::Orderable, BooleanPicker, BinaryValueComputer<SMoreThan> > (er, location, pf.identifier.identifier (), result, arguments, locations);
    }
    virtual void visit (const MoreEqual& pf)
    {
      binary_check<type::Orderable, BooleanPicker, BinaryValueComputer<SMoreEqual> > (er, location, pf.identifier.identifier (), result, arguments, locations);
    }

    virtual void visit (const BitAnd& pf)
    {
      binary_check<type::Integral, PassThroughPicker, BinaryValueComputer<SBitAnd> > (er, location, pf.identifier.identifier (), result, arguments, locations);
    }
    virtual void visit (const BitAndNot& pf)
    {
      binary_check<type::Integral, PassThroughPicker, BinaryValueComputer<SBitAndNot> > (er, location, pf.identifier.identifier (), result, arguments, locations);
    }
    virtual void visit (const BitOr& pf)
    {
      binary_check<type::Integral, PassThroughPicker, BinaryValueComputer<SBitOr> > (er, location, pf.identifier.identifier (), result, arguments, locations);
    }
    virtual void visit (const BitXor& pf)
    {
      binary_check<type::Integral, PassThroughPicker, BinaryValueComputer<SBitXor> > (er, location, pf.identifier.identifier (), result, arguments, locations);
    }
    virtual void visit (const LeftShift& pf)
    {
      shift_check<runtime::LeftShifter> (er, location, pf.identifier.identifier (), result, arguments, locations);
    }
    virtual void visit (const RightShift& pf)
    {
      shift_check<runtime::RightShifter> (er, location, pf.identifier.identifier (), result, arguments, locations);
    }

    virtual void visit (const LogicAnd& pf)
    {
      binary_check<type::Logical, PassThroughPicker, LogicAndComputer> (er, location, pf.identifier.identifier (), result, arguments, locations);
    }
    virtual void visit (const LogicOr& pf)
    {
      binary_check<type::Logical, PassThroughPicker, LogicOrComputer> (er, location, pf.identifier.identifier (), result, arguments, locations);
    }

    virtual void visit (const New& pf)
    {
      if (arguments.size () == 0)
        {
          er.func_expects_count (location, pf.identifier.identifier (), 1, arguments.size ());
          result = ExpressionValue::make_error ();
          return;
        }

      if (arguments.size () > 1)
        {
          er.func_expects_count (location, pf.identifier.identifier (), 1, arguments.size ());
        }

      ExpressionValue& arg = arguments[0];

      // TODO:  Can we safely allocate any type or should some types not be allocated directly?

      if (arg.is_error ())
        {
          result = ExpressionValue::make_error ();
          return;
        }

      if (!arg.is_type ())
        {
          er.expected_a_type (location);
          result = ExpressionValue::make_error ();
          return;
        }

      result = ExpressionValue::make_value (arg.type->get_pointer (), Immutable, Mutable);
    }
    virtual void visit (const Make& pf)
    {
      // Check that the first argument is a type.
      if (arguments.size () == 0)
        {
          er.func_expects_count (location, pf.identifier.identifier (), 1, arguments.size ());
          result = ExpressionValue::make_error ();
          return;
        }
      ExpressionValue& arg = arguments[0];
      const source::Location& loc = locations[0];

      if (arg.is_error ())
        {
          result = ExpressionValue::make_error ();
          return;
        }

      if (!arg.is_type ())
        {
          er.expected_a_type (loc);
          result = ExpressionValue::make_error ();
          return;
        }

      // Making a slice.
      const type::Slice* slice_type = arg.type->to_slice ();
      if (slice_type != NULL)
        {
          if (arguments.size () < 2)
            {
              er.func_expects_count (location, pf.identifier.identifier (), 2, arguments.size ());
            }

          bool has_length = false;
          bool has_capacity = false;
          if (arguments.size () >= 2 && !arguments[1].is_error ())
            {
              // Check the length.
              ExpressionValue& len_arg = arguments[1];
              const source::Location& len_loc = locations[1];
              has_length = check_dimension (er, len_loc, len_arg);
            }

          if (arguments.size () >= 3 && !arguments[2].is_error ())
            {
              // Check the capacity.
              ExpressionValue& cap_arg = arguments[2];
              const source::Location& cap_loc = locations[2];
              has_capacity = check_dimension (er, cap_loc, cap_arg);
            }

          if (has_length && has_capacity)
            {
              ExpressionValue& len_arg = arguments[1];
              ExpressionValue& cap_arg = arguments[2];
              if (len_arg.is_constant () && cap_arg.is_constant () &&
                  cap_arg.value.int_value < len_arg.value.int_value)
                {
                  er.length_exceeds_capacity (location, len_arg.value.int_value, cap_arg.value.int_value);
                }
            }

          if (arguments.size () >= 4)
            {
              er.func_expects_count (location, pf.identifier.identifier (), 3, arguments.size ());
            }

          // Slice return.
          result = ExpressionValue::make_value (slice_type, Immutable, Mutable);
          return;
        }

      // Making a map.
      const type::Map* map_type = arg.type->to_map ();
      if (map_type != NULL)
        {
          if (arguments.size () >= 2)
            {
              // Check the capacity.
              ExpressionValue& cap_arg = arguments[1];
              const source::Location& cap_loc = locations[1];
              check_dimension (er, cap_loc, cap_arg);
            }

          if (arguments.size () > 2)
            {
              er.func_expects_count (location, pf.identifier.identifier (), 2, arguments.size ());
            }

          // Map return.
          result = ExpressionValue::make_value (map_type, Immutable, Mutable);
          return;
        }

      er.cannot_be_applied (location, pf.identifier.identifier (), arg.type);
      result = ExpressionValue::make_error ();
    }
    virtual void visit (const Move& pf)
    {
      if (arguments.size () == 0)
        {
          er.func_expects_count (location, pf.identifier.identifier (), 1, arguments.size ());
          result = ExpressionValue::make_error ();
          return;
        }

      if (arguments.size () > 1)
        {
          er.func_expects_count (location, pf.identifier.identifier (), 1, arguments.size ());
        }

      ExpressionValue& arg = arguments[0];

      if (arg.is_error ())
        {
          result = ExpressionValue::make_error ();
          return;
        }

      if (!arg.is_rvalue ())
        {
          er.expected_an_rvalue (location);
          result = ExpressionValue::make_error ();
          return;
        }

      const type::Type* in = arg.type;
      const type::Type* out = in->move ();
      if (out == NULL)
        {
          er.cannot_be_applied (location, pf.identifier.identifier (), in);
          result = ExpressionValue::make_error ();
          return;
        }

      result = ExpressionValue::make_value (out, Immutable, Mutable);
    }
    virtual void visit (const Merge& pf)
    {
      if (arguments.size () == 0)
        {
          er.func_expects_count (location, pf.identifier.identifier (), 1, arguments.size ());
          result = ExpressionValue::make_error ();
          return;
        }

      if (arguments.size () > 1)
        {
          er.func_expects_count (location, pf.identifier.identifier (), 1, arguments.size ());
        }

      ExpressionValue& arg = arguments[0];

      if (arg.is_error ())
        {
          result = ExpressionValue::make_error ();
          return;
        }

      if (!arg.is_rvalue ())
        {
          er.expected_an_rvalue (location);
          result = ExpressionValue::make_error ();
          return;
        }

      const type::Type* in = arg.type;
      const type::Type* out = in->merge_change ();
      if (out == NULL)
        {
          er.cannot_be_applied (location, pf.identifier.identifier (), in);
          result = ExpressionValue::make_error ();
          return;
        }

      result = ExpressionValue::make_value (out, Immutable, Mutable);
    }
    virtual void visit (const Len& pf)
    {
      if (arguments.size () == 0)
        {
          er.func_expects_count (location, pf.identifier.identifier (), 1, arguments.size ());
          result = ExpressionValue::make_error ();
          return;
        }

      if (arguments.size () > 1)
        {
          er.func_expects_count (location, pf.identifier.identifier (), 1, arguments.size ());
        }

      ExpressionValue& arg = arguments[0];

      if (arg.is_error ())
        {
          result = ExpressionValue::make_error ();
          return;
        }

      if (!arg.is_rvalue ())
        {
          er.expected_an_rvalue (location);
          result = ExpressionValue::make_error ();
          return;
        }

      const type::Type* type = arg.type;
      if (type->is_typed_string ())
        {
          if (arg.is_constant ())
            {
              result = ExpressionValue::make_int (&named_int, arg.value.string_value.length);
            }
          else
            {
              result = ExpressionValue::make_value (&named_int, Immutable, Immutable);
            }
          return;
        }

      const type::Array* arr = type->to_array ();
      if (arr)
        {
          result = ExpressionValue::make_int (&named_int, arr->dimension);
          return;
        }

      const type::Pointer* ptr = type->pointer_to_array ();
      if (ptr)
        {
          arr = ptr->base_type->to_array ();
          result = ExpressionValue::make_int (&named_int, arr->dimension);
          return;
        }

      if (type->to_slice () ||
          type->to_map ())
        {
          result = ExpressionValue::make_value (&named_int, Immutable, Immutable);
          return;
        }

      er.cannot_be_applied (location, pf.identifier.identifier (), type);
      result = ExpressionValue::make_error ();
    }
    virtual void visit (const Cap& pf)
    {
      if (arguments.size () == 0)
        {
          er.func_expects_count (location, pf.identifier.identifier (), 1, arguments.size ());
          result = ExpressionValue::make_error ();
          return;
        }

      if (arguments.size () > 1)
        {
          er.func_expects_count (location, pf.identifier.identifier (), 1, arguments.size ());
        }

      ExpressionValue& arg = arguments[0];

      if (arg.is_error ())
        {
          result = ExpressionValue::make_error ();
          return;
        }

      if (!arg.is_rvalue ())
        {
          er.expected_an_rvalue (location);
          result = ExpressionValue::make_error ();
          return;
        }

      const type::Type* type = arg.type;

      const type::Array* arr = type->to_array ();
      if (arr)
        {
          result = ExpressionValue::make_int (&named_int, arr->dimension);
          return;
        }

      const type::Pointer* ptr = type->pointer_to_array ();
      if (ptr)
        {
          arr = ptr->base_type->to_array ();
          result = ExpressionValue::make_int (&named_int, arr->dimension);
          return;
        }

      if (type->to_slice ())
        {
          result = ExpressionValue::make_value (&named_int, Immutable, Immutable);
          return;
        }

      er.cannot_be_applied (location, pf.identifier.identifier (), type);
      result = ExpressionValue::make_error ();
    }
    virtual void visit (const Append& pf)
    {
      if (arguments.size () == 0)
        {
          er.func_expects_count (location, pf.identifier.identifier (), 1, arguments.size ());
          result = ExpressionValue::make_error ();
          return;
        }

      ExpressionValue& slice = arguments[0];

      if (slice.is_error ())
        {
          result = ExpressionValue::make_error ();
          return;
        }

      if (!slice.is_rvalue ())
        {
          er.expected_an_rvalue (location);
          result = ExpressionValue::make_error ();
          return;
        }

      if (slice.type->underlying_kind () != Slice_Kind)
        {
          er.cannot_be_applied (location, pf.identifier.identifier (), slice.type);
          result = ExpressionValue::make_error ();
          return;
        }

      const type::Slice* st = slice.type->underlying_type ()->to_slice ();

      if (arguments.size () == 2)
        {
          // Accept a slice of the same type.
          ExpressionValue& element = arguments[1];

          if (element.is_rvalue () &&
              element.is_variadic () &&
              are_identical (st, element.type))
            {
              result = ExpressionValue::make_value (st, Immutable, Mutable);
              return;
            }

          // Accept a slice of bytes and a string.
          if (element.is_rvalue () &&
              element.is_variadic () &&
              st->is_slice_of_bytes () &&
              element.type == &named_string)
            {
              result = ExpressionValue::make_value (st, Immutable, Mutable);
              return;
            }
        }

      for (size_t idx = 1, limit = arguments.size (); idx != limit; ++idx)
        {
          ExpressionValue& element = arguments[idx];

          if (element.is_error ())
            {
              continue;
            }

          if (!element.is_rvalue ())
            {
              er.expected_an_rvalue (location);
              continue;
            }

          if (!element.is_assignable (st->base_type))
            {
              er.func_expects_arg (location, pf.identifier.identifier (), idx, st->base_type, element.type);
              continue;
            }
        }

      result = ExpressionValue::make_value (st, Immutable, Mutable);
    }
    virtual void visit (const Copy& pf)
    {
      result = ExpressionValue::make_value (&named_int, Immutable, Mutable);

      if (arguments.size () != 2)
        {
          er.func_expects_count (location, pf.identifier.identifier (), 2, arguments.size ());
          return;
        }

      if (arguments.size () < 1)
        {
          return;
        }

      ExpressionValue& dst = arguments[0];
      const source::Location& dst_loc = locations[0];

      if (!dst.is_rvalue ())
        {
          er.expected_an_rvalue (dst_loc);
          return;
        }

      if (dst.type->underlying_kind () != Slice_Kind)
        {
          er.cannot_be_applied (location, pf.identifier.identifier (), dst.type);
          return;
        }

      if (dst.indirection_mutability != Mutable)
        {
          er.destination_is_not_mutable (dst_loc);
        }

      if (arguments.size () < 2)
        {
          return;
        }

      ExpressionValue& src = arguments[1];
      const source::Location& src_loc = locations[1];

      if (!src.is_rvalue ())
        {
          er.expected_an_rvalue (src_loc);
          return;
        }

      if (src.type->underlying_kind () == Slice_Kind)
        {
          const type::Slice* st = src.type->underlying_type ()->to_slice ();
          if (st->base_type->contains_pointer () && src.indirection_mutability != Mutable)
            {
              er.assignment_leaks_immutable_pointers (src_loc);
            }

          if (dst.type != src.type)
            {
              er.func_expects_arg (src_loc, pf.identifier.identifier (), 1, dst.type, src.type);
            }
        }
      else if (dst.type->is_slice_of_bytes () && src.type == &named_string)
        {
          // Okay.
        }
      else
        {
          er.func_expects_arg (location, pf.identifier.identifier (), 1, dst.type, src.type);
        }
    }
    virtual void visit (const Println& pf)
    {
      for (ExpressionValueList::iterator pos = arguments.begin (), limit = arguments.end ();
           pos != limit;
           ++pos)
        {
          pos->convert (pos->type->default_type ());
        }

      result.kind = ExpressionValue::Void;
    }
  };

  Visitor visitor (er, loc, result, arguments, locations);
  pf->accept (visitor);
}

namespace
{

/*
  A reference has:
  - a type
  - a region
  - an intrinsic mutability
  - a dereference mutability
  - the value of the data to which this reference refers (possibly)

  A value has:
  - a type
  - an intrinsic mutability
  - a dereference mutability
  - an actual value (possibly)

  The four operations that contributed to the "const correctness" of
  the language are implicit dereference, dereference (*), address of
  (&), and assignment (=).

  Implicit Dereference
  --------------------

  This operation converts a reference to a value.  The type,
  intrinsic mutability, dereference mutability, and actual value of
  the reference are passed on to the value.

  | REFERENCE | VALUE |
  -----------------------+-----------+-------+
  Intrinsic Mutability |         A |     A |
  Dereference Mutability |         B |     B |


  Dereference (*)
  ---------------

  This operation converts a value to a reference and must be applied
  to a pointer.  The region of the reference is assumed to be the
  heap.  (This is points-to analysis which is undecidable in the
  general case.)  The intrinsic mutability and dereference
  mutability of the reference is determined by dereference
  mutability of the value.  The only actual values for a value with
  pointer type is nil but this could be improved.  Similarly, it is
  not defined how the actual value passes to the reference.

  | VALUE | REFERENCE |
  -----------------------+-------+-----------+
  Intrinsic Mutability |     A |         B |
  Dereference Mutability |     B |         B |


  Address Of (&)
  --------------

  This operation converts a reference to a pointer value.  The type
  and mutability of the pointer is determined by the mutability of
  the reference.

  | REFERENCE | VALUE |
  -----------------------+-----------+-------+
  Intrinsic Mutability |         A |     A |
  Dereference Mutability |         B |     B |

  Assignment (=)
  --------------

  Assignment must check if a value can be copied to the location
  indicated by a reference.  The two simple checks are (1) the types
  must be equivalent and (2) the reference should be mutable.  (All
  references to constant data should be immutable.)

  If the type being assigned does not contain a pointer, then no
  further checks are necessary as new references cannot be generated
  from the value.  However, if the type being assigned does contain
  a pointer, then we must be careful to ensure that any generated
  reference has the correct mutability.  To this end, we check the
  dereference mutability of the reference with the intrinsic
  mutability of the value.

  MUTABLE | IMMUTABLE | FOREIGN (VALUE)
  +---------+-----------+--------
  MUTABLE     |   OK    |     NO    |    NO
  IMMUTABLE   |   OK    |     OK    |    NO
  FOREIGN     |   OK    |     OK    |    OK
  (REFERENCE)

  If the value has FOREIGN dereference mutability then we require that
  the reference refer to the STACK.

  (This check is currently checked externally.)

  Port Calls and Getters
  ----------------------

  Getters (component methods) can only be defined on immutable
  components and can only be called in the immutable phase.  There
  is no restriction on the values going in but the return values
  must have FOREIGN intrinsic mutability.  This allows a component
  to return pointers but prevents them from being saved in the heap.

  Parameters to ports and reactions must have FOREIGN dereference
  mutability if the associated type contains a pointer.
*/

static void require_rvalue (const Node* node)
{
  assert (!node->eval.is_unknown ());
  if (!node->eval.is_rvalue ())
    {
      error_at_line (-1, 0, node->location.file ()->path ().c_str (), node->location.line (),
                     "required a value (E78)");
    }
}

static void require_rvalue_or_void (const Node* node)
{
  assert (!node->eval.is_unknown ());
  if (!node->eval.is_rvalue_or_void ())
    {
      error_at_line (-1, 0, node->location.file ()->path ().c_str (), node->location.line (),
                     "required a value (E78)");
    }
}

static void require_variable (const Node* node)
{
  assert (node->eval.kind != ExpressionValue::Unknown);
  if (!(node->eval.kind == ExpressionValue::Variable))
    {
      error_at_line (-1, 0, node->location.file ()->path ().c_str (), node->location.line (),
                     "required a variable (E2)");
    }
}

static const decl::Reaction* bind (Node& node, ast::Node* port_node, ast::Node* reaction_node)
{
  const type::PushPort* push_port_type = port_node->eval.type->to_push_port ();

  if (push_port_type == NULL)
    {
      error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                     "source of bind is not a port (E38)");
    }

  require_variable (port_node);

  const type::Reaction* reaction_type = reaction_node->eval.type->to_reaction ();
  if (reaction_type == NULL)
    {
      error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                     "target of bind is not a reaction (E39)");
    }

  require_variable (reaction_node);

  type::PushPort f (reaction_type->parameter_list, reaction_type->return_parameter_list);
  if (!are_identical (push_port_type, &f))
    {
      UNIMPLEMENTED;
      // error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
      //                "cannot bind %s to %s (E40)", push_port_type->to_error_string ().c_str (), reaction_type->to_error_string ().c_str ());
    }

  return static_cast<const decl::Reaction*> (reaction_node->callable);
}

static void check_condition (Node* node)
{
  const type::Type* condition = node->eval.type;
  if (!(condition->is_any_boolean ()))
    {
      error_at_line (-1, 0,
                     node->location.file ()->path ().c_str (),
                     node->location.line (),
                     "condition is not boolean (E155)");
    }
  node->eval.convert (condition->default_type ());
  require_rvalue (node);
}

// Determine the type of an expression.
struct Visitor : public ast::DefaultNodeVisitor
{
  enum Context
  {
    Other,
    Action,
    Reaction,
    Initializer,
    Getter,
  };

  ErrorReporter& er;
  SymbolTable& symbol_table;
  Parameter* receiver_parameter;
  Context context;
  bool in_mutable_phase;

  Visitor (ErrorReporter& a_er,
           SymbolTable& a_symbol_table)
    : er (a_er)
    , symbol_table (a_symbol_table)
    , receiver_parameter (NULL)
    , context (Other)
    , in_mutable_phase (false)
  { }

  void default_action (Node& node)
  {
    NODE_NOT_REACHED (node);
  }

  void visit (Call& node)
  {
    node.visit_children (*this);
    assert (node.expression->eval.kind != ExpressionValue::Unknown);
    if (node.expression->eval.kind == ExpressionValue::Type)
      {
        // Conversion.
        if (node.arguments->size () != 1)
          {
            error_at_line (-1, 0, node.location.file ()->path ().c_str (),
                           node.location.line (),
                           "conversion requires one argument (E8)");

          }

        conversion (node, node.expression, node.arguments->at (0));
        return;
      }

    // Collect the arguments.
    List* args = node.arguments;
    ExpressionValueList arguments = collect_evals (args);
    source::LocationList locations = collect_locations (args);

    if (node.expression->eval.is_polymorphic_function ())
      {
        check_polymorphic_function_call (er, node.expression->eval.polymorphic_function, node.location, node.eval, arguments, locations);
        distribute_evals (arguments, args);
        return;
      }
    else
      {
        node.callable = node.expression->callable;
      }

    node.function_type = node.expression->eval.type->to_function ();
    node.push_port_type = node.expression->eval.type->to_push_port ();
    node.pull_port_type = node.expression->eval.type->to_pull_port ();
    node.method_type = node.expression->eval.type->to_method ();
    node.initializer_type = node.expression->eval.type->to_initializer ();
    node.getter_type = node.expression->eval.type->to_getter ();
    node.reaction_type = node.expression->eval.type->to_reaction ();

    if (node.function_type)
      {
        // No restrictions on caller.
        node.parameters = node.function_type->parameter_list;
        node.return_parameters = node.function_type->return_parameter_list;
      }
    else if (node.push_port_type)
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "push ports cannot be called (E202)");

      }
    else if (node.pull_port_type)
      {
        // Must be called from either a getter, an action, or reaction.
        if (!(context == Getter ||
              context == Action ||
              context == Reaction))
          {
            error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                           "pull ports may only be called from a getter, an action, or a reaction (E201)");
          }
        if (in_mutable_phase)
          {
            error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                           "cannot call pull port in mutable section (E198)");
          }
        node.parameters = node.pull_port_type->parameter_list;
        node.return_parameters = node.pull_port_type->return_parameter_list;
      }
    else if (node.method_type)
      {
        // No restrictions on caller.
        node.parameters = node.method_type->parameter_list;
        node.return_parameters = node.method_type->return_parameter_list;
      }
    else if (node.initializer_type)
      {
        // Caller must be an initializer.
        if (context != Initializer)
          {
            error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                           "initializers may only be called from initializers (E197)");
          }
        node.parameters = node.initializer_type->parameter_list;
        node.return_parameters = node.initializer_type->return_parameter_list;
      }
    else if (node.getter_type)
      {
        // Must be called from either a getter, action, reaction, or initializer.
        if (!(context == Getter ||
              context == Action ||
              context == Reaction ||
              context == Initializer))
          {
            error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                           "getters may only be called from a getter, an action, a reaction, or an initializer (E196)");
          }
        if (in_mutable_phase)
          {
            error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                           "cannot call getter in mutable section (E34)");
          }
        node.parameters = node.getter_type->parameter_list;
        node.return_parameters = node.getter_type->return_parameter_list;
      }
    else if (node.reaction_type)
      {
        UNIMPLEMENTED;
      }
    else
      {
        NOT_REACHED;
      }

    if (node.callable != NULL)
      {
        require_rvalue (node.expression);
        node.callable->check (args);
      }
    else
      {
        node.field = node.expression->field;
        check_types_arguments (args, node.parameters);
        require_rvalue (node.expression);
        require_value_or_const_or_variable_list (node.arguments);
        check_mutability_arguments (node.arguments, node.parameters);
      }

    if (node.return_parameters->empty ())
      {
        node.eval.kind = ExpressionValue::Void;
      }
    else if (node.return_parameters->size () == 1)
      {
        node.eval.type = node.return_parameters->at (0)->type;
        node.eval.kind = ExpressionValue::Value;
        node.eval.intrinsic_mutability = Immutable;
        node.eval.indirection_mutability = node.return_parameters->at (0)->indirection_mutability;
        node.eval.fix_string_indirection_mutability ();
      }
    else
      {
        UNIMPLEMENTED;
      }
  }

  void conversion (Node& node,
                   Node* type_expr,
                   Node* expr)
  {
    if (type_expr->eval.is_error () || expr->eval.is_error ())
      {
        node.eval = ExpressionValue::make_error ();
        return;
      }

    if (!type_expr->eval.is_type ())
      {
        er.expected_a_type (type_expr->location);
        node.eval = ExpressionValue::make_error ();
        return;
      }

    if (!expr->eval.is_rvalue ())
      {
        er.expected_an_rvalue (expr->location);
        node.eval = ExpressionValue::make_error ();
        return;
      }

    const type::Type* to = type_expr->eval.type;

    if (expr->eval.is_constant ())
      {
        if (expr->eval.representable (to))
          {
            UNIMPLEMENTED;
          }
        else if (expr->eval.type->is_typed_float () && to->is_typed_float ())
          {
            UNIMPLEMENTED;
          }
        else if (expr->eval.type->is_typed_integer () && to->is_any_string ())
          {
            UNIMPLEMENTED;
            node.reset_mutability = true;
          }
        else if (expr->eval.type->is_any_string () && to->is_slice_of_bytes ())
          {
            if (expr->eval.type->is_untyped ())
              {
                expr->eval.convert (expr->eval.type->default_type ());
              }
            node.reset_mutability = true;
          }
        else
          {
            error_at_line (-1, 0, node.location.file ()->path ().c_str (),
                           node.location.line (),
                           "illegal conversion (E156)");
          }
      }
    else
      {
        if (expr->eval.is_assignable (to))
          {
            // Okay.
          }
        else if (are_identical (expr->eval.type->underlying_type (), to->underlying_type ()))
          {
            // Okay.
          }
        else if (expr->eval.type->to_pointer () &&
                 to->to_pointer () &&
                 are_identical (expr->eval.type->to_pointer (), to->to_pointer ()))
          {
            // Okay.
          }
        else if ((expr->eval.type->is_typed_integer () || expr->eval.type->is_typed_float ()) &&
                 (to->is_typed_integer () || to->is_typed_float ()))
          {
            // Okay.
          }
        else if (expr->eval.type->is_typed_complex () && to->is_typed_complex ())
          {
            // Okay.
          }
        else if ((expr->eval.type->is_typed_integer () || expr->eval.type->is_slice_of_bytes () || expr->eval.type->is_slice_of_runes ()) && to->is_typed_string ())
          {
            // Okay.
            node.reset_mutability = true;
          }
        else if (expr->eval.type->is_typed_string () && (to->is_slice_of_bytes () || to->is_slice_of_runes ()))
          {
            // Okay.
            node.reset_mutability = true;
          }
        else
          {
            error_at_line (-1, 0, node.location.file ()->path ().c_str (),
                           node.location.line (),
                           "illegal conversion (E122)");
          }
      }

    node.eval.type = to;
    node.eval.kind = ExpressionValue::Value;
    node.eval.intrinsic_mutability = Immutable;
    node.eval.indirection_mutability = expr->eval.indirection_mutability;
    if (node.reset_mutability)
      {
        node.eval.indirection_mutability = Mutable;
      }
    node.eval.fix_string_indirection_mutability ();
  }

  void visit (Conversion& node)
  {
    node.visit_children (*this);
    conversion (node, node.type, node.argument);
  }

  void visit (ExpressionList& node)
  {
    node.visit_children (*this);
  }

  void visit (Literal& node)
  {
    assert (node.eval.is_constant () || node.eval.is_polymorphic_function ());
    node.eval.intrinsic_mutability = Immutable;
    node.eval.indirection_mutability = Immutable;
  }

  void visit (IdentifierExpression& node)
  {
    node.symbol = symbol_table.retrieve_symbol (node.identifier);
    if (node.symbol == NULL)
      {
        er.not_declared (node.identifier);
        node.eval.kind = ExpressionValue::Error;
        return;
      }

    if (!node.symbol->process_declaration (er, symbol_table))
      {
        // process_declaration reports error.
        node.eval.kind = ExpressionValue::Error;
        return;
      }

    struct visitor : public ConstSymbolVisitor
    {
      IdentifierExpression& node;
      ErrorReporter& er;

      visitor (IdentifierExpression& n,
               ErrorReporter& a_er)
        : node (n)
        , er (a_er)
      { }

      void default_action (const Symbol& symbol)
      {
        NOT_REACHED;
      }

      void visit (const decl::PolymorphicFunction& symbol)
      {
        node.eval.kind = ExpressionValue::Polymorphic_Function;
        node.eval.polymorphic_function = &symbol;
        node.eval.intrinsic_mutability = Immutable;
        node.eval.indirection_mutability = Immutable;
      }

      void visit (const decl::FunctionBase& symbol)
      {
        node.callable = &symbol;
        node.eval.type = symbol.type;
        node.eval.kind = ExpressionValue::Value;
        node.eval.intrinsic_mutability = Immutable;
        node.eval.indirection_mutability = Immutable;
      }

      void visit (const Parameter& symbol)
      {
        node.eval.type = symbol.type;
        node.eval.kind = ExpressionValue::Variable;
        node.eval.intrinsic_mutability = symbol.intrinsic_mutability;
        node.eval.indirection_mutability = symbol.indirection_mutability;
      }

      void visit (const NamedType& symbol)
      {
        node.eval.type = &symbol;
        node.eval.kind = ExpressionValue::Type;
      }

      void visit (const Constant& symbol)
      {
        node.eval = symbol.value ();
        assert (node.eval.is_constant ());
        node.eval.intrinsic_mutability = Immutable;
        node.eval.indirection_mutability = Mutable;
      }

      void visit (const Variable& symbol)
      {
        node.eval.type = symbol.type;
        node.eval.kind = ExpressionValue::Variable;
        node.eval.intrinsic_mutability = symbol.intrinsic_mutability;
        node.eval.indirection_mutability = symbol.indirection_mutability;
      }

      void visit (const decl::Hidden& symbol)
      {
        er.hidden_symbol (symbol.identifier.location (), symbol.identifier.identifier ());
        node.eval.kind = ExpressionValue::Error;
      }
    };
    visitor v (node, er);
    node.symbol->accept (v);
    node.eval.fix_string_indirection_mutability ();
  }

  void visit (ast::TypeDecl& node)
  {
    // Do nothing.
  }

  void visit (ast::InstanceDecl& node)
  {
    // Check the arguments.
    node.arguments->accept (*this);
    check_types_arguments (node.arguments, node.symbol->initializer ()->type->parameter_list);
    require_value_or_const_or_variable_list (node.arguments);
    check_mutability_arguments (node.arguments, node.symbol->initializer ()->type->parameter_list);
  }

  void visit (ast::InitializerDecl& node)
  {
    symbol_table.open_scope (node.initializer->parameter_list (),
                             node.initializer->return_parameter_list ());
    Visitor v (*this);
    v.receiver_parameter = node.initializer->type->receiver_parameter;
    v.context = Initializer;
    node.body->accept (v);
    symbol_table.close_scope ();
  }

  void visit (ast::GetterDecl& node)
  {
    symbol_table.open_scope (node.getter->parameter_list (),
                             node.getter->return_parameter_list ());
    Visitor v (*this);
    v.receiver_parameter = node.getter->type->receiver_parameter;
    v.context = Getter;
    node.body->accept (v);
    symbol_table.close_scope ();
  }

  void visit (ast::ActionDecl& node)
  {
    symbol_table.open_scope ();

    if (node.action->dimension () != -1)
      {
        symbol_table.enter_symbol (node.action->iota_parameter ());
      }
    symbol_table.enter_symbol (node.action->receiver_parameter ());
    Visitor v (*this);
    v.receiver_parameter = node.action->receiver_parameter ();
    v.context = Action;
    node.precondition->accept (v);
    check_condition (node.precondition);
    node.body->accept (v);

    if (node.precondition->eval.is_constant ())
      {
        if (node.precondition->eval.value.bool_value)
          {
            node.action->precondition_kind = decl::Action::Static_True;
          }
        else
          {
            node.action->precondition_kind = decl::Action::Static_False;
          }
      }
    symbol_table.close_scope ();
  }

  void visit (ast::ReactionDecl& node)
  {
    if (node.reaction->dimension () != -1)
      {
        symbol_table.open_scope (node.reaction->iota (),
                                 node.reaction->parameter_list (),
                                 node.reaction->return_parameter_list ());
      }
    else
      {
        symbol_table.open_scope (node.reaction->parameter_list (),
                                 node.reaction->return_parameter_list ());
      }
    // No return type.
    Visitor v (*this);
    v.receiver_parameter = node.reaction->type->receiver_parameter;
    v.context = Reaction;
    node.body->accept (v);
    symbol_table.close_scope ();
  }

  void visit (ast::BinderDecl& node)
  {
    symbol_table.open_scope ();
    symbol_table.enter_symbol (node.bind->receiver_parameter ());
    node.body->accept (*this);
    symbol_table.close_scope ();
  }

  void visit (ast::FunctionDecl& node)
  {
    symbol_table.open_scope (node.symbol->parameter_list (),
                             node.symbol->return_parameter_list ());
    node.body->accept (*this);
    symbol_table.close_scope ();
  }

  void visit (ast::MethodDecl& node)
  {
    symbol_table.open_scope (node.method->parameter_list (),
                             node.method->return_parameter_list ());
    Visitor v (*this);
    v.receiver_parameter = node.method->type->receiver_parameter;
    node.body->accept (v);
    symbol_table.close_scope ();
  }

  void visit (StatementList& node)
  {
    symbol_table.open_scope ();
    node.visit_children (*this);
    symbol_table.close_scope ();
  }

  void visit (ExpressionStatement& node)
  {
    node.visit_children (*this);
    require_rvalue_or_void (node.child);
  }

  void visit (Return& node)
  {
    // Check the expression.
    node.visit_children (*this);

    // Get the return symbol.
    const decl::ParameterList* return_parameter_list =
      symbol_table.return_parameter_list ();

    if (return_parameter_list == NULL)
      {
        UNIMPLEMENTED;
      }

    if (return_parameter_list->size () != 1)
      {
        UNIMPLEMENTED;
      }

    node.return_symbol = return_parameter_list->at (0);

    if (!node.child->eval.is_assignable (node.return_symbol->type))
      {
        UNIMPLEMENTED;
        // error_at_line (-1, 0, node.location.file.c_str (),
        //                node.location.line, "cannot convert %s to %s in return (E160)",
        //                node.child->eval.type->to_error_string ().c_str (), node.return_symbol->type->to_error_string ().c_str ());
      }
    node.child->eval.convert (node.return_symbol->type);

    require_rvalue (node.child);

    if (node.child->eval.type->contains_pointer () &&
        node.return_symbol->indirection_mutability < node.child->eval.indirection_mutability)
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "return casts away +const or +foreign (E149)");
      }
  }

  void visit (If& node)
  {
    symbol_table.open_scope ();
    node.visit_children (*this);
    check_condition (node.condition);
    symbol_table.close_scope ();
  }

  void visit (While& node)
  {
    symbol_table.open_scope ();
    node.visit_children (*this);
    check_condition (node.condition);
    symbol_table.close_scope ();
  }

  void visit (ForIota& node)
  {
    // TODO:  Check for error.
    node.limit_value = process_array_dimension (er, symbol_table, node.limit);
    node.symbol = new Variable (node.identifier, /*Int::instance (),*/ Immutable, Immutable);
    symbol_table.open_scope ();
    symbol_table.enter_symbol (node.symbol);
    node.body->accept (*this);
    symbol_table.close_scope ();
  }

  void visit (Change& node)
  {
    node.argument->accept (*this);

    const type::Type* root_type = node.argument->eval.type->merge_change ();
    if (root_type == NULL)
      {
        UNIMPLEMENTED;
        // error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
        //                "cannot change expression of type %s (E96)", node.argument->eval.type->to_error_string ().c_str ());
      }

    require_rvalue (node.argument);

    // Enter all parameters and variables in scope that are pointers as pointers to foreign.
    symbol_table.open_scope ();
    symbol_table.change ();

    // Enter the new heap root.
    // Don't know dereference mutability yet.
    node.root_symbol = new Variable (node.identifier, /*root_type,*/ Immutable, Foreign);
    node.root_symbol->indirection_mutability = node.argument->eval.indirection_mutability;
    symbol_table.enter_symbol (node.root_symbol);

    // Check the body.
    node.body->accept (*this);
    symbol_table.close_scope ();
  }

  void visit (Activate& node)
  {
    if (!(context == Action ||
          context == Reaction))
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "activation outside of action or reaction (E53)");
      }

    if (in_mutable_phase)
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "activations within activations are not allowed (E54)");
      }

    // Check the activations.
    node.arguments->accept (*this);

    // Re-insert this as a pointer to mutable.
    symbol_table.open_scope ();
    symbol_table.activate ();
    // Check the body.
    Visitor v (*this);
    v.in_mutable_phase = true;
    node.body->accept (v);
    symbol_table.close_scope ();

    node.in_action = context == Action;
  }

  void visit (ConstDecl& node)
  {
    if (node.symbols.empty ())
      {
        // Not at top level so process.
        populate_package_block (&node, er, symbol_table);
        process_top_level_declarations (&node, er, symbol_table);
      }
  }

  void visit (EmptyStatement& node)
  {
    // Do nothing.
  }

  void visit (VarDecl& node)
  {
    ast::IdentifierList* identifier_list = node.identifiers;
    ast::Node* type_spec = node.type;
    ast::List* expression_list = node.expressions;

    if (expression_list->size () != 0 &&
        identifier_list->size () != expression_list->size ())
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "wrong number of initializers (E184)");
      }

    // Process the type spec.
    process_type (type_spec, er, symbol_table);
    if (type_spec->eval.kind == ExpressionValue::Error)
      {
        // TODO:  Check for failure.
        UNIMPLEMENTED;
      }

    if (expression_list->size () == 0)
      {
        // Type, no expressions.
        if (type_spec->eval.kind == ExpressionValue::EmptyType)
          {
            error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                           "missing type (E183)");

          }

        //const type::Type* type = type_spec->eval.type;

        // Enter each symbol.
        for (IdentifierList::const_iterator id_pos = identifier_list->begin (),
             id_limit = identifier_list->end ();
             id_pos != id_limit;
             ++id_pos)
          {
            Variable* symbol = new Variable (*id_pos, /*type,*/ node.mutability, node.indirection_mutability);
            symbol_table.enter_symbol (symbol);
            node.symbols.push_back (symbol);
          }

        return;
      }

    if (type_spec->eval.kind == ExpressionValue::Type)
      {
        // Type, expressions.
        const type::Type* type = type_spec->eval.type;

        // Enter each symbol.
        ast::List::ConstIterator init_pos;
        IdentifierList::const_iterator id_pos, id_limit;
        for (id_pos = identifier_list->begin (),
             id_limit = identifier_list->end (),
             init_pos = expression_list->begin ();
             id_pos != id_limit;
             ++id_pos, ++init_pos)
          {
            Node* n = *init_pos;
            n->accept (*this);

            if (!n->eval.is_assignable (type))
              {
                UNIMPLEMENTED;
                // error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                //                "cannot assign %s to %s in initialization (E62)", n->eval.type->to_error_string ().c_str (), type->to_error_string ().c_str ());
              }
            n->eval.convert (type);
            require_rvalue (n);

            Variable* symbol = new Variable (*id_pos, /*type,*/ node.mutability, node.indirection_mutability);
            symbol_table.enter_symbol (symbol);
            node.symbols.push_back (symbol);
          }

        goto done;
      }

    // No type, expressions.

    // Enter each symbol.
    {
      IdentifierList::const_iterator id_pos, id_limit;
      ast::List::ConstIterator init_pos;
      for (id_pos = identifier_list->begin (),
           id_limit = identifier_list->end (),
           init_pos = expression_list->begin ();
           id_pos != id_limit;
           ++id_pos, ++init_pos)
        {
          Node* n = *init_pos;
          n->accept (*this);

          if (n->eval.type->is_untyped ())
            {
              n->eval.convert (n->eval.type->default_type ());
            }
          require_rvalue (n);

          Variable* symbol = new Variable (*id_pos, /*n->eval.type,*/ node.mutability, node.indirection_mutability);
          symbol_table.enter_symbol (symbol);
          node.symbols.push_back (symbol);
        }
    }
done:
    if (!node.expressions->empty ())
      {
        size_t idx = 0;
        for (List::ConstIterator pos = node.expressions->begin (), limit = node.expressions->end ();
             pos != limit;
             ++pos, ++idx)
          {
            Node* n = *pos;
            Variable* symbol = node.symbols[idx];

            if (n->eval.type->contains_pointer () &&
                symbol->indirection_mutability < n->eval.indirection_mutability)
              {
                error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                               "assignment casts away +const or +foreign (E92)");
              }
          }
      }
  }

  void visit (Assign& node)
  {
    node.visit_children (*this);
    const type::Type* to = node.left->eval.type;
    //const type::Type*& from = node.right->eval.type;
    if (!node.right->eval.is_assignable (to))
      {
        UNIMPLEMENTED;
        // error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
        //                "cannot assign value of type %s to variable of type %s (E199)",
        //                from->to_error_string ().c_str (),
        //                to->to_error_string ().c_str ());
      }
    node.right->eval.convert (to);
    require_variable (node.left);
    require_rvalue (node.right);

    if (node.left->eval.intrinsic_mutability != Mutable)
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "target of assignment is not mutable (E86)");
      }

    if (node.right->eval.type->contains_pointer () &&
        node.left->eval.indirection_mutability < node.right->eval.indirection_mutability)
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "assignment casts away +const or +foreign (E161)");
      }
  }

  void visit (AddAssign& node)
  {
    node.visit_children (*this);

    const type::Type* to = node.left->eval.type;
    //const type::Type*& from = node.right->eval.type;
    if (!to->is_arithmetic ())
      {
        UNIMPLEMENTED;
        // error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
        //                "+= cannot be applied to %s (E200)",
        //                to->to_error_string ().c_str ());
      }
    if (!node.right->eval.is_assignable (to))
      {
        UNIMPLEMENTED;
        // error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
        //                "cannot assign value of type %s to variable of type %s (E76)",
        //                from->to_error_string ().c_str (),
        //                to->to_error_string ().c_str ());
      }
    node.right->eval.convert (to);
    require_variable (node.left);
    require_rvalue (node.right);

    if (node.left->eval.intrinsic_mutability != Mutable)
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "target of assignment is not mutable (E15)");
      }
  }

  void visit (IncrementDecrement& node)
  {
    const char* op = "";
    switch (node.kind)
      {
      case IncrementDecrement::Increment:
        op = "++";
        break;
      case IncrementDecrement::Decrement:
        op = "--";
        break;
      }

    node.visit_children (*this);
    if (!node.child->eval.type->is_arithmetic ())
      {
        UNIMPLEMENTED;
        // error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
        //                "%s cannot be applied to %s (E77)",
        //                op, node.child->eval.type->to_error_string ().c_str ());
      }
    require_variable (node.child);

    if (node.child->eval.intrinsic_mutability != Mutable)
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "target of %s is not mutable (E177)", op);
      }
  }

  void visit (BindPushPort& node)
  {
    node.visit_children (*this);
    bind (node, node.left, node.right);
  }

  void visit (BindPushPortParameter& node)
  {
    node.visit_children (*this);
    const decl::Reaction* reaction = bind (node, node.left, node.right);
    long dimension = reaction->dimension ();
    if (dimension == -1)
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "parameter specified for non-parameterized reaction (E41)");
      }

    check_array_index (reaction->type->get_array (dimension), node.param, false);
  }

  void visit (BindPullPort& node)
  {
    node.visit_children (*this);

    const type::PullPort* pull_port_type = node.left->eval.type->to_pull_port ();

    if (pull_port_type == NULL)
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "target of bind is not a pull port (E193)");
      }

    require_variable (node.left);

    const type::Getter* getter_type = node.right->eval.type->to_getter ();

    if (getter_type == NULL)
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "source of bind is not a getter (E192)");
      }

    require_variable (node.right);

    type::PullPort g (getter_type->parameter_list, getter_type->return_parameter_list);
    if (!are_identical (pull_port_type, &g))
      {
        UNIMPLEMENTED;
        // error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
        //                "cannot bind %s to %s (E191)", pull_port_type->to_error_string ().c_str (), getter_type->to_error_string ().c_str ());
      }
  }

  void visit (Dereference& node)
  {
    node.visit_children (*this);
    const type::Type* t = node.child->eval.type;
    const type::Pointer* p = t->underlying_type ()->to_pointer ();
    if (p == NULL)
      {
        UNIMPLEMENTED;
        // error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
        //                "* cannot be applied to %s (E21)", t->to_error_string ().c_str ());
      }
    require_rvalue (node.child);
    node.eval.type = p->base_type;
    node.eval.kind = ExpressionValue::Variable;
    node.eval.intrinsic_mutability = node.child->eval.indirection_mutability;
    node.eval.indirection_mutability = node.child->eval.indirection_mutability;
    node.eval.fix_string_indirection_mutability ();
  }

  void visit (AddressOf& node)
  {
    node.visit_children (*this);
    require_variable (node.child);
    node.eval.type = node.child->eval.type->get_pointer ();
    node.eval.kind = ExpressionValue::Value;
    node.eval.intrinsic_mutability = node.child->eval.intrinsic_mutability;
    node.eval.indirection_mutability = std::max (node.child->eval.intrinsic_mutability, node.child->eval.indirection_mutability);
    node.eval.fix_string_indirection_mutability ();
  }

  void visit (Select& node)
  {
    node.base->accept (*this);
    const type::Type* base_type = node.base->eval.type;
    require_rvalue (node.base);

    node.field = base_type->find_field (node.identifier.identifier ());
    node.callable = base_type->find_callable (node.identifier.identifier ());
    if (node.field != NULL)
      {
        node.eval.type = node.field->type;
      }
    else if (node.callable != NULL)
      {
        node.eval.type = node.callable->callable_type ();
      }
    else
      {
        UNIMPLEMENTED;
        // error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
        //                "cannot select %s from expression of type %s (E154)",
        //                identifier.c_str (), base_type->to_error_string ().c_str ());
      }

    if (base_type->underlying_type ()->to_pointer ())
      {
        // Selecting through pointer always yields a variable.
        node.eval.kind = ExpressionValue::Variable;
        node.eval.intrinsic_mutability = node.base->eval.indirection_mutability;
        node.eval.indirection_mutability = node.base->eval.indirection_mutability;
        node.eval.fix_string_indirection_mutability ();
      }
    else
      {
        // Otherwise, use the base kind.
        node.eval.kind = node.base->eval.kind;
        node.eval.intrinsic_mutability = node.base->eval.intrinsic_mutability;
        node.eval.indirection_mutability = node.base->eval.indirection_mutability;
        node.eval.fix_string_indirection_mutability ();
      }
  }

  void check_array_index (const type::Array* array_type, Node* index, bool allow_equal)
  {
    const type::Type*& index_type = index->eval.type;
    Value& index_value = index->eval.value;

    if (index_type->is_untyped_numeric ())
      {
        if (!index->eval.representable (Int::instance ()))
          {
            error_at_line (-1, 0, index->location.file ()->path ().c_str (), index->location.line (),
                           "array index is not an integer (E20)");
          }
        index->eval.convert (Int::instance ());
        index_type = Int::instance ();
        long idx = index_value.int_value;
        if (idx < 0)
          {
            error_at_line (-1, 0, index->location.file ()->path ().c_str (), index->location.line (),
                           "array index is negative (E162)");
          }
        if (array_type)
          {
            if ((!allow_equal && idx >= array_type->dimension) ||
                (allow_equal && idx > array_type->dimension))
              {
                error_at_line (-1, 0, index->location.file ()->path ().c_str (), index->location.line (),
                               "array index is out of bounds (E163)");
              }
          }
      }
    else if (index_type->is_typed_integer ())
      {
        if (index->eval.is_constant ())
          {
            long idx = index->eval.to_int ();
            if (idx < 0)
              {
                error_at_line (-1, 0, index->location.file ()->path ().c_str (), index->location.line (),
                               "array index is negative (E164)");
              }
            if (array_type)
              {
                if (idx >= array_type->dimension)
                  {
                    error_at_line (-1, 0, index->location.file ()->path ().c_str (), index->location.line (),
                                   "array index is out of bounds (E165)");
                  }
              }
          }
      }
    else
      {
        error_at_line (-1, 0, index->location.file ()->path ().c_str (), index->location.line (),
                       "array index is not an integer (E203)");
      }
    require_rvalue (index);
  }

  void visit (Index& node)
  {
    node.visit_children (*this);
    const type::Type* base_type = node.base->eval.type;
    Node* index_node = node.index;
    Value& index_value = index_node->eval.value;
    const type::Type*& index_type = node.index->eval.type;

    node.array_type = base_type->underlying_type ()->to_array ();
    if (node.array_type != NULL)
      {
        check_array_index (node.array_type, node.index, false);
        require_rvalue (node.base);
        node.eval.type = node.array_type->base_type;
        node.eval.kind = node.base->eval.kind;
        node.eval.intrinsic_mutability = node.base->eval.intrinsic_mutability;
        node.eval.indirection_mutability = node.base->eval.indirection_mutability;
        node.eval.fix_string_indirection_mutability ();
        return;
      }

    node.slice_type = base_type->underlying_type ()->to_slice ();
    if (node.slice_type != NULL)
      {
        if (index_type->is_untyped_numeric ())
          {
            if (!index_node->eval.representable (Int::instance ()))
              {
                error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                               "slice index is not an integer (E204)");
              }
            index_node->eval.convert (Int::instance ());
            index_type = Int::instance ();
            long idx = index_value.int_value;
            if (idx < 0)
              {
                error_at_line (-1, 0, index_node->location.file ()->path ().c_str (), index_node->location.line (),
                               "slice index is negative (E166)");
              }
          }
        else if (index_type->is_typed_integer ())
          {
            if (index_node->eval.is_constant ())
              {
                long idx = index_node->eval.to_int ();
                if (idx < 0)
                  {
                    error_at_line (-1, 0, index_node->location.file ()->path ().c_str (), index_node->location.line (),
                                   "slice index is negative (E167)");
                  }
              }
          }
        else
          {
            error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                           "slice index is not an integer (E205)");
          }

        require_rvalue (node.base);
        require_rvalue (node.index);

        node.eval.type = node.slice_type->base_type;
        node.eval.kind = ExpressionValue::Variable;
        node.eval.intrinsic_mutability = node.base->eval.intrinsic_mutability;
        node.eval.indirection_mutability = node.base->eval.indirection_mutability;
        node.eval.fix_string_indirection_mutability ();
        return;
      }

    UNIMPLEMENTED;
    // error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
    //                "cannot index expression of type %s (E168)",
    //                base_type->to_error_string ().c_str ());
  }

  void visit (IndexSlice& node)
  {
    Node* base = node.base;
    base->accept (*this);
    require_rvalue (base);
    const type::Type* base_type = node.base->eval.type;

    Node* low_node = node.low;
    if (node.low_present)
      {
        low_node->accept (*this);
      }

    Node* high_node = node.high;
    if (node.high_present)
      {
        high_node->accept (*this);
      }

    Node* max_node = node.max;
    if (node.max_present)
      {
        max_node->accept (*this);
      }

    if (base_type->underlying_type ()->kind () == Untyped_String_Kind)
      {
        UNIMPLEMENTED;
        return;
      }

    node.pointer_to_array_type = base_type->pointer_to_array ();
    if (node.pointer_to_array_type)
      {
        UNIMPLEMENTED;
        return;
      }

    node.array_type = base_type->underlying_type ()->to_array ();
    if (node.array_type != NULL)
      {
        if (node.low_present)
          {
            check_array_index (node.array_type, low_node, true);
          }
        if (node.high_present)
          {
            check_array_index (node.array_type, high_node, true);
          }
        if (node.max_present)
          {
            check_array_index (node.array_type, max_node, true);
          }

        node.eval.type = node.array_type->base_type->get_slice ();
        node.eval.kind = ExpressionValue::Value;
        node.eval.intrinsic_mutability = Immutable;
        node.eval.indirection_mutability = node.base->eval.indirection_mutability;
        node.eval.fix_string_indirection_mutability ();
        goto done;
      }

    node.slice_type = base_type->underlying_type ()->to_slice ();
    if (node.slice_type != NULL)
      {
        if (node.low_present)
          {
            check_array_index (NULL, low_node, true);
          }
        if (node.high_present)
          {
            check_array_index (NULL, high_node, true);
          }
        if (node.max_present)
          {
            check_array_index (NULL, max_node, true);
          }

        node.eval.type = node.slice_type;
        node.eval.kind = ExpressionValue::Value;
        node.eval.intrinsic_mutability = Immutable;
        node.eval.indirection_mutability = node.base->eval.indirection_mutability;
        node.eval.fix_string_indirection_mutability ();
        goto done;
      }

    UNIMPLEMENTED;
    // error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
    //                "cannot slice expression of type %s (E225)",
    //                base_type->to_error_string ().c_str ());

done:
    if (low_node->eval.is_constant () &&
        high_node->eval.is_constant ())
      {
        if (!(low_node->eval.to_int () <= high_node->eval.to_int ()))
          {
            error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                           "slice indices are out of range (E224)");
          }
      }

    if (low_node->eval.is_constant () &&
        max_node->eval.is_constant ())
      {
        if (!(low_node->eval.to_int () <= max_node->eval.to_int ()))
          {
            error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                           "slice indices are out of range (E6)");
          }
      }

    if (high_node->eval.is_constant () &&
        max_node->eval.is_constant ())
      {
        if (!(high_node->eval.to_int () <= max_node->eval.to_int ()))
          {
            error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                           "slice indices are out of range (E7)");
          }
      }
  }

  void visit (TypeExpression& node)
  {
    process_type (node.child, er, symbol_table);
    node.eval.kind = node.child->eval.kind;
    node.eval.type = node.child->eval.type;
  }

  void visit (PushPortCall& node)
  {
    node.receiver_parameter = receiver_parameter;
    const std::string& port_identifier = node.identifier.identifier ();
    const type::Type* this_type = receiver_parameter->type;
    node.field = this_type->find_field (port_identifier);
    if (node.field == NULL)
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "no port named %s (E194)", port_identifier.c_str ());
      }
    node.push_port_type = node.field->type->to_push_port ();
    if (node.push_port_type == NULL)
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "no port named %s (E195)", port_identifier.c_str ());
      }

    node.arguments->accept (*this);
    check_types_arguments (node.arguments, node.push_port_type->parameter_list);
    require_value_or_const_or_variable_list (node.arguments);

    node.eval.kind = ExpressionValue::Void;
    node.eval.intrinsic_mutability = Immutable;
    node.eval.indirection_mutability = Immutable;
  }

  void visit (IndexedPushPortCall& node)
  {
    node.receiver_parameter = receiver_parameter;
    const std::string& port_identifier = node.identifier.identifier ();
    const type::Type* this_type = receiver_parameter->type;
    node.field = this_type->find_field (port_identifier);
    if (node.field == NULL)
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "no port named %s (E74)", port_identifier.c_str ());
      }
    node.array_type = node.field->type->to_array ();
    if (node.array_type == NULL)
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "%s is not an array of ports (E16)", port_identifier.c_str ());
      }
    node.push_port_type = node.array_type->base_type->to_push_port ();
    if (node.push_port_type == NULL)
      {
        error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
                       "%s is not an array of ports (E17)", port_identifier.c_str ());
      }

    node.index->accept (*this);
    check_array_index (node.array_type, node.index, false);

    node.arguments->accept (*this);
    check_types_arguments (node.arguments, node.push_port_type->parameter_list);
    require_value_or_const_or_variable_list (node.arguments);

    node.eval.kind = ExpressionValue::Void;
    node.eval.intrinsic_mutability = Immutable;
    node.eval.indirection_mutability = Immutable;
  }

  void visit (CompositeLiteral& node)
  {
    // TODO:  Check for failure.
    node.eval.type = NULL;
    process_type (node.type, er, symbol_table);
    assert (false);
    node.eval.kind = ExpressionValue::Variable;

    switch (node.eval.type->underlying_kind ())
      {
      case Struct_Kind:
      {
        for (List::ConstIterator pos = node.value->begin (),
             limit = node.value->end ();
             pos != limit;
             ++pos)
          {
            //Node* element = *pos;
            UNIMPLEMENTED;
          }
      }
      break;

      case Array_Kind:
        UNIMPLEMENTED;
        break;

      case Slice_Kind:
        UNIMPLEMENTED;
        break;

      default:
        UNIMPLEMENTED;
        // error_at_line (-1, 0, node.location.file ()->path ().c_str (), node.location.line (),
        //                "cannot define composite literals for %s (E5)", node.eval.type->to_error_string ().c_str ());
      }
  }
};
}

void check_types_arguments (ast::List* args, const decl::ParameterList* signature)
{
  if (args->size () != signature->size ())
    {
      error_at_line (-1, 0, args->location.file ()->path ().c_str (), args->location.line (),
                     "call expects %lu arguments but given %lu (E150)", signature->size (), args->size ());
    }

  size_t i = 0;
  for (List::ConstIterator pos = args->begin (), limit = args->end ();
       pos != limit;
       ++pos, ++i)
    {
      //const type::Type*& arg = (*pos)->eval.type;
      const type::Type* param = signature->at (i)->type;
      if (!(*pos)->eval.is_assignable (param))
        {
          UNIMPLEMENTED;
          // error_at_line (-1, 0, (*pos)->location.file.c_str (), (*pos)->location.line,
          //                "cannot assign %s to %s in call (E151)", arg->to_error_string ().c_str (), param->to_error_string ().c_str ());
        }
      (*pos)->eval.convert (param);
    }
}

void require_value_or_const_or_variable_list (const List* node)
{
  for (List::ConstIterator pos = node->begin (), limit = node->end ();
       pos != limit;
       ++pos)
    {
      require_rvalue (*pos);
    }
}

void check_all (ast::Node* root, ErrorReporter& er, SymbolTable& symbol_table)
{
  Visitor visitor (er, symbol_table);
  root->accept (visitor);
}

bool check_constant_expression (ast::Node* root, util::ErrorReporter& er, decl::SymbolTable& symbol_table)
{
  check_all (root, er, symbol_table);
  if (!root->eval.is_constant ())
    {
      er.expression_is_not_constant (root->location);
      return false;
    }
  return true;
}

void check_mutability_arguments (ast::Node* node, const decl::ParameterList* signature)
{
  ExpressionList* args = node_cast<ExpressionList> (node);

  size_t i = 0;
  for (List::ConstIterator pos = args->begin (), limit = args->end ();
       pos != limit;
       ++pos, ++i)
    {
      const type::Type* arg = (*pos)->eval.type;
      if (arg->contains_pointer ())
        {
          if (signature->at (i)->indirection_mutability < (*pos)->eval.indirection_mutability)
            {
              error_at_line (-1, 0, (*pos)->location.file ()->path ().c_str (), (*pos)->location.line (),
                             "argument %zd casts away +const or +foreign (E85)", i + 1);
            }
        }
    }
}

}
