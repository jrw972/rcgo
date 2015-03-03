#ifndef action_h
#define action_h

#include "types.hpp"
#include "strtab.hpp"
#include "type.hpp"
#include "memory_model.hpp"

class action_reaction_base_t
{
public:
  typedef std::vector<trigger_t*> TriggersType;

  action_reaction_base_t (type_t* component_type, ast_t* node)
    : component_type_ (component_type)
    , node_ (node)
    , has_dimension_ (false)
  { }

  action_reaction_base_t (type_t* component_type, ast_t* node, size_t dimension)
    : component_type_ (component_type)
    , node_ (node)
    , has_dimension_ (true)
    , dimension_ (dimension)
  { }

  virtual ~action_reaction_base_t() { };

  type_t* component_type () const { return component_type_; }
  ast_t* node () const { return node_; }

  void
  add_trigger (trigger_t * trigger)
  {
    triggers_.push_back (trigger);
  }

  TriggersType::const_iterator begin () const { return triggers_.begin (); }
  TriggersType::const_iterator end () const { return triggers_.end (); }

  memory_model_t memory_model;

  bool has_dimension () const { return has_dimension_; }

  size_t dimension () const
  {
    assert (has_dimension_);
    return dimension_;
  }

private:
  type_t *component_type_;	/* Back-pointer to component type. */
  ast_t* node_;
  TriggersType triggers_;
  bool has_dimension_;
  size_t dimension_;
};

class action_t : public action_reaction_base_t
{
public:
  action_t (named_type_t* type, ast_t* node)
    : action_reaction_base_t (type, node)
  { }

  action_t (named_type_t* type, ast_t* node, size_t dimension)
    : action_reaction_base_t (type, node, dimension)
  { }
};

class reaction_t : public action_reaction_base_t
{
public:
  reaction_t (named_type_t* type, ast_t* node, string_t name, const signature_type_t* signature)
    : action_reaction_base_t (type, node)
    , name_ (name)
    , reaction_type_ (new reaction_type_t (signature))
  { }

  reaction_t (named_type_t* type, ast_t* node, string_t name, const signature_type_t* signature, size_t dimension)
    : action_reaction_base_t (type, node, dimension)
    , name_ (name)
    , reaction_type_ (new reaction_type_t (signature))
  { }

  string_t name () const { return name_; }
  const reaction_type_t* reaction_type () const { return reaction_type_; }

private:
  string_t name_;
  reaction_type_t* reaction_type_;
};

#endif /* action_h */
