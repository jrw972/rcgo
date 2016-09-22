#ifndef RCGO_SRC_ENTER_TOP_LEVEL_IDENTIFIERS_HPP
#define RCGO_SRC_ENTER_TOP_LEVEL_IDENTIFIERS_HPP

#include "types.hpp"

namespace semantic
{

// Enter constants, types, functions, and instances into the package scope.
void enter_top_level_identifiers (ast::Node* root, util::ErrorReporter& er, decl::Scope* package_scope, decl::Scope* file_scope);

}

#endif /* RCGO_SRC_ENTER_TOP_LEVEL_IDENTIFIERS_HPP */
