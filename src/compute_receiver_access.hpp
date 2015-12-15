#ifndef rc_src_compute_receiver_access_hpp
#define rc_src_compute_receiver_access_hpp

#include "types.hpp"

namespace semantic
{
void compute_receiver_access_arguments (ast::Node* args, const type::Signature* signature, ReceiverAccess& receiver_access, bool& flag);
void compute_receiver_access (ast::Node* root);
}

#endif // rc_src_compute_receiver_access_hpp
