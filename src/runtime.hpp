#ifndef runtime_hpp
#define runtime_hpp

#include "types.hpp"
#include "ast.hpp"
#include "heap.hpp"
#include "field.hpp"
#include <error.h>
#include "trigger.hpp"
#include <string.h>
#include "instance_table.hpp"
#include "executor_base.hpp"

namespace runtime
{
  void
  allocate_instances (instance_table_t& instance_table);

  void
  create_bindings (instance_table_t& instance_table);

  void
  initialize (executor_base_t& exec, instance_t* instance);

  void exec (executor_base_t& exec, component_t* instance, const action_t* action, size_t iota);

}

#endif /* runtime_hpp */
