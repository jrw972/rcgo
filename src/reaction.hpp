#ifndef RC_SRC_REACTION_HPP
#define RC_SRC_REACTION_HPP

#include "callable.hpp"

namespace decl
{

struct Reaction : public Callable
{
  Reaction (ast::Node* a_body,
            const std::string& a_name,
            const type::Reaction* a_reaction_type);
  Reaction (ast::Node* a_body,
            const std::string& a_name,
            const type::Reaction* a_reaction_type,
            Symbol* a_iota,
            type::Int::ValueType a_dimension);

  runtime::Operation* operation;
  ast::Node* const body;
  std::string const name;
  ReceiverAccess immutable_phase_access;
  Symbol* const iota;
  type::Int::ValueType const dimension;
  const type::Reaction* const reaction_type;

  bool has_dimension () const;

  // Callable
  void call (runtime::ExecutorBase& exec) const;
  const type::Type* callable_type () const;
  size_t return_size_on_stack () const;
  virtual size_t receiver_size_on_stack () const;
  size_t parameters_size_on_stack () const;
  virtual const ParameterList* parameter_list () const;
};

}

#endif // RC_SRC_REACTION_HPP
