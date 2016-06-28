#ifndef RC_SRC_RUNTIME_HPP
#define RC_SRC_RUNTIME_HPP

#include "types.hpp"
#include "runtime_types.hpp"

namespace runtime
{
void
allocate_instances (composition::Composer& instance_table);

void
create_bindings (composition::Composer& instance_table);

void
initialize (ExecutorBase& exec, ComponentInfoBase* info);

// Returns true if the action is enabled.
bool
enabled (ExecutorBase& exec,
         component_t* instance,
         const decl::Action* action,
         long iota);

// Execute the action without checking the precondition.
void
execute_no_check (ExecutorBase& exec,
                  component_t* instance,
                  const decl::Action* action,
                  long iota);
}

#endif // RC_SRC_RUNTIME_HPP
