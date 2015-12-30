#ifndef RC_SRC_COMPUTE_RECEIVER_ACCESS_HPP
#define RC_SRC_COMPUTE_RECEIVER_ACCESS_HPP

#include "types.hpp"

namespace semantic
{
void compute_receiver_access_arguments (ast::List* args, const type::Signature* signature, ReceiverAccess& receiver_access, bool& flag);
void compute_receiver_access (ast::Node* root);
}

#endif // RC_SRC_COMPUTE_RECEIVER_ACCESS_HPP
