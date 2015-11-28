#ifndef rc_check_types_hpp
#define rc_check_types_hpp

#include "types.hpp"

namespace semantic
{
  void check_types_arguments (ast::Node* node, const Type::Signature* signature);
  void check_types (ast::Node* root);
}

#endif // rc_check_types_hpp
