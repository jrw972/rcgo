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
  symbol::Table st(nullptr);
  MutableBlock blk(&st, nullptr);
  symbol::Symbol* s = blk.MakeConstant("my symbol", Location(), nullptr);
  REQUIRE(s->identifier == "my symbol");
  REQUIRE(s->location == Location());
  REQUIRE(s->IsNew());
}

TEST_CASE("Symbol::MakeInProgress()") {
  symbol::Table st(nullptr);
  MutableBlock blk(&st, nullptr);
  symbol::Symbol* s = blk.MakeConstant("my symbol", Location(), nullptr);
  s->MakeInProgress();
  REQUIRE(s->IsInProgress());
}

TEST_CASE("Symbol::MakeDefined()") {
  symbol::Table st(nullptr);
  MutableBlock blk(&st, nullptr);
  symbol::Symbol* s = blk.MakeConstant("my symbol", Location(), nullptr);
  s->MakeDefined(value::Value::MakeError());
  REQUIRE(s->IsDefined());
}

TEST_CASE("Constant::Constant(value)") {
  symbol::Table st(nullptr);
  MutableBlock blk(&st, nullptr);
  value::Value v = value::Value::MakeUntypedConstant(value::UntypedConstant::MakeBoolean(true));
  symbol::Constant* s = blk.MakeConstant("my symbol", Location(), v);
  REQUIRE(s->GetValue() == v);
}

TEST_CASE("Constant::Constant(ast)") {
  symbol::Table st(nullptr);
  MutableBlock blk(&st, nullptr);
  Location loc;
  ast::Error node(loc);
  symbol::Constant* s = blk.MakeConstant("my symbol", Location(), &node);
  REQUIRE(s->ast == &node);
}

TEST_CASE("Type::Type(type)") {
  symbol::Table st(nullptr);
  MutableBlock blk(&st, nullptr);
  type::Factory factory;
  type::DefinedType* dt = factory.MakeDefinedType(&type::Int::instance);
  symbol::Type* s = blk.MakeType("my symbol", Location(), dt);
  REQUIRE(s->type() == dt);
}

TEST_CASE("Type::Type(ast)") {
  symbol::Table st(nullptr);
  MutableBlock blk(&st, nullptr);
  Location loc;
  ast::Error ast(loc);
  symbol::Type* s = blk.MakeType("my symbol", Location(), &ast);
  REQUIRE(s->ast == &ast);
}

TEST_CASE("Variable::Variable(ast)") {
  symbol::Table st(nullptr);
  MutableBlock blk(&st, nullptr);
  Location loc;
  ast::Error ast(loc);
  symbol::Variable* s = blk.MakeVariable("my symbol", Location(), &ast);
  REQUIRE(s->ast == &ast);
}

TEST_CASE("Function::Function(ast)") {
  symbol::Table st(nullptr);
  MutableBlock blk(&st, nullptr);
  Location loc;
  ast::Error ast(loc);
  symbol::Function* s = blk.MakeFunction("my symbol", Location(), &ast);
  REQUIRE(s->ast == &ast);
  REQUIRE(s->type() == NULL);
}

TEST_CASE("Function::type()") {
  symbol::Table st(nullptr);
  MutableBlock blk(&st, nullptr);
  type::Factory factory;
  Location loc;
  ast::Error ast(loc);
  symbol::Function* s = blk.MakeFunction("my symbol", Location(), &ast);
  type::Function* ft = factory.MakeFunction(nullptr);
  s->type(ft);
  REQUIRE(s->type() == ft);
}

TEST_CASE("Package::Package(package)") {
  symbol::Table st(nullptr);
  MutableBlock blk(&st, nullptr);
  Package p("import path", YAML::Node());
  symbol::Package* s = blk.MakePackage("my symbol", Location(), &p);
  REQUIRE(s->the_package == &p);
}

TEST_CASE("Field::Field(type, tag)") {
  type::Factory factory;
  type::Struct* st = factory.MakeStruct(nullptr);
  symbol::Field* s =
      st->AppendField("my symbol", Location(), &type::Int::instance, "tag", true);
  REQUIRE(s->type == &type::Int::instance);
  REQUIRE(s->tag == "tag");
  REQUIRE(s->is_embedded == true);
}

TEST_CASE("Parameter::Parameter(type, is_variadic)") {
  type::Factory factory;
  type::Function* f = factory.MakeFunction(nullptr);
  symbol::Parameter* s =
      f->AppendParameter("my symbol", Location(), &type::Int::instance, true);
  REQUIRE(s->type == &type::Int::instance);
  REQUIRE(s->is_variadic == true);
}

