#ifndef rc_reaction_hpp
#define rc_reaction_hpp

class reaction_t
{
public:
  reaction_t (Type::NamedType* type, ast::Node* a_node, Symbol* a_receiver, ast::Node* body_, const std::string& name_, const Type::Method* rt)
    : type_ (type)
    , node (a_node)
    , receiver (a_receiver)
    , body (body_)
    , name (name_)
    , has_dimension_ (false)
    , iota (NULL)
    , reaction_type (rt)
  { }

  reaction_t (Type::NamedType* type, ast::Node* a_node, Symbol* a_receiver, ast::Node* body_, const std::string& name_, const Type::Method* rt, Symbol* a_iota, Type::Uint::ValueType dimension)
    : type_ (type)
    , node (a_node)
    , receiver (a_receiver)
    , body (body_)
    , name (name_)
    , has_dimension_ (true)
    , iota (a_iota)
    , dimension_ (dimension)
    , reaction_type (rt)
  { }

private:
  Type::NamedType* type_;
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
};

#endif // rc_reaction_hpp
