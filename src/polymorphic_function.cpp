#include "polymorphic_function.hpp"

#include "symbol_visitor.hpp"

namespace decl
{
using namespace semantic;

PolymorphicFunction::PolymorphicFunction (const std::string& id,
    const util::Location& loc)
  : Symbol (id, loc)
{ }

void
PolymorphicFunction::accept (SymbolVisitor& visitor)
{
  visitor.visit (*this);
}

void
PolymorphicFunction::accept (ConstSymbolVisitor& visitor) const
{
  visitor.visit (*this);
}

}
