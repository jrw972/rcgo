/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"

#include "SymbolVisitor.hpp"
#include "Symbol.hpp"

using namespace rcgo;

static Location loc;

#define VISIT_TEST(c, args...) TEST_CASE ("DefaultSymbolVisitor::visit(" #c ")") { \
    c* symbol = new c (args);                                           \
    DefaultSymbolVisitor visitor;                                          \
    visitor.visit (*symbol);                                            \
    delete symbol;                                                         \
}

VISIT_TEST (ConstantSymbol, "x", Location::builtin, NULL, NULL);
VISIT_TEST (TypeSymbol, "x", Location::builtin, NULL, static_cast<Ast*> (NULL));
VISIT_TEST (VariableSymbol, "x", Location::builtin, NULL, NULL);
VISIT_TEST (FunctionSymbol, "x", Location::builtin, NULL, NULL);
VISIT_TEST (ImportedSymbol, "x", Location::builtin, NULL, NULL);
VISIT_TEST (PackageSymbol, "x", Location::builtin, NULL, NULL);
VISIT_TEST (FieldSymbol, "x", Location::builtin, NULL, NULL, "tag", false);
VISIT_TEST (ParameterSymbol, "x", Location::builtin, NULL, NULL, false);
VISIT_TEST (InterfaceMethodSymbol, "x", Location::builtin, NULL, NULL);
