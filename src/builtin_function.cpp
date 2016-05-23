#include "builtin_function.hpp"

#include "semantic.hpp"
#include "parameter_list.hpp"

namespace decl
{
using namespace semantic;

BuiltinFunction::BuiltinFunction (const std::string& id,
                                  const util::Location& loc,
                                  const type::Function* a_type)
  : Symbol (id, loc)
  , type (a_type)
{
  allocate_parameters (memory_model_, type->parameter_list);
  allocate_symbol (memory_model_, type->GetReturnParameter ());
}

void
BuiltinFunction::accept (decl::SymbolVisitor& visitor)
{
  visitor.visit (*this);
}

void
BuiltinFunction::accept (decl::ConstSymbolVisitor& visitor) const
{
  visitor.visit (*this);
}

size_t BuiltinFunction::arguments_size () const
{
  return type->parameter_list->allocation_size ();
}

}
