#ifndef RC_SRC_REACTION_HPP
#define RC_SRC_REACTION_HPP

#include "callable.hpp"

namespace decl
{

class Reaction : public Callable
{
public:
  Reaction (decl::Symbol* a_receiver, ast::Node* body_, const std::string& name_, const type::Method* rt)
    : operation (NULL)
    , receiver (a_receiver)
    , body (body_)
    , name (name_)
    , has_dimension_ (false)
    , iota (NULL)
    , reaction_type (rt)
  { }

  Reaction (decl::Symbol* a_receiver, ast::Node* body_, const std::string& name_, const type::Method* rt, decl::Symbol* a_iota, type::Int::ValueType dimension)
    : operation (NULL)
    , receiver (a_receiver)
    , body (body_)
    , name (name_)
    , has_dimension_ (true)
    , iota (a_iota)
    , dimension_ (dimension)
    , reaction_type (rt)
  { }

public:
  runtime::Operation* operation;
  decl::Symbol* const receiver;
  ast::Node* const body;
  std::string const name;
  ReceiverAccess immutable_phase_access;
private:
  bool has_dimension_;
public:
  decl::Symbol* const iota;
private:
  type::Int::ValueType dimension_;
public:
  const type::Method* const reaction_type;

  void call (runtime::ExecutorBase& exec) const;

  const type::Type* type () const
  {
    // This used to the named type.
    return reaction_type;
  }

  size_t return_size () const
  {
    UNIMPLEMENTED;
  }
  virtual size_t receiver_size () const
  {
    return reaction_type->receiver_type ()->Size ();
  }
  size_t arguments_size () const
  {
    UNIMPLEMENTED;
  }
  size_t locals_size () const
  {
    return memory_model.locals_size ();
  }
  virtual const decl::ParameterList* signature () const
  {
    return reaction_type->parameter_list;
  }

  runtime::MemoryModel memory_model;

  bool has_dimension () const
  {
    return has_dimension_;
  }

  type::Int::ValueType dimension () const
  {
    assert (has_dimension_);
    return dimension_;
  }
};

}

#endif // RC_SRC_REACTION_HPP
