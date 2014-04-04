/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Symbol.hpp"
#include "SymbolVisitor.hpp"
#include "Type.hpp"
#include "Utf8.hpp"

#define ACCEPT(c)  void c::accept (SymbolVisitor& visitor) { visitor.visit (*this); }

namespace rcgo {

  bool is_exported (const std::string& id)
  {
    return is_unicode_upper (first_rune (id));
  }

  Symbol::Symbol (const std::string& a_identifier,
                  const Location& a_location,
                  const Package* a_package,
                  Ast* a_ast)
    : identifier (a_identifier)
    , location (a_location)
    , package (a_package)
    , ast (a_ast)
    , exported (is_exported (a_identifier))
    , m_state (New)
  { }

  Symbol::~Symbol ()
  { }

  bool Symbol::is_new () const
  {
    return m_state == New;
  }

  bool Symbol::is_in_progress () const
  {
    return m_state == InProgress;
  }

  bool Symbol::is_defined () const
  {
    return m_state == Defined;
  }

  void Symbol::in_progress ()
  {
    assert (m_state == New);
    m_state = InProgress;
  }

  void Symbol::defined ()
  {
    assert (m_state == New || m_state == InProgress);
    m_state = Defined;
  }

  ConstantSymbol::ConstantSymbol (const std::string& a_identifier,
                                  const Location& a_location,
                                  const Package* a_package,
                                  const Value& a_value)
    : Symbol (a_identifier, a_location, a_package, NULL)
  {
    value (a_value);
  }

  ConstantSymbol::ConstantSymbol (const std::string& a_identifier,
                                  const Location& a_location,
                                  const Package* a_package,
                                  Ast* a_ast)
    : Symbol (a_identifier, a_location, a_package, a_ast)
  { }

  ConstantSymbol::~ConstantSymbol ()
  { }

  ACCEPT (ConstantSymbol);

  Value ConstantSymbol::value () const
  {
    return m_value;
  }

  void ConstantSymbol::value (const Value& a_value)
  {
    assert (m_value.kind == Value::Unknown);
    assert (a_value.kind != Value::Unknown);
    m_value = a_value;
    defined ();
  }

  TypeSymbol::TypeSymbol (const std::string& a_identifier,
                          const Location& a_location,
                          const Package* a_package,
                          const NamedType* a_type)
    : Symbol (a_identifier, a_location, a_package, NULL)
    , m_type (NULL)
  {
    type (a_type);
  }

  TypeSymbol::TypeSymbol (const std::string& a_identifier,
                          const Location& a_location,
                          const Package* a_package,
                          Ast* a_ast)
    : Symbol (a_identifier, a_location, a_package, a_ast)
    , m_type (NULL)
  { }

  TypeSymbol::~TypeSymbol ()
  { }

  ACCEPT (TypeSymbol);

  const NamedType* TypeSymbol::type () const
  {
    return m_type;
  }

  void TypeSymbol::type (const NamedType* a_type)
  {
    assert (m_type == NULL);
    assert (a_type != NULL);
    m_type = a_type;
    defined ();
  }

  VariableSymbol::VariableSymbol (const std::string& a_identifier,
                                  const Location& a_location,
                                  const Package* a_package,
                                  Ast* a_ast)
    : Symbol (a_identifier, a_location, a_package, a_ast)
  { }

  VariableSymbol::~VariableSymbol ()
  { }

  ACCEPT (VariableSymbol);

  FunctionSymbol::FunctionSymbol (const std::string& a_identifier,
                                  const Location& a_location,
                                  const Package* a_package,
                                  Ast* a_ast)
    : Symbol (a_identifier, a_location, a_package, a_ast)
    , m_type (NULL)
  { }

  FunctionSymbol::~FunctionSymbol ()
  { }

  ACCEPT (FunctionSymbol);

  void FunctionSymbol::type (const FunctionType* a_type)
  {
    assert (m_type == NULL);
    assert (a_type != NULL);
    m_type = a_type;
    defined ();
  }

  const FunctionType* FunctionSymbol::type () const
  {
    return m_type;
  }

  ImportedSymbol::ImportedSymbol (const std::string& a_identifier,
                                  const Location& a_location,
                                  const Package* a_package,
                                  const Symbol* a_imported_symbol)
    : Symbol (a_identifier, a_location, a_package, NULL)
    , imported_symbol (a_imported_symbol)
  { }

  ImportedSymbol::~ImportedSymbol ()
  { }

  ACCEPT (ImportedSymbol);

  PackageSymbol::PackageSymbol (const std::string& a_identifier,
                                const Location& a_location,
                                const Package* a_package,
                                const Package* a_the_package)
    : Symbol (a_identifier, a_location, a_package, NULL)
    , the_package (a_the_package)
  { }

  PackageSymbol::~PackageSymbol ()
  { }

  ACCEPT (PackageSymbol);

  FieldSymbol::FieldSymbol (const std::string& a_identifier,
                            const Location& a_location,
                            const Package* a_package,
                            const Type* a_type,
                            const std::string& a_tag,
                            bool a_is_embedded)
    : Symbol (a_identifier, a_location, a_package, NULL)
    , type (a_type)
    , tag (a_tag)
    , is_embedded (a_is_embedded)
  { }

  FieldSymbol::~FieldSymbol ()
  { }

  ACCEPT (FieldSymbol);

  ParameterSymbol::ParameterSymbol (const std::string& a_identifier,
                                    const Location& a_location,
                                    const Package* a_package,
                                    const Type* a_type,
                                    bool a_is_variadic)
    : Symbol (a_identifier, a_location, a_package, NULL)
    , type (a_type)
    , is_variadic (a_is_variadic)
  { }

  ParameterSymbol::~ParameterSymbol ()
  { }

  ACCEPT (ParameterSymbol);

  InterfaceMethodSymbol::InterfaceMethodSymbol (const std::string& a_identifier,
                                                const Location& a_location,
                                                const Package* a_package,
                                                const FunctionType* a_type)
    : Symbol (a_identifier, a_location, a_package, NULL)
    , type (a_type)
  { }

  InterfaceMethodSymbol::~InterfaceMethodSymbol ()
  { }

  ACCEPT (InterfaceMethodSymbol);

}
