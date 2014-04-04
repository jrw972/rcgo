#ifndef RCGO_SRC_TOKEN_HPP
#define RCGO_SRC_TOKEN_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.hpp"

#include <gmpxx.h>

#include "Location.hpp"
#include "Utf8.hpp"
#include "Value.hpp"

namespace rcgo {

  struct Token { // NOT_COVERED

    enum RelOpKind {
      Equal,
      NotEqual,
      LessThan,
      LessThanEqual,
      GreaterThan,
      GreaterThanEqual,
    };

    enum AddOpKind {
      Add,      // Posate
      Subtract, // Negate
      BitOr,
      BitXor,   // BitNot
    };

    enum MulOpKind {
      Multiply, // Indirection
      Divide,
      Modulus,
      LeftShift,
      RightShift,
      BitAnd,   // Address
      BitAndNot,
    };

    enum UnaryOpKind {
      Posate,
      Negate,
      LogicalNot,
      BitNot,
      Indirection, // Pointer
      Address,
    };

    enum AssignOpKind {
      AddAssign,
      SubtractAssign,
      BitOrAssign,
      BitXorAssign,

      MultiplyAssign,
      DivideAssign,
      ModulusAssign,
      LeftShiftAssign,
      RightShiftAssign,
      BitAndAssign,
      BitAndNotAssign,
    };

    enum Kind {
      Identifier,

      // Keywords
      Break,
      Case,
      Const,
      Continue,
      Default,
      Defer,
      Else,
      Fallthrough,
      For,
      Func,
      Goto,
      If,
      Import,
      Interface,
      Map,
      Package,
      Range,
      Return,
      Struct,
      Switch,
      Type,
      Var,

      // Operators and Delimiters
      Assign,
      Colon,
      Comma,
      Decrement,
      Ellipsis,
      Increment,
      LeftBrace,
      LeftBracket,
      LeftParen,
      LogicalAnd,
      LogicalOr,
      Period,
      Pointer,
      RightBrace,
      RightBracket,
      RightParen,
      Semicolon,
      ShortAssign,

      RelOp,
      AddOp,
      MulOp,
      UnaryOp,
      AssignOp,

      // Literals
      Literal,

      End,
    };

    static Token make (const Location& a_location, Kind a_kind);
    static Token make (RelOpKind a_kind);
    static Token make (AddOpKind a_kind);
    static Token make (MulOpKind a_kind);
    static Token make (UnaryOpKind a_kind);
    static Token make (AssignOpKind a_kind);
    static Token make (Kind a_kind);
    static Token make (const Location& a_location, const Token& a_token);
    static Token make_identifier (const Location& a_location,
                                  const std::string& a_identifier);
    static Token make_literal (const Location& a_location,
                               const Value& a_value);

    bool operator== (Kind kind) const;
    bool operator!= (Kind kind) const;

    const Location& location () const;
    Kind kind () const;
    bool is_rel_op () const;
    RelOpKind rel_op_kind () const;
    bool is_add_op () const;
    AddOpKind add_op_kind () const;
    bool is_mul_op () const;
    MulOpKind mul_op_kind () const;
    bool is_unary_op () const;
    UnaryOpKind unary_op_kind () const;
    bool is_assign_op () const;
    AssignOpKind assign_op_kind () const;
    const std::string& identifier () const;
    const Value& value () const;

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

  std::ostream& operator<< (std::ostream& a_out, const Token::Kind a_kind);
  std::ostream& operator<< (std::ostream& a_out, const Token::RelOpKind a_kind);
  std::ostream& operator<< (std::ostream& a_out, const Token::AddOpKind a_kind);
  std::ostream& operator<< (std::ostream& a_out, const Token::MulOpKind a_kind);
  std::ostream& operator<< (std::ostream& a_out, const Token::UnaryOpKind a_kind);
  std::ostream& operator<< (std::ostream& a_out, const Token::AssignOpKind a_kind);
  std::ostream& operator<< (std::ostream& a_out, const Token& a_token);
}

#endif // RCGO_SRC_TOKEN_HPP
