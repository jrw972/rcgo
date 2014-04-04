/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"
#include "spies.hpp"

#include "Ast.hpp"
#include "AstVisitor.hpp"
#include "Token.hpp"

using namespace rcgo;

static Location loc;

#define ACCEPT_TEST(c, args...) TEST_CASE (#c "::accept(visitor)") {    \
    c* ast = new c (args);                                              \
    AcceptTestVisitor<DefaultAstVisitor, Ast> visitor;                  \
    ast->accept (visitor);                                              \
    REQUIRE (ast == visitor.t);                                         \
    delete ast;                                                         \
  }

ACCEPT_TEST (ListAst, loc);
ACCEPT_TEST (ArrayAst, loc, new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (SliceTypeAst, loc, new ErrorAst (loc));
ACCEPT_TEST (StructAst, loc);
ACCEPT_TEST (InterfaceAst, loc);
ACCEPT_TEST (MapAst, loc, new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (EmbeddedFieldAst, loc, false, new ErrorAst (loc), "");
ACCEPT_TEST (FieldAst, new ErrorAst (loc), new ErrorAst (loc), "");
ACCEPT_TEST (AnonymousMethodSpecAst, new ErrorAst (loc));
ACCEPT_TEST (MethodSpecAst, new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (PointerAst, loc, new ErrorAst (loc));
ACCEPT_TEST (FunctionTypeAst, loc, new ErrorAst (loc));
ACCEPT_TEST (SignatureAst, new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (ParameterDeclAst, new ErrorAst (loc), false, new ErrorAst (loc));
ACCEPT_TEST (LiteralAst, Token::make_literal (loc, Value::make_string ("")));
ACCEPT_TEST (CompositeLiteralAst, new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (KeyedElementAst, new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (FunctionLiteralAst, loc, new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (IdentifierAst, Token::make_identifier (loc, ""));
ACCEPT_TEST (ConstSpecAst, loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (TypeSpecAst, new ErrorAst (loc), new ErrorAst (loc), false);
ACCEPT_TEST (VarSpecAst, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (FuncDeclAst, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (MethodDeclAst, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (TypeAssertionAst, new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (SelectorAst, new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (SliceAst, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (IndexAst, new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (CallAst, new ErrorAst (loc), new ErrorAst (loc), false);
ACCEPT_TEST (BinaryAst, BinaryAst::Add, new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (AssignAst, AssignAst::Add, new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (UnaryAst, loc, UnaryAst::Posate, new ErrorAst (loc));
ACCEPT_TEST (TypeExpressionAst, loc);
ACCEPT_TEST (OrAst, loc);
ACCEPT_TEST (AndAst, loc);
ACCEPT_TEST (LabeledStatementAst, new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (GotoAst, loc, new ErrorAst (loc));
ACCEPT_TEST (FallthroughAst, loc);
ACCEPT_TEST (DeferAst, loc, new ErrorAst (loc));
ACCEPT_TEST (EmptyStatementAst, loc);
ACCEPT_TEST (ExpressionStatementAst, new ErrorAst (loc));
ACCEPT_TEST (IncrementAst, new ErrorAst (loc));
ACCEPT_TEST (DecrementAst, new ErrorAst (loc));
ACCEPT_TEST (ReturnAst, loc, new ErrorAst (loc));
ACCEPT_TEST (BreakAst, loc, new ErrorAst (loc));
ACCEPT_TEST (ContinueAst, loc, new ErrorAst (loc));
ACCEPT_TEST (IfAst, loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (SwitchAst, loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (CaseAst, loc, new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (ForAst, loc, new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (RangeAst, loc, new ErrorAst (loc));
ACCEPT_TEST (ForClauseAst, loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (SourceFileAst, loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
ACCEPT_TEST (ImportDeclListAst, loc);
ACCEPT_TEST (ImportDeclAst, loc);
ACCEPT_TEST (ImportSpecAst, loc, false, new ErrorAst (loc), "");
ACCEPT_TEST (ErrorAst, loc);

struct VisitChildrenTestVisitor : public DefaultAstVisitor {
  unsigned int n;

  VisitChildrenTestVisitor ()
    : n (0)
  { }

  virtual void default_action (Ast& ast) { ++n; }
};

#define VISIT_CHILDREN_TEST(c, N, args...) TEST_CASE (#c "::visit_children(visitor)") { \
    c* ast = new c (args);                                              \
    VisitChildrenTestVisitor visitor;                                   \
    ast->visit_children (visitor);                                      \
    REQUIRE (N == visitor.n);                                           \
    delete ast;                                                         \
}

VISIT_CHILDREN_TEST (ListAst, 1, new ErrorAst (loc));
VISIT_CHILDREN_TEST (ArrayAst, 2, loc, new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (SliceTypeAst, 1, loc, new ErrorAst (loc));
VISIT_CHILDREN_TEST (StructAst, 0, loc);
VISIT_CHILDREN_TEST (InterfaceAst, 0, loc);
VISIT_CHILDREN_TEST (MapAst, 2, loc, new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (EmbeddedFieldAst, 1, loc, false, new ErrorAst (loc), "");
VISIT_CHILDREN_TEST (FieldAst, 2, new ErrorAst (loc), new ErrorAst (loc), "");
VISIT_CHILDREN_TEST (AnonymousMethodSpecAst, 1, new ErrorAst (loc));
VISIT_CHILDREN_TEST (MethodSpecAst, 2, new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (PointerAst, 1, loc, new ErrorAst (loc));
VISIT_CHILDREN_TEST (FunctionTypeAst, 1, loc, new ErrorAst (loc));
VISIT_CHILDREN_TEST (SignatureAst, 2, new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (ParameterDeclAst, 2, new ErrorAst (loc), false, new ErrorAst (loc));
VISIT_CHILDREN_TEST (LiteralAst, 0, Token::make_literal (loc, Value::make_string ("")));
VISIT_CHILDREN_TEST (CompositeLiteralAst, 2, new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (KeyedElementAst, 2, new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (FunctionLiteralAst, 2, loc, new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (IdentifierAst, 0, Token::make_identifier (loc, ""));
VISIT_CHILDREN_TEST (ConstSpecAst, 3, loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (TypeSpecAst, 2, new ErrorAst (loc), new ErrorAst (loc), false);
VISIT_CHILDREN_TEST (VarSpecAst, 3, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (FuncDeclAst, 3, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (MethodDeclAst, 4, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (TypeAssertionAst, 2, new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (SelectorAst, 2, new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (SliceAst, 4, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (IndexAst, 2, new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (CallAst, 2, new ErrorAst (loc), new ErrorAst (loc), false);
VISIT_CHILDREN_TEST (BinaryAst, 2, BinaryAst::Add, new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (AssignAst, 2, AssignAst::Add, new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (UnaryAst, 1, loc, UnaryAst::Posate, new ErrorAst (loc));
VISIT_CHILDREN_TEST (TypeExpressionAst, 0, loc);
VISIT_CHILDREN_TEST (OrAst, 0, loc);
VISIT_CHILDREN_TEST (AndAst, 0, loc);
VISIT_CHILDREN_TEST (LabeledStatementAst, 2, new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (GotoAst, 1, loc, new ErrorAst (loc));
VISIT_CHILDREN_TEST (FallthroughAst, 0, loc);
VISIT_CHILDREN_TEST (DeferAst, 1, loc, new ErrorAst (loc));
VISIT_CHILDREN_TEST (EmptyStatementAst, 0, loc);
VISIT_CHILDREN_TEST (ExpressionStatementAst, 1, new ErrorAst (loc));
VISIT_CHILDREN_TEST (IncrementAst, 1, new ErrorAst (loc));
VISIT_CHILDREN_TEST (DecrementAst, 1, new ErrorAst (loc));
VISIT_CHILDREN_TEST (ReturnAst, 1, loc, new ErrorAst (loc));
VISIT_CHILDREN_TEST (BreakAst, 1, loc, new ErrorAst (loc));
VISIT_CHILDREN_TEST (ContinueAst, 1, loc, new ErrorAst (loc));
VISIT_CHILDREN_TEST (IfAst, 4, loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (SwitchAst, 3, loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (CaseAst, 2, loc, new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (ForAst, 2, loc, new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (RangeAst, 1, loc, new ErrorAst (loc));
VISIT_CHILDREN_TEST (ForClauseAst, 3, loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (SourceFileAst, 3, loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
VISIT_CHILDREN_TEST (ImportDeclListAst, 0, loc);
VISIT_CHILDREN_TEST (ImportDeclAst, 0, loc);
VISIT_CHILDREN_TEST (ImportSpecAst, 1, loc, false, new ErrorAst (loc), "");
VISIT_CHILDREN_TEST (ErrorAst, 0, loc);

#define PRINT_TEST(c, s, args...) TEST_CASE ("operator<< (" #c ")") {   \
    std::stringstream ss;                                               \
    c* ast = new c (args);                                              \
    ss << *ast;                                                         \
    REQUIRE (ss.str () == s);                                           \
    delete ast;                                                         \
}

PRINT_TEST (ListAst,
            "ListAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc));
PRINT_TEST (ArrayAst,
            "ArrayAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (SliceTypeAst,
            "SliceTypeAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc));
PRINT_TEST (StructAst,
            "StructAst\n",
            loc);
PRINT_TEST (InterfaceAst,
            "InterfaceAst\n",
            loc);
PRINT_TEST (MapAst,
            "MapAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (EmbeddedFieldAst,
            "EmbeddedFieldAst\n"
            "  ErrorAst\n",
            loc, false, new ErrorAst (loc), "x");
PRINT_TEST (FieldAst,
            "FieldAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc), new ErrorAst (loc), "x");
PRINT_TEST (AnonymousMethodSpecAst,
            "AnonymousMethodSpecAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc));
PRINT_TEST (MethodSpecAst,
            "MethodSpecAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (PointerAst,
            "PointerAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc));
PRINT_TEST (FunctionTypeAst,
            "FunctionTypeAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc));
PRINT_TEST (SignatureAst,
            "SignatureAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (ParameterDeclAst,
            "ParameterDeclAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc), false, new ErrorAst (loc));
PRINT_TEST (LiteralAst,
            "LiteralAst \"x\"\n",
            Token::make_literal (loc, Value::make_string ("x")));
PRINT_TEST (CompositeLiteralAst,
            "CompositeLiteralAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (KeyedElementAst,
            "KeyedElementAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (FunctionLiteralAst,
            "FunctionLiteralAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (IdentifierAst,
            "IdentifierAst x\n",
            Token::make_identifier (loc, "x"));
PRINT_TEST (ConstSpecAst,
            "ConstSpecAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (TypeSpecAst,
            "TypeSpecAst is_alias=0\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc), new ErrorAst (loc), false);
PRINT_TEST (VarSpecAst,
            "VarSpecAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (FuncDeclAst,
            "FuncDeclAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (MethodDeclAst,
            "MethodDeclAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (TypeAssertionAst,
            "TypeAssertionAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (SelectorAst,
            "SelectorAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (SliceAst,
            "SliceAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (IndexAst,
            "IndexAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (CallAst,
            "CallAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc), new ErrorAst (loc), false);
PRINT_TEST (BinaryAst,
            "BinaryAst +\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            BinaryAst::Add, new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (AssignAst,
            "AssignAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            AssignAst::Add, new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (UnaryAst,
            "UnaryAst\n"
            "  ErrorAst\n",
            loc, UnaryAst::Posate, new ErrorAst (loc));
PRINT_TEST (TypeExpressionAst,
            "TypeExpressionAst\n",
            loc);
PRINT_TEST (OrAst,
            "OrAst\n",
            loc);
PRINT_TEST (AndAst,
            "AndAst\n",
            loc);
PRINT_TEST (LabeledStatementAst,
            "LabeledStatementAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (GotoAst,
            "GotoAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc));
PRINT_TEST (FallthroughAst,
            "FallthroughAst\n",
            loc);
PRINT_TEST (DeferAst,
            "DeferAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc));
PRINT_TEST (EmptyStatementAst,
            "EmptyStatementAst\n",
            loc);
PRINT_TEST (ExpressionStatementAst,
            "ExpressionStatementAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc));
PRINT_TEST (IncrementAst,
            "IncrementAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc));
PRINT_TEST (DecrementAst,
            "DecrementAst\n"
            "  ErrorAst\n",
            new ErrorAst (loc));
PRINT_TEST (ReturnAst,
            "ReturnAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc));
PRINT_TEST (BreakAst,
            "BreakAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc));
PRINT_TEST (ContinueAst,
            "ContinueAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc));
PRINT_TEST (IfAst,
            "IfAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (SwitchAst,
            "SwitchAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (CaseAst,
            "CaseAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (ForAst,
            "ForAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (RangeAst,
            "RangeAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc));
PRINT_TEST (ForClauseAst,
            "ForClauseAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (SourceFileAst,
            "SourceFileAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n"
            "  ErrorAst\n",
            loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
PRINT_TEST (ImportDeclListAst,
            "ImportDeclListAst\n",
            loc);
PRINT_TEST (ImportDeclAst,
            "ImportDeclAst\n",
            loc);
PRINT_TEST (ImportSpecAst,
            "ImportSpecAst\n"
            "  ErrorAst\n",
            loc, false, new ErrorAst (loc), "x");
PRINT_TEST (ErrorAst,
            "ErrorAst\n",
            loc);

TEST_CASE ("Ast::at (size_t)") {
  Ast* a0 = new ErrorAst (loc);
  Ast* a1 = new ErrorAst (loc);
  Ast* a2 = new ErrorAst (loc);
  SourceFileAst ast (loc, a0, a1, a2);
  REQUIRE (ast.at (0) == a0);
  REQUIRE (ast.at (1) == a1);
  REQUIRE (ast.at (2) == a2);
}

TEST_CASE ("Ast::size ()") {
  SourceFileAst ast (loc, new ErrorAst (loc), new ErrorAst (loc), new ErrorAst (loc));
  REQUIRE (ast.size () == 3);
}

TEST_CASE ("ListAst::ListAst (ChildrenType)") {
  Ast::ChildrenType children;
  children.push_back (new ErrorAst (loc));
  ListAst ast (children);
  REQUIRE (ast.location == loc);
  REQUIRE (ast.size () == 1);
  REQUIRE (ast.at (0) == children[0]);
}

TEST_CASE ("IdentifierEqual::IdentifierEqual ()") {
  IdentifierEqual ie ("blah");
  REQUIRE (ie.identifier == "blah");
}

TEST_CASE ("IdentifierEqual::operator ()") {
  IdentifierEqual ie ("blah");
  IdentifierAst a1 (Token::make_identifier (loc, "blah"));
  IdentifierAst a2 (Token::make_identifier (loc, "blam"));
  REQUIRE (ie (&a1) == true);
  REQUIRE (ie (&a2) == false);
}

#define BINARY_AST_KIND(kind, s) TEST_CASE ("operator<< (" #kind ")") { \
  std::stringstream ss; \
  ss << kind; \
  REQUIRE (ss.str () == s); \
}

BINARY_AST_KIND (BinaryAst::Equal, "==");
BINARY_AST_KIND (BinaryAst::NotEqual, "!=");
BINARY_AST_KIND (BinaryAst::LessThan, "<");
BINARY_AST_KIND (BinaryAst::LessThanEqual, "<=");
BINARY_AST_KIND (BinaryAst::GreaterThan, ">");
BINARY_AST_KIND (BinaryAst::GreaterThanEqual, ">=");
BINARY_AST_KIND (BinaryAst::Add, "+");
BINARY_AST_KIND (BinaryAst::Subtract, "-");
BINARY_AST_KIND (BinaryAst::BitOr, "|");
BINARY_AST_KIND (BinaryAst::BitXor, "^");
BINARY_AST_KIND (BinaryAst::Multiply, "*");
BINARY_AST_KIND (BinaryAst::Divide, "/");
BINARY_AST_KIND (BinaryAst::Modulus, "%");
BINARY_AST_KIND (BinaryAst::LeftShift, "<<");
BINARY_AST_KIND (BinaryAst::RightShift, ">>");
BINARY_AST_KIND (BinaryAst::BitAnd, "&");
BINARY_AST_KIND (BinaryAst::BitAndNot, "&^");
