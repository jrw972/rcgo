#ifndef bind_hpp
#define bind_hpp

#include "memory_model.hpp"

class bind_t
{
public:
  bind_t (ast_t* node)
    : node_ (node)
  { }

  ast_t* node () const { return node_; }

  memory_model_t memory_model;

private:
  ast_t* node_;
};

#endif /* bind_hpp */
