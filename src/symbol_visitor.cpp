#include "symbol_visitor.hpp"

#include "builtin_function.hpp"
#include "template.hpp"

namespace decl
{

SymbolVisitor::~SymbolVisitor() { }
void SymbolVisitor::visit (BuiltinFunction& s)
{
  default_action (s);
}
void SymbolVisitor::visit (TemplateSymbol& s)
{
  default_action (s);
}
void SymbolVisitor::visit (Function& s)
{
  default_action (s);
}
void SymbolVisitor::visit (InstanceSymbol& s)
{
  default_action (s);
}
void SymbolVisitor::visit (ParameterSymbol& s)
{
  default_action (s);
}
void SymbolVisitor::visit (TypeSymbol& s)
{
  default_action (s);
}
void SymbolVisitor::visit (ConstantSymbol& s)
{
  default_action (s);
}
void SymbolVisitor::visit (VariableSymbol& s)
{
  default_action (s);
}
void SymbolVisitor::visit (HiddenSymbol& s)
{
  default_action (s);
}
void SymbolVisitor::default_action (Symbol& s) { }

ConstSymbolVisitor::~ConstSymbolVisitor() { }
void ConstSymbolVisitor::visit (const BuiltinFunction& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const TemplateSymbol& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const Function& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const InstanceSymbol& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const ParameterSymbol& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const TypeSymbol& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const ConstantSymbol& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const VariableSymbol& s)
{
  default_action (s);
}
void ConstSymbolVisitor::visit (const HiddenSymbol& s)
{
  default_action (s);
}
void ConstSymbolVisitor::default_action (const Symbol& s) { }

}
