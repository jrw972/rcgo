#include "action.hpp"

namespace decl
{
using namespace ast;
using namespace type;

Action::Action (decl::Parameter* a_receiver_parameter,
                Node* a_precondition,
                Node* a_body,
                const std::string& a_name)
  : receiver_parameter (a_receiver_parameter)
  , precondition (a_precondition)
  , body (a_body)
  , name (a_name)
  , iota_parameter (NULL)
  , dimension (0)
  , precondition_kind (Dynamic)
{ }

Action::Action (decl::Parameter* a_receiver_parameter,
                Node* a_precondition,
                Node* a_body,
                const std::string& a_name,
                decl::Parameter* a_iota_parameter,
                long a_dimension)
  : receiver_parameter (a_receiver_parameter)
  , precondition (a_precondition)
  , body (a_body)
  , name (a_name)
  , iota_parameter (a_iota_parameter)
  , dimension (a_dimension)
  , precondition_kind (Dynamic)
{ }

bool Action::has_dimension () const
{
  return dimension != 0;
}

}
