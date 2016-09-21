#ifndef RC_SRC_PROCESS_FUNCTIONS_AND_METHODS_HPP
#define RC_SRC_PROCESS_FUNCTIONS_AND_METHODS_HPP

#include "types.hpp"

namespace semantic
{
void process_functions_and_methods (ast::Node* root, util::ErrorReporter& er, decl::Scope* scope);
}

#endif // RC_SRC_PROCESS_FUNCTIONS_AND_METHODS_HPP
