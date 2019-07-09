// Copyright 201
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/value.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>

#include "src/type.h"
#include "src/utf8.h"

namespace rcgo {
namespace value {

namespace {

Error cannotApply(const Location& a_location, char a_operator,
                  value::Value const & a_value) {
  Error error(a_location);
  error.message << "error: cannot apply '" << a_operator << "' to " << a_value << std::endl;
  return error;
}

Error cannotApply2(const Location& a_location, const std::string& a_operator,
                   value::Value const & a_value1,
                   value::Value const & a_value2) {
  Error error(a_location);
  error.message << "error: cannot apply '" << a_operator << "' to " << a_value1 << " and " << a_value2 << std::endl;
  return error;
}

Error divisionByZero(const Location& a_location) {
  Error error(a_location);
  error.message << "error: division by zero" << std::endl;
  return error;
}

Error operandCannotBeCalled(const Location& a_location) {
  Error error(a_location);
  error.message << "error: operand cannot be called" << std::endl;
  return error;
}

Error callExpectsNArguments(
    const Location& a_location, size_t a_expected, size_t a_actual) {
  Error error(a_location);
  error.message << "error: call expects " << a_expected << " arguments but received " << a_actual << std::endl;
  return error;
}

Error cannotConvert(
    const Location& a_location, value::Value const & a_value,
    const type::Type* a_type) {
  Error error(a_location);
  error.message << "error: cannot convert " << a_value << " to " << a_type << std::endl;
  return error;
}

}  // namespace

Value::Value() : m_kind(kUninitialized), m_type(nullptr) {}

Value::Value(Kind a_kind) : m_kind(a_kind), m_type(nullptr) {}

Value Value::MakeUntypedConstant(UntypedConstant const & a_value) {
  assert(a_value.IsInitialized());
  Value v(kUntypedConstant);
  v.m_untyped_constant = a_value;
  return v;
}

Value Value::MakeFunction(const type::Function* a_type) {
  Value v(kFunction);
  v.m_type = a_type;
  return Value(v);
}

Value Value::MakeLValue(const type::Type* a_type) {
  Value v(kLValue);
  v.m_type = a_type;
  return Value(v);
}

Value Value::MakeRValue(const type::Type* a_type) {
  Value v(kRValue);
  v.m_type = a_type;
  return Value(v);
}

Value Value::MakeType(const type::Type* a_type) {
  Value v(kType);
  v.m_type = a_type;
  return Value(v);
}

type::Type const * Value::type() const {
  switch (m_kind) {
    case kFunction:
    case kLValue:
    case kType:
    case kRValue:
      return m_type;
    case kTypedConstant:
      return m_typed_constant.type();
    default:
      return nullptr;
  }
}

UntypedConstant const & Value::untyped_constant() const {
  assert(m_kind == kUntypedConstant);
  return m_untyped_constant;
}

bool Value::IsArithmetic() const {
  if (m_kind == kUntypedConstant) {
    return m_untyped_constant.IsArithmetic();
  }
  const type::Type* t = type();
  if (t) {
    return type::IsArithmetic(t);
  }
  return false;
}

bool Value::IsComparable() const {
  if (m_kind == kUntypedConstant) {
    return true;
  }
  const type::Type* t = type();
  if (t) {
    return type::IsComparable(t);
  }
  return false;
}

bool Value::IsSigned() const {
  if (m_kind == kUntypedConstant) {
    return m_untyped_constant.IsSigned();
  }
  const type::Type* t = type();
  if (t) {
    return type::IsSigned(t);
  }
  return false;
}

bool Value::IsInteger() const {
  if (m_kind == kUntypedConstant) {
    return m_untyped_constant.IsInteger();
  }
  const type::Type* t = type();
  if (t) {
    return type::IsInteger(t);
  }
  return false;
}

bool Value::IsIntegral() const {
  if (m_kind == kUntypedConstant) {
      return m_untyped_constant.ToInteger().IsInitialized();
  }
  const type::Type* t = type();
  if (t) {
    return type::IsInteger(t);
  }
  return false;
}

bool Value::IsZero() const {
  switch (m_kind) {
    case kUntypedConstant:
      return m_untyped_constant.IsZero();
    case kTypedConstant:
      return m_typed_constant.IsZero();
    default:
      return false;
  }
}

bool Value::IsString() const {
  if (m_kind == kUntypedConstant) {
    return m_untyped_constant.IsString();
  }
  const type::Type* t = type();
  if (t) {
    return type::IsString(t);
  }
  return false;
}

bool Value::IsBoolean() const {
  if (m_kind == kUntypedConstant) {
    return m_untyped_constant.IsBoolean();
  }
  const type::Type* t = type();
  if (t) {
    return type::IsBoolean(t);
  }
  return false;
}

bool Value::IsCallable() const {
  switch (m_kind) {
    case kFunction:
      return true;
    default:
      return false;
  }
}

bool Value::IsConstant() const {
  switch (m_kind) {
    case kUntypedConstant:
    case kTypedConstant:
      return true;
    default:
      return false;
  }
}

bool Value::IsRValueish() const {
  switch (m_kind) {
    case kUntypedConstant:
    case kTypedConstant:
    case kRValue:
      return true;
    default:
      return false;
  }
}

