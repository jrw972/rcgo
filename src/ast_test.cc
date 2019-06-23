// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/ast.h"
#include "src/catch.hpp"
#include "src/test_helpers.h"
#include "src/token.h"

namespace rcgo {
namespace test {

static ast::ListType EmptyList() {
  return ast::ListType();
}

static ast::ListType NonEmptyList() {
  ast::ListType list;
  list.push_back(new ast::Error(Location()));
  return list;
}

#define ACCEPT_TEST(c, args...) TEST_CASE(#c "::accept(visitor)") {     \
    c* ast = new c(args);                                               \
    AcceptTestVisitor<ast::DefaultNodeVisitor, ast::Node> visitor;      \
    ast->Accept(&visitor);                                              \
    REQUIRE(ast == visitor.t);                                          \
    delete ast;                                                         \
  }

ACCEPT_TEST(ast::Array, Location(), new ast::Error(Location()),
            new ast::Error(Location()));
ACCEPT_TEST(ast::Slice, Location(), new ast::Error(Location()));
ACCEPT_TEST(ast::Struct, Location(), EmptyList());
ACCEPT_TEST(ast::Interface, Location(), EmptyList());
ACCEPT_TEST(ast::Map, Location(), new ast::Error(Location()),
            new ast::Error(Location()));
ACCEPT_TEST(ast::EmbeddedField, Location(), false, new ast::Error(Location()), "");
ACCEPT_TEST(ast::Field, Location(), NonEmptyList(), new ast::Error(Location()), "");
ACCEPT_TEST(ast::AnonymousMethodSpec, Location(), new ast::Error(Location()));
ACCEPT_TEST(ast::MethodSpec, Location(), new ast::Error(Location()),
            new ast::Error(Location()));
ACCEPT_TEST(ast::Pointer, Location(), new ast::Error(Location()));
ACCEPT_TEST(ast::FunctionType, Location(), new ast::Error(Location()));
ACCEPT_TEST(ast::Signature, Location(), EmptyList(), EmptyList());
ACCEPT_TEST(ast::ParameterDecl, Location(), EmptyList(), false,
            new ast::Error(Location()));
ACCEPT_TEST(
    ast::Literal, Token::MakeLiteral(Location(), value::UntypedConstant::MakeString("")));
ACCEPT_TEST(ast::CompositeLiteral, Location(), new ast::Error(Location()),
            new ast::Error(Location()));
ACCEPT_TEST(ast::KeyedElement, Location(), new ast::Error(Location()),
            new ast::Error(Location()));
ACCEPT_TEST(ast::FunctionLiteral, Location(), new ast::Error(Location()),
            new ast::Error(Location()));
ACCEPT_TEST(ast::Identifier, Token::MakeIdentifier(Location(), ""));
ACCEPT_TEST(ast::ConstSpec, Location(), NonEmptyList(), new ast::Error(Location()),
            EmptyList());
ACCEPT_TEST(ast::TypeSpec, Location(), new ast::Error(Location()),
            new ast::Error(Location()), false);
ACCEPT_TEST(ast::VarSpec, Location(), NonEmptyList(), new ast::Error(Location()),
            EmptyList());
ACCEPT_TEST(ast::FuncDecl, Location(), new ast::Error(Location()),
            new ast::Error(Location()), new ast::Error(Location()));
ACCEPT_TEST(ast::MethodDecl, Location(), NonEmptyList(), new ast::Error(Location()),
            new ast::Error(Location()), new ast::Error(Location()));
ACCEPT_TEST(ast::TypeAssertion, Location(), new ast::Error(Location()),
            new ast::Error(Location()));
ACCEPT_TEST(ast::Selector, Location(), new ast::Error(Location()),
            new ast::Error(Location()));
ACCEPT_TEST(ast::SliceOp, Location(), new ast::Error(Location()),
            new ast::Error(Location()), new ast::Error(Location()),
            new ast::Error(Location()));
ACCEPT_TEST(ast::Index, Location(), new ast::Error(Location()),
            new ast::Error(Location()));
ACCEPT_TEST(ast::Call, Location(), new ast::Error(Location()), EmptyList(), false);
ACCEPT_TEST(ast::TypeExpression, Location());
ACCEPT_TEST(ast::Or, Location(), EmptyList());
ACCEPT_TEST(ast::And, Location(), EmptyList());
ACCEPT_TEST(ast::LabeledStatement, Location(), new ast::Error(Location()),
            new ast::Error(Location()));
ACCEPT_TEST(ast::Goto, Location(), new ast::Error(Location()));
ACCEPT_TEST(ast::Fallthrough, Location());
ACCEPT_TEST(ast::Defer, Location(), new ast::Error(Location()));
ACCEPT_TEST(ast::EmptyStatement, Location());
ACCEPT_TEST(ast::ExpressionStatement, Location(), NonEmptyList());
ACCEPT_TEST(ast::Increment, Location(), NonEmptyList());
ACCEPT_TEST(ast::Decrement, Location(), NonEmptyList());
ACCEPT_TEST(ast::Return, Location(), EmptyList());
ACCEPT_TEST(ast::Break, Location(), new ast::Error(Location()));
ACCEPT_TEST(ast::Continue, Location(), new ast::Error(Location()));
ACCEPT_TEST(ast::If, Location(), new ast::Error(Location()),
            new ast::Error(Location()), new ast::Error(Location()),
            new ast::Error(Location()));
ACCEPT_TEST(ast::Switch, Location(), new ast::Error(Location()),
            new ast::Error(Location()), EmptyList());
ACCEPT_TEST(ast::Case, Location(), EmptyList(), EmptyList());
ACCEPT_TEST(ast::For, Location(), new ast::Error(Location()),
            new ast::Error(Location()));
ACCEPT_TEST(ast::Range, Location(), new ast::Error(Location()));
ACCEPT_TEST(ast::ForClause, Location(), new ast::Error(Location()),
            new ast::Error(Location()), new ast::Error(Location()));
ACCEPT_TEST(ast::SourceFile, Location(), new ast::Error(Location()), EmptyList(),
            EmptyList());
ACCEPT_TEST(ast::ImportDecl, Location(), EmptyList());
ACCEPT_TEST(ast::ImportSpec, Location(), false, new ast::Error(Location()), "");
ACCEPT_TEST(ast::Error, Location());

struct VisitChildrenTestVisitor : public ast::DefaultNodeVisitor {
  unsigned int n;

