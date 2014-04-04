/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Token.hpp"
#include "ErrorReporter.hpp"

#include <sstream>

#define SPACE 0x20
#define TAB 0x09
#define CARRIAGE_RETURN 0x0D
#define NEWLINE 0x0A

#define PRECISION 256

namespace rcgo {

  Token Token::make (const Location& location, Token::Kind kind)
  {
    Token t;
    t.m_location = location;
    t.m_kind = kind;
    return t;
  }

  Token Token::make (RelOpKind a_kind)
  {
    Token t;
    t.m_location = Location::builtin;
    t.m_kind = Token::RelOp;
    t.m_rel_op_kind = a_kind;
    return t;
  }

  Token Token::make (AddOpKind a_kind)
  {
    assert (a_kind != Token::Add &&
            a_kind != Token::Subtract &&
            a_kind != Token::BitXor);
    Token t;
    t.m_location = Location::builtin;
    t.m_kind = Token::AddOp;
    t.m_add_op_kind = a_kind;
    return t;
  }

  Token Token::make (MulOpKind a_kind)
  {
    assert (a_kind != Token::Multiply &&
            a_kind != Token::BitAnd);
    Token t;
    t.m_location = Location::builtin;
    t.m_kind = Token::MulOp;
    t.m_mul_op_kind = a_kind;
    return t;
  }

  Token Token::make (UnaryOpKind a_kind)
  {
    assert (a_kind != Token::Indirection);
    Token t;
    t.m_location = Location::builtin;
    t.m_kind = Token::UnaryOp;
    t.m_unary_op_kind = a_kind;
    return t;
  }

  Token Token::make (AssignOpKind a_kind)
  {
    Token t;
    t.m_location = Location::builtin;
    t.m_kind = Token::AssignOp;
    t.m_assign_op_kind = a_kind;
    return t;
  }

  Token Token::make (Kind a_kind)
  {
    Token t;
    t.m_location = Location::builtin;
    t.m_kind = a_kind;
    return t;
  }

  Token Token::make (const Location& a_location, const Token& a_token)
  {
    Token t (a_token);
    t.m_location = a_location;
    return t;
  }

  Token Token::make_identifier (const Location& location, const std::string& s)
  {
    Token t;
    t.m_location = location;
    t.m_kind = Identifier;
    t.m_identifier = s;
    return t;
  }

  Token Token::make_literal (const Location& location, const Value& a_value)
  {
    Token t;
    t.m_location = location;
    t.m_kind = Literal;
    t.m_value = a_value;
    return t;
  }

  bool Token::operator== (Token::Kind k) const
  {
    return m_kind == k;
  }

  bool Token::operator!= (Token::Kind k) const
  {
    return m_kind != k;
  }

  const Location& Token::location () const
  {
    return m_location;
  }

  Token::Kind Token::kind () const
  {
    return m_kind;
  }

  bool Token::is_rel_op () const
  {
    return m_kind == Token::RelOp;
  }

  Token::RelOpKind Token::rel_op_kind () const
  {
    assert (is_rel_op ());
    return m_rel_op_kind;
  }

  bool Token::is_add_op () const
  {
    if (m_kind == Token::UnaryOp)
      {
        switch (m_unary_op_kind)
          {
          case Token::Posate:
          case Token::Negate:
          case Token::BitNot:
            return true;
          default:
            return false;
          }
      }

    return m_kind == Token::AddOp;
  }

  Token::AddOpKind Token::add_op_kind () const
  {
    assert (is_add_op ());

    if (m_kind == Token::UnaryOp)
      {
        switch (m_unary_op_kind)
          {
          case Token::Posate:
            return Token::Add;
          case Token::Negate:
            return Token::Subtract;
          case Token::BitNot:
            return Token::BitXor;
          default:
            abort (); // NOT_COVERED
          }
      }

    return m_add_op_kind;
  }

