#ifndef rc_reaction_hpp
#define rc_reaction_hpp

#include "Callable.hpp"

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

  reaction_t (ast::Node* a_node, Symbol* a_receiver, ast::Node* body_, const std::string& name_, const Type::Method* rt, Symbol* a_iota, Type::Uint::ValueType dimension)
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
  Type::Uint::ValueType dimension_;
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
  size_t arguments_size () const
  {
    unimplemented;
  }
  size_t locals_size () const
  {
    unimplemented;
  }
  virtual const Type::Signature* signature () const
  {
    unimplemented;
  }
  virtual void check_types (ast::Node* args) const
  {
    unimplemented;
  }

  MemoryModel memory_model;

  bool has_dimension () const
  {
    return has_dimension_;
  }

  Type::Uint::ValueType dimension () const
  {
    assert (has_dimension_);
    return dimension_;
  }
};

#endif // rc_reaction_hpp
