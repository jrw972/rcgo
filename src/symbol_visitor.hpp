#ifndef RC_SRC_SYMBOL_VISITOR_HPP
#define RC_SRC_SYMBOL_VISITOR_HPP

#include "types.hpp"

namespace decl
{

struct SymbolVisitor
{
  virtual ~SymbolVisitor();
  virtual void visit (BuiltinFunction& s);
  virtual void visit (TemplateSymbol& s);
  virtual void visit (Function& s);
  virtual void visit (InstanceSymbol& s);
  virtual void visit (ParameterSymbol& s);
  virtual void visit (TypeSymbol& s);
  virtual void visit (ConstantSymbol& s);
  virtual void visit (VariableSymbol& s);
  virtual void visit (HiddenSymbol& s);
  virtual void default_action (Symbol& s);
};

struct ConstSymbolVisitor
{
  virtual ~ConstSymbolVisitor();
  virtual void visit (const BuiltinFunction& s);
  virtual void visit (const TemplateSymbol& s);
  virtual void visit (const Function& s);
  virtual void visit (const InstanceSymbol& s);
  virtual void visit (const ParameterSymbol& s);
  virtual void visit (const TypeSymbol& s);
  virtual void visit (const ConstantSymbol& s);
  virtual void visit (const VariableSymbol& s);
  virtual void visit (const HiddenSymbol& s);
  virtual void default_action (const Symbol& s);
};

}

#endif // RC_SRC_SYMBOL_VISITOR_HPP
