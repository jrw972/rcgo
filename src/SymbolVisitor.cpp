/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "SymbolVisitor.hpp"

#define VISIT(c) void DefaultSymbolVisitor::visit (c& symbol) { default_action (symbol); }

namespace rcgo {

  SymbolVisitor::~SymbolVisitor ()
  { }

  DefaultSymbolVisitor::DefaultSymbolVisitor ()
  { }

  DefaultSymbolVisitor::~DefaultSymbolVisitor ()
  { }

  VISIT (ConstantSymbol)
  VISIT (TypeSymbol)
  VISIT (VariableSymbol)
  VISIT (FunctionSymbol)
  VISIT (ImportedSymbol)
  VISIT (PackageSymbol)
  VISIT (FieldSymbol)
  VISIT (ParameterSymbol)
  VISIT (InterfaceMethodSymbol)

  void DefaultSymbolVisitor::default_action (Symbol& symbol)
  { }

}
