#ifndef action_hpp
#define action_hpp

#include "types.hpp"
#include "Type.hpp"
#include "MemoryModel.hpp"

class action_reaction_base_t
{
public:
  action_reaction_base_t (Type::NamedType* type, ast_t* node_, Symbol* receiver_, ast_t* body_, const std::string& name_)
    : type_ (type)
    , node (node_)
    , receiver (receiver_)
    , body (body_)
    , name (name_)
    , has_dimension_ (false)
    , iota (NULL)
  { }

  action_reaction_base_t (Type::NamedType* type, ast_t* node_, Symbol* receiver_, ast_t* body_, const std::string& name_, Symbol* iota_, Type::Uint::ValueType dimension)
    : type_ (type)
    , node (node_)
    , receiver (receiver_)
    , body (body_)
    , name (name_)
    , has_dimension_ (true)
    , iota (iota_)
    , dimension_ (dimension)
  { }

  virtual ~action_reaction_base_t() { };

  Type::NamedType* type () const
  {
    return type_;
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

private:
  Type::NamedType* type_;
public:
  ast_t* const node;
  Symbol* const receiver;
  ast_t* const body;
  std::string const name;
  ReceiverAccess immutable_phase_access;
private:
  bool has_dimension_;
public:
  Symbol* const iota;
private:
  Type::Uint::ValueType dimension_;
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

  action_t (Type::NamedType* type, ast_t* node, Symbol* receiver, ast_t* body_, const std::string& name_)
    : action_reaction_base_t (type, node, receiver, body_, name_)
    , precondition_kind (DYNAMIC)
    , precondition (NULL)
  { }

  action_t (Type::NamedType* type, ast_t* node, Symbol* receiver, ast_t* body_, const std::string& name, Symbol* iota, Type::Uint::ValueType dimension)
    : action_reaction_base_t (type, node, receiver, body_, name, iota, dimension)
    , precondition_kind (DYNAMIC)
    , precondition (NULL)
  { }

  PreconditionKind precondition_kind;
  ast_t* precondition;
  ReceiverAccess precondition_access;
};

class reaction_t : public action_reaction_base_t
{
public:
  reaction_t (Type::NamedType* type, ast_t* node, Symbol* receiver, ast_t* body_, const std::string& name_, const Type::Method* rt)
    : action_reaction_base_t (type, node, receiver, body_, name_)
    , reaction_type (rt)
  { }

  reaction_t (Type::NamedType* type, ast_t* node, Symbol* receiver, ast_t* body_, const std::string& name_, const Type::Method* rt, Symbol* iota, Type::Uint::ValueType dimension)
    : action_reaction_base_t (type, node, receiver, body_, name_, iota, dimension)
    , reaction_type (rt)
  { }

  const Type::Method* const reaction_type;
};

#endif /* action_hpp */
