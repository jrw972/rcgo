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
    StaticTrue,
    StaticFalse,
  };

  Action (decl::ParameterSymbol* receiver_parameter,
          ast::Node* a_body,
          const std::string& a_name);

  Action (decl::ParameterSymbol* receiver_parameter,
          ast::Node* a_body,
          const std::string& a_name,
          decl::ParameterSymbol* iota_parameter,
          type::Int::ValueType a_dimension);

  decl::ParameterSymbol* const receiver_parameter;
  // TODO:  Make this const and initialize upon construction.
  ast::Node* precondition;
  ast::Node* const body;
  std::string const name;
  decl::ParameterSymbol* const iota_parameter;
  type::Int::ValueType const dimension;
  PreconditionKind precondition_kind;
  ReceiverAccess precondition_access;
  ReceiverAccess immutable_phase_access;
  runtime::MemoryModel memory_model;

  bool has_dimension () const
  {
    return dimension != 0;
  }
};

}

#endif // RC_SRC_ACTION_HPP
