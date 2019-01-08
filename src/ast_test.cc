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
  list.push_back(new ast::Error(location));
  return list;
}

#define ACCEPT_TEST(c, args...) TEST_CASE(#c "::accept(visitor)") {     \
    c* ast = new c(args);                                               \
    AcceptTestVisitor<ast::DefaultNodeVisitor, ast::Node> visitor;      \
    ast->Accept(&visitor);                                              \
    REQUIRE(ast == visitor.t);                                          \
    delete ast;                                                         \
  }

ACCEPT_TEST(ast::Array, location, new ast::Error(location),
            new ast::Error(location));
ACCEPT_TEST(ast::Slice, location, new ast::Error(location));
ACCEPT_TEST(ast::Struct, location, EmptyList());
ACCEPT_TEST(ast::Interface, location, EmptyList());
ACCEPT_TEST(ast::Map, location, new ast::Error(location),
            new ast::Error(location));
ACCEPT_TEST(ast::EmbeddedField, location, false, new ast::Error(location), "");
ACCEPT_TEST(ast::Field, location, NonEmptyList(), new ast::Error(location), "");
ACCEPT_TEST(ast::AnonymousMethodSpec, location, new ast::Error(location));
ACCEPT_TEST(ast::MethodSpec, location, new ast::Error(location),
            new ast::Error(location));
ACCEPT_TEST(ast::Pointer, location, new ast::Error(location));
ACCEPT_TEST(ast::FunctionType, location, new ast::Error(location));
ACCEPT_TEST(ast::Signature, location, EmptyList(), EmptyList());
ACCEPT_TEST(ast::ParameterDecl, location, EmptyList(), false,
            new ast::Error(location));
ACCEPT_TEST(ast::Literal, Token::MakeLiteral(location, value::Value::MakeString("")));
ACCEPT_TEST(ast::CompositeLiteral, location, new ast::Error(location),
            new ast::Error(location));
ACCEPT_TEST(ast::KeyedElement, location, new ast::Error(location),
            new ast::Error(location));
ACCEPT_TEST(ast::FunctionLiteral, location, new ast::Error(location),
            new ast::Error(location));
ACCEPT_TEST(ast::Identifier, Token::MakeIdentifier(location, ""));
ACCEPT_TEST(ast::ConstSpec, location, NonEmptyList(), new ast::Error(location),
            EmptyList());
ACCEPT_TEST(ast::TypeSpec, location, new ast::Error(location),
            new ast::Error(location), false);
ACCEPT_TEST(ast::VarSpec, location, NonEmptyList(), new ast::Error(location),
            EmptyList());
ACCEPT_TEST(ast::FuncDecl, location, new ast::Error(location),
            new ast::Error(location), new ast::Error(location));
ACCEPT_TEST(ast::MethodDecl, location, NonEmptyList(), new ast::Error(location),
            new ast::Error(location), new ast::Error(location));
ACCEPT_TEST(ast::TypeAssertion, location, new ast::Error(location),
            new ast::Error(location));
ACCEPT_TEST(ast::Selector, location, new ast::Error(location),
            new ast::Error(location));
ACCEPT_TEST(ast::SliceOp, location, new ast::Error(location),
            new ast::Error(location), new ast::Error(location),
            new ast::Error(location));
ACCEPT_TEST(ast::Index, location, new ast::Error(location),
            new ast::Error(location));
ACCEPT_TEST(ast::Call, location, new ast::Error(location), EmptyList(), false);
ACCEPT_TEST(ast::TypeExpression, location);
ACCEPT_TEST(ast::Or, location, EmptyList());
ACCEPT_TEST(ast::And, location, EmptyList());
ACCEPT_TEST(ast::LabeledStatement, location, new ast::Error(location),
            new ast::Error(location));
ACCEPT_TEST(ast::Goto, location, new ast::Error(location));
ACCEPT_TEST(ast::Fallthrough, location);
ACCEPT_TEST(ast::Defer, location, new ast::Error(location));
ACCEPT_TEST(ast::EmptyStatement, location);
ACCEPT_TEST(ast::ExpressionStatement, location, NonEmptyList());
ACCEPT_TEST(ast::Increment, location, NonEmptyList());
ACCEPT_TEST(ast::Decrement, location, NonEmptyList());
ACCEPT_TEST(ast::Return, location, EmptyList());
ACCEPT_TEST(ast::Break, location, new ast::Error(location));
ACCEPT_TEST(ast::Continue, location, new ast::Error(location));
ACCEPT_TEST(ast::If, location, new ast::Error(location),
            new ast::Error(location), new ast::Error(location),
            new ast::Error(location));
