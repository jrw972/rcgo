#ifndef RC_SRC_CHECK_MUTABILITY_HPP
#define RC_SRC_CHECK_MUTABILITY_HPP

#include "types.hpp"

namespace semantic
{
void check_mutability_arguments (ast::Node* node, const type::Signature* signature);
void check_mutability (ast::Node* root);
}

#endif // RC_SRC_CHECK_MUTABILITY_HPP