    void Value::Assign(Value const* lhs, Value* rhs, ErrorList* error_list) {
        bool left_okay = true;
        bool right_okay = true;
        if (lhs->IsInitialized()) {
            if (lhs->m_kind != kLValue) {
                error_list->push_back(NotAssignable(lhs));
                left_okay = false;
            }
        }

        if (rhs->IsInitialized()) {
            if (!rhs->IsRValueish()) {
                error_list->push_back(CannotBeUsedInAnExpression(rhs));
                right_okay = false;
            }
        }

        if (!left_okay || !right_okay) {
            return;
        }

        if (rhs->m_kind == kUntypedConstant) {
            TypedConstant tc = TypedConstant(lhs->type(), rhs->m_untyped_constant);
            if (tc.IsInitialized()) {
                *rhs = MakeTypedConstant(tc);
            } else {
                error_list->push_back(CannotConvert(rhs, lhs->type()));
                return;
            }
        }

        if (!type::Identical(lhs->type(), rhs->type())) {
            error_list->push_back(CannotAssign(lhs, rhs));
        }
    }

void Value::Dereference() {
  if (m_kind == kLValue) {
    m_kind = kRValue;
  }
}

Value Value::MakeTypedConstant(TypedConstant const & a_value) {
  Value v;
  v.m_kind = kTypedConstant;
  v.m_typed_constant = a_value;
  return v;
}

void Value::Promote(Value * a_x, Value * a_y) {
  assert(a_x->IsInitialized());
  assert(a_y->IsInitialized());
  assert(!a_x->IsError());
  assert(!a_y->IsError());
  assert(a_x->m_kind >= kUntypedConstant);
  assert(a_y->m_kind >= kUntypedConstant);

  if (a_x->m_kind == a_y->m_kind) { return; }

  if (a_x->m_kind > a_y->m_kind) {
    std::swap(a_x, a_y);
  }

  if (a_x->m_kind == kUntypedConstant && a_y->m_kind == kTypedConstant) {
    *a_x = MakeTypedConstant(TypedConstant(a_y->type(), a_x->m_untyped_constant));
  } else if (a_x->m_kind == kUntypedConstant && a_y->m_kind == kRValue) {
    *a_x = MakeTypedConstant(TypedConstant(a_y->type(), a_x->m_untyped_constant));
    if (a_x->IsError()) { return; }
    *a_x = MakeRValue(a_y->type());
  } else if (a_x->m_kind == kTypedConstant && a_y->m_kind == kRValue) {
    if (type::Identical(a_x->type(), a_y->type())) {
      *a_x = MakeRValue(a_y->type());
    } else {
      *a_x = MakeError();
    }
  } else {
    abort();
  }
}

bool Value::IsUninitialized() const {
  return m_kind == kUninitialized;
}

bool Value::IsInitialized() const {
  return m_kind != kUninitialized;
}

// TODO(jrw972):  Handle overflow and underflow.

Value Value::ConvertConstant(Value const & a_value,
                            type::Type const * a_type) {
  assert(a_value.IsInitialized());

  if (a_value.IsError()) {
    return a_value;
  }

  if (!a_value.IsConstant()) {
    return MakeError();
  }

  UntypedConstant uc;
  if (a_value.m_kind == kUntypedConstant) {
    uc = a_value.m_untyped_constant;
  } else {
    uc = a_value.m_typed_constant.ToUntypedConstant();
  }

  TypedConstant tc = TypedConstant(a_type, a_value.m_untyped_constant);
  if (!tc.IsInitialized()) {
    return MakeError();
  }
  return MakeTypedConstant(tc);
}

Value Value::Posate(
    Location const & location, Value * x,
    ErrorList* error_list) {
  assert(x->IsInitialized());

  if (x->IsError()) {
    return Value::MakeError();
  }

  if (!x->IsArithmetic()) {
    error_list->push_back(cannotApply(location, '+', *x));
    return Value::MakeError();
  }

  // TODO(jrw972):  Remove.
  x->Dereference();

  switch (x->m_kind) {
    case Value::kUntypedConstant:
      return MakeUntypedConstant(UntypedConstant::Posate(x->m_untyped_constant));
    case Value::kTypedConstant:
      return MakeTypedConstant(TypedConstant::Posate(x->m_typed_constant));
    case Value::kRValue:
      return MakeRValue(x->type());
    default:
      abort();  // NOT_COVERED
  }
}

Value Value::Negate(
    Location const & location, Value * x,
    ErrorList* error_list) {
  assert(x->IsInitialized());

  if (x->IsError()) {
    return Value::MakeError();
  }

  if (!x->IsArithmetic()) {
    error_list->push_back(cannotApply(location, '-', *x));
    return Value::MakeError();
  }

  // TODO(jrw972):  Remove.
  x->Dereference();

  switch (x->m_kind) {
    case Value::kUntypedConstant:
      return MakeUntypedConstant(UntypedConstant::Negate(x->m_untyped_constant));
    case Value::kTypedConstant:
      return MakeTypedConstant(TypedConstant::Negate(x->m_typed_constant));
    case Value::kRValue:
      return MakeRValue(x->type());
    default:
      abort();  // NOT_COVERED
  }
}

Value Value::LogicNot(
    Location const & location, Value * x,
    ErrorList* error_list) {
  assert(x->IsInitialized());

  if (x->IsError()) {
    return Value::MakeError();
  }

  if (!x->IsBoolean()) {
    error_list->push_back(cannotApply(location, '!', *x));
    return Value::MakeError();
  }

  // TODO(jrw972):  Remove.
  x->Dereference();

  switch (x->m_kind) {
    case Value::kUntypedConstant:
      return MakeUntypedConstant(UntypedConstant::LogicNot(x->m_untyped_constant));
    case Value::kTypedConstant:
      return MakeTypedConstant(TypedConstant::LogicNot(x->m_typed_constant));
    case Value::kRValue:
      return MakeRValue(x->type());
    default:
      abort();  // NOT_COVERED
  }
}

Value Value::BitNot(
    Location const & location, Value * x,
    ErrorList* error_list) {
  assert(x->IsInitialized());

  if (x->IsError()) {
    return Value::MakeError();
  }

  if (!x->IsInteger()) {
    error_list->push_back(cannotApply(location, '~', *x));
    return Value::MakeError();
  }

  // TODO(jrw972):  Remove.
  x->Dereference();

  switch (x->m_kind) {
    case Value::kUntypedConstant:
      return MakeUntypedConstant(UntypedConstant::BitNot(x->m_untyped_constant));
    case Value::kTypedConstant:
      return MakeTypedConstant(TypedConstant::BitNot(x->m_typed_constant));
    case Value::kRValue:
      return MakeRValue(x->type());
    default:
      abort();  // NOT_COVERED
  }
}

Value Value::Add(
    Location const & location, Value * x, Value * y,
    ErrorList* error_list) {
  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!((x->IsArithmetic() && y->IsArithmetic()) ||
       (x->IsString() && y->IsString()))) {
    // TODO(jrw972):  Make this check and other look at the left and right
    // independently.
    error_list->push_back(cannotApply2(location, "+", *x, *y));
    return Value::MakeError();
  }

