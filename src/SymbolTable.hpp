#ifndef RCGO_SRC_SYMBOL_TABLE_HPP
#define RCGO_SRC_SYMBOL_TABLE_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.hpp"

namespace rcgo {

  struct SymbolTable {
    typedef std::map<std::string, Symbol*> MapType;
    typedef MapType::const_iterator const_iterator;

    SymbolTable ();
    virtual ~SymbolTable ();
    Symbol* find (const std::string& identifier) const;
    void insert (Symbol* symbol);
    const_iterator begin () const;
    const_iterator end () const;

  private:
    MapType m_map;
  };

  struct Block {
    Block (SymbolTable& a_symbol_table, Block* a_parent, const Package* a_package = NULL);
    Symbol* find_local_symbol (const std::string& identifier) const;
    Symbol* find_global_symbol (const std::string& identifier) const;
    void insert (Symbol* symbol);
    const Package* package () const;
  private:
    SymbolTable& m_symbol_table;
    Block* m_parent;
    const Package* m_package;
  };
}

#endif // RCGO_SRC_SYMBOL_TABLE_HPP
