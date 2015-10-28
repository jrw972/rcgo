#include "Template.hpp"
#include "SymbolVisitor.hpp"
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
