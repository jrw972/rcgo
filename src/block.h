#ifndef SRC_BLOCK_H_
#define SRC_BLOCK_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "src/symbol.h"
#include "src/package.h"

namespace rcgo {

struct Block {
  Block(const symbol::Table* a_symbol_table, Block* a_parent)
      : m_symbol_table(a_symbol_table), m_parent(a_parent)
  {}

  virtual ~Block() {}
  symbol::Symbol* FindLocalSymbol(const std::string& identifier) const {
    return m_symbol_table->Find(identifier);
  }
  symbol::Symbol* FindGlobalSymbol(const std::string& identifier) const;

  const Package* package() const { return m_symbol_table->package; }

 private:
  const symbol::Table* const m_symbol_table;
  Block* const m_parent;
};

struct MutableBlock : public Block {
  MutableBlock(symbol::Table* a_symbol_table, Block* a_parent)
      : Block(a_symbol_table, a_parent), m_mutable_symbol_table(a_symbol_table)
  {}

  void Insert(symbol::Symbol* symbol) {
    m_mutable_symbol_table->Insert(symbol);
  }

  symbol::Constant* MakeConstant(
      const std::string& a_identifier, const Location& a_location,
      value::ConstValuePtr a_value) {
    symbol::Constant* c =
        new symbol::Constant(a_identifier, a_location, package(), a_value);
    m_mutable_symbol_table->PushBack(c);
    return c;
  }

  symbol::Constant* MakeConstant(
      const std::string& a_identifier, const Location& a_location,
      ast::Node* a_ast) {
    symbol::Constant* c =
        new symbol::Constant(a_identifier, a_location, package(), a_ast);
    m_mutable_symbol_table->PushBack(c);
    return c;
  }

  symbol::Type* MakeType(
      const std::string& a_identifier, const Location& a_location,
      const type::NamedType* a_type) {
    symbol::Type* t =
        new symbol::Type(a_identifier, a_location, package(), a_type);
    m_mutable_symbol_table->PushBack(t);
    return t;
  }

  symbol::Type* MakeType(
      const std::string& a_identifier, const Location& a_location,
      ast::Node* a_ast) {
    symbol::Type* t =
        new symbol::Type(a_identifier, a_location, package(), a_ast);
    m_mutable_symbol_table->PushBack(t);
    return t;
  }

  symbol::Variable* MakeVariable(
      const std::string& a_identifier, const Location& a_location,
      ast::Node* a_ast) {
    symbol::Variable* v =
        new symbol::Variable(a_identifier, a_location, package(), a_ast);
    m_mutable_symbol_table->PushBack(v);
    return v;
  }

  symbol::Function* MakeFunction(
      const std::string& a_identifier, const Location& a_location,
      ast::Node* a_ast) {
    symbol::Function* f =
        new symbol::Function(a_identifier, a_location, package(), a_ast);
    m_mutable_symbol_table->PushBack(f);
    return f;
  }

  symbol::Function* MakeFunction(
      const std::string& a_identifier, const Location& a_location,
      type::Function* function) {
    symbol::Function* f =
        new symbol::Function(a_identifier, a_location, package(), function);
    m_mutable_symbol_table->PushBack(f);
    return f;
  }

  symbol::Package* MakePackage(
      const std::string& a_identifier, const Location& a_location,
      const rcgo::Package* a_the_package) {
    symbol::Package* p =
        new symbol::Package(a_identifier, a_location, package(), a_the_package);
    m_mutable_symbol_table->PushBack(p);
    return p;
  }

 private:
  symbol::Table* const m_mutable_symbol_table;
};

}  // namespace rcgo

#endif  // SRC_BLOCK_H_
