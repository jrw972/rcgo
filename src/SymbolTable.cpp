/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "SymbolTable.hpp"

#include "Symbol.hpp"

namespace rcgo {

  SymbolTable::SymbolTable ()
  { }

  SymbolTable::~SymbolTable ()
  {
    for (const_iterator pos = m_map.begin (),
           limit = m_map.end ();
         pos != limit;
         ++pos)
      {
        delete pos->second;
      }
  }

  Symbol* SymbolTable::find (const std::string& identifier) const
  {
    MapType::const_iterator pos = m_map.find (identifier);
    if (pos != m_map.end ())
      {
        return pos->second;
      }
    else
      {
        return NULL;
      }
  }

  void SymbolTable::insert (Symbol* symbol)
  {
    m_map.insert (std::make_pair (symbol->identifier, symbol));
  }

  SymbolTable::const_iterator SymbolTable::begin () const
  {
    return m_map.begin ();
  }

  SymbolTable::const_iterator SymbolTable::end () const
  {
    return m_map.end ();
  }

  Block::Block (SymbolTable& a_symbol_table, Block* a_parent, const Package* a_package)
    : m_symbol_table (a_symbol_table)
    , m_parent (a_parent)
    , m_package (a_package)
  { }

  Symbol* Block::find_local_symbol (const std::string& identifier) const
  {
    return m_symbol_table.find (identifier);
  }

  Symbol* Block::find_global_symbol (const std::string& identifier) const
  {
    Symbol* s = m_symbol_table.find (identifier);
    if (s != NULL)
      {
        return s;
      }
    if (m_parent != NULL)
      {
        return m_parent->find_global_symbol (identifier);
      }
    return NULL;
  }

  void Block::insert (Symbol* symbol)
  {
    m_symbol_table.insert (symbol);
  }

  const Package* Block::package () const
  {
    if (m_parent == NULL || m_package != NULL)
      {
        return m_package;
      }
    else
      {
        return m_parent->package ();
      }
  }
}
