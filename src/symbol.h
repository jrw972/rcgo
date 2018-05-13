#ifndef SRC_SYMBOL_H_
#define SRC_SYMBOL_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cassert>
#include <string>

#include "src/location.h"
#include "src/value.h"

namespace rcgo {

// These forward declarations are used to break circular dependencies.
namespace type {
struct NamedType;
struct Function;
}
struct Package;
namespace ast {
struct Node;
}

bool IsExported(const std::string& id);

// TODO(jrw972):  Put in symbol namespace.

// forward declaration for visitor pattern.
struct SymbolVisitor;

struct Symbol {
  std::string const identifier;
  Location const location;
  const Package* const package;
  ast::Node* const ast;
  bool const exported;

  virtual ~Symbol() {}
  virtual void Accept(SymbolVisitor* visitor) = 0;

  bool IsNew() const { return m_state == kNew; }
  bool IsInProgress() const { return m_state == kInProgress; }
  bool IsDefined() const { return m_state == kDefined; }
  void MakeInProgress() {
    assert(m_state == kNew);
    m_state = kInProgress;
  }
  void MakeDefined() {
    assert(m_state == kNew || m_state == kInProgress);
    m_state = kDefined;
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
};

struct ConstantSymbol : public Symbol {
  ConstantSymbol(const std::string& a_identifier, const Location& a_location,
                 const Package* a_package, const Value& a_value)
      : Symbol(a_identifier, a_location, a_package, NULL) { value(a_value); }
  ConstantSymbol(const std::string& a_identifier, const Location& a_location,
                 const Package* a_package, ast::Node* a_ast)
      : Symbol(a_identifier, a_location, a_package, a_ast) { }

  void Accept(SymbolVisitor* visitor) override;

  Value value() const { return m_value; }
  void value(const Value& a_value);

 private:
  Value m_value;
};

struct TypeSymbol : public Symbol {
  TypeSymbol(const std::string& a_identifier, const Location& a_location,
             const Package* a_package, const type::NamedType* a_type)
      : Symbol(a_identifier, a_location, a_package, NULL), m_type(NULL) {
    type(a_type);
  }
  TypeSymbol(const std::string& a_identifier, const Location& a_location,
             const Package* a_package, ast::Node* a_ast)
      : Symbol(a_identifier, a_location, a_package, a_ast), m_type(NULL) {}

  void Accept(SymbolVisitor* visitor) override;

  const type::NamedType* type() const { return m_type; }
  void type(const type::NamedType* a_type);

 private:
  const type::NamedType* m_type;
};

struct VariableSymbol : public Symbol {
  VariableSymbol(const std::string& a_identifier, const Location& a_location,
                 const Package* a_package, ast::Node* a_ast)
      : Symbol(a_identifier, a_location, a_package, a_ast) {}

  void Accept(SymbolVisitor* visitor) override;
};

struct FunctionSymbol : public Symbol {
  FunctionSymbol(const std::string& a_identifier, const Location& a_location,
                 const Package* a_package, ast::Node* a_ast)
      : Symbol(a_identifier, a_location, a_package, a_ast), m_type(NULL) {}

  void Accept(SymbolVisitor* visitor) override;

  void type(const type::Function* a_type);
  const type::Function* type() const { return m_type; }

 private:
  const type::Function* m_type;
};

struct ImportedSymbol : public Symbol {
  ImportedSymbol(const std::string& a_identifier, const Location& a_location,
                 const Package* a_package, const Symbol* a_imported_symbol)
      : Symbol(a_identifier, a_location, a_package, NULL),
        imported_symbol(a_imported_symbol) {}

  void Accept(SymbolVisitor* visitor) override;

  const Symbol* imported_symbol;
};

struct PackageSymbol : public Symbol {
  PackageSymbol(const std::string& a_identifier, const Location& a_location,
                const Package* a_package, const Package* a_the_package)
      : Symbol(a_identifier, a_location, a_package, NULL),
        the_package(a_the_package) {}

  void Accept(SymbolVisitor* visitor) override;

  const Package* the_package;
};

struct FieldSymbol : public Symbol {
  FieldSymbol(const std::string& a_identifier, const Location& a_location,
              const Package* a_package, const type::Type* a_type,
              const std::string& a_tag, bool a_is_embedded)
      : Symbol(a_identifier, a_location, a_package, NULL), type(a_type),
        tag(a_tag), is_embedded(a_is_embedded) {}

  void Accept(SymbolVisitor* visitor) override;

  const type::Type* const type;
  std::string const tag;
  bool const is_embedded;
};

struct ParameterSymbol : public Symbol {
  ParameterSymbol(const std::string& a_identifier, const Location& a_location,
                  const Package* a_package, const type::Type* a_type,
                  bool a_is_variadic)
      : Symbol(a_identifier, a_location, a_package, NULL), type(a_type),
        is_variadic(a_is_variadic) {}

  void Accept(SymbolVisitor* visitor) override;

  const type::Type* const type;
  bool const is_variadic;
};

struct InterfaceMethodSymbol : public Symbol {
  InterfaceMethodSymbol(const std::string& a_identifier,
                        const Location& a_location, const Package* a_package,
                        const type::Function* a_type)
      : Symbol(a_identifier, a_location, a_package, NULL), type(a_type) {}

  void Accept(SymbolVisitor* visitor) override;

  const type::Function* const type;
};

struct SymbolVisitor {
  virtual ~SymbolVisitor() {}
  virtual void visit(ConstantSymbol* symbol) = 0;
  virtual void visit(TypeSymbol* symbol) = 0;
  virtual void visit(VariableSymbol* symbol) = 0;
  virtual void visit(FunctionSymbol* symbol) = 0;
  virtual void visit(ImportedSymbol* symbol) = 0;
  virtual void visit(PackageSymbol* symbol) = 0;
  virtual void visit(FieldSymbol* symbol) = 0;
  virtual void visit(ParameterSymbol* symbol) = 0;
  virtual void visit(InterfaceMethodSymbol* symbol) = 0;
};

struct DefaultSymbolVisitor : public SymbolVisitor {
  void visit(ConstantSymbol* symbol) override { DefaultAction(symbol); }
  void visit(TypeSymbol* symbol) override { DefaultAction(symbol); }
  void visit(VariableSymbol* symbol) override { DefaultAction(symbol); }
  void visit(FunctionSymbol* symbol) override { DefaultAction(symbol); }
  void visit(ImportedSymbol* symbol) override { DefaultAction(symbol); }
  void visit(PackageSymbol* symbol) override { DefaultAction(symbol); }
  void visit(FieldSymbol* symbol) override { DefaultAction(symbol); }
  void visit(ParameterSymbol* symbol) override { DefaultAction(symbol); }
  void visit(InterfaceMethodSymbol* symbol) override { DefaultAction(symbol); }
  virtual void DefaultAction(Symbol* symbol) {}
};

template<typename T>
T* symbol_cast(Symbol* symbol) {
  if (symbol == NULL) return NULL;

  struct Visitor : public DefaultSymbolVisitor {
    T* retval;
    Visitor() : retval(NULL) { }
    void visit(T* ast) override { retval = ast; }
  };

  Visitor v;
  symbol->Accept(&v);
  return v.retval;
}

}  // namespace rcgo

#endif  // SRC_SYMBOL_H_