TEST_CASE("InterfaceMethod::InterfaceMethod()") {
  type::Factory factory;
  type::Interface* interface = factory.MakeInterface(nullptr);
  type::Function* type = factory.MakeFunction(nullptr);
  symbol::InterfaceMethod* s =
      interface->AppendMethod("my symbol", Location(), type);
  REQUIRE(s->type == type);
}

#define ACCEPT_TEST(c, f, args...) TEST_CASE(#c "::accept(visitor)") {  \
    symbol::Table st(nullptr);                                          \
    MutableBlock blk(&st, nullptr);                                     \
    c* symbol = blk . f(args);                                          \
    AcceptTestVisitor<symbol::DefaultVisitor, symbol::Symbol> visitor;  \
    symbol->Accept(&visitor);                                           \
    REQUIRE(symbol == visitor.t);                                       \
  }

ACCEPT_TEST(symbol::Constant, MakeConstant, "my symbol", Location(), nullptr);
ACCEPT_TEST(symbol::Type, MakeType, "my symbol", Location(),
            static_cast<ast::Node*>(nullptr));
ACCEPT_TEST(symbol::Variable, MakeVariable, "my symbol", Location(), nullptr);
ACCEPT_TEST(symbol::Function, MakeFunction, "my symbol", Location(),
            static_cast<ast::Node*>(nullptr));
ACCEPT_TEST(symbol::Package, MakePackage, "my symbol", Location(), nullptr);

TEST_CASE("Field::accept(visitor)") {
  type::Factory factory;
  type::Struct* st = factory.MakeStruct(nullptr);
  symbol::Field* symbol =
      st->AppendField("my symbol", Location(), nullptr, "", false);
  AcceptTestVisitor<symbol::DefaultVisitor, symbol::Symbol> visitor;
  symbol->Accept(&visitor);
  REQUIRE(symbol == visitor.t);
}

TEST_CASE("Parameter::accept(visitor)") {
  type::Factory factory;
  type::Function* st = factory.MakeFunction(nullptr);
  symbol::Parameter* symbol =
      st->AppendParameter("my symbol", Location(), nullptr, false);
  AcceptTestVisitor<symbol::DefaultVisitor, symbol::Symbol> visitor;
  symbol->Accept(&visitor);
  REQUIRE(symbol == visitor.t);
}

TEST_CASE("InterfaceMethod::accept(visitor)") {
  type::Factory factory;
  type::Interface* st = factory.MakeInterface(nullptr);
  symbol::InterfaceMethod* symbol =
      st->AppendMethod("my symbol", Location(), nullptr);
  AcceptTestVisitor<symbol::DefaultVisitor, symbol::Symbol> visitor;
  symbol->Accept(&visitor);
  REQUIRE(symbol == visitor.t);
}

#define VISIT_TEST(c, f, args...)                                       \
  TEST_CASE("DefaultVisitor::visit(" #c ")") {                          \
    symbol::Table st(nullptr);                                          \
    MutableBlock blk(&st, nullptr);                                     \
    c* symbol = blk . f(args);                                          \
    symbol::DefaultVisitor visitor;                                     \
    visitor.Visit(symbol);                                              \
  }

VISIT_TEST(symbol::Constant, MakeConstant, "x", Location(), nullptr);
VISIT_TEST(symbol::Type, MakeType, "x", Location(),
           static_cast<ast::Node*>(nullptr));
VISIT_TEST(symbol::Variable, MakeVariable, "x", Location(), nullptr);
VISIT_TEST(symbol::Function, MakeFunction, "x", Location(),
           static_cast<ast::Node*>(nullptr));
VISIT_TEST(symbol::Package, MakePackage, "x", Location(), nullptr);

TEST_CASE("DefaultVisitor::visit(Field)") {
  type::Factory factory;
  type::Struct* st = factory.MakeStruct(nullptr);
  symbol::Field* symbol =
      st->AppendField("my symbol", Location(), nullptr, "", false);
  symbol::DefaultVisitor visitor;
  visitor.Visit(symbol);
}

TEST_CASE("DefaultVisitor::visit(Parameter)") {
  type::Factory factory;
  type::Function* st = factory.MakeFunction(nullptr);
  symbol::Parameter* symbol =
      st->AppendParameter("my symbol", Location(), nullptr, false);
  symbol::DefaultVisitor visitor;
  visitor.Visit(symbol);
}

TEST_CASE("DefaultVisitor::visit(InterfaceMethod)") {
  type::Factory factory;
  type::Interface* st = factory.MakeInterface(nullptr);
  symbol::InterfaceMethod* symbol =
      st->AppendMethod("my symbol", Location(), nullptr);
  symbol::DefaultVisitor visitor;
  visitor.Visit(symbol);
}

}  // namespace test
}  // namespace rcgo
