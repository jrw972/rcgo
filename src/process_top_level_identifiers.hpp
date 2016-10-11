#ifndef RCGO_SRC_PROCESS_TOP_LEVEL_DECLARATIONS_HPP
#define RCGO_SRC_PROCESS_TOP_LEVEL_DECLARATIONS_HPP

#include "types.hpp"

namespace semantic
{

// Process constants, types, functions, methods, initializers, getters,
// actions, reactions, binders, and instances.
void process_top_level_declarations (ast::Node* root, util::ErrorReporter& er, decl::Scope* file_scope);
}

#endif /* RCGO_SRC_PROCESS_TOP_LEVEL_DECLARATIONS_HPP */
