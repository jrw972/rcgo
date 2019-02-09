// Copyright 2019 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/untyped_constant.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "src/utf8.h"

namespace rcgo {
namespace value {

complex_t::complex_t() : m_real(0, PRECISION), m_imag(0, PRECISION) {}

complex_t::complex_t(const mpf_class& a_real, const mpf_class& a_imag)
    : m_real(a_real), m_imag(a_imag) {}

bool complex_t::operator==(const complex_t& a_other) const {
  return this->m_real == a_other.m_real && this->m_imag == a_other.m_imag;
}

bool complex_t::operator!=(const complex_t& a_other) const {
  return !(*this == a_other);
}

complex_t complex_t::operator+(const complex_t& a_other) const {
  return complex_t(m_real + a_other.m_real, m_imag + a_other.m_imag);
}

complex_t complex_t::operator-(const complex_t& a_other) const {
  return complex_t(m_real - a_other.m_real, m_imag - a_other.m_imag);
}

complex_t complex_t::operator*(const complex_t& a_other) const {
  return complex_t(m_real * a_other.m_real - m_imag * a_other.m_imag,
                   m_real * a_other.m_imag + m_imag * a_other.m_real);
}

complex_t complex_t::operator/(const complex_t& a_other) const {
  mpf_class denom = a_other.m_real * a_other.m_real +
      a_other.m_imag * a_other.m_imag;
  return complex_t((m_real * a_other.m_real + m_imag * a_other.m_real) / denom,
                   (m_imag * a_other.m_real - m_real * a_other.m_imag) / denom);
}

complex_t complex_t::operator+() const {
  return complex_t(+m_real, +m_imag);
}

complex_t complex_t::operator-() const {
  return complex_t(-m_real, -m_imag);
}

std::ostream& operator<<(std::ostream& out, const complex_t& val) {
  out << val.real() << (val.imag() >= 0 ? "+" : "") << val.imag() << 'i';
  return out;
}

UntypedConstant::UntypedConstant() : m_kind(kUninitialized) {}

UntypedConstant::UntypedConstant(Kind a_kind) : m_kind(a_kind) {}

UntypedConstant UntypedConstant::MakeError() {
  return UntypedConstant(kError);
}

UntypedConstant UntypedConstant::MakeBoolean(bool a_value) {
  UntypedConstant uc(kBoolean);
  uc.m_boolean_value = a_value;
  return uc;
}

UntypedConstant UntypedConstant::MakeInteger(mpz_class const & a_value) {
  UntypedConstant uc(kInteger);
  uc.m_integer_value = a_value;
  return uc;
}

UntypedConstant UntypedConstant::MakeFloat(mpf_class const & a_value) {
  UntypedConstant uc(kFloat);
  uc.m_float_value = a_value;
  return uc;
}

UntypedConstant UntypedConstant::MakeComplex(
    mpf_class const & a_real,
    mpf_class const & a_imag) {
  return MakeComplex(complex_t(a_real, a_imag));
}

UntypedConstant UntypedConstant::MakeComplex(complex_t const & a_value) {
  UntypedConstant uc(kComplex);
  uc.m_complex_value = a_value;
  return uc;
}

UntypedConstant UntypedConstant::MakeRune(mpz_class const & a_value) {
  UntypedConstant uc(kRune);
  uc.m_rune_value = a_value;
  return uc;
}

UntypedConstant UntypedConstant::MakeString(std::string const & a_value) {
  UntypedConstant uc(kString);
  uc.m_string_value = a_value;
  return uc;
}

bool UntypedConstant::IsInitialized() const {
  return m_kind != kUninitialized;
}

bool UntypedConstant::IsError() const {
  return m_kind == kError;
}

bool UntypedConstant::IsArithmetic(Kind a_kind) {
  switch (a_kind) {
    case kInteger:
    case kRune:
    case kFloat:
    case kComplex:
      return true;
    default:
      return false;
  }
}

bool UntypedConstant::IsArithmetic() const {
  return IsArithmetic(m_kind);
}

bool UntypedConstant::IsSigned() const {
  switch (m_kind) {
    case kInteger:
    case kRune:
    case kFloat:
    case kComplex:
      return true;
    default:
      return false;
  }
}

bool UntypedConstant::IsInteger() const {
  switch (m_kind) {
    case kInteger:
    case kRune:
      return true;
    default:
      return false;
  }
}

bool UntypedConstant::IsZero() const {
  switch (m_kind) {
    case kInteger:
      return m_integer_value == 0;
    case kRune:
      return m_rune_value == 0;
    case kFloat:
      return m_float_value == 0;
    case kComplex:
      return m_complex_value == complex_t(0, 0);
    default:
      return false;
  }
}

bool UntypedConstant::IsString() const {
  return m_kind == kString;
}

bool UntypedConstant::IsBoolean() const {
  return m_kind == kBoolean;
}

UntypedConstant UntypedConstant::ToInteger(UntypedConstant const & a_x) {
  assert(a_x.IsInitialized());

  switch (a_x.m_kind) {
    case kInteger:
      return a_x;
    case kRune:
      return MakeInteger(a_x.m_rune_value);
    case kFloat:
      {
        mpz_class x(a_x.m_float_value);
        if (a_x.m_float_value == mpf_class(x)) {
          return MakeInteger(x);
        } else {
          return MakeError();
        }
      }
    case kComplex:
      {
        mpz_class x(a_x.m_complex_value.real());
        if (a_x.m_complex_value == complex_t(x, 0)) {
          return MakeInteger(x);
        } else {
          return MakeError();
        }
      }
      break;
    default:
      return MakeError();
  }
}

UntypedConstant UntypedConstant::LogicNot(UntypedConstant const & a_x) {
  assert(a_x.IsInitialized());

  if (!a_x.IsBoolean()) {
    return MakeError();
  }

  return MakeBoolean(!a_x.m_boolean_value);
}

UntypedConstant UntypedConstant::Add(
    UntypedConstant const & a_x,
    UntypedConstant const & a_y) {
  assert(a_x.IsInitialized());
  assert(a_y.IsInitialized());

  if (a_x.IsError() || a_y.IsError()) {
    return MakeError();
  }

  if (a_x.IsString()) {
    if (a_y.IsString()) {
      return MakeString(a_x.m_string_value + a_y.m_string_value);
    } else {
      return MakeError();
    }
  }

  if (a_x.IsArithmetic()) {
    if (a_y.IsArithmetic()) {
      Kind k = std::max(a_x.m_kind, a_y.m_kind);
      UntypedConstant x = PromoteTo(a_x, k);
      UntypedConstant y = PromoteTo(a_y, k);
      switch (k) {
        case kInteger:
          return MakeInteger(a_x.m_integer_value + a_y.m_integer_value);
        case kRune:
          return MakeRune(a_x.m_rune_value + a_y.m_rune_value);
        case kFloat:
          return MakeFloat(a_x.m_float_value + a_y.m_float_value);
        case kComplex:
          return MakeComplex(a_x.m_complex_value + a_y.m_complex_value);
        default:
          abort();
      }
    } else {
      return MakeError();
    }
  }

  return MakeError();
}


UntypedConstant UntypedConstant::Equal(
    UntypedConstant const & a_x,
    UntypedConstant const & a_y) {
  assert(a_x.IsInitialized());
  assert(a_y.IsInitialized());

  if (a_x.IsError() || a_y.IsError()) {
    return MakeError();
  }

  if (a_x.IsBoolean()) {
    if (a_y.IsBoolean()) {
      return MakeBoolean(a_x.m_boolean_value == a_y.m_boolean_value);
    } else {
      return MakeError();
    }
  }

  if (a_x.IsString()) {
    if (a_y.IsString()) {
      return MakeBoolean(a_x.m_string_value == a_y.m_string_value);
    } else {
      return MakeError();
    }
  }

  assert(a_x.IsArithmetic());

  if (a_y.IsArithmetic()) {
    Kind k = std::max(a_x.m_kind, a_y.m_kind);
    UntypedConstant x = PromoteTo(a_x, k);
    UntypedConstant y = PromoteTo(a_y, k);
    switch (k) {
      case kInteger:
        return MakeBoolean(a_x.m_integer_value == a_y.m_integer_value);
      case kRune:
        return MakeBoolean(a_x.m_rune_value == a_y.m_rune_value);
      case kFloat:
        return MakeBoolean(a_x.m_float_value == a_y.m_float_value);
      case kComplex:
        return MakeBoolean(a_x.m_complex_value == a_y.m_complex_value);
      default:
        abort();
    }
  } else {
    return MakeError();
  }
}

bool UntypedConstant::operator==(UntypedConstant const & a_other) const {
  if (this->m_kind != a_other.m_kind) {
    return false;
  }

  switch (m_kind) {
    case kUninitialized:
    case kError:
      return true;
    case kBoolean:
      return this->m_boolean_value == a_other.m_boolean_value;
    case kString:
      return this->m_string_value == a_other.m_string_value;
    case kInteger:
      return this->m_integer_value == a_other.m_integer_value;
    case kRune:
      return this->m_rune_value == a_other.m_rune_value;
    case kFloat:
      return this->m_float_value == a_other.m_float_value;
    case kComplex:
      return this->m_complex_value == a_other.m_complex_value;
  }

  abort();
}

UntypedConstant UntypedConstant::PromoteTo(
    UntypedConstant const & a_x,
    Kind a_kind) {
  assert(a_x.IsInitialized());
  assert(a_x.IsArithmetic());
  assert(IsArithmetic(a_kind));

  UntypedConstant retval = a_x;

  if (retval.m_kind == kInteger && retval.m_kind < a_kind) {
    retval = MakeRune(retval.m_integer_value);
  }

  if (retval.m_kind == kRune && retval.m_kind < a_kind) {
    retval = MakeFloat(retval.m_rune_value);
  }

  if (retval.m_kind == kFloat && retval.m_kind < a_kind) {
    retval = MakeComplex(retval.m_float_value, 0);
  }

  assert(retval.m_kind == a_kind);
  return retval;
}

std::ostream & operator<<(
    std::ostream & a_out,
    UntypedConstant const & a_value) {
  switch (a_value.kind()) {
    case UntypedConstant::kUninitialized:
      a_out << "<uninitialized>";
      break;
    case UntypedConstant::kError:
      a_out << "<error>";
      break;
    case UntypedConstant::kBoolean:
      a_out << (a_value.boolean_value() ? "true" : "false");
      break;
    case UntypedConstant::kString:
      a_out << '"' << a_value.string_value() << '"';
      break;
    case UntypedConstant::kInteger:
      a_out << a_value.integer_value();
      break;
    case UntypedConstant::kRune:
      if (a_value.rune_value() < INVALID &&
          (a_value.rune_value() < SURROGATE_FIRST ||
           a_value.rune_value() > SURROGATE_LAST)) {
        std::string s;
        Append(&s, a_value.rune_value().get_ui());
        a_out << "'" << s << "'";
      } else {
        a_out << a_value.rune_value();
      }
      break;
    case UntypedConstant::kFloat:
      a_out << a_value.float_value();
      break;
    case UntypedConstant::kComplex:
      a_out << a_value.complex_value();
      break;
  }

  return a_out;
}

}  // namespace value
}  // namespace rcgo