  Promote(x, y);
  if (x->IsError()) { return *x; }
  if (y->IsError()) { return *y; }
  // TODO(jrw972):  Check that all return MakeError() use the error_reporter.

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  switch (x->m_kind) {
    case kUntypedConstant:
      return MakeUntypedConstant(UntypedConstant::Add(x->m_untyped_constant, y->m_untyped_constant));
    case Value::kTypedConstant:
      return MakeTypedConstant(TypedConstant::Add(x->m_typed_constant, y->m_typed_constant));
    case Value::kRValue:
      if (type::Identical(x->m_type, y->m_type) &&
          ((type::IsString(x->m_type) && type::IsString(y->m_type)) ||
           (type::IsArithmetic(x->m_type) && type::IsArithmetic(y->m_type)))) {
        return MakeRValue(type::Choose(x->m_type, y->m_type));
      }
    default:
      return MakeError();
  }
}

Value Value::Subtract(
    Location const & location, Value * x, Value * y,
    ErrorList* error_list) {
  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsArithmetic() && y->IsArithmetic())) {
    error_list->push_back(cannotApply2(location, "-", *x, *y));
    return Value::MakeError();
  }

  Promote(x, y);
  if (x->IsError()) { return *x; }
  if (y->IsError()) { return *y; }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  switch (x->m_kind) {
    case Value::kUntypedConstant:
      return MakeUntypedConstant(UntypedConstant::Subtract(x->m_untyped_constant, y->m_untyped_constant));
    case Value::kTypedConstant:
      return MakeTypedConstant(TypedConstant::Subtract(x->m_typed_constant, y->m_typed_constant));
    case Value::kRValue:
      if (type::Identical(x->m_type, y->m_type)) {
        return MakeRValue(type::Choose(x->m_type, y->m_type));
      }
    default:
      return MakeError();
  }
}

