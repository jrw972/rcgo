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
  allocate_parameters (memory_model, type->parameter_list);
  allocate_parameters (memory_model, type->return_parameter_list);
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

const type::Type* BuiltinFunction::symbol_type () const
{
  return type;
}

const type::Type* BuiltinFunction::callable_type () const
{
  return type;
}
const decl::ParameterList* BuiltinFunction::parameter_list () const
{
  return type->parameter_list;
}
size_t BuiltinFunction::return_size_on_stack () const
{
  return type->return_parameter_list->size_on_stack ();
}
size_t BuiltinFunction::receiver_size_on_stack () const
{
  return 0;
}

size_t BuiltinFunction::parameters_size_on_stack () const
{
  return type->parameter_list->size_on_stack ();
}

}
