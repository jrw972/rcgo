#ifndef SRC_VALUE_H_
#define SRC_VALUE_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <stdint.h>

#include <string>
#include <vector>

#include "src/error.h"
#include "src/location.h"
#include "src/untyped_constant.h"
#include "src/typed_constant.h"

namespace rcgo {

// Forward.
namespace type {
struct Type;
struct Function;
}

namespace value {

struct Value {
  enum Kind {
    kUninitialized,
    kError,

    kFunction,  // TODO(jrw72): May be able to use kRValue instead.
    kLValue,
    kType,
    kVoid,  // TODO(jrw72): May be able to use empty list instead.
    kList,

    kUntypedConstant,  // ordered for rvalue
    kTypedConstant,    // ordered for rvalue
    kRValue,           // ordered for rvalue
  };

  Value();

  static Value MakeError();
  static Value MakeUntypedConstant(UntypedConstant const & a_value);
  static Value MakeFunction(type::Function const* a_type);
  static Value MakeLValue(type::Type const* a_type);
  static Value MakeRValue(type::Type const* a_type);
  static Value MakeType(type::Type const* a_type);

  Kind kind() const { return m_kind; }
  type::Type const* type() const;
  UntypedConstant const & untyped_constant() const;

  // TODO(jrw972): Remove these.
  bool bool_value() const { return m_typed_constant.bool_value(); }
  std::string const & string_value() const { return m_typed_constant.string_value(); }
  complex64_t complex64_value() const { return m_typed_constant.complex64_value(); }
  complex128_t complex128_value() const { return m_typed_constant.complex128_value(); }
  float float32_value() const { return m_typed_constant.float32_value(); }
  double float64_value() const { return m_typed_constant.float64_value(); }
  int int_value() const { return m_typed_constant.int_value(); }
  int8_t int8_value() const { return m_typed_constant.int8_value(); }
  int16_t int16_value() const { return m_typed_constant.int16_value(); }
  int32_t int32_value() const { return m_typed_constant.int32_value(); }
  int64_t int64_value() const { return m_typed_constant.int64_value(); }
  unsigned int uint_value() const { return m_typed_constant.uint_value(); }
  uint8_t uint8_value() const { return m_typed_constant.uint8_value(); }
  uint16_t uint16_value() const { return m_typed_constant.uint16_value(); }
  uint32_t uint32_value() const { return m_typed_constant.uint32_value(); }
  uint64_t uint64_value() const { return m_typed_constant.uint64_value(); }
  unsigned int uintptr_value() const { return m_typed_constant.uintptr_value(); }

  bool IsUninitialized() const;
  bool IsInitialized() const;
  bool IsError() const;

  // Operations.

  // Uninitialized Values may be passed in as arguments.  They will be
  // ignored when checking.

  // These functions return an initialized Value when successful or
  // partially successful and an uninitialized one otherwise.  These
  // functions also populate a list of errors if an error is
  // discovered.

  // Some of these functions have arguments that are pointers to
  // mutable objects so they can convert untyped constants to typed
  // constants when necessary.  If a conversion is necessary but
  // fails, the arguments are not changed and an error is reported.
  // The arguments are not changed for any other reason.

  static Value ConvertConstant(Value const & x, type::Type const* a_type);
  static Value Posate(Location const & location, Value* x,
                      ErrorList* error_list);
  static Value Negate(Location const & location, Value* x,
                      ErrorList* error_list);
  static Value LogicNot(Location const & location, Value* x,
                        ErrorList* error_list);
  static Value BitNot(Location const & location, Value* x,
                      ErrorList* error_list);
  static Value Add(Location const & location, Value* x, Value* y,
                   ErrorList* error_list);
  static Value Subtract(Location const & location, Value* x, Value* y,
                        ErrorList* error_list);
  static Value Multiply(Location const & location, Value* x, Value* y,
                        ErrorList* error_list);
  static Value Divide(Location const & location, Value* x, Value* y,
                      ErrorList* error_list);
  static Value Modulo(Location const & location, Value* x, Value* y,
                      ErrorList* error_list);
  static Value LeftShift(Value* x, Value* y, ErrorList* error_list);
  static Value RightShift(Value* x, Value* y, ErrorList* error_list);
  static Value BitAnd(Location const & location, Value* x, Value* y,
                      ErrorList* error_list);
  static Value BitAndNot(Location const & location, Value* x, Value* y,
                         ErrorList* error_list);
  static Value BitOr(Location const & location, Value* x, Value* y,
                     ErrorList* error_list);
  static Value BitXor(Location const & location, Value* x, Value* y,
                      ErrorList* error_list);
  static Value Equal(Location const & location, Value* x, Value* y,
                     ErrorList* error_list);

  static Value Call(Location const & location, Value* operand,
                    std::vector<Value*> const & arguments,
                    LocationList const & locations,
                    ErrorList* error_list);

  bool operator==(Value const & y) const;
  bool operator!=(Value const & y) const;

 private:
  explicit Value(Kind a_kind);
  bool IsArithmetic() const;
  bool IsComparable() const;
  bool IsSigned() const;
  bool IsInteger() const;
  bool IsIntegral() const;
  bool IsZero() const;
  bool IsString() const;
  bool IsBoolean() const;
  bool IsCallable() const;
  bool IsConstant() const;
  bool IsRValueish() const;
  static void IsAssignableFrom(Value* lhs, Value* rhs);

  // Explicit conversions and checks.
  void Dereference();
  static Value MakeTypedConstant(TypedConstant const & a_value);
  static void Promote(Value* a_x, Value* a_y);

  Kind m_kind;
  type::Type const* m_type;

  UntypedConstant m_untyped_constant;
  TypedConstant m_typed_constant;

  typedef std::vector<Value> ListType;
  ListType m_list;
};

std::ostream& operator<<(std::ostream & out, value::Value const & value);

Error CannotBeUsedInAnExpression(Value const* x);
Error IsNotAnInteger(Value const* x);
Error CannotConvert(Value const* x, type::Type const* type);

}  // namespace value
}  // namespace rcgo

#endif  // SRC_VALUE_H_
