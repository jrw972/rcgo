#ifndef rc_src_bind_hpp
#define rc_src_bind_hpp

#include "memory_model.hpp"

class bind_t
{
public:
  bind_t (ast::Node* node, const std::string& name_, ParameterSymbol* rp)
    : node_ (node)
    , name (name_)
    , receiver_parameter (rp)
  { }

  ast::Node* node () const
  {
    return node_;
  }

  MemoryModel memory_model;

private:
  ast::Node* node_;
public:
  const std::string& name;
  ParameterSymbol* receiver_parameter;
};

#endif // rc_src_bind_hpp