  bool Token::is_mul_op () const
  {
    if (m_kind == Token::Pointer)
      {
        return true;
      }

    if (m_kind == Token::UnaryOp)
      {
        return m_unary_op_kind == Token::Address;
      }

    return m_kind == Token::MulOp;
  }

  Token::MulOpKind Token::mul_op_kind () const
  {
    assert (is_mul_op ());

    if (m_kind == Token::Pointer)
      {
        return Token::Multiply;
      }

    if (m_kind == Token::UnaryOp)
      {
        return Token::BitAnd;
      }

    return m_mul_op_kind;
  }

  bool Token::is_unary_op () const
  {
    if (m_kind == Token::Pointer)
      {
        return true;
      }

    return m_kind == Token::UnaryOp;
  }

  Token::UnaryOpKind Token::unary_op_kind () const
  {
    assert (is_unary_op ());

    if (m_kind == Token::Pointer)
      {
        return Token::Indirection;
      }

    return m_unary_op_kind;
  }

  bool Token::is_assign_op () const
  {
    return m_kind == Token::AssignOp;
  }

  Token::AssignOpKind Token::assign_op_kind () const
  {
    assert (is_assign_op ());
    return m_assign_op_kind;
  }

  const std::string& Token::identifier () const
  {
    assert (m_kind == Identifier);
    return m_identifier;
  }

  const Value& Token::value () const {
    assert (m_kind == Literal);
    return m_value;
  }

  std::ostream& operator<< (std::ostream& out, const Token::Kind kind)
  {
    switch (kind)
      {
      case Token::Identifier:
        out << "identifier";
        break;

      case Token::Break:
        out << "break";
        break;
      case Token::Case:
        out << "case";
        break;
      case Token::Const:
        out << "const";
        break;
      case Token::Continue:
        out << "continue";
        break;
      case Token::Default:
        out << "default";
        break;
      case Token::Defer:
        out << "defer";
        break;
      case Token::Else:
        out << "else";
        break;
      case Token::Fallthrough:
        out << "fallthrough";
        break;
      case Token::For:
        out << "for";
        break;
      case Token::Func:
        out << "func";
        break;
      case Token::Goto:
        out << "goto";
        break;
      case Token::If:
        out << "if";
        break;
      case Token::Import:
        out << "import";
        break;
      case Token::Interface:
        out << "interface";
        break;
      case Token::Map:
        out << "map";
        break;
      case Token::Package:
        out << "package";
        break;
      case Token::Range:
        out << "range";
        break;
      case Token::Return:
        out << "return";
        break;
      case Token::Struct:
        out << "struct";
        break;
      case Token::Switch:
        out << "switch";
        break;
      case Token::Type:
        out << "type";
        break;
      case Token::Var:
        out << "var";
        break;

      case Token::Assign:
        out << "=";
        break;
      case Token::Colon:
        out << ":";
        break;
      case Token::Comma:
        out << ",";
        break;
      case Token::Decrement:
        out << "--";
        break;
      case Token::Ellipsis:
        out << "...";
        break;
      case Token::Increment:
        out << "++";
        break;
      case Token::LeftBrace:
        out << "{";
        break;
      case Token::LeftBracket:
        out << "[";
        break;
      case Token::LeftParen:
        out << "(";
        break;
      case Token::LogicalAnd:
        out << "&&";
        break;
      case Token::LogicalOr:
        out << "||";
        break;
      case Token::Period:
        out << ".";
        break;
      case Token::Pointer:
        out << "*";
        break;
      case Token::RightBrace:
        out << "}";
        break;
      case Token::RightBracket:
        out << "]";
        break;
      case Token::RightParen:
        out << ")";
        break;
      case Token::Semicolon:
        out << ";";
        break;
      case Token::ShortAssign:
        out << ":=";
        break;

      case Token::RelOp:
        out << "RelOp";
        break;
      case Token::AddOp:
        out << "AddOp";
        break;
      case Token::MulOp:
        out << "MulOp";
        break;
      case Token::UnaryOp:
        out << "UnaryOp";
        break;
      case Token::AssignOp:
        out << "AssignOp";
        break;

      case Token::Literal:
        out << "Literal";
        break;
      case Token::End:
        out << "End";
        break;
      }
    return out;
  }

