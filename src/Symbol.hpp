#ifndef RCGO_SRC_SYMBOL_HPP
#define RCGO_SRC_SYMBOL_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.hpp"

#include "Location.hpp"
#include "Value.hpp"

namespace rcgo {

  bool is_exported (const std::string& id);

  struct Symbol {
    std::string const identifier;
    Location const location;
    const Package* const package;
    Ast* const ast;
    bool const exported;

    virtual ~Symbol ();
    virtual void accept (SymbolVisitor& visitor) = 0;
    bool is_new () const;
    bool is_in_progress () const;
    bool is_defined () const;
    void in_progress ();
    void defined ();

  protected:
    Symbol (const std::string& a_identifier,
            const Location& a_location,
            const Package* a_package,
            Ast* a_ast);

  private:
    enum State {
      New,
      InProgress,
      Defined
    };
    State m_state;
  };

  struct ConstantSymbol : public Symbol {
    ConstantSymbol (const std::string& a_identifier,
                    const Location& a_location,
                    const Package* a_package,
                    const Value& a_value);
    ConstantSymbol (const std::string& a_identifier,
                    const Location& a_location,
                    const Package* a_package,
                    Ast* a_ast);
    ~ConstantSymbol ();
    virtual void accept (SymbolVisitor& visitor);
    Value value () const;
    void value (const Value& a_value);
  private:
    Value m_value;
  };

  struct TypeSymbol : public Symbol {
    TypeSymbol (const std::string& a_identifier,
                       const Location& a_location,
                       const Package* a_package,
                       const NamedType* a_type);
    TypeSymbol (const std::string& a_identifier,
                       const Location& a_location,
                       const Package* a_package,
                       Ast* a_ast);
    ~TypeSymbol ();
    virtual void accept (SymbolVisitor& visitor);
    const NamedType* type () const;
    void type (const NamedType* a_type);
  private:
    const NamedType* m_type;
  };

  struct VariableSymbol : public Symbol {
    VariableSymbol (const std::string& a_identifier,
                    const Location& a_location,
                    const Package* a_package,
                    Ast* a_ast);
    ~VariableSymbol ();
    virtual void accept (SymbolVisitor& visitor);
  };

  struct FunctionSymbol : public Symbol {
    FunctionSymbol (const std::string& a_identifier,
                    const Location& a_location,
                    const Package* a_package,
                    Ast* a_ast);
    ~FunctionSymbol ();
    virtual void accept (SymbolVisitor& visitor);
    void type (const FunctionType* a_type);
    const FunctionType* type () const;
  private:
    const FunctionType* m_type;
  };

  struct ImportedSymbol : public Symbol {
    ImportedSymbol (const std::string& a_identifier,
                    const Location& a_location,
                    const Package* a_package,
                    const Symbol* a_imported_symbol);
    ~ImportedSymbol ();
    virtual void accept (SymbolVisitor& visitor);
    const Symbol* imported_symbol;
  };

  struct PackageSymbol : public Symbol {
    PackageSymbol (const std::string& a_identifier,
                   const Location& a_location,
                   const Package* a_package,
                   const Package* a_the_package);
    ~PackageSymbol ();
    virtual void accept (SymbolVisitor& visitor);
    const Package* the_package;
  };

  struct FieldSymbol : public Symbol {
    FieldSymbol (const std::string& a_identifier,
                 const Location& a_location,
                 const Package* a_package,
                 const Type* a_type,
                 const std::string& a_tag,
                 bool a_is_embedded);
    ~FieldSymbol ();
    virtual void accept (SymbolVisitor& visitor);
    const Type* const type;
    std::string const tag;
    bool const is_embedded;
  };

  struct ParameterSymbol : public Symbol {
    ParameterSymbol (const std::string& a_identifier,
                     const Location& a_location,
                     const Package* a_package,
                     const Type* a_type,
                     bool a_is_variadic);
    ~ParameterSymbol ();
    virtual void accept (SymbolVisitor& visitor);
    const Type* const type;
    bool const is_variadic;
  };

  struct InterfaceMethodSymbol : public Symbol {
    InterfaceMethodSymbol (const std::string& a_identifier,
                           const Location& a_location,
                           const Package* a_package,
                           const FunctionType* a_type);
    ~InterfaceMethodSymbol ();
    virtual void accept (SymbolVisitor& visitor);
    const FunctionType* const type;
  };

}

#endif // RCGO_SRC_SYMBOL_HPP
