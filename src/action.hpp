#ifndef action_hpp
#define action_hpp

#include "types.hpp"
#include "Type.hpp"
#include "MemoryModel.hpp"
#include "typed_value.hpp"

class action_reaction_base_t
{
public:
  typedef std::vector<Activation*> ActivationsType;

  action_reaction_base_t (Type::NamedType* type, ast_t* node_, ast_t* body_)
    : type_ (type)
    , node (node_)
    , body (body_)
    , has_dimension_ (false)
  { }

  action_reaction_base_t (Type::NamedType* type, ast_t* node_, ast_t* body_, const typed_value_t& dimension)
    : type_ (type)
    , node (node_)
    , body (body_)
    , has_dimension_ (true)
    , dimension_ (dimension)
  { }

  virtual ~action_reaction_base_t() { };

  Type::NamedType* type () const
  {
    return type_;
  }

  void
  add_activation (Activation * activation)
  {
    activations_.push_back (activation);
  }

  ActivationsType::const_iterator begin () const
  {
    return activations_.begin ();
  }
  ActivationsType::const_iterator end () const
  {
    return activations_.end ();
  }

  MemoryModel memory_model;

  bool has_dimension () const
  {
    return has_dimension_;
  }

  typed_value_t dimension () const
  {
    assert (has_dimension_);
    return dimension_;
  }

private:
  Type::NamedType* type_;
public:
  ast_t* const node;
  ast_t* const body;
private:
  ActivationsType activations_;
  bool has_dimension_;
  typed_value_t dimension_;
};

class action_t : public action_reaction_base_t
{
public:
  enum PreconditionKind
  {
    DYNAMIC,
    STATIC_TRUE,
    STATIC_FALSE,
  };

  action_t (Type::NamedType* type, ast_t* node, ast_t* body_)
    : action_reaction_base_t (type, node, body_)
    , precondition_kind (DYNAMIC)
    , precondition (NULL)
  { }

  action_t (Type::NamedType* type, ast_t* node, ast_t* body_, const typed_value_t& dimension)
    : action_reaction_base_t (type, node, body_, dimension)
    , precondition_kind (DYNAMIC)
    , precondition (NULL)
  { }

  PreconditionKind precondition_kind;
  ast_t* precondition;
};

class reaction_t : public action_reaction_base_t
{
public:
  reaction_t (Type::NamedType* type, ast_t* node, ast_t* body_, const std::string& name_, const Type::Method* rt)
    : action_reaction_base_t (type, node, body_)
    , name (name_)
    , reaction_type (rt)
  { }

  reaction_t (Type::NamedType* type, ast_t* node, ast_t* body_, const std::string& name_, const Type::Method* rt, const typed_value_t& dimension)
    : action_reaction_base_t (type, node, body_, dimension)
    , name (name_)
    , reaction_type (rt)
  { }

  std::string const name;
  const Type::Method* const reaction_type;
};

#endif /* action_hpp */
