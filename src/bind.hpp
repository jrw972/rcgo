#ifndef RC_SRC_BIND_HPP
#define RC_SRC_BIND_HPP

#include "memory_model.hpp"

namespace decl
{

struct Bind
{
  Bind (ast::Node* node,
        const std::string& name,
        Parameter* receiver_parameter);

  runtime::MemoryModel memory_model;
  ast::Node* const node;
  std::string const name;
  Parameter* const receiver_parameter;
};

}

#endif // RC_SRC_BIND_HPP
