#ifndef rc_check_mutability_hpp
#define rc_check_mutability_hpp

#include "types.hpp"

namespace semantic
{
  void check_mutability_arguments (ast::Node* node, const Type::Signature* signature);
  void check_mutability (ast::Node* root);
}

#endif // rc_check_mutability_hpp
