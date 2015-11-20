#ifndef action_hpp
#define action_hpp

#include "types.hpp"
#include "Type.hpp"
#include "MemoryModel.hpp"

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

    Action (ast::Node* a_body,
            const std::string& a_name);

    Action (ast::Node* a_body,
            const std::string& a_name,
            Type::Int::ValueType a_dimension);

    // TODO:  Make this const and initialize upon construction.
    ast::Node* precondition;
    ast::Node* const body;
    std::string const name;
    Type::Int::ValueType const dimension;
    PreconditionKind precondition_kind;
    ReceiverAccess precondition_access;
    ReceiverAccess immutable_phase_access;
    MemoryModel memory_model;

    bool has_dimension () const
    {
      return dimension != 0;
    }
  };

}

#endif // action_hpp
