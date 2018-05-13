#ifndef SRC_TOKEN_H_
#define SRC_TOKEN_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <gmpxx.h>

#include <string>

#include "src/location.h"
#include "src/value.h"

namespace rcgo {

struct Token {  // NOT_COVERED
  enum RelOpKind {
    kEqual,
    kNotEqual,
    kLessThan,
    kLessThanEqual,
    kGreaterThan,
    kGreaterThanEqual,
  };

  enum AddOpKind {
    kAdd,       // Posate
    kSubtract,  // Negate
    kBitOr,
    kBitXor,    // BitNot
  };

  enum MulOpKind {
    kMultiply,  // Indirection
    kDivide,
    kModulus,
    kLeftShift,
    kRightShift,
    kBitAnd,    // Address
    kBitAndNot,
  };

  enum UnaryOpKind {
    kPosate,
    kNegate,
    kLogicalNot,
    kBitNot,
    kIndirection,  // Pointer
    kAddress,
  };

  enum AssignOpKind {
    kAddAssign,
    kSubtractAssign,
    kBitOrAssign,
    kBitXorAssign,

    kMultiplyAssign,
    kDivideAssign,
    kModulusAssign,
    kLeftShiftAssign,
    kRightShiftAssign,
    kBitAndAssign,
    kBitAndNotAssign,
  };

  enum Kind {
    kIdentifier,

    // Keywords
    kBreak,
    kCase,
    kConst,
    kContinue,
    kDefault,
    kDefer,
    kElse,
    kFallthrough,
    kFor,
    kFunc,
    kGoto,
    kIf,
    kImport,
    kInterface,
    kMap,
    kPackage,
    kRange,
    kReturn,
    kStruct,
    kSwitch,
    kType,
    kVar,

    // Operators and Delimiters
    kAssign,
    kColon,
    kComma,
    kDecrement,
    kEllipsis,
    kIncrement,
    kLeftBrace,
    kLeftBracket,
    kLeftParen,
    kLogicalAnd,
    kLogicalOr,
    kPeriod,
    kPointer,
    kRightBrace,
    kRightBracket,
    kRightParen,
    kSemicolon,
    kShortAssign,

    kRelOp,
    kAddOp,
    kMulOp,
    kUnaryOp,
    kAssignOp,

    // Literals
    kLiteral,

    kEnd,
  };

  static Token Make(const Location& a_location, Kind a_kind);
  static Token Make(RelOpKind a_kind);
  static Token Make(AddOpKind a_kind);
  static Token Make(MulOpKind a_kind);
  static Token Make(UnaryOpKind a_kind);
  static Token Make(AssignOpKind a_kind);
  static Token Make(Kind a_kind);
  static Token Make(const Location& a_location, const Token& a_token);
  static Token MakeIdentifier(const Location& a_location,
                              const std::string& a_identifier);
  static Token MakeLiteral(const Location& a_location,
                           const Value& a_value);

  bool operator==(Kind kind) const { return m_kind == kind; }
  bool operator!=(Kind kind) const { return m_kind != kind; }

  const Location& location() const { return m_location; }
  Kind kind() const { return m_kind; }

  bool IsRelOp() const;
  RelOpKind rel_op_kind() const;
  bool IsAddOp() const;
  AddOpKind add_op_kind() const;
  bool IsMulOp() const;
  MulOpKind mul_op_kind() const;
  bool IsUnaryOp() const;
  UnaryOpKind unary_op_kind() const;
  bool IsAssignOp() const;
  AssignOpKind assign_op_kind() const;
  const std::string& identifier() const;
  const Value& value() const;

 private:
  Location m_location;
  Kind m_kind;
  union {
    RelOpKind m_rel_op_kind;
    AddOpKind m_add_op_kind;
    MulOpKind m_mul_op_kind;
    UnaryOpKind m_unary_op_kind;
    AssignOpKind m_assign_op_kind;
  };
  std::string m_identifier;
  Value m_value;
};

std::ostream& operator<<(std::ostream& a_out, const Token::Kind a_kind);
std::ostream& operator<<(std::ostream& a_out, const Token::RelOpKind a_kind);
std::ostream& operator<<(std::ostream& a_out, const Token::AddOpKind a_kind);
std::ostream& operator<<(std::ostream& a_out, const Token::MulOpKind a_kind);
std::ostream& operator<<(std::ostream& a_out, const Token::UnaryOpKind a_kind);
std::ostream& operator<<(std::ostream& a_out, const Token::AssignOpKind a_kind);
std::ostream& operator<<(std::ostream& a_out, const Token& a_token);

}  // namespace rcgo

#endif  // SRC_TOKEN_H_