  VisitChildrenTestVisitor()
    : n(0)
  { }

  void DefaultAction(ast::Node* ast) override { ++n; }
};

#define PRINT_TEST(c, s, args...) TEST_CASE("operator<<(" #c ")") {   \
    std::stringstream ss;                                               \
    c* ast = new c(args);                                              \
    ss << *ast;                                                         \
    REQUIRE(ss.str() == s);                                           \
    delete ast;                                                         \
}

PRINT_TEST(ast::Array,
           "{\"class\":\"Array\",\"optional_length\":{\"class\":\"Error\"},"
           "\"element_type\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()), new ast::Error(Location()));
PRINT_TEST(ast::Slice,
           "{\"class\":\"Slice\",\"element_type\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()));
PRINT_TEST(ast::Struct,
           "{\"class\":\"Struct\",\"fields\":[]}",
           Location(), EmptyList());
PRINT_TEST(ast::Interface,
           "{\"class\":\"Interface\",\"methods\":[]}",
           Location(), EmptyList());
PRINT_TEST(ast::Map,
           "{\"class\":\"Map\",\"key_type\":{\"class\":\"Error\"},"
           "\"value_type\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()), new ast::Error(Location()));
PRINT_TEST(ast::EmbeddedField,
           "{\"class\":\"EmbeddedField\",\"is_pointer\":\"false\","
           "\"type_name\":{\"class\":\"Error\"},\"tag\":\"x\"}",
           Location(), false, new ast::Error(Location()), "x");
PRINT_TEST(ast::Field,
           "{\"class\":\"Field\",\"identifier_list\":[{\"class\":\"Error\"}],"
           "\"type_literal\":{\"class\":\"Error\"},\"tag\":\"x\"}",
           Location(), NonEmptyList(), new ast::Error(Location()), "x");
PRINT_TEST(ast::AnonymousMethodSpec,
           "{\"class\":\"AnonymousMethodSpec\",\"type_name\":{\"class\":"
           "\"Error\"}}",
           Location(), new ast::Error(Location()));
PRINT_TEST(ast::MethodSpec,
           "{\"class\":\"MethodSpec\",\"identifier\":{\"class\":\"Error\"},"
           "\"signature\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()), new ast::Error(Location()));
PRINT_TEST(ast::Pointer,
           "{\"class\":\"Pointer\",\"base_type\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()));
PRINT_TEST(ast::FunctionType,
           "{\"class\":\"FunctionType\",\"signature\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()));
PRINT_TEST(ast::Signature,
           "{\"class\":\"Signature\",\"parameters\":[],\"results\":[]}",
           Location(), EmptyList(), EmptyList());
PRINT_TEST(ast::ParameterDecl,
           "{\"class\":\"ParameterDecl\",\"optional_identifier_list\":[],"
           "\"variadic\":\"false\",\"type_literal\":{\"class\":\"Error\"}}",
           Location(), EmptyList(), false, new ast::Error(Location()));
PRINT_TEST(ast::Literal,
           "{\"class\":\"Literal\",\"in_value\":\"<value>\"}",
           Token::MakeLiteral(Location(), value::UntypedConstant::MakeString("x")));
PRINT_TEST(
    ast::CompositeLiteral,
    "{\"class\":\"CompositeLiteral\",\"type_literal\":{\"class\":\"Error\"},"
    "\"value\":{\"class\":\"Error\"}}",
    Location(), new ast::Error(Location()), new ast::Error(Location()));
PRINT_TEST(ast::KeyedElement,
           "{\"class\":\"KeyedElement\",\"optional_key\":{\"class\":\"Error\"},"
           "\"element\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()), new ast::Error(Location()));
PRINT_TEST(
    ast::FunctionLiteral,
    "{\"class\":\"FunctionLiteral\",\"type_literal\":{\"class\":\"Error\"},"
    "\"body\":{\"class\":\"Error\"}}",
    Location(), new ast::Error(Location()), new ast::Error(Location()));
PRINT_TEST(ast::Identifier,
           "{\"class\":\"Identifier\",\"identifier\":\"x\"}",
           Token::MakeIdentifier(Location(), "x"));
PRINT_TEST(ast::ConstSpec,
           "{\"class\":\"ConstSpec\",\"identifier_list\":[{\"class\":"
           "\"Error\"}],\"optional_type_literal\":{\"class\":\"Error\"},"
           "\"expression_list\":[]}",
           Location(), NonEmptyList(), new ast::Error(Location()), EmptyList());
PRINT_TEST(ast::TypeSpec,
           "{\"class\":\"TypeSpec\",\"identifier\":{\"class\":\"Error\"},"
           "\"type_literal\":{\"class\":\"Error\"},\"is_alias\":\"false\"}",
           Location(), new ast::Error(Location()), new ast::Error(Location()), false);
PRINT_TEST(ast::VarSpec,
           "{\"class\":\"VarSpec\",\"identifier_list\":[{\"class\":\"Error\"}],"
           "\"optional_type_literal\":{\"class\":\"Error\"},"
           "\"optional_expression_list\":[]}",
           Location(), NonEmptyList(), new ast::Error(Location()), EmptyList());
PRINT_TEST(ast::FuncDecl,
           "{\"class\":\"FuncDecl\",\"identifier\":{\"class\":\"Error\"},"
           "\"signature\":{\"class\":\"Error\"},\"optional_body\":{\"class\":"
           "\"Error\"}}",
           Location(), new ast::Error(Location()), new ast::Error(Location()),
           new ast::Error(Location()));
PRINT_TEST(ast::MethodDecl,
           "{\"class\":\"MethodDecl\",\"receiver\":[{\"class\":\"Error\"}],"
           "\"identifier\":{\"class\":\"Error\"},\"signature\":{\"class\":"
           "\"Error\"},\"optional_body\":{\"class\":\"Error\"}}",
           Location(), NonEmptyList(), new ast::Error(Location()),
           new ast::Error(Location()), new ast::Error(Location()));
PRINT_TEST(ast::TypeAssertion,
           "{\"class\":\"TypeAssertion\",\"operand\":{\"class\":\"Error\"},"
           "\"type_literal\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()), new ast::Error(Location()));
PRINT_TEST(ast::Selector,
           "{\"class\":\"Selector\",\"operand\":{\"class\":\"Error\"},"
           "\"member\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()), new ast::Error(Location()));
PRINT_TEST(ast::SliceOp,
           "{\"class\":\"SliceOp\",\"operand\":{\"class\":\"Error\"},"
           "\"optional_low\":{\"class\":\"Error\"},\"optional_high\":{"
           "\"class\":\"Error\"},\"optional_capacity\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()), new ast::Error(Location()),
           new ast::Error(Location()), new ast::Error(Location()));
PRINT_TEST(ast::Index,
           "{\"class\":\"Index\",\"operand\":{\"class\":\"Error\"},\"index\":{"
           "\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()), new ast::Error(Location()));
PRINT_TEST(ast::Call,
           "{\"class\":\"Call\",\"operand\":{\"class\":\"Error\"},"
           "\"arguments\":[],\"variadic\":\"false\"}",
           Location(), new ast::Error(Location()), EmptyList(), false);
PRINT_TEST(ast::TypeExpression,
           "{\"class\":\"TypeExpression\"}",
           Location());
PRINT_TEST(ast::Or,
           "{\"class\":\"Or\",\"expressions\":[]}",
           Location(), EmptyList());
PRINT_TEST(ast::And,
           "{\"class\":\"And\",\"expressions\":[]}",
           Location(), EmptyList());
PRINT_TEST(ast::LabeledStatement,
           "{\"class\":\"LabeledStatement\",\"label\":{\"class\":\"Error\"},"
           "\"statement\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()), new ast::Error(Location()));
PRINT_TEST(ast::Goto,
           "{\"class\":\"Goto\",\"label\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()));
PRINT_TEST(ast::Fallthrough,
           "{\"class\":\"Fallthrough\"}",
           Location());
PRINT_TEST(ast::Defer,
           "{\"class\":\"Defer\",\"expression\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()));
PRINT_TEST(ast::EmptyStatement,
           "{\"class\":\"EmptyStatement\"}",
           Location());
PRINT_TEST(ast::ExpressionStatement,
           "{\"class\":\"ExpressionStatement\",\"expression\":[{\"class\":"
           "\"Error\"}]}",
           Location(), NonEmptyList());
PRINT_TEST(ast::Increment,
           "{\"class\":\"Increment\",\"expression\":[{\"class\":\"Error\"}]}",
           Location(), NonEmptyList());
PRINT_TEST(ast::Decrement,
           "{\"class\":\"Decrement\",\"expression\":[{\"class\":\"Error\"}]}",
           Location(), NonEmptyList());
PRINT_TEST(ast::Return,
           "{\"class\":\"Return\",\"expression\":[]}",
           Location(), EmptyList());
PRINT_TEST(ast::Break,
           "{\"class\":\"Break\",\"optional_label\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()));
PRINT_TEST(ast::Continue,
           "{\"class\":\"Continue\",\"optional_label\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()));
PRINT_TEST(ast::If,
           "{\"class\":\"If\",\"optional_statement\":{\"class\":\"Error\"},"
           "\"expression\":{\"class\":\"Error\"},\"true_branch\":{\"class\":"
           "\"Error\"},\"false_branch\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()), new ast::Error(Location()),
           new ast::Error(Location()), new ast::Error(Location()));
PRINT_TEST(ast::Switch,
           "{\"class\":\"Switch\",\"optional_statement\":{\"class\":\"Error\"},"
           "\"optional_expression\":{\"class\":\"Error\"},\"body\":[]}",
           Location(), new ast::Error(Location()), new ast::Error(Location()),
           EmptyList());
PRINT_TEST(ast::Case,
           "{\"class\":\"Case\",\"optional_expression_list\":[],\"statement\":"
           "[]}",
           Location(), EmptyList(), EmptyList());
PRINT_TEST(ast::For,
           "{\"class\":\"For\",\"clause\":{\"class\":\"Error\"},\"body\":{"
           "\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()), new ast::Error(Location()));
PRINT_TEST(ast::Range,
           "{\"class\":\"Range\",\"expression\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()));
PRINT_TEST(ast::ForClause,
           "{\"class\":\"ForClause\",\"optional_init_stmt\":{\"class\":"
           "\"Error\"},\"optional_condition\":{\"class\":\"Error\"},"
           "\"optional_post_stmt\":{\"class\":\"Error\"}}",
           Location(), new ast::Error(Location()), new ast::Error(Location()),
           new ast::Error(Location()));
PRINT_TEST(ast::SourceFile,
           "{\"class\":\"SourceFile\",\"package\":{\"class\":\"Error\"},"
           "\"imports\":[],\"decls\":[]}",
           Location(), new ast::Error(Location()), EmptyList(), EmptyList());
PRINT_TEST(ast::ImportDecl,
           "{\"class\":\"ImportDecl\",\"import_specs\":[]}",
           Location(), EmptyList());
PRINT_TEST(ast::ImportSpec,
           "{\"class\":\"ImportSpec\",\"dot\":\"false\","
           "\"optional_identifier\":{\"class\":\"Error\"},\"import_path\":\"x\""
           "}",
           Location(), false, new ast::Error(Location()), "x");
PRINT_TEST(ast::Error,
           "{\"class\":\"Error\"}",
           Location());

TEST_CASE("IdentifierEqual::IdentifierEqual()") {
  ast::IdentifierEqual ie("blah");
  REQUIRE(ie.identifier == "blah");
}

TEST_CASE("IdentifierEqual::operator()") {
  ast::IdentifierEqual ie("blah");
  ast::Identifier a1(Token::MakeIdentifier(Location(), "blah"));
  ast::Identifier a2(Token::MakeIdentifier(Location(), "blam"));
  REQUIRE(ie(&a1) == true);
  REQUIRE(ie(&a2) == false);
}

TEST_CASE("cast") {
  ast::Node* ast = new ast::Identifier(Token::MakeIdentifier(Location(), ""));
  ast::Identifier* id = ast::Cast<ast::Identifier>(ast);
  REQUIRE(id != NULL);
  delete id;
}

}  // namespace test
}  // namespace rcgo
