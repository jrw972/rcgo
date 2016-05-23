#ifndef RC_SRC_COMPUTE_RECEIVER_ACCESS_HPP
#define RC_SRC_COMPUTE_RECEIVER_ACCESS_HPP

#include "types.hpp"
#include "expression_value.hpp"

namespace semantic
{
void compute_receiver_access_arguments (const ExpressionValueList& args, const decl::ParameterList* signature, ReceiverAccess& receiver_access, bool& flag);
void compute_receiver_access (ast::Node* root);
}

#endif // RC_SRC_COMPUTE_RECEIVER_ACCESS_HPP
