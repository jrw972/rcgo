/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"

#include "AstVisitor.hpp"
#include "Ast.hpp"
#include "Token.hpp"

using namespace rcgo;

static Location loc;

#define VISIT_TEST(c, args...) TEST_CASE ("DefaultAstVisitor::visit(" #c ")") { \
  c* ast = new c (args);                                                  \
  DefaultAstVisitor visitor; \
  visitor.visit (*ast); \
  delete ast; \
}

VISIT_TEST (ListAst, loc);
VISIT_TEST (ArrayAst, loc, new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (SliceTypeAst, loc, new ErrorAst (loc));
VISIT_TEST (StructAst, loc);
VISIT_TEST (InterfaceAst, loc);
VISIT_TEST (MapAst, loc, new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (EmbeddedFieldAst, loc, false, new ErrorAst (loc), "");
VISIT_TEST (FieldAst, new ErrorAst (loc), new ErrorAst (loc), "");
VISIT_TEST (AnonymousMethodSpecAst, new ErrorAst (loc));
VISIT_TEST (MethodSpecAst, new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (PointerAst, loc, new ErrorAst (loc));
VISIT_TEST (FunctionTypeAst, loc, new ErrorAst (loc));
VISIT_TEST (SignatureAst, new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (ParameterDeclAst, new ErrorAst (loc), false, new ErrorAst (loc));
VISIT_TEST (LiteralAst, Token::make_literal (loc, Value::make_string ("")));
VISIT_TEST (CompositeLiteralAst, new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (KeyedElementAst, new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (FunctionLiteralAst, loc, new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (IdentifierAst, Token::make_identifier (loc, ""));
VISIT_TEST (ConstSpecAst, loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (TypeSpecAst, new ErrorAst (loc), new ErrorAst (loc), false);
VISIT_TEST (VarSpecAst, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (FuncDeclAst, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (MethodDeclAst, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (TypeAssertionAst, new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (SelectorAst, new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (SliceAst, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (IndexAst, new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (CallAst, new ErrorAst (loc), new ErrorAst (loc), false);
VISIT_TEST (BinaryAst, BinaryAst::Add, new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (AssignAst, AssignAst::Add, new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (UnaryAst, loc, UnaryAst::Posate, new ErrorAst (loc));
VISIT_TEST (TypeExpressionAst, loc);
VISIT_TEST (OrAst, loc);
VISIT_TEST (AndAst, loc);
VISIT_TEST (LabeledStatementAst, new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (GotoAst, loc, new ErrorAst (loc));
VISIT_TEST (FallthroughAst, loc);
VISIT_TEST (DeferAst, loc, new ErrorAst (loc));
VISIT_TEST (EmptyStatementAst, loc);
VISIT_TEST (ExpressionStatementAst, new ErrorAst (loc));
VISIT_TEST (IncrementAst, new ErrorAst (loc));
VISIT_TEST (DecrementAst, new ErrorAst (loc));
VISIT_TEST (ReturnAst, loc, new ErrorAst (loc));
VISIT_TEST (BreakAst, loc, new ErrorAst (loc));
VISIT_TEST (ContinueAst, loc, new ErrorAst (loc));
VISIT_TEST (IfAst, loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (SwitchAst, loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (CaseAst, loc, new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (ForAst, loc, new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (RangeAst, loc, new ErrorAst (loc));
VISIT_TEST (ForClauseAst, loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (SourceFileAst, loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_TEST (ImportDeclListAst, loc);
VISIT_TEST (ImportDeclAst, loc);
VISIT_TEST (ImportSpecAst, loc, false, new ErrorAst (loc), "");
VISIT_TEST (ErrorAst, loc);

TEST_CASE ("to_identifier") {
  Ast* ast = new IdentifierAst (Token::make_identifier (loc, ""));
  IdentifierAst* id = to_identifier (ast);
  REQUIRE (id != NULL);
  delete id;
}
