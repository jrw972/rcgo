#include "template.hpp"

#include "symbol_visitor.hpp"

namespace decl
{

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

}
