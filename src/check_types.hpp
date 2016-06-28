#ifndef RC_SRC_CHECK_TYPES_HPP
#define RC_SRC_CHECK_TYPES_HPP

#include "types.hpp"

namespace semantic
{
void check_types_arguments (ast::List* node, const decl::ParameterList* signature);
void require_value_or_variable_list (const ast::List* node);
void check_mutability_arguments (ast::Node* node, const decl::ParameterList* signature);

void check_types (ast::Node* root, util::ErrorReporter& er, decl::SymbolTable& symtab);
}

#endif // RC_SRC_CHECK_TYPES_HPP
