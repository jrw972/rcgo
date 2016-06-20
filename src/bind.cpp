#include "bind.hpp"

namespace decl
{

Bind::Bind (ast::Node* a_node,
            const std::string& a_name,
            decl::ParameterSymbol* a_receiver_parameter)
  : node (a_node)
  , name (a_name)
  , receiver_parameter (a_receiver_parameter)
{ }

}
