#ifndef RC_SRC_BIND_HPP
#define RC_SRC_BIND_HPP

#include "memory_model.hpp"

namespace decl
{

struct Bind
{
  Bind (ast::Node* node,
        const std::string& name,
        ParameterSymbol* receiver_parameter);

  runtime::MemoryModel memory_model;
  ast::Node* const node;
  std::string const name;
  ParameterSymbol* const receiver_parameter;
};

}

#endif // RC_SRC_BIND_HPP
