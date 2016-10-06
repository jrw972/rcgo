#include "symbol_visitor.hpp"

#include "polymorphic_function.hpp"
#include "callable.hpp"

namespace decl
{

SymbolVisitor::~SymbolVisitor() { }
void SymbolVisitor::visit (PolymorphicFunction& s)
{
  default_action (s);
}
void SymbolVisitor::visit (FunctionBase& s)
{
  default_action (s);
}
void SymbolVisitor::visit (MethodBase& s)
{
  default_action (s);
}
void SymbolVisitor::visit (Instance& s)
{
  default_action (s);
}
void SymbolVisitor::visit (Parameter& s)
{
  default_action (s);
}
void SymbolVisitor::visit (type::NamedType& s)
{
  default_action (s);
}
void SymbolVisitor::visit (Constant& s)
{
  default_action (s);
}
void SymbolVisitor::visit (Variable& s)
{
  default_action (s);
}
void SymbolVisitor::visit (Hidden& s)
{
  default_action (s);
}
void SymbolVisitor::visit (Field& s)
{
  default_action (s);
}
void SymbolVisitor::default_action (Symbol& s) { }

ConstSymbolVisitor::~ConstSymbolVisitor() { }
void ConstSymbolVisitor::visit (const PolymorphicFunction& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const FunctionBase& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const MethodBase& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const Instance& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const Parameter& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const type::NamedType& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const Constant& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const Variable& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const Hidden& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const Field& s)
{
  default_action (s);
}
void ConstSymbolVisitor::default_action (const Symbol& s) { }

}
