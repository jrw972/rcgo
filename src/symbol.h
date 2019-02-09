#ifndef SRC_SYMBOL_H_
#define SRC_SYMBOL_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cassert>
#include <map>
#include <string>
#include <vector>

#include "src/location.h"
#include "src/value.h"

namespace rcgo {

// These forward declarations are used to break circular dependencies.
namespace type {
struct NamedType;
struct Function;
struct Interface;
struct Struct;
}

namespace ast {
struct Node;
}

struct Package;
struct MutableBlock;

namespace symbol {

bool IsExported(const std::string& id);

// forward declaration for visitor pattern.
struct Visitor;

struct Table;

struct Symbol {
  std::string const identifier;
  Location const location;
  const rcgo::Package* const package;
  ast::Node* const ast;
  bool const exported;

  virtual ~Symbol() {}
  virtual void Accept(Visitor* visitor) = 0;

  value::Value GetValue() const { return m_value; }

  bool IsNew() const { return m_state == kNew; }
  bool IsInProgress() const { return m_state == kInProgress; }
  bool IsDefined() const { return m_state == kDefined; }
  void MakeInProgress() {
    assert(m_state == kNew);
    m_state = kInProgress;
  }
  void MakeDefined(value::Value const & a_value) {
    assert(m_state == kNew || m_state == kInProgress);
    assert(m_value.IsUninitialized());
    assert(a_value.IsInitialized());
    m_state = kDefined;
    m_value = a_value;
  }

 protected:
  Symbol(const std::string& a_identifier, const Location& a_location,
         const Package* a_package, ast::Node* a_ast)
      : identifier(a_identifier), location(a_location), package(a_package),
        ast(a_ast), exported(IsExported(a_identifier)), m_state(kNew) { }

 private:
  enum State {
    kNew,
    kInProgress,
    kDefined
  };
  State m_state;
  value::Value m_value;
};

struct Constant : public Symbol {
  void Accept(Visitor* visitor) override;
  void value(value::Value const & a_value);

 private:
  friend struct rcgo::MutableBlock;
  Constant(const std::string& a_identifier, const Location& a_location,
           const Package* a_package, value::Value const & a_value)
      : Symbol(a_identifier, a_location, a_package, nullptr)
  { MakeDefined(a_value); }
  Constant(const std::string& a_identifier, const Location& a_location,
           const Package* a_package, ast::Node* a_ast)
      : Symbol(a_identifier, a_location, a_package, a_ast) { }
};

struct Type : public Symbol {
  void Accept(Visitor* visitor) override;
  const type::NamedType* type() const { return m_type; }
  void type(const type::NamedType* a_type);

 private:
  const type::NamedType* m_type;
  friend struct rcgo::MutableBlock;
  Type(const std::string& a_identifier, const Location& a_location,
       const Package* a_package, const type::NamedType* a_type)
      : Symbol(a_identifier, a_location, a_package, nullptr),
        m_type(nullptr) {
    type(a_type);
  }
  Type(const std::string& a_identifier, const Location& a_location,
       const Package* a_package, ast::Node* a_ast)
      : Symbol(a_identifier, a_location, a_package, a_ast),
        m_type(nullptr) {}
};

struct Variable : public Symbol {
  void Accept(Visitor* visitor) override;

 private:
  friend struct rcgo::MutableBlock;
  Variable(const std::string& a_identifier, const Location& a_location,
           const Package* a_package, ast::Node* a_ast)
      : Symbol(a_identifier, a_location, a_package, a_ast) {}
};

struct Function : public Symbol {
  void Accept(Visitor* visitor) override;
  void type(const type::Function* a_type);
  const type::Function* type() const { return m_type; }

 private:
  const type::Function* m_type;
  friend struct rcgo::MutableBlock;
  Function(const std::string& a_identifier, const Location& a_location,
           const Package* a_package, ast::Node* a_ast)
      : Symbol(a_identifier, a_location, a_package, a_ast),
        m_type(nullptr) {}
  Function(const std::string& a_identifier, const Location& a_location,
           const Package* a_package, type::Function* a_type)
      : Symbol(a_identifier, a_location, a_package, nullptr),
        m_type(nullptr) { type(a_type); }
};

struct Package : public Symbol {
  void Accept(Visitor* visitor) override;
  const rcgo::Package* const the_package;