Value Value::Multiply(
    Location const & location, Value * x, Value * y,
    ErrorList* error_list) {
  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsArithmetic() && y->IsArithmetic())) {
    error_list->push_back(cannotApply2(location, "*", *x, *y));
    return Value::MakeError();
  }

  Promote(x, y);
  if (x->IsError()) { return *x; }
  if (y->IsError()) { return *y; }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  switch (x->m_kind) {
    case Value::kUntypedConstant:
      return MakeUntypedConstant(UntypedConstant::Multiply(x->m_untyped_constant, y->m_untyped_constant));
    case Value::kTypedConstant:
      return MakeTypedConstant(TypedConstant::Multiply(x->m_typed_constant, y->m_typed_constant));
    case Value::kRValue:
      if (type::Identical(x->m_type, y->m_type)) {
        return MakeRValue(type::Choose(x->m_type, y->m_type));
      }
    default:
      return MakeError();
  }
}

Value Value::Divide(
    Location const & location, Value * x, Value * y,
    ErrorList* error_list) {
  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsArithmetic() && y->IsArithmetic())) {
    error_list->push_back(cannotApply2(location, "/", *x, *y));
    return Value::MakeError();
  }

  if (y->IsZero()) {
    error_list->push_back(divisionByZero(location));
    return Value::MakeError();
  }

  Promote(x, y);
  if (x->IsError()) { return *x; }
  if (y->IsError()) { return *y; }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  switch (x->m_kind) {
    case Value::kUntypedConstant:
      return MakeUntypedConstant(UntypedConstant::Divide(x->m_untyped_constant, y->m_untyped_constant));
    case Value::kTypedConstant:
      return MakeTypedConstant(TypedConstant::Divide(x->m_typed_constant, y->m_typed_constant));
    case Value::kRValue:
      if (type::Identical(x->m_type, y->m_type)) {
        return MakeRValue(type::Choose(x->m_type, y->m_type));
      }
    default:
      return MakeError();
  }
}

