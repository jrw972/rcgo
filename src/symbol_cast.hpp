#ifndef RC_SRC_SYMBOL_CAST_HPP
#define RC_SRC_SYMBOL_CAST_HPP

#include "symbol.hpp"
#include "symbol_visitor.hpp"

namespace decl
{

template <typename T>
T* symbol_cast (Symbol* symbol)
{
  struct visitor : public SymbolVisitor
  {
    T* retval;
    visitor () : retval (NULL) { }
    virtual void visit (T& s)
    {
      retval = & s;
    }
  };
  visitor v;
  symbol->accept (v);
  return v.retval;
}

template <typename T>
const T* symbol_cast (const Symbol* symbol)
{
  struct visitor : public ConstSymbolVisitor
  {
    const T* retval;
    visitor () : retval (NULL) { }
    virtual void visit (const T& s)
    {
      retval = & s;
    }
  };
  visitor v;
  symbol->accept (v);
  return v.retval;
}

}

#endif // RC_SRC_SYMBOL_CAST_HPP
