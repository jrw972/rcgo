#ifndef RCGO_SRC_ENTER_METHOD_IDENTIFIERS_HPP
#define RCGO_SRC_ENTER_METHOD_IDENTIFIERS_HPP

#include "types.hpp"

namespace semantic
{

// Enter method like identifiers into the corresponding named types.
void enter_method_identifiers (ast::Node* root, util::ErrorReporter& er, decl::Scope* file_scope);

}

#endif /* RCGO_SRC_ENTER_METHOD_IDENTIFIERS_HPP */