Value Value::Modulo(
    Location const & location, Value * x, Value * y,
    ErrorList* error_list) {
  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsInteger() && x->IsInteger())) {
    error_list->push_back(cannotApply2(location, "%", *x, *y));
    return Value::MakeError();
  }

  if (y->IsZero()) {
    error_list->push_back(divisionByZero(location));
    return Value::MakeError();
  }

  Promote(x, y);
  if (x->IsError()) { return *x; }
  if (y->IsError()) { return *y; }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  switch (x->m_kind) {
    case Value::kUntypedConstant:
      return MakeUntypedConstant(UntypedConstant::Modulo(x->m_untyped_constant, y->m_untyped_constant));
    case Value::kTypedConstant:
      return MakeTypedConstant(TypedConstant::Modulo(x->m_typed_constant, y->m_typed_constant));
      break;
    case Value::kRValue:
      if (type::Identical(x->m_type, y->m_type)) {
        return MakeRValue(type::Choose(x->m_type, y->m_type));
      }
    default:
      return MakeError();
  }
}

Value Value::LeftShift(Value* x, Value* y, ErrorList* error_list) {
  assert(x->IsInitialized());
  assert(y->IsInitialized());

  bool x_error = false;
  if (!x->IsError()) {
    if (!x->IsRValueish()) {
      error_list->push_back(CannotBeUsedInAnExpression(x));
      x_error = true;
    } else if (!x->IsIntegral()) {
      error_list->push_back(IsNotAnInteger(x));
      x_error = true;
    }
  } else {
    x_error = true;
  }

  bool y_error = false;
  if (!y->IsError()) {
    if (!y->IsRValueish()) {
      error_list->push_back(CannotBeUsedInAnExpression(y));
      y_error = true;
    } else {
      if (y->IsConstant()) {
        Value yy = ConvertConstant(*y, &type::Uint::instance);
        if (!yy.IsError()) {
          *y = yy;
        } else {
          error_list->push_back(IsNotAnInteger(y));
          y_error = true;
        }
      } else if (!y->IsInteger()) {
        error_list->push_back(IsNotAnInteger(y));
        y_error = true;
      }
    }
  } else {
    y_error = true;
  }

  if (x_error) {
    return Value::MakeError();
  }

  if (x->m_kind == kRValue) {
    return MakeRValue(x->type());
  }

  if (y_error) {
    return Value::MakeError();
  }

  assert(x->IsConstant());

  if (y->IsConstant()) {
    unsigned int yyy = y->uint_value();
    switch (x->m_kind) {
      case Value::kUntypedConstant:
        return MakeUntypedConstant(UntypedConstant::LeftShift(x->m_untyped_constant, yyy));
      case Value::kTypedConstant:
        return MakeTypedConstant(TypedConstant::LeftShift(x->m_typed_constant, yyy));
      default:
        abort();
    }
  } else {
    // y is not constant.
    switch (x->m_kind) {
      case Value::kUntypedConstant:
        {
          // Convert to default integer type.
          TypedConstant xx = TypedConstant(&type::Int64::instance, x->m_untyped_constant);
          if (!xx.IsInitialized()) {
            error_list->push_back(CannotConvert(x, &type::Int64::instance));
            return MakeError();
          }
          return MakeRValue(&type::Int64::instance);
        }
      case Value::kTypedConstant:
        return MakeRValue(x->m_typed_constant.type());
      default:
        abort();
    }
  }
}

