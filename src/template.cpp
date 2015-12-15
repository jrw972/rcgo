#include "template.hpp"

#include "symbol_visitor.hpp"

void
Template::accept (SymbolVisitor& visitor)
{
  visitor.visit (*this);
}

void
Template::accept (ConstSymbolVisitor& visitor) const
{
  visitor.visit (*this);
}
