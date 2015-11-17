#ifndef bind_hpp
#define bind_hpp

#include "MemoryModel.hpp"

class bind_t
{
public:
  bind_t (Ast::Node* node, const std::string& name_)
    : node_ (node)
    , name (name_)
  { }

  Ast::Node* node () const
  {
    return node_;
  }

  MemoryModel memory_model;

private:
  Ast::Node* node_;
public:
  const std::string& name;
};

#endif /* bind_hpp */
