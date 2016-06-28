#ifndef RC_SRC_ACTION_HPP
#define RC_SRC_ACTION_HPP

#include "types.hpp"
#include "type.hpp"
#include "memory_model.hpp"

namespace decl
{

struct Action
{
  enum PreconditionKind
  {
    Dynamic,
    Static_True,
    Static_False,
  };

  Action (Parameter* receiver_parameter,
          ast::Node* precondition,
          ast::Node* body,
          const std::string& name);

  Action (Parameter* receiver_parameter,
          ast::Node* precondition,
          ast::Node* body,
          const std::string& name,
          Parameter* iota_parameter,
          long dimension);

  Parameter* const receiver_parameter;
  ast::Node* const precondition;
  ast::Node* const body;
  std::string const name;
  Parameter* const iota_parameter;
  long const dimension;
  PreconditionKind precondition_kind;
  ReceiverAccess precondition_access;
  ReceiverAccess immutable_phase_access;
  runtime::MemoryModel memory_model;

  bool has_dimension () const;
};

}

#endif // RC_SRC_ACTION_HPP
