#ifndef RC_SRC_EXPRESSION_VALUE_HPP
#define RC_SRC_EXPRESSION_VALUE_HPP

#include "types.hpp"
#include "value.hpp"

namespace semantic
{

struct ExpressionValue
{
  ExpressionValue ();
  semantic::ExpressionKind expression_kind;
  const type::Type* type;
  semantic::Value value;
  Mutability intrinsic_mutability;
  Mutability indirection_mutability;
  ReceiverAccess receiver_access;
  bool receiver_state;

  bool is_unknown () const;
  bool is_type () const;
  bool is_value_or_variable () const;
  void fix_string_indirection_mutability ();
  void convert (const type::Type* type);
};

typedef std::vector<ExpressionValue> ExpressionValueList;
}

#endif // RC_SRC_EXPRESSION_VALUE_HPP
