// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/symbol.h"

#include "src/ast.h"
#include "src/catch.hpp"
#include "src/package.h"
#include "src/test_helpers.h"
#include "src/type.h"

namespace rcgo {
namespace test {

TEST_CASE("Symbol::Symbol()") {
  Symbol* s = new ConstantSymbol("my symbol", location, NULL, NULL);
  REQUIRE(s->identifier == "my symbol");
  REQUIRE(s->location == location);
  REQUIRE(s->IsNew());
  delete s;
}

TEST_CASE("Symbol::MakeInProgress()") {
  Symbol* s = new ConstantSymbol("my symbol", location, NULL, NULL);
  s->MakeInProgress();
  REQUIRE(s->IsInProgress());
  delete s;
}

TEST_CASE("Symbol::MakeDefined()") {
  Symbol* s = new ConstantSymbol("my symbol", location, NULL, NULL);
  s->MakeDefined();
  REQUIRE(s->IsDefined());
  delete s;
}

TEST_CASE("ConstantSymbol::ConstantSymbol(value)") {
  Value v = Value::MakeBoolean(true);
  ConstantSymbol s("my symbol", location, NULL, v);
  REQUIRE(s.value() == v);
}

TEST_CASE("ConstantSymbol::ConstantSymbol(ast)") {
  ast::Error ast(location);
  ConstantSymbol s("my symbol", location, NULL, &ast);
  REQUIRE(s.ast == &ast);
}

TEST_CASE("TypeSymbol::TypeSymbol(type)") {
  type::Factory factory;
  type::DefinedType* dt = factory.MakeDefinedType(&type::Int::instance);
  TypeSymbol s("my symbol", location, NULL, dt);
  REQUIRE(s.type() == dt);
}

TEST_CASE("TypeSymbol::TypeSymbol(ast)") {
  ast::Error ast(location);
  TypeSymbol s("my symbol", location, NULL, &ast);
  REQUIRE(s.ast == &ast);
}

TEST_CASE("VariableSymbol::VariableSymbol(ast)") {
  ast::Error ast(location);
  VariableSymbol s("my symbol", location, NULL, &ast);
  REQUIRE(s.ast == &ast);
}

TEST_CASE("FunctionSymbol::FunctionSymbol(ast)") {
  ast::Error ast(location);
  FunctionSymbol s("my symbol", location, NULL, &ast);
  REQUIRE(s.ast == &ast);
  REQUIRE(s.type() == NULL);
}

TEST_CASE("FunctionSymbol::type()") {
  type::Factory factory;
  ast::Error ast(location);
  FunctionSymbol s("my symbol", location, NULL, &ast);
  type::Function* ft = factory.MakeFunction();
  s.type(ft);
  REQUIRE(s.type() == ft);
}

TEST_CASE("ImportedSymbol::ImportedSymbol(symbol)") {
  ConstantSymbol c("my symbol", location, NULL, NULL);
  ImportedSymbol s("my symbol", location, NULL, &c);
  REQUIRE(s.imported_symbol == &c);
}

TEST_CASE("PackageSymbol::PackageSymbol(package)") {
  Package p("import path");
  PackageSymbol s("my symbol", location, NULL, &p);
  REQUIRE(s.the_package == &p);
}

TEST_CASE("FieldSymbol::FieldSymbol(type, tag)") {
  FieldSymbol s("my symbol", location, NULL, &type::Int::instance, "tag", true);
  REQUIRE(s.type == &type::Int::instance);
  REQUIRE(s.tag == "tag");
  REQUIRE(s.is_embedded == true);
}

TEST_CASE("ParameterSymbol::ParameterSymbol(type, is_variadic)") {
  ParameterSymbol s("my symbol", location, NULL, &type::Int::instance, true);
  REQUIRE(s.type == &type::Int::instance);
  REQUIRE(s.is_variadic == true);
}

TEST_CASE("InterfaceMethodSymbol::InterfaceMethodSymbol()") {
  type::Factory factory;
  type::Function* type = factory.MakeFunction();
  InterfaceMethodSymbol s("my symbol", location, NULL, type);
  REQUIRE(s.type == type);
}

#define ACCEPT_TEST(c, args...) TEST_CASE(#c "::accept(visitor)") {    \
    c* symbol = new c(args);                                           \
    AcceptTestVisitor<DefaultSymbolVisitor, Symbol> visitor;      \
    symbol->Accept(&visitor);                                           \
    REQUIRE(symbol == visitor.t);                                      \
    delete symbol;                                                      \
  }

ACCEPT_TEST(ConstantSymbol, "my symbol", location, NULL, NULL);
ACCEPT_TEST(TypeSymbol, "my symbol", location, NULL,
            static_cast<ast::Node*>(NULL));
ACCEPT_TEST(VariableSymbol, "my symbol", location, NULL, NULL);
ACCEPT_TEST(FunctionSymbol, "my symbol", location, NULL, NULL);
ACCEPT_TEST(ImportedSymbol, "my symbol", location, NULL, NULL);
ACCEPT_TEST(PackageSymbol, "my symbol", location, NULL, NULL);
ACCEPT_TEST(FieldSymbol, "my symbol", location, NULL, NULL, "", false);
ACCEPT_TEST(ParameterSymbol, "my symbol", location, NULL, NULL, false);
ACCEPT_TEST(InterfaceMethodSymbol, "my symbol", location, NULL, NULL);

#define VISIT_TEST(c, args...) \
  TEST_CASE("DefaultSymbolVisitor::visit(" #c ")") {                    \
    c* symbol = new c(args);                                           \
    DefaultSymbolVisitor visitor;                                       \
    visitor.visit(symbol);                                              \
    delete symbol;                                                      \
  }

VISIT_TEST(ConstantSymbol, "x", location, NULL, NULL);
VISIT_TEST(TypeSymbol, "x", location, NULL,
           static_cast<ast::Node*>(NULL));
VISIT_TEST(VariableSymbol, "x", location, NULL, NULL);
VISIT_TEST(FunctionSymbol, "x", location, NULL, NULL);
VISIT_TEST(ImportedSymbol, "x", location, NULL, NULL);
VISIT_TEST(PackageSymbol, "x", location, NULL, NULL);
VISIT_TEST(FieldSymbol, "x", location, NULL, NULL, "tag", false);
VISIT_TEST(ParameterSymbol, "x", location, NULL, NULL, false);
VISIT_TEST(InterfaceMethodSymbol, "x", location, NULL, NULL);

}  // namespace test
}  // namespace rcgo
