// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/symbol.h"

#include "src/type.h"
#include "src/utf8.h"

namespace rcgo {

bool IsExported(const std::string& id) {
  return IsUnicodeUpper(FirstRune(id));
}

void ConstantSymbol::Accept(SymbolVisitor* visitor) { visitor->visit(this); }

void ConstantSymbol::value(const Value& a_value) {
  assert(m_value.kind == Value::kUnknown);
  assert(a_value.kind != Value::kUnknown);
  m_value = a_value;
  MakeDefined();
}

void TypeSymbol::Accept(SymbolVisitor* visitor) { visitor->visit(this); }

void TypeSymbol::type(const type::NamedType* a_type) {
  assert(m_type == NULL);
  assert(a_type != NULL);
  m_type = a_type;
  MakeDefined();
}

void VariableSymbol::Accept(SymbolVisitor* visitor) { visitor->visit(this); }

void FunctionSymbol::Accept(SymbolVisitor* visitor) { visitor->visit(this); }

void FunctionSymbol::type(const type::Function* a_type) {
  assert(m_type == NULL);
  assert(a_type != NULL);
  m_type = a_type;
  MakeDefined();
}

void ImportedSymbol::Accept(SymbolVisitor* visitor) { visitor->visit(this); }

void PackageSymbol::Accept(SymbolVisitor* visitor) { visitor->visit(this); }

void FieldSymbol::Accept(SymbolVisitor* visitor) { visitor->visit(this); }

void ParameterSymbol::Accept(SymbolVisitor* visitor) { visitor->visit(this); }

void InterfaceMethodSymbol::Accept(SymbolVisitor* visitor) {
  visitor->visit(this);
}

}  // namespace rcgo
