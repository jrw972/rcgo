// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/symbol_table.h"

#include "src/symbol.h"

namespace rcgo {

Symbol* SymbolTable::Find(const std::string& identifier) const {
  MapType::const_iterator pos = m_map.find(identifier);
  if (pos != m_map.end()) return pos->second;
  return NULL;
}

Symbol* Block::FindGlobalSymbol(const std::string& identifier) const {
  Symbol* s = m_symbol_table->Find(identifier);
  if (s != NULL) return s;
  if (m_parent != NULL) return m_parent->FindGlobalSymbol(identifier);
  return NULL;
}

}  // namespace rcgo
