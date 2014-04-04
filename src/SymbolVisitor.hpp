#ifndef RCGO_SRC_SYMBOL_VISITOR_HPP
#define RCGO_SRC_SYMBOL_VISITOR_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.hpp"
#include "Symbol.hpp"

namespace rcgo {

  struct SymbolVisitor {
    virtual ~SymbolVisitor ();
    virtual void visit (ConstantSymbol& symbol) = 0;
    virtual void visit (TypeSymbol& symbol) = 0;
    virtual void visit (VariableSymbol& symbol) = 0;
    virtual void visit (FunctionSymbol& symbol) = 0;
    virtual void visit (ImportedSymbol& symbol) = 0;
    virtual void visit (PackageSymbol& symbol) = 0;
    virtual void visit (FieldSymbol& symbol) = 0;
    virtual void visit (ParameterSymbol& symbol) = 0;
    virtual void visit (InterfaceMethodSymbol& symbol) = 0;
  };

  struct DefaultSymbolVisitor : public SymbolVisitor {
    DefaultSymbolVisitor ();
    virtual ~DefaultSymbolVisitor ();
    virtual void visit (ConstantSymbol& symbol);
    virtual void visit (TypeSymbol& symbol);
    virtual void visit (VariableSymbol& symbol);
    virtual void visit (FunctionSymbol& symbol);
    virtual void visit (ImportedSymbol& symbol);
    virtual void visit (PackageSymbol& symbol);
    virtual void visit (FieldSymbol& symbol);
    virtual void visit (ParameterSymbol& symbol);
    virtual void visit (InterfaceMethodSymbol& symbol);
    virtual void default_action (Symbol& ast);
  };

  template<typename T>
  T* symbol_cast (Symbol* symbol)
  {
    if (symbol == NULL) { return NULL; }

    struct visitor : public DefaultSymbolVisitor {
      T* retval;
      visitor () : retval (NULL) { }
      void visit (T& ast) { retval = &ast; }
    };

    visitor v;
    symbol->accept (v);
    return v.retval;
  }

}

#endif // RCGO_SRC_SYMBOL_VISITOR_HPP
