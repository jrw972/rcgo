#ifndef rc_src_bind_hpp
#define rc_src_bind_hpp

#include "memory_model.hpp"

namespace decl
{

class bind_t
{
public:
  bind_t (ast::Node* node, const std::string& name_, decl::ParameterSymbol* rp)
    : node_ (node)
    , name (name_)
    , receiver_parameter (rp)
  { }

  ast::Node* node () const
  {
    return node_;
  }

  runtime::MemoryModel memory_model;

private:
  ast::Node* node_;
public:
  const std::string& name;
  decl::ParameterSymbol* receiver_parameter;
};

}

#endif // rc_src_bind_hpp
