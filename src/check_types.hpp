#ifndef rc_src_check_types_hpp
#define rc_src_check_types_hpp

#include "types.hpp"

namespace semantic
{
void check_types_arguments (ast::Node* node, const Type::Signature* signature);
void require_type (const ast::Node* node);
void require_value_or_variable_list (const ast::Node* node);
void check_types (ast::Node* root);
}

#endif // rc_src_check_types_hpp