Value Value::RightShift(Value* x, Value* y, ErrorList* error_list) {
  assert(x->IsInitialized());
  assert(y->IsInitialized());

  bool x_error = false;
  if (!x->IsError()) {
    if (!x->IsRValueish()) {
      error_list->push_back(CannotBeUsedInAnExpression(x));
      x_error = true;
    } else if (!x->IsIntegral()) {
      error_list->push_back(IsNotAnInteger(x));
      x_error = true;
    }
  } else {
    x_error = true;
  }

  bool y_error = false;
  if (!y->IsError()) {
    if (!y->IsRValueish()) {
      error_list->push_back(CannotBeUsedInAnExpression(y));
      y_error = true;
    } else {
      if (y->IsConstant()) {
        Value yy = ConvertConstant(*y, &type::Uint::instance);
        if (!yy.IsError()) {
          *y = yy;
        } else {
          error_list->push_back(IsNotAnInteger(y));
          y_error = true;
        }
      } else if (!y->IsInteger()) {
        error_list->push_back(IsNotAnInteger(y));
        y_error = true;
      }
    }
  } else {
    y_error = true;
  }

  if (x_error) {
    return Value::MakeError();
  }

  if (x->m_kind == kRValue) {
    return MakeRValue(x->type());
  }

  if (y_error) {
    return Value::MakeError();
  }

  assert(x->IsConstant());

  if (y->IsConstant()) {
    unsigned int yyy = y->uint_value();
    switch (x->m_kind) {
      case Value::kUntypedConstant:
        return MakeUntypedConstant(UntypedConstant::RightShift(x->m_untyped_constant, yyy));
      case Value::kTypedConstant:
        return MakeTypedConstant(TypedConstant::RightShift(x->m_typed_constant, yyy));
      default:
        abort();
    }
  } else {
    // y is not constant.
    switch (x->m_kind) {
      case Value::kUntypedConstant:
        {
          // Convert to default integer type.
          TypedConstant xx = TypedConstant(&type::Int64::instance, x->m_untyped_constant);
          if (!xx.IsInitialized()) {
            error_list->push_back(CannotConvert(x, &type::Int64::instance));
            return MakeError();
          }
          return MakeRValue(&type::Int64::instance);
        }
      case Value::kTypedConstant:
        return MakeRValue(x->m_typed_constant.type());
      default:
        abort();
    }
  }
}

Value Value::BitAnd(
    Location const & location, Value * x, Value * y,
    ErrorList* error_list) {
  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsInteger() && y->IsInteger())) {
    error_list->push_back(cannotApply2(location, "&", *x, *y));
    return Value::MakeError();
  }

  Promote(x, y);
  if (x->IsError()) { return *x; }
  if (y->IsError()) { return *y; }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  switch (x->m_kind) {
    case Value::kUntypedConstant:
      return MakeUntypedConstant(UntypedConstant::BitAnd(x->m_untyped_constant, y->m_untyped_constant));
    case Value::kTypedConstant:
      return MakeTypedConstant(TypedConstant::BitAnd(x->m_typed_constant, y->m_typed_constant));
    case Value::kRValue:
      if (type::Identical(x->m_type, y->m_type)) {
        return MakeRValue(type::Choose(x->m_type, y->m_type));
      }
    default:
      return MakeError();
  }
}

Value Value::BitAndNot(
    Location const & location, Value * x, Value * y,
    ErrorList* error_list) {
  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsInteger() && y->IsInteger())) {
    error_list->push_back(cannotApply2(location, "&~", *x, *y));
    return Value::MakeError();
  }

  Promote(x, y);
  if (x->IsError()) { return *x; }
  if (y->IsError()) { return *y; }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  switch (x->m_kind) {
    case Value::kUntypedConstant:
      return MakeUntypedConstant(UntypedConstant::BitAndNot(x->m_untyped_constant, y->m_untyped_constant));
    case Value::kTypedConstant:
      return MakeTypedConstant(TypedConstant::BitAndNot(x->m_typed_constant, y->m_typed_constant));
    case Value::kRValue:
      if (type::Identical(x->m_type, y->m_type)) {
        return MakeRValue(type::Choose(x->m_type, y->m_type));
      }
    default:
      return MakeError();
  }
}

Value Value::BitOr(
    Location const & location, Value * x, Value * y,
    ErrorList* error_list) {
  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsInteger() && y->IsInteger())) {
    error_list->push_back(cannotApply2(location, "|", *x, *y));
    return Value::MakeError();
  }

  Promote(x, y);
  if (x->IsError()) { return *x; }
  if (y->IsError()) { return *y; }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  switch (x->m_kind) {
    case Value::kUntypedConstant:
      return MakeUntypedConstant(UntypedConstant::BitOr(x->m_untyped_constant, y->m_untyped_constant));
    case Value::kTypedConstant:
      return MakeTypedConstant(TypedConstant::BitOr(x->m_typed_constant, y->m_typed_constant));
    case Value::kRValue:
      if (type::Identical(x->m_type, y->m_type)) {
        return MakeRValue(type::Choose(x->m_type, y->m_type));
      }
    default:
      return MakeError();
  }
}