ACCEPT_TEST(ast::Switch, location, new ast::Error(location),
            new ast::Error(location), EmptyList());
ACCEPT_TEST(ast::Case, location, EmptyList(), EmptyList());
ACCEPT_TEST(ast::For, location, new ast::Error(location),
            new ast::Error(location));
ACCEPT_TEST(ast::Range, location, new ast::Error(location));
ACCEPT_TEST(ast::ForClause, location, new ast::Error(location),
            new ast::Error(location), new ast::Error(location));
ACCEPT_TEST(ast::SourceFile, location, new ast::Error(location), EmptyList(),
            EmptyList());
ACCEPT_TEST(ast::ImportDecl, location, EmptyList());
ACCEPT_TEST(ast::ImportSpec, location, false, new ast::Error(location), "");
ACCEPT_TEST(ast::Error, location);

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
           location, new ast::Error(location), new ast::Error(location));
PRINT_TEST(ast::Slice,
           "{\"class\":\"Slice\",\"element_type\":{\"class\":\"Error\"}}",
           location, new ast::Error(location));
PRINT_TEST(ast::Struct,
           "{\"class\":\"Struct\",\"fields\":[]}",
           location, EmptyList());
PRINT_TEST(ast::Interface,
           "{\"class\":\"Interface\",\"methods\":[]}",
           location, EmptyList());
PRINT_TEST(ast::Map,
           "{\"class\":\"Map\",\"key_type\":{\"class\":\"Error\"},"
           "\"value_type\":{\"class\":\"Error\"}}",
           location, new ast::Error(location), new ast::Error(location));
PRINT_TEST(ast::EmbeddedField,
           "{\"class\":\"EmbeddedField\",\"is_pointer\":\"false\","
           "\"type_name\":{\"class\":\"Error\"},\"tag\":\"x\"}",
           location, false, new ast::Error(location), "x");
PRINT_TEST(ast::Field,
           "{\"class\":\"Field\",\"identifier_list\":[{\"class\":\"Error\"}],"
           "\"type_literal\":{\"class\":\"Error\"},\"tag\":\"x\"}",
           location, NonEmptyList(), new ast::Error(location), "x");
PRINT_TEST(ast::AnonymousMethodSpec,
           "{\"class\":\"AnonymousMethodSpec\",\"type_name\":{\"class\":"
           "\"Error\"}}",
           location, new ast::Error(location));
PRINT_TEST(ast::MethodSpec,
           "{\"class\":\"MethodSpec\",\"identifier\":{\"class\":\"Error\"},"
           "\"signature\":{\"class\":\"Error\"}}",
           location, new ast::Error(location), new ast::Error(location));
PRINT_TEST(ast::Pointer,
           "{\"class\":\"Pointer\",\"base_type\":{\"class\":\"Error\"}}",
           location, new ast::Error(location));
PRINT_TEST(ast::FunctionType,
           "{\"class\":\"FunctionType\",\"signature\":{\"class\":\"Error\"}}",
           location, new ast::Error(location));
PRINT_TEST(ast::Signature,
           "{\"class\":\"Signature\",\"parameters\":[],\"results\":[]}",
           location, EmptyList(), EmptyList());
PRINT_TEST(ast::ParameterDecl,
           "{\"class\":\"ParameterDecl\",\"optional_identifier_list\":[],"
           "\"variadic\":\"false\",\"type_literal\":{\"class\":\"Error\"}}",
           location, EmptyList(), false, new ast::Error(location));
PRINT_TEST(ast::Literal,
           "{\"class\":\"Literal\",\"in_value\":\"<value>\"}",
           Token::MakeLiteral(location, value::Value::MakeString("x")));
PRINT_TEST(ast::CompositeLiteral,
           "{\"class\":\"CompositeLiteral\",\"type_literal\":{\"class\":\"Error\"},"
           "\"value\":{\"class\":\"Error\"}}",
           location, new ast::Error(location), new ast::Error(location));
PRINT_TEST(ast::KeyedElement,
           "{\"class\":\"KeyedElement\",\"optional_key\":{\"class\":\"Error\"},"
           "\"element\":{\"class\":\"Error\"}}",
           location, new ast::Error(location), new ast::Error(location));
PRINT_TEST(ast::FunctionLiteral,
           "{\"class\":\"FunctionLiteral\",\"type_literal\":{\"class\":\"Error\"},"
           "\"body\":{\"class\":\"Error\"}}",
           location, new ast::Error(location), new ast::Error(location));
