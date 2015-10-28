#ifndef runtime_hpp
#define runtime_hpp

#include "types.hpp"
#include "ast.hpp"
#include "heap.hpp"
#include "field.hpp"
#include <error.h>
#include "Activation.hpp"
#include <string.h>
#include "instance_table.hpp"
#include "executor_base.hpp"
#include "Callable.hpp"

namespace runtime
{
void
allocate_instances (instance_table_t& instance_table);

void
create_bindings (instance_table_t& instance_table);

void
initialize (executor_base_t& exec, instance_t* instance);

// Returns true if the action is enabled.
bool enabled (executor_base_t& exec, component_t* instance, const action_t* action, size_t iota);

// Returns true if the action was executed.
bool exec (executor_base_t& exec, component_t* instance, const action_t* action, size_t iota);

// Execute the action without checking the precondition.
// Returns true.
bool exec_no_check (executor_base_t& exec, component_t* instance, const action_t* action, size_t iota);

enum ControlAction
{
  RETURN,
  CONTINUE,
};

ControlAction
evaluate_statement (executor_base_t& exec,
                    ast_t* node);

void
evaluate_expr (executor_base_t& exec,
               ast_t* node);

struct NewImpl : public Callable
{
  NewImpl (const type_t* t, ast_t* definingNode);

  virtual void call (executor_base_t& exec, const ast_call_expr_t& node) const;
  virtual const type_t* type () const
  {
    return function_type_;
  }
  const type_t* const type_;
  const type_t* const function_type_;
  static const type_t* makeFunctionType (const type_t* type, ast_t* definingNode);
};
}

#endif /* runtime_hpp */
