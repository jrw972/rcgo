#ifndef rc_check_references_hpp
#define rc_check_references_hpp

#include "types.hpp"

namespace semantic
{
  void require_type (const ast::Node* node);
  void require_value_or_variable_list (const ast::Node* node);
  void check_references (ast::Node* root);
}

#endif // rc_check_references_hpp
