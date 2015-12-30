#ifndef RC_SRC_PROCESS_TYPES_AND_CONSTANTS_HPP
#define RC_SRC_PROCESS_TYPES_AND_CONSTANTS_HPP

#include "types.hpp"

namespace semantic
{
void process_types_and_constants (ast::Node* root, decl::SymbolTable& symtab);
}

#endif // RC_SRC_PROCESS_TYPES_AND_CONSTANTS_HPP
