#include "template.hpp"
#include "symbol_visitor.hpp"
#include "ast.hpp"
#include <error.h>
#include "runtime.hpp"

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
