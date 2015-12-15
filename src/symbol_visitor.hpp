#ifndef rc_src_symbol_visitor_hpp
#define rc_src_symbol_visitor_hpp

#include "symbol.hpp"
#include "builtin_function.hpp"
#include "template.hpp"
#include "callable.hpp"

namespace decl
{

struct SymbolVisitor
{
  virtual ~SymbolVisitor() { }
  virtual void visit (BuiltinFunction& s)
  {
    defaultAction (s);
  }
  virtual void visit (decl::Template& s)
  {
    defaultAction (s);
  }
  virtual void visit (Function& s)
  {
    defaultAction (s);
  }
  virtual void visit (InstanceSymbol& s)
  {
    defaultAction (s);
  }
  virtual void visit (ParameterSymbol& s)
  {
    defaultAction (s);
  }
  virtual void visit (TypeSymbol& s)
  {
    defaultAction (s);
  }
  virtual void visit (ConstantSymbol& s)
  {
    defaultAction (s);
  }
  virtual void visit (VariableSymbol& s)
  {
    defaultAction (s);
  }
  virtual void visit (HiddenSymbol& s)
  {
    defaultAction (s);
  }
  virtual void defaultAction (Symbol& s) { }
};

struct ConstSymbolVisitor
{
  virtual ~ConstSymbolVisitor() { }
  virtual void visit (const BuiltinFunction& s)
  {
    defaultAction (s);
  }
  virtual void visit (const decl::Template& s)
  {
    defaultAction (s);
  }
  virtual void visit (const Function& s)
  {
    defaultAction (s);
  }
  virtual void visit (const InstanceSymbol& s)
  {
    defaultAction (s);
  }
  virtual void visit (const ParameterSymbol& s)
  {
    defaultAction (s);
  }
  virtual void visit (const TypeSymbol& s)
  {
    defaultAction (s);
  }
  virtual void visit (const ConstantSymbol& s)
  {
    defaultAction (s);
  }
  virtual void visit (const VariableSymbol& s)
  {
    defaultAction (s);
  }
  virtual void visit (const HiddenSymbol& s)
  {
    defaultAction (s);
  }
  virtual void defaultAction (const Symbol& s) { }
};

template <typename T>
T* SymbolCast (Symbol* symbol)
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
const T* SymbolCast (const Symbol* symbol)
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

#endif // rc_src_symbol_visitor_hpp
