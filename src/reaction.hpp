#ifndef rc_src_reaction_hpp
#define rc_src_reaction_hpp

#include "callable.hpp"

class reaction_t : public Callable
{
public:
  reaction_t (ast::Node* a_node, Symbol* a_receiver, ast::Node* body_, const std::string& name_, const Type::Method* rt)
    : node (a_node)
    , receiver (a_receiver)
    , body (body_)
    , name (name_)
    , has_dimension_ (false)
    , iota (NULL)
    , reaction_type (rt)
  { }

  reaction_t (ast::Node* a_node, Symbol* a_receiver, ast::Node* body_, const std::string& name_, const Type::Method* rt, Symbol* a_iota, Type::Int::ValueType dimension)
    : node (a_node)
    , receiver (a_receiver)
    , body (body_)
    , name (name_)
    , has_dimension_ (true)
    , iota (a_iota)
    , dimension_ (dimension)
    , reaction_type (rt)
  { }

public:
  ast::Node* const node;
  Symbol* const receiver;
  ast::Node* const body;
  std::string const name;
  ReceiverAccess immutable_phase_access;
private:
  bool has_dimension_;
public:
  Symbol* const iota;
private:
  Type::Int::ValueType dimension_;
public:
  const Type::Method* const reaction_type;

  void call (executor_base_t& exec) const;

  const Type::Type* type () const
  {
    // This used to the named type.
    return reaction_type;
  }

  size_t return_size () const
  {
    unimplemented;
  }
  virtual size_t receiver_size () const
  {
    return reaction_type->receiver_type ()->Size ();
  }
  size_t arguments_size () const
  {
    unimplemented;
  }
  size_t locals_size () const
  {
    return memory_model.LocalsSize ();
  }
  virtual const Type::Signature* signature () const
  {
    return reaction_type->signature;
  }

  MemoryModel memory_model;

  bool has_dimension () const
  {
    return has_dimension_;
  }

  Type::Int::ValueType dimension () const
  {
    assert (has_dimension_);
    return dimension_;
  }
};

#endif // rc_src_reaction_hpp
