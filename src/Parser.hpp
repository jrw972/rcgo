#ifndef RCGO_SRC_PARSER_HPP
#define RCGO_SRC_PARSER_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.hpp"

#include "Scanner.hpp"

namespace rcgo {

  struct Parser {
    Parser (ByteStream& a_byte_stream,
            ErrorReporter& a_error_reporter,
            std::ostream& a_out);
    ~Parser ();

    Ast* Identifier ();
    Ast* Type ();
    Ast* TypeName ();
    Ast* FieldDecl ();
    Ast* Signature ();
    Ast* Parameters ();
    Ast* MethodSpec ();
    Ast* Operand ();
    Ast* Block ();
    Ast* StatementList ();
    Ast* Declaration ();
    Ast* TopLevelDecl ();
    Ast* ConstDecl ();
    Ast* ConstSpec ();
    Ast* IdentifierList (Ast* first = NULL);
    Ast* ExpressionList (bool prefer_no_list = false);
    Ast* TypeDecl ();
    Ast* TypeSpec ();
    Ast* VarDecl ();
    Ast* VarSpec ();
    Ast* FuncDecl ();
    Ast* MethodDecl ();
    Ast* LiteralValue ();
    Ast* KeyedElement ();
    Ast* PrimaryExpr ();
    Ast* PrimaryExprSuffix (Ast* operand);
    Ast* OptionalSelector (Ast* operand);
    Ast* Expression ();
    Ast* OrExpr ();
    Ast* AndExpr ();
    Ast* RelExpr ();
    Ast* AddExpr ();
    Ast* MulExpr ();
    Ast* UnaryExpr ();
    Ast* Statement ();
    Ast* SimpleStmt ();
    Ast* ReturnStmt ();
    Ast* BreakStmt ();
    Ast* ContinueStmt ();
    Ast* IfStmt ();
    Ast* SwitchStmt ();
    Ast* SwitchBody ();
    Ast* ForStmt ();
    Ast* SourceFile ();
    Ast* SourceFileHeader ();
    Ast* ImportDecl ();
    Ast* ImportSpec ();

    void trace (bool t);
    bool trace () const;

  private:
    Scanner m_scanner;
    ErrorReporter& m_error_reporter;
    std::ostream& m_out;
    bool m_trace;
    int m_expression_level;

    Token peek (size_t offset = 0);
    bool got (Token::Kind kind);
    Token next ();
    bool want (Token::Kind kind);
    void advance (Token::Kind kind, ...);

    bool optional_semicolon (Token::Kind follow);
  };

}

#endif // RCGO_SRC_PARSER_HPP
