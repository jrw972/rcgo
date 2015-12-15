#ifndef rc_src_reaction_hpp
#define rc_src_reaction_hpp

#include "callable.hpp"

namespace decl
{

class reaction_t : public Callable
{
public:
  reaction_t (ast::Node* a_node, decl::Symbol* a_receiver, ast::Node* body_, const std::string& name_, const type::Method* rt)
    : node (a_node)
    , receiver (a_receiver)
    , body (body_)
    , name (name_)
    , has_dimension_ (false)
    , iota (NULL)
    , reaction_type (rt)
  { }

  reaction_t (ast::Node* a_node, decl::Symbol* a_receiver, ast::Node* body_, const std::string& name_, const type::Method* rt, decl::Symbol* a_iota, type::Int::ValueType dimension)
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

  void call (runtime::executor_base_t& exec) const;

  const type::Type* type () const
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
  virtual const type::Signature* signature () const
  {
    return reaction_type->signature;
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

#endif // rc_src_reaction_hpp