Value Value::BitXor(
    Location const & location, Value * x, Value * y,
    ErrorList* error_list) {
  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsInteger() && y->IsInteger())) {
    error_list->push_back(cannotApply2(location, "^", *x, *y));
    return Value::MakeError();
  }

  Promote(x, y);
  if (x->IsError()) { return *x; }
  if (y->IsError()) { return *y; }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  switch (x->m_kind) {
    case Value::kUntypedConstant:
      return MakeUntypedConstant(UntypedConstant::BitXor(x->m_untyped_constant, y->m_untyped_constant));
    case Value::kTypedConstant:
      return MakeTypedConstant(TypedConstant::BitXor(x->m_typed_constant, y->m_typed_constant));
    case Value::kRValue:
      if (type::Identical(x->m_type, y->m_type)) {
        return MakeRValue(type::Choose(x->m_type, y->m_type));
      }
    default:
      return MakeError();
  }
}

Value Value::Equal(
    Location const & location, Value * x, Value * y,
    ErrorList* error_list) {
  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsComparable() && y->IsComparable())) {
    error_list->push_back(cannotApply2(location, "==", *x, *y));
    return Value::MakeError();
  }

  Promote(x, y);
  if (x->IsError()) {
      error_list->push_back(cannotApply2(location, "==", *x, *y));
      return *x;
  }
  if (y->IsError()) {
      error_list->push_back(cannotApply2(location, "==", *x, *y));
      return *y;
  }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  switch (x->m_kind) {
    case Value::kUntypedConstant:
    {
        if (!UntypedConstant::AreComparable(x->m_untyped_constant, y->m_untyped_constant)) {
            error_list->push_back(cannotApply2(location, "==", *x, *y));
            return MakeError();
        }
      Value v = MakeUntypedConstant(UntypedConstant::Equal(x->m_untyped_constant, y->m_untyped_constant));
      if (v.IsError()) {
        error_list->push_back(cannotApply2(location, "==", *x, *y));
      }
      return v;
    }
    case Value::kTypedConstant:
      return MakeUntypedConstant(TypedConstant::Equal(x->m_typed_constant, y->m_typed_constant));
    case Value::kRValue:
      if (type::Identical(x->m_type, y->m_type)) {
        return MakeRValue(type::Choose(x->m_type, y->m_type));
      }
    default:
      error_list->push_back(cannotApply2(location, "==", *x, *y));
      return MakeError();
  }
}

Value Value::Call(
    Location const & location, Value * operand,
    std::vector<Value *> const & arguments,
    const LocationList& locations, ErrorList* error_list) {
  assert(operand->IsInitialized());
  assert(arguments.size() == locations.size());

  if (operand->IsError()) {
    return Value::MakeError();
  }

  if (!operand->IsCallable()) {
    error_list->push_back(operandCannotBeCalled(location));
    return Value::MakeError();
  }

  const type::Function* function =
      type::Cast<const type::Function>(operand->m_type);

  if (arguments.size() != function->ParameterCount()) {
    error_list->push_back(
        callExpectsNArguments(
            location, function->ParameterCount(), arguments.size()));
  }

  for (size_t idx = 0,
           limit = std::min(arguments.size(), function->ParameterCount());
       idx != limit; ++idx) {
    Value * argument = arguments.at(idx);
    if (argument->IsError()) {
      continue;
    }

    // TODO(jrw972):  Remove.
    argument->Dereference();

    symbol::Parameter const * parameter = function->ParameterAt(idx);
    Value variable = MakeLValue(parameter->type);
    IsAssignableFrom(&variable, argument);
    if (variable.IsError() || argument->IsError()) {
      error_list->push_back(cannotConvert(locations.at(idx), *argument,
                                           parameter->type));
    }
  }

  if (function->ResultCount() == 0) {
    return Value(kVoid);
  } else if (function->ResultCount() == 1) {
    return Value::MakeRValue((*function->ResultBegin())->type);
  } else {
    Value retval(kList);
    for (auto pos = function->ResultBegin(), limit = function->ResultEnd();
         pos != limit; ++pos) {
      const symbol::Parameter* parameter = *pos;
      retval.m_list.push_back(Value::MakeRValue(parameter->type));
    }
    return retval;
  }
}