PRINT_TEST(ast::Identifier,
           "{\"class\":\"Identifier\",\"identifier\":\"x\"}",
           Token::MakeIdentifier(location, "x"));
PRINT_TEST(ast::ConstSpec,
           "{\"class\":\"ConstSpec\",\"identifier_list\":[{\"class\":"
           "\"Error\"}],\"optional_type_literal\":{\"class\":\"Error\"},"
           "\"expression_list\":[]}",
           location, NonEmptyList(), new ast::Error(location), EmptyList());
PRINT_TEST(ast::TypeSpec,
           "{\"class\":\"TypeSpec\",\"identifier\":{\"class\":\"Error\"},"
           "\"type_literal\":{\"class\":\"Error\"},\"is_alias\":\"false\"}",
           location, new ast::Error(location), new ast::Error(location), false);
PRINT_TEST(ast::VarSpec,
           "{\"class\":\"VarSpec\",\"identifier_list\":[{\"class\":\"Error\"}],"
           "\"optional_type_literal\":{\"class\":\"Error\"},"
           "\"optional_expression_list\":[]}",
           location, NonEmptyList(), new ast::Error(location), EmptyList());
PRINT_TEST(ast::FuncDecl,
           "{\"class\":\"FuncDecl\",\"identifier\":{\"class\":\"Error\"},"
           "\"signature\":{\"class\":\"Error\"},\"optional_body\":{\"class\":"
           "\"Error\"}}",
           location, new ast::Error(location), new ast::Error(location),
           new ast::Error(location));
PRINT_TEST(ast::MethodDecl,
           "{\"class\":\"MethodDecl\",\"receiver\":[{\"class\":\"Error\"}],"
           "\"identifier\":{\"class\":\"Error\"},\"signature\":{\"class\":"
           "\"Error\"},\"optional_body\":{\"class\":\"Error\"}}",
           location, NonEmptyList(), new ast::Error(location),
           new ast::Error(location), new ast::Error(location));
PRINT_TEST(ast::TypeAssertion,
           "{\"class\":\"TypeAssertion\",\"operand\":{\"class\":\"Error\"},"
           "\"type_literal\":{\"class\":\"Error\"}}",
           location, new ast::Error(location), new ast::Error(location));
PRINT_TEST(ast::Selector,
           "{\"class\":\"Selector\",\"operand\":{\"class\":\"Error\"},"
           "\"member\":{\"class\":\"Error\"}}",
           location, new ast::Error(location), new ast::Error(location));
PRINT_TEST(ast::SliceOp,
           "{\"class\":\"SliceOp\",\"operand\":{\"class\":\"Error\"},"
           "\"optional_low\":{\"class\":\"Error\"},\"optional_high\":{"
           "\"class\":\"Error\"},\"optional_capacity\":{\"class\":\"Error\"}}",
           location, new ast::Error(location), new ast::Error(location),
           new ast::Error(location), new ast::Error(location));
PRINT_TEST(ast::Index,
           "{\"class\":\"Index\",\"operand\":{\"class\":\"Error\"},\"index\":{"
           "\"class\":\"Error\"}}",
           location, new ast::Error(location), new ast::Error(location));
PRINT_TEST(ast::Call,
           "{\"class\":\"Call\",\"operand\":{\"class\":\"Error\"},"
           "\"arguments\":[],\"variadic\":\"false\"}",
           location, new ast::Error(location), EmptyList(), false);
PRINT_TEST(ast::TypeExpression,
           "{\"class\":\"TypeExpression\"}",
           location);
PRINT_TEST(ast::Or,
           "{\"class\":\"Or\",\"expressions\":[]}",
           location, EmptyList());
PRINT_TEST(ast::And,
           "{\"class\":\"And\",\"expressions\":[]}",
           location, EmptyList());
PRINT_TEST(ast::LabeledStatement,
           "{\"class\":\"LabeledStatement\",\"label\":{\"class\":\"Error\"},"
           "\"statement\":{\"class\":\"Error\"}}",
           location, new ast::Error(location), new ast::Error(location));
PRINT_TEST(ast::Goto,
           "{\"class\":\"Goto\",\"label\":{\"class\":\"Error\"}}",
           location, new ast::Error(location));
PRINT_TEST(ast::Fallthrough,
           "{\"class\":\"Fallthrough\"}",
           location);
PRINT_TEST(ast::Defer,
           "{\"class\":\"Defer\",\"expression\":{\"class\":\"Error\"}}",
           location, new ast::Error(location));
