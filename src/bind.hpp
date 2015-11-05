#ifndef bind_hpp
#define bind_hpp

#include "MemoryModel.hpp"

class bind_t
{
public:
  bind_t (ast_t* node, const std::string& name_)
    : node_ (node)
    , name (name_)
  { }

  ast_t* node () const
  {
    return node_;
  }

  MemoryModel memory_model;

private:
  ast_t* node_;
public:
  const std::string& name;
};

#endif /* bind_hpp */
