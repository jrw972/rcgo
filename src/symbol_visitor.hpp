#ifndef RC_SRC_SYMBOL_VISITOR_HPP
#define RC_SRC_SYMBOL_VISITOR_HPP

#include "types.hpp"

namespace decl
{

struct SymbolVisitor
{
  virtual ~SymbolVisitor();
  virtual void visit (PolymorphicFunction& s);
  virtual void visit (FunctionBase& s);
  virtual void visit (MethodBase& s);
  virtual void visit (Instance& s);
  virtual void visit (Parameter& s);
  virtual void visit (type::NamedType& s);
  virtual void visit (Constant& s);
  virtual void visit (Variable& s);
  virtual void visit (Hidden& s);
  virtual void visit (Field& s);
  virtual void visit (Package& s);
  virtual void visit (ImportedSymbol& s);
  virtual void default_action (Symbol& s);
};

struct ConstSymbolVisitor
{
  virtual ~ConstSymbolVisitor();
  virtual void visit (const PolymorphicFunction& s);
  virtual void visit (const FunctionBase& s);
  virtual void visit (const MethodBase& s);
  virtual void visit (const Instance& s);
  virtual void visit (const Parameter& s);
  virtual void visit (const type::NamedType& s);
  virtual void visit (const Constant& s);
  virtual void visit (const Variable& s);
  virtual void visit (const Hidden& s);
  virtual void visit (const Field& s);
  virtual void visit (const Package& s);
  virtual void visit (const ImportedSymbol& s);
  virtual void default_action (const Symbol& s);
};

}

#endif // RC_SRC_SYMBOL_VISITOR_HPP
