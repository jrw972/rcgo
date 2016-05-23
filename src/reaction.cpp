#include "reaction.hpp"

#include "runtime.hpp"

namespace decl
{

Reaction::Reaction (decl::Symbol* a_receiver, ast::Node* a_body, const std::string& a_name, const type::Method* a_reaction_type)
  : operation (NULL)
  , receiver (a_receiver)
  , body (a_body)
  , name (a_name)
  , iota (NULL)
  , dimension (-1)
  , reaction_type (a_reaction_type)
{ }

Reaction::Reaction (decl::Symbol* a_receiver, ast::Node* a_body, const std::string& a_name, const type::Method* a_reaction_type, decl::Symbol* a_iota, type::Int::ValueType a_dimension)
  : operation (NULL)
  , receiver (a_receiver)
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
  // This used to the named type.
  return reaction_type;
}

size_t Reaction::return_size () const
{
  UNIMPLEMENTED;
}

size_t Reaction::receiver_size () const
{
  return reaction_type->receiver_type ()->Size ();
}

size_t Reaction::arguments_size () const
{
  UNIMPLEMENTED;
}

size_t Reaction::locals_size () const
{
  return memory_model.locals_size ();
}

const decl::ParameterList* Reaction::signature () const
{
  return reaction_type->parameter_list;
}

}
