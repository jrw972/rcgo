#include "reaction.hpp"

#include "runtime.hpp"
#include "parameter_list.hpp"

namespace decl
{

Reaction::Reaction (ast::Node* a_body,
                    const std::string& a_name,
                    const type::Method* a_reaction_type)
  : operation (NULL)
  , body (a_body)
  , name (a_name)
  , iota (NULL)
  , dimension (-1)
  , reaction_type (a_reaction_type)
{ }

Reaction::Reaction (ast::Node* a_body,
                    const std::string& a_name,
                    const type::Method* a_reaction_type,
                    Symbol* a_iota,
                    type::Int::ValueType a_dimension)
  : operation (NULL)
  , body (a_body)
  , name (a_name)
  , iota (a_iota)
  , dimension (a_dimension)
  , reaction_type (a_reaction_type)
{ }

void
Reaction::call (runtime::ExecutorBase& exec) const
{
  operation->execute (exec);
}

bool Reaction::has_dimension () const
{
  return iota != NULL;
}

const type::Type* Reaction::callable_type () const
{
  return reaction_type;
}

size_t Reaction::return_size_on_stack () const
{
  return reaction_type->return_parameter_list->size_on_stack ();
}

size_t Reaction::receiver_size_on_stack () const
{
  return util::align_up (reaction_type->receiver_type ()->Size (), arch::stack_alignment ());
}

size_t Reaction::parameters_size_on_stack () const
{
  return reaction_type->parameter_list->size_on_stack ();
}

const ParameterList* Reaction::parameter_list () const
{
  return reaction_type->parameter_list;
}

}
