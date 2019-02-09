// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/symbol.h"

#include "src/type.h"
#include "src/utf8.h"

namespace rcgo {
namespace symbol {

bool IsExported(const std::string& id) {
  return IsUnicodeUpper(FirstRune(id));
}

void Constant::Accept(Visitor* visitor) { visitor->Visit(this); }

void Constant::value(value::Value const & a_value) {
  MakeDefined(a_value);
}

void Type::Accept(Visitor* visitor) { visitor->Visit(this); }

void Type::type(const type::NamedType* a_type) {
  assert(m_type == nullptr);
  assert(a_type != nullptr);
  m_type = a_type;
  MakeDefined(value::Value::MakeType(a_type));
}

void Variable::Accept(Visitor* visitor) { visitor->Visit(this); }

void Function::Accept(Visitor* visitor) { visitor->Visit(this); }

void Function::type(const type::Function* a_type) {
  assert(m_type == nullptr);
  assert(a_type != nullptr);
  m_type = a_type;
  MakeDefined(value::Value::MakeFunction(a_type));
}

void Package::Accept(Visitor* visitor) { visitor->Visit(this); }

void Field::Accept(Visitor* visitor) { visitor->Visit(this); }

void Parameter::Accept(Visitor* visitor) { visitor->Visit(this); }

void InterfaceMethod::Accept(Visitor* visitor) {
  visitor->Visit(this);
}

symbol::Symbol* Table::Find(const std::string& identifier) const {
  MapType::const_iterator pos = m_map.find(identifier);
  if (pos != m_map.end()) return pos->second;
  return nullptr;
}

}  // namespace symbol
}  // namespace rcgo