bool Value::operator==(const Value& y) const {
  if (this->m_kind != y.m_kind) {
    return false;
  }

  switch (this->m_kind) {
    case Value::kUninitialized:
      return true;
    case Value::kUntypedConstant:
      return this->m_untyped_constant == y.m_untyped_constant;
    case Value::kTypedConstant:
      return this->m_typed_constant == y.m_typed_constant;
    case Value::kFunction:
    case Value::kRValue:
    case Value::kLValue:
      return this->m_type == y.m_type;
    case Value::kType:
      return this->m_type == y.m_type;
    case Value::kVoid:
      return true;
    case Value::kList:
      return this->m_list == y.m_list;
  }

  abort();  // NOT_COVERED
}

bool Value::operator!=(const Value& y) const {
  return !(*this == y);
}

std::ostream& operator<<(std::ostream& out, const Value& value) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(std::ostream& a_out, const Value& a_value)
        : out(a_out)
        , value(a_value) {}

    std::ostream& out;
    const Value& value;

    void Visit(const type::Bool&) override {
      out <<(value.bool_value() ? "true" : "false");
    }
    void Visit(const type::Int8&) override {
      out << static_cast<int>(value.int8_value());
    }
    void Visit(const type::Int16&) override {
      out << value.int16_value();
    }
    void Visit(const type::Int32&) override {
      out << value.int32_value();
    }
    void Visit(const type::Int64&) override {
      out << value.int64_value();
    }
    void Visit(const type::Uint8&) override {
      out << static_cast<int>(value.uint8_value());
    }
    void Visit(const type::Uint16&) override {
      out << value.uint16_value();
    }
    void Visit(const type::Uint32&) override {
      out << value.uint32_value();
    }
    void Visit(const type::Uint64&) override {
      out << value.uint64_value();
    }
    void Visit(const type::Int&) override {
      out << value.int_value();
    }
    void Visit(const type::Uint&) override {
      out << value.uint_value();
    }
    void Visit(const type::Uintptr&) override {
      out << value.uintptr_value();
    }
    void Visit(const type::Float32&) override {
      out << value.float32_value();
    }
    void Visit(const type::Float64&) override {
      out << value.float64_value();
    }
    void Visit(const type::Complex64&) override {
      out << value.complex64_value();
    }
    void Visit(const type::Complex128&) override {
      out << value.complex128_value();
    }
    void Visit(const type::String&) override {
      out << '"' << value.string_value() << '"';
    }
  };

  switch (value.kind()) {
    case Value::kUninitialized:
      out << "uninitialized";
      break;
    case Value::kUntypedConstant:
      out << value.untyped_constant();
      break;
    case Value::kTypedConstant:
      {
        Visitor v(out, value);
        value.type()->UnderlyingType()->Accept(&v);
      }
      break;
    case Value::kFunction:
      out << "<function>";
      break;
    case Value::kLValue:
      out << "<lvalue>";
      break;
    case Value::kRValue:
      out << "<rvalue>";
      break;
    case Value::kType:
      out << "<type>";
      break;
    case Value::kVoid:
      out << "<void>";
      break;
    case Value::kList:
      out << "<list>";
      break;
  }
  return out;
}

Error CannotBeUsedInAnExpression(Value const* x) {
  Error error;  // TODO(jrw972): Use location of x.
  error.message << "error: cannot be used in expression" << std::endl;
  return error;
}

Error IsNotAnInteger(Value const* x) {
  Error error;  // TODO(jrw972): Use location of x.
  error.message << "error: is not an integer" << std::endl;
  return error;
}

Error CannotConvert(Value const* x, type::Type const* type) {
  Error error;  // TODO(jrw972): Use location of x.
  error.message << "error: cannot convert " << *x << " to " << *type << std::endl;
  return error;
}

}  // namespace value
}  // namespace rcgo
