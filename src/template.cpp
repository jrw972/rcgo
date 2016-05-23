#include "template.hpp"

#include "symbol_visitor.hpp"

namespace decl
{

TemplateSymbol::TemplateSymbol (const std::string& id,
                                const util::Location& loc,
                                const type::Type* a_type)
  : Symbol (id, loc)
  , type (a_type)
{ }

void
TemplateSymbol::accept (SymbolVisitor& visitor)
{
  visitor.visit (*this);
}

void
TemplateSymbol::accept (ConstSymbolVisitor& visitor) const
{
  visitor.visit (*this);
}

const char* TemplateSymbol::kindString () const
{
  return "Template";
}
const type::Type* TemplateSymbol::symbol_type () const
{
  return type;
}

}
