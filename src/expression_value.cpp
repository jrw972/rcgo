#include "expression_value.hpp"

namespace semantic
{

ExpressionValue::ExpressionValue ()
  : expression_kind (UnknownExpressionKind)
  , type (NULL)
  , receiver_access (AccessNone)
  , receiver_state (false)
{ }

bool ExpressionValue::is_unknown () const
{
  return expression_kind == UnknownExpressionKind;
}

bool ExpressionValue::is_type () const
{
  return expression_kind == TypeExpressionKind;
}

bool ExpressionValue::is_value_or_variable () const
{
  return expression_kind == ValueExpressionKind || expression_kind == VariableExpressionKind;
}

void ExpressionValue::fix_string_indirection_mutability ()
{
  if (expression_kind != ErrorExpressionKind &&
      type->underlying_kind () == type::kStringU)
    {
      indirection_mutability = std::max (indirection_mutability, Immutable);
    }
}

void ExpressionValue::convert (const type::Type* new_type)
{
  if (type != new_type)
    {
      if (value.present)
        {
          value.convert (type, new_type);
        }
      type = new_type;
    }
}

}
