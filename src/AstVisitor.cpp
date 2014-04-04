/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "AstVisitor.hpp"

#define VISIT(c) void DefaultAstVisitor::visit (c& ast) { default_action (ast); }

namespace rcgo {

  AstVisitor::AstVisitor ()
  { }

  AstVisitor::~AstVisitor ()
  { }

  DefaultAstVisitor::DefaultAstVisitor ()
  { }

  DefaultAstVisitor::~DefaultAstVisitor ()
  { }

  VISIT (ListAst)
  VISIT (ArrayAst)
  VISIT (SliceTypeAst)
  VISIT (StructAst)
  VISIT (InterfaceAst)
  VISIT (MapAst)
  VISIT (EmbeddedFieldAst)
  VISIT (FieldAst)
  VISIT (AnonymousMethodSpecAst)
  VISIT (MethodSpecAst)
  VISIT (PointerAst)
  VISIT (FunctionTypeAst)
  VISIT (SignatureAst)
  VISIT (ParameterDeclAst)
  VISIT (LiteralAst)
  VISIT (CompositeLiteralAst)
  VISIT (KeyedElementAst)
  VISIT (FunctionLiteralAst)
  VISIT (IdentifierAst)
  VISIT (ConstSpecAst)
  VISIT (TypeSpecAst)
  VISIT (VarSpecAst)
  VISIT (FuncDeclAst)
  VISIT (MethodDeclAst)
  VISIT (TypeAssertionAst)
  VISIT (SelectorAst)
  VISIT (SliceAst)
  VISIT (IndexAst)
  VISIT (CallAst)
  VISIT (BinaryAst)
  VISIT (AssignAst)
  VISIT (UnaryAst)
  VISIT (TypeExpressionAst)
  VISIT (OrAst)
  VISIT (AndAst)
  VISIT (LabeledStatementAst)
  VISIT (GotoAst)
  VISIT (FallthroughAst)
  VISIT (DeferAst)
  VISIT (EmptyStatementAst)
  VISIT (ExpressionStatementAst)
  VISIT (IncrementAst)
  VISIT (DecrementAst)
  VISIT (ReturnAst)
  VISIT (BreakAst)
  VISIT (ContinueAst)
  VISIT (IfAst)
  VISIT (SwitchAst)
  VISIT (CaseAst)
  VISIT (ForAst)
  VISIT (RangeAst)
  VISIT (ForClauseAst)
  VISIT (SourceFileAst)
  VISIT (ImportDeclListAst)
  VISIT (ImportDeclAst)
  VISIT (ImportSpecAst)
  VISIT (ErrorAst)

  void DefaultAstVisitor::default_action (Ast& ast)
  { }

  IdentifierAst* to_identifier (Ast* ast)
  {
    return ast_cast<IdentifierAst> (ast);
  }
}
