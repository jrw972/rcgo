#ifndef RC_SRC_REACTION_HPP
#define RC_SRC_REACTION_HPP

#include "callable.hpp"

namespace decl
{

struct Reaction : public Callable
{
  Reaction (decl::Symbol* a_receiver, ast::Node* a_body, const std::string& a_name, const type::Method* a_reaction_type);
  Reaction (decl::Symbol* a_receiver, ast::Node* a_body, const std::string& a_name, const type::Method* a_reaction_type, decl::Symbol* a_iota, type::Int::ValueType a_dimension);

  runtime::Operation* operation;
  decl::Symbol* const receiver;
  ast::Node* const body;
  std::string const name;
  ReceiverAccess immutable_phase_access;
  decl::Symbol* const iota;
  type::Int::ValueType const dimension;
  const type::Method* const reaction_type;
  runtime::MemoryModel memory_model;

  bool has_dimension () const;

  void call (runtime::ExecutorBase& exec) const;
  const type::Type* callable_type () const;
  size_t return_size () const;
  virtual size_t receiver_size () const;
  size_t arguments_size () const;
  size_t locals_size () const;
  virtual const decl::ParameterList* signature () const;
};

}

#endif // RC_SRC_REACTION_HPP
