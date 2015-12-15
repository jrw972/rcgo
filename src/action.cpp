#include "action.hpp"

namespace decl
{
using namespace ast;
using namespace type;

Action::Action (Node* a_body,
                const std::string& a_name)
  : precondition (NULL)
  , body (a_body)
  , name (a_name)
  , dimension (0)
  , precondition_kind (Dynamic)
{ }

Action::Action (Node* a_body,
                const std::string& a_name,
                Int::ValueType a_dimension)
  : precondition (NULL)
  , body (a_body)
  , name (a_name)
  , dimension (a_dimension)
  , precondition_kind (Dynamic)
{ }

}
