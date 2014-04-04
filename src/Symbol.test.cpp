/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"
#include "spies.hpp"

#include "Symbol.hpp"
#include "SymbolVisitor.hpp"
#include "Ast.hpp"
#include "Type.hpp"
#include "Package.hpp"

using namespace rcgo;

TEST_CASE ("Symbol::Symbol ()") {
  Symbol* s = new ConstantSymbol ("my symbol", Location::builtin, NULL, NULL);
  REQUIRE (s->identifier == "my symbol");
  REQUIRE (s->location == Location::builtin);
  REQUIRE (s->is_new ());
  delete s;
}

TEST_CASE ("Symbol::in_progress ()") {
  Symbol* s = new ConstantSymbol ("my symbol", Location::builtin, NULL, NULL);
  s->in_progress ();
  REQUIRE (s->is_in_progress ());
  delete s;
}

TEST_CASE ("Symbol::defined ()") {
  Symbol* s = new ConstantSymbol ("my symbol", Location::builtin, NULL, NULL);
  s->defined ();
  REQUIRE (s->is_defined ());
  delete s;
}

TEST_CASE ("ConstantSymbol::ConstantSymbol (value)") {
  Value v = Value::make_boolean (true);
  ConstantSymbol s ("my symbol", Location::builtin, NULL, v);
  REQUIRE (s.value () == v);
}

TEST_CASE ("ConstantSymbol::ConstantSymbol (ast)") {
  ErrorAst ast (Location::builtin);
  ConstantSymbol s ("my symbol", Location::builtin, NULL, &ast);
  REQUIRE (s.ast == &ast);
}

TEST_CASE ("TypeSymbol::TypeSymbol (type)") {
  TypeFactory factory;
  DefinedType* dt = factory.make_defined_type (&IntType::instance);
  TypeSymbol s ("my symbol", Location::builtin, NULL, dt);
  REQUIRE (s.type () == dt);
}

TEST_CASE ("TypeSymbol::TypeSymbol (ast)") {
  ErrorAst ast (Location::builtin);
  TypeSymbol s ("my symbol", Location::builtin, NULL, &ast);
  REQUIRE (s.ast == &ast);
}

TEST_CASE ("VariableSymbol::VariableSymbol (ast)") {
  ErrorAst ast (Location::builtin);
  VariableSymbol s ("my symbol", Location::builtin, NULL, &ast);
  REQUIRE (s.ast == &ast);
}

TEST_CASE ("FunctionSymbol::FunctionSymbol (ast)") {
  ErrorAst ast (Location::builtin);
  FunctionSymbol s ("my symbol", Location::builtin, NULL, &ast);
  REQUIRE (s.ast == &ast);
  REQUIRE (s.type () == NULL);
}

TEST_CASE ("FunctionSymbol::type ()") {
  TypeFactory factory;
  ErrorAst ast (Location::builtin);
  FunctionSymbol s ("my symbol", Location::builtin, NULL, &ast);
  FunctionType* ft = factory.make_function_type ();
  s.type (ft);
  REQUIRE (s.type () == ft);
}

TEST_CASE ("ImportedSymbol::ImportedSymbol (symbol)") {
  ConstantSymbol c ("my symbol", Location::builtin, NULL, NULL);
  ImportedSymbol s ("my symbol", Location::builtin, NULL, &c);
  REQUIRE (s.imported_symbol == &c);
}

TEST_CASE ("PackageSymbol::PackageSymbol (package)") {
  Package p ("import path");
  PackageSymbol s ("my symbol", Location::builtin, NULL, &p);
  REQUIRE (s.the_package == &p);
}

TEST_CASE ("FieldSymbol::FieldSymbol (type, tag)") {
  FieldSymbol s ("my symbol", Location::builtin, NULL, &IntType::instance, "tag", true);
  REQUIRE (s.type == &IntType::instance);
  REQUIRE (s.tag == "tag");
  REQUIRE (s.is_embedded == true);
}

TEST_CASE ("ParameterSymbol::ParameterSymbol (type, is_variadic)") {
  ParameterSymbol s ("my symbol", Location::builtin, NULL, &IntType::instance, true);
  REQUIRE (s.type == &IntType::instance);
  REQUIRE (s.is_variadic == true);
}

TEST_CASE ("InterfaceMethodSymbol::InterfaceMethodSymbol ()") {
  TypeFactory factory;
  FunctionType* type = factory.make_function_type ();
  InterfaceMethodSymbol s ("my symbol", Location::builtin, NULL, type);
  REQUIRE (s.type == type);
}

#define ACCEPT_TEST(c, args...) TEST_CASE (#c "::accept(visitor)") {    \
    c* symbol = new c (args);                                           \
    AcceptTestVisitor<DefaultSymbolVisitor, Symbol> visitor;            \
    symbol->accept (visitor);                                           \
    REQUIRE (symbol == visitor.t);                                      \
    delete symbol;                                                      \
  }

ACCEPT_TEST (ConstantSymbol, "my symbol", Location::builtin, NULL, NULL);
ACCEPT_TEST (TypeSymbol, "my symbol", Location::builtin, NULL, static_cast<Ast*> (NULL));
ACCEPT_TEST (VariableSymbol, "my symbol", Location::builtin, NULL, NULL);
ACCEPT_TEST (FunctionSymbol, "my symbol", Location::builtin, NULL, NULL);
ACCEPT_TEST (ImportedSymbol, "my symbol", Location::builtin, NULL, NULL);
ACCEPT_TEST (PackageSymbol, "my symbol", Location::builtin, NULL, NULL);
ACCEPT_TEST (FieldSymbol, "my symbol", Location::builtin, NULL, NULL, "", false);
ACCEPT_TEST (ParameterSymbol, "my symbol", Location::builtin, NULL, NULL, false);
ACCEPT_TEST (InterfaceMethodSymbol, "my symbol", Location::builtin, NULL, NULL);
