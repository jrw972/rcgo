// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/token.h"

#include <cassert>
#include <sstream>

#include "src/error_reporter.h"

#define SPACE 0x20
#define TAB 0x09
#define CARRIAGE_RETURN 0x0D
#define NEWLINE 0x0A

namespace rcgo {

Token Token::Make(const Location& location, Kind kind) {
  Token t;
  t.m_location = location;
  t.m_kind = kind;
  return t;
}

Token Token::Make(RelOpKind a_kind) {
  Token t;
  t.m_location = Location::builtin;
  t.m_kind = kRelOp;
  t.m_rel_op_kind = a_kind;
  return t;
}

Token Token::Make(AddOpKind a_kind) {
  assert(a_kind != kAdd &&
          a_kind != kSubtract &&
          a_kind != kBitXor);
  Token t;
  t.m_location = Location::builtin;
  t.m_kind = kAddOp;
  t.m_add_op_kind = a_kind;
  return t;
}

Token Token::Make(MulOpKind a_kind) {
  assert(a_kind != kMultiply &&
          a_kind != kBitAnd);
  Token t;
  t.m_location = Location::builtin;
  t.m_kind = kMulOp;
  t.m_mul_op_kind = a_kind;
  return t;
}

Token Token::Make(UnaryOpKind a_kind) {
  assert(a_kind != kIndirection);
  Token t;
  t.m_location = Location::builtin;
  t.m_kind = kUnaryOp;
  t.m_unary_op_kind = a_kind;
  return t;
}

Token Token::Make(AssignOpKind a_kind) {
  Token t;
  t.m_location = Location::builtin;
  t.m_kind = kAssignOp;
  t.m_assign_op_kind = a_kind;
  return t;
}

Token Token::Make(Kind a_kind) {
  Token t;
  t.m_location = Location::builtin;
  t.m_kind = a_kind;
  return t;
}

Token Token::Make(const Location& a_location, const Token& a_token) {
  Token t(a_token);
  t.m_location = a_location;
  return t;
}

Token Token::MakeIdentifier(const Location& location, const std::string& s) {
  Token t;
  t.m_location = location;
  t.m_kind = kIdentifier;
  t.m_identifier = s;
  return t;
}

Token Token::MakeLiteral(
    Location const & location, value::UntypedConstant const & a_value) {
  Token t;
  t.m_location = location;
  t.m_kind = kLiteral;
  t.m_value = a_value;
  return t;
}

bool Token::IsRelOp() const {
  return m_kind == kRelOp;
}

Token::RelOpKind Token::rel_op_kind() const {
  assert(IsRelOp());
  return m_rel_op_kind;
}

bool Token::IsAddOp() const {
  if (m_kind == kUnaryOp) {
    switch (m_unary_op_kind) {
      case kPosate:
      case kNegate:
      case kBitNot:
        return true;
      default:
        return false;
    }
  }

  return m_kind == kAddOp;
}

Token::AddOpKind Token::add_op_kind() const {
  assert(IsAddOp());

  if (m_kind == kUnaryOp) {
    switch (m_unary_op_kind) {
      case kPosate:
        return kAdd;
      case kNegate:
        return kSubtract;
      case kBitNot:
        return kBitXor;
      default:
        abort();  // NOT_COVERED
    }
  }

  return m_add_op_kind;
}

bool Token::IsMulOp() const {
  if (m_kind == kPointer) {
    return true;
  }

  if (m_kind == kUnaryOp) {
    return m_unary_op_kind == kAddress;
  }

  return m_kind == kMulOp;
}

Token::MulOpKind Token::mul_op_kind() const {
  assert(IsMulOp());

  if (m_kind == kPointer) {
    return kMultiply;
  }

  if (m_kind == kUnaryOp) {
    return kBitAnd;
  }

  return m_mul_op_kind;
}

bool Token::IsUnaryOp() const {
  if (m_kind == kPointer) {
    return true;
  }

  return m_kind == kUnaryOp;
}

Token::UnaryOpKind Token::unary_op_kind() const {
  assert(IsUnaryOp());

  if (m_kind == kPointer) {
    return kIndirection;
  }

  return m_unary_op_kind;
}

bool Token::IsAssignOp() const {
  return m_kind == kAssignOp;
}

Token::AssignOpKind Token::assign_op_kind() const {
  assert(IsAssignOp());
  return m_assign_op_kind;
}

const std::string& Token::identifier() const {
  assert(m_kind == kIdentifier);
  return m_identifier;
}

value::UntypedConstant const & Token::value() const {
  assert(m_kind == kLiteral);
  return m_value;
}

std::ostream& operator<<(std::ostream& out, const Token::Kind kind) {
  switch (kind) {
    case Token::kIdentifier:
      out << "identifier";
      break;

    case Token::kBreak:
      out << "break";
      break;
    case Token::kCase:
      out << "case";
      break;
    case Token::kConst:
      out << "const";
      break;
    case Token::kContinue:
      out << "continue";
      break;
    case Token::kDefault:
      out << "default";
      break;
    case Token::kDefer:
      out << "defer";
      break;
    case Token::kElse:
      out << "else";
      break;
    case Token::kFallthrough:
      out << "fallthrough";
      break;
    case Token::kFor:
      out << "for";
      break;
    case Token::kFunc:
      out << "func";
      break;
    case Token::kGoto:
      out << "goto";
      break;
    case Token::kIf:
      out << "if";
      break;
    case Token::kImport:
      out << "import";
      break;
    case Token::kInterface:
      out << "interface";
      break;
    case Token::kMap:
      out << "map";
      break;
    case Token::kPackage:
      out << "package";
      break;
    case Token::kRange:
      out << "range";
      break;
    case Token::kReturn:
      out << "return";
      break;
    case Token::kStruct:
      out << "struct";
      break;
    case Token::kSwitch:
      out << "switch";
      break;
    case Token::kType:
      out << "type";
      break;
    case Token::kVar:
      out << "var";
      break;

    case Token::kAssign:
      out << "=";
      break;
    case Token::kColon:
      out << ":";
      break;
    case Token::kComma:
      out << ",";
      break;
    case Token::kDecrement:
      out << "--";
      break;
    case Token::kEllipsis:
      out << "...";
      break;
    case Token::kIncrement:
      out << "++";
      break;
    case Token::kLeftBrace:
      out << "{";
      break;
    case Token::kLeftBracket:
      out << "[";
      break;
    case Token::kLeftParen:
      out << "(";
      break;
    case Token::kLogicalAnd:
      out << "&&";
      break;
    case Token::kLogicalOr:
      out << "||";
      break;
    case Token::kPeriod:
      out << ".";
      break;
    case Token::kPointer:
      out << "*";
      break;
    case Token::kRightBrace:
      out << "}";
      break;
    case Token::kRightBracket:
      out << "]";
      break;
    case Token::kRightParen:
      out << ")";
      break;
    case Token::kSemicolon:
      out << ";";
      break;
    case Token::kShortAssign:
      out << ":=";
      break;

    case Token::kRelOp:
      out << "RelOp";
      break;
    case Token::kAddOp:
      out << "AddOp";
      break;
    case Token::kMulOp:
      out << "MulOp";
      break;
    case Token::kUnaryOp:
      out << "UnaryOp";
      break;
    case Token::kAssignOp:
      out << "AssignOp";
      break;

    case Token::kLiteral:
      out << "Literal";
      break;
    case Token::kEnd:
      out << "End";
      break;
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const Token::RelOpKind a_kind) {
  switch (a_kind) {
    case Token::kEqual:
      out << "==";
      break;
    case Token::kGreaterThan:
      out << ">";
      break;
    case Token::kGreaterThanEqual:
      out << ">=";
      break;
    case Token::kLessThan:
      out << "<";
      break;
    case Token::kLessThanEqual:
      out << "<=";
      break;
    case Token::kNotEqual:
      out << "!=";
      break;
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const Token::AddOpKind a_kind) {
  switch (a_kind) {
    case Token::kAdd:
      out << "+";
      break;
    case Token::kBitOr:
      out << "|";
      break;
    case Token::kBitXor:
      out << "^";
      break;
    case Token::kSubtract:
      out << "-";
      break;
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const Token::MulOpKind a_kind) {
  switch (a_kind) {
    case Token::kMultiply:
      out << "*";
      break;
    case Token::kDivide:
      out << "/";
      break;
    case Token::kModulus:
      out << "%";
      break;
    case Token::kLeftShift:
      out << "<<";
      break;
    case Token::kRightShift:
      out << ">>";
      break;
    case Token::kBitAnd:
      out << "&";
      break;
    case Token::kBitAndNot:
      out << "&^";
      break;
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const Token::UnaryOpKind a_kind) {
  switch (a_kind) {
    case Token::kPosate:
      out << "+";
      break;
    case Token::kNegate:
      out << "-";
      break;
    case Token::kLogicalNot:
      out << "!";
      break;
    case Token::kBitNot:
      out << "^";
      break;
    case Token::kIndirection:
      out << "*";
      break;
    case Token::kAddress:
      out << "&";
      break;
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const Token::AssignOpKind a_kind) {
  switch (a_kind) {
    case Token::kAddAssign:
      out << "+=";
      break;
    case Token::kBitAndAssign:
      out << "&=";
      break;
    case Token::kBitAndNotAssign:
      out << "&^=";
      break;
    case Token::kBitOrAssign:
      out << "|=";
      break;
    case Token::kBitXorAssign:
      out << "^=";
      break;
    case Token::kDivideAssign:
      out << "/=";
      break;
    case Token::kLeftShiftAssign:
      out << "<<=";
      break;
    case Token::kModulusAssign:
      out << "%=";
      break;
    case Token::kMultiplyAssign:
      out << "*=";
      break;
    case Token::kRightShiftAssign:
      out << ">>=";
      break;
    case Token::kSubtractAssign:
      out << "-=";
      break;
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const Token& token) {
  switch (token.kind()) {
    case Token::kIdentifier:
      out << token.identifier();
      break;
    case Token::kLiteral:
      out << token.value();
      break;
    case Token::kRelOp:
      out << token.rel_op_kind();
      break;
    case Token::kAddOp:
      out << token.add_op_kind();
      break;
    case Token::kMulOp:
      out << token.mul_op_kind();
      break;
    case Token::kUnaryOp:
      out << token.unary_op_kind();
      break;
    case Token::kAssignOp:
      out << token.assign_op_kind();
      break;
    default:
      out << token.kind();
      break;
  }
  return out;
}

}  // namespace rcgo
