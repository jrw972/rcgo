#ifndef SRC_PARSER_H_
#define SRC_PARSER_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/ast.h"
#include "src/scanner.h"

namespace rcgo {

struct Parser {
  Parser(ByteStreamI* a_byte_stream, ErrorReporter* a_error_reporter,
         std::ostream& a_out)
      : m_scanner(a_byte_stream, a_error_reporter),
        m_error_reporter(a_error_reporter), m_out(a_out), m_trace(false),
        m_expression_level(0) {}
  ~Parser() {}

  ast::Node* Identifier();
  ast::Node* Type();
  ast::Node* TypeName();
  ast::Node* FieldDecl();
  ast::Node* Signature();
  ast::ListType Parameters();
  ast::Node* MethodSpec();
  ast::Node* Operand();
  ast::Node* Block();
  ast::ListType StatementList();
  ast::Node* Declaration();
  ast::Node* TopLevelDecl();
  ast::Node* ConstDecl();
  ast::Node* ConstSpec();
  ast::ListType IdentifierList(ast::Node* first = NULL);
  ast::ListType ExpressionList();
  ast::Node* TypeDecl();
  ast::Node* TypeSpec();
  ast::Node* VarDecl();
  ast::Node* VarSpec();
  ast::Node* FuncDecl();
  ast::Node* MethodDecl();
  ast::Node* LiteralValue();
  ast::Node* KeyedElement();
  ast::Node* PrimaryExpr();
  ast::Node* PrimaryExprSuffix(ast::Node* operand);
  ast::Node* OptionalSelector(ast::Node* operand);
  ast::Node* Expression();
  ast::Node* OrExpr();
  ast::Node* AndExpr();
  ast::Node* RelExpr();
  ast::Node* AddExpr();
  ast::Node* MulExpr();
  ast::Node* UnaryExpr();
  ast::Node* Statement();
  ast::Node* SimpleStmt();
  ast::Node* ReturnStmt();
  ast::Node* BreakStmt();
  ast::Node* ContinueStmt();
  ast::Node* IfStmt();
  ast::Node* SwitchStmt();
  ast::ListType SwitchBody();
  ast::Node* ForStmt();
  ast::SourceFile* SourceFile();
  ast::Node* SourceFileHeader();
  ast::Node* ImportDecl();
  ast::Node* ImportSpec();

  void trace(bool t);
  bool trace() const;

 private:
  Scanner m_scanner;
  ErrorReporter* m_error_reporter;
  std::ostream& m_out;
  bool m_trace;
  int m_expression_level;

  Token Peek(size_t offset = 0);
  bool Got(Token::Kind kind);
  Token Next();
  bool Want(Token::Kind kind);
  void Advance(Token::Kind kind, ...);

  bool OptionalSemicolon(Token::Kind follow);
};

}  // namespace rcgo

#endif  // SRC_PARSER_H_
