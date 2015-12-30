#include "action.hpp"

namespace decl
{
using namespace ast;
using namespace type;

Action::Action (decl::ParameterSymbol* a_receiver_parameter,
                Node* a_body,
                const std::string& a_name)
  : receiver_parameter (a_receiver_parameter)
  , precondition (NULL)
  , body (a_body)
  , name (a_name)
  , iota_parameter (NULL)
  , dimension (0)
  , precondition_kind (Dynamic)
{ }

Action::Action (decl::ParameterSymbol* a_receiver_parameter,
                Node* a_body,
                const std::string& a_name,
                decl::ParameterSymbol* a_iota_parameter,
                Int::ValueType a_dimension)
  : receiver_parameter (a_receiver_parameter)
  , precondition (NULL)
  , body (a_body)
  , name (a_name)
  , iota_parameter (a_iota_parameter)
  , dimension (a_dimension)
  , precondition_kind (Dynamic)
{ }

}
