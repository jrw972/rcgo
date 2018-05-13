#ifndef SRC_SYMBOL_TABLE_H_
#define SRC_SYMBOL_TABLE_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <map>
#include <string>
#include <utility>

#include "src/symbol.h"

namespace rcgo {

// TODO(jrw972):  Is there a way around these forward decls?
struct Package;

struct SymbolTable {
  typedef std::map<std::string, Symbol*> MapType;
  typedef MapType::const_iterator const_iterator;

  virtual ~SymbolTable() {
    for (MapType::value_type p : m_map) delete p.second;
  }

  Symbol* Find(const std::string& identifier) const;
  void Insert(Symbol* symbol) {
    m_map.insert(std::make_pair(symbol->identifier, symbol));
  }
  SymbolTable::const_iterator Begin() const { return m_map.begin(); }
  SymbolTable::const_iterator End() const { return m_map.end(); }

 private:
  MapType m_map;
};

struct Block {
  Block(SymbolTable* a_symbol_table, Block* a_parent,
        const Package* a_package = NULL)
      : m_symbol_table(a_symbol_table), m_parent(a_parent),
        m_package(a_package) {}

  Symbol* FindLocalSymbol(const std::string& identifier) const {
    return m_symbol_table->Find(identifier);
  }
  Symbol* FindGlobalSymbol(const std::string& identifier) const;
  void Insert(Symbol* symbol) { m_symbol_table->Insert(symbol); }
  const Package* GetPackage() const {
    return (m_parent == NULL || m_package != NULL) ? m_package :
        m_parent->GetPackage();
  }

 private:
  SymbolTable* m_symbol_table;
  Block* m_parent;
  const Package* m_package;
};

}  // namespace rcgo

#endif  // SRC_SYMBOL_TABLE_H_
