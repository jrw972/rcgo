#ifndef RC_SRC_ALLOCATE_HPP
#define RC_SRC_ALLOCATE_HPP

#include "types.hpp"

namespace code
{

void
allocate_stack_variables (ast::Node* node);

void
allocate_symbol (runtime::MemoryModel& memory_model,
                 decl::Symbol* symbol);

void
allocate_parameters (runtime::MemoryModel& memory_model,
                     const decl::ParameterList* signature);
}

#endif // RC_SRC_ALLOCATE_HPP