PRINT_TEST(ast::EmptyStatement,
           "{\"class\":\"EmptyStatement\"}",
           location);
PRINT_TEST(ast::ExpressionStatement,
           "{\"class\":\"ExpressionStatement\",\"expression\":[{\"class\":"
           "\"Error\"}]}",
           location, NonEmptyList());
PRINT_TEST(ast::Increment,
           "{\"class\":\"Increment\",\"expression\":[{\"class\":\"Error\"}]}",
           location, NonEmptyList());
PRINT_TEST(ast::Decrement,
           "{\"class\":\"Decrement\",\"expression\":[{\"class\":\"Error\"}]}",
           location, NonEmptyList());
PRINT_TEST(ast::Return,
           "{\"class\":\"Return\",\"expression\":[]}",
           location, EmptyList());
PRINT_TEST(ast::Break,
           "{\"class\":\"Break\",\"optional_label\":{\"class\":\"Error\"}}",
           location, new ast::Error(location));
PRINT_TEST(ast::Continue,
           "{\"class\":\"Continue\",\"optional_label\":{\"class\":\"Error\"}}",
           location, new ast::Error(location));
PRINT_TEST(ast::If,
           "{\"class\":\"If\",\"optional_statement\":{\"class\":\"Error\"},"
           "\"expression\":{\"class\":\"Error\"},\"true_branch\":{\"class\":"
           "\"Error\"},\"false_branch\":{\"class\":\"Error\"}}",
           location, new ast::Error(location), new ast::Error(location),
           new ast::Error(location), new ast::Error(location));
PRINT_TEST(ast::Switch,
           "{\"class\":\"Switch\",\"optional_statement\":{\"class\":\"Error\"},"
           "\"optional_expression\":{\"class\":\"Error\"},\"body\":[]}",
           location, new ast::Error(location), new ast::Error(location),
           EmptyList());
PRINT_TEST(ast::Case,
           "{\"class\":\"Case\",\"optional_expression_list\":[],\"statement\":"
           "[]}",
           location, EmptyList(), EmptyList());
PRINT_TEST(ast::For,
           "{\"class\":\"For\",\"clause\":{\"class\":\"Error\"},\"body\":{"
           "\"class\":\"Error\"}}",
           location, new ast::Error(location), new ast::Error(location));
PRINT_TEST(ast::Range,
           "{\"class\":\"Range\",\"expression\":{\"class\":\"Error\"}}",
           location, new ast::Error(location));
PRINT_TEST(ast::ForClause,
           "{\"class\":\"ForClause\",\"optional_init_stmt\":{\"class\":"
           "\"Error\"},\"optional_condition\":{\"class\":\"Error\"},"
           "\"optional_post_stmt\":{\"class\":\"Error\"}}",
           location, new ast::Error(location), new ast::Error(location),
           new ast::Error(location));
PRINT_TEST(ast::SourceFile,
           "{\"class\":\"SourceFile\",\"package\":{\"class\":\"Error\"},"
           "\"imports\":[],\"decls\":[]}",
           location, new ast::Error(location), EmptyList(), EmptyList());
PRINT_TEST(ast::ImportDecl,
           "{\"class\":\"ImportDecl\",\"import_specs\":[]}",
           location, EmptyList());
PRINT_TEST(ast::ImportSpec,
           "{\"class\":\"ImportSpec\",\"dot\":\"false\","
           "\"optional_identifier\":{\"class\":\"Error\"},\"import_path\":\"x\""
           "}",
           location, false, new ast::Error(location), "x");
PRINT_TEST(ast::Error,
           "{\"class\":\"Error\"}",
           location);

TEST_CASE("IdentifierEqual::IdentifierEqual()") {
  ast::IdentifierEqual ie("blah");
  REQUIRE(ie.identifier == "blah");
}

TEST_CASE("IdentifierEqual::operator()") {
  ast::IdentifierEqual ie("blah");
  ast::Identifier a1(Token::MakeIdentifier(location, "blah"));
  ast::Identifier a2(Token::MakeIdentifier(location, "blam"));
  REQUIRE(ie(&a1) == true);
  REQUIRE(ie(&a2) == false);
}

TEST_CASE("cast") {
  ast::Node* ast = new ast::Identifier(Token::MakeIdentifier(location, ""));
  ast::Identifier* id = ast::Cast<ast::Identifier>(ast);
  REQUIRE(id != NULL);
  delete id;
}

}  // namespace test
}  // namespace rcgo