  std::ostream& operator<< (std::ostream& out, const Token::RelOpKind a_kind)
  {
    switch (a_kind)
      {
      case Token::Equal:
        out << "==";
        break;
      case Token::GreaterThan:
        out << ">";
        break;
      case Token::GreaterThanEqual:
        out << ">=";
        break;
      case Token::LessThan:
        out << "<";
        break;
      case Token::LessThanEqual:
        out << "<=";
        break;
      case Token::NotEqual:
        out << "!=";
        break;
      }
    return out;
  }

  std::ostream& operator<< (std::ostream& out, const Token::AddOpKind a_kind)
  {
    switch (a_kind)
      {
      case Token::Add:
        out << "+";
        break;
      case Token::BitOr:
        out << "|";
        break;
      case Token::BitXor:
        out << "^";
        break;
      case Token::Subtract:
        out << "-";
        break;
      }
    return out;
  }

  std::ostream& operator<< (std::ostream& out, const Token::MulOpKind a_kind)
  {
    switch (a_kind)
      {
      case Token::Multiply:
        out << "*";
        break;
      case Token::Divide:
        out << "/";
        break;
      case Token::Modulus:
        out << "%";
        break;
      case Token::LeftShift:
        out << "<<";
        break;
      case Token::RightShift:
        out << ">>";
        break;
      case Token::BitAnd:
        out << "&";
        break;
      case Token::BitAndNot:
        out << "&^";
        break;
      }
    return out;
  }

  std::ostream& operator<< (std::ostream& out, const Token::UnaryOpKind a_kind)
  {
    switch (a_kind)
      {
      case Token::Posate:
        out << "+";
        break;
      case Token::Negate:
        out << "-";
        break;
      case Token::LogicalNot:
        out << "!";
        break;
      case Token::BitNot:
        out << "^";
        break;
      case Token::Indirection:
        out << "*";
        break;
      case Token::Address:
        out << "&";
        break;
      }
    return out;
  }

  std::ostream& operator<< (std::ostream& out, const Token::AssignOpKind a_kind)
  {
    switch (a_kind)
      {
      case Token::AddAssign:
        out << "+=";
        break;
      case Token::BitAndAssign:
        out << "&=";
        break;
      case Token::BitAndNotAssign:
        out << "&^=";
        break;
      case Token::BitOrAssign:
        out << "|=";
        break;
      case Token::BitXorAssign:
        out << "^=";
        break;
      case Token::DivideAssign:
        out << "/=";
        break;
      case Token::LeftShiftAssign:
        out << "<<=";
        break;
      case Token::ModulusAssign:
        out << "%=";
        break;
      case Token::MultiplyAssign:
        out << "*=";
        break;
      case Token::RightShiftAssign:
        out << ">>=";
        break;
      case Token::SubtractAssign:
        out << "-=";
        break;
      }
    return out;
  }

  std::ostream& operator<< (std::ostream& out, const Token& token)
  {
    switch (token.kind ())
      {
      case Token::Identifier:
        out << token.identifier ();
        break;
      case Token::Literal:
        out << token.value ();
        break;
      case Token::RelOp:
        out << token.rel_op_kind ();
        break;
      case Token::AddOp:
        out << token.add_op_kind ();
        break;
      case Token::MulOp:
        out << token.mul_op_kind ();
        break;
      case Token::UnaryOp:
        out << token.unary_op_kind ();
        break;
      case Token::AssignOp:
        out << token.assign_op_kind ();
        break;
      default:
        out << token.kind ();
        break;
      }
    return out;
  }
}