 private:
  friend struct rcgo::MutableBlock;
  Package(const std::string& a_identifier, const Location& a_location,
          const rcgo::Package* a_package, const rcgo::Package* a_the_package)
      : Symbol(a_identifier, a_location, a_package, nullptr),
        the_package(a_the_package) {}
};

struct Field : public Symbol {
  void Accept(Visitor* visitor) override;
  const type::Type* const type;
  std::string const tag;
  bool const is_embedded;

 private:
  friend struct type::Struct;
  Field(const std::string& a_identifier, const Location& a_location,
        const rcgo::Package* a_package, const type::Type* a_type,
        const std::string& a_tag, bool a_is_embedded)
      : Symbol(a_identifier, a_location, a_package, nullptr),
        type(a_type), tag(a_tag), is_embedded(a_is_embedded) {}
};

struct Parameter : public Symbol {
  void Accept(Visitor* visitor) override;
  const type::Type* const type;
  bool const is_variadic;

 private:
  friend struct type::Function;
  Parameter(const std::string& a_identifier, const Location& a_location,
            const rcgo::Package* a_package, const type::Type* a_type,
            bool a_is_variadic)
      : Symbol(a_identifier, a_location, a_package, nullptr),
        type(a_type), is_variadic(a_is_variadic) {}
};

struct InterfaceMethod : public Symbol {
  void Accept(Visitor* visitor) override;
  const type::Function* const type;

 private:
  friend struct type::Interface;
  InterfaceMethod(const std::string& a_identifier, const Location& a_location,
                  const rcgo::Package* a_package, const type::Function* a_type)
      : Symbol(a_identifier, a_location, a_package, nullptr),
        type(a_type) {}
};

struct Visitor {
  virtual ~Visitor() {}
  virtual void Visit(Constant* symbol) = 0;
  virtual void Visit(Type* symbol) = 0;
  virtual void Visit(Variable* symbol) = 0;
  virtual void Visit(Function* symbol) = 0;
  virtual void Visit(Package* symbol) = 0;
  virtual void Visit(Field* symbol) = 0;
  virtual void Visit(Parameter* symbol) = 0;
  virtual void Visit(InterfaceMethod* symbol) = 0;
};

struct DefaultVisitor : public Visitor {
  void Visit(Constant* symbol) override { DefaultAction(symbol); }
  void Visit(Type* symbol) override { DefaultAction(symbol); }
  void Visit(Variable* symbol) override { DefaultAction(symbol); }
  void Visit(Function* symbol) override { DefaultAction(symbol); }
  void Visit(Package* symbol) override { DefaultAction(symbol); }
  void Visit(Field* symbol) override { DefaultAction(symbol); }
  void Visit(Parameter* symbol) override { DefaultAction(symbol); }
  void Visit(InterfaceMethod* symbol) override { DefaultAction(symbol); }
  virtual void DefaultAction(Symbol* symbol) {}
};

template<typename T>
T* Cast(Symbol* symbol) {
  if (symbol == nullptr) return nullptr;

  struct Visitor : public DefaultVisitor {
    T* retval;
    Visitor() : retval(nullptr) { }
    void Visit(T* ast) override { retval = ast; }
  };

  Visitor v;
  symbol->Accept(&v);
  return v.retval;
}

struct Table {
  typedef std::map<std::string, Symbol*> MapType;
  typedef MapType::const_iterator const_iterator;

  explicit Table(const rcgo::Package* a_package) : package(a_package) {}
  virtual ~Table() { for (auto symbol : m_list) delete symbol; }

  Symbol* Find(const std::string& identifier) const;
  void Insert(Symbol* symbol) { m_map[symbol->identifier] = symbol; }
  const_iterator Begin() const { return m_map.begin(); }
  const_iterator End() const { return m_map.end(); }

  const rcgo::Package* const package;

 protected:
  friend struct rcgo::MutableBlock;
  void PushBack(Symbol* symbol) { m_list.push_back(symbol); }

 private:
  MapType m_map;
  typedef std::vector<Symbol*> ListType;
  ListType m_list;
};

}  // namespace symbol
}  // namespace rcgo

#endif  // SRC_SYMBOL_H_
