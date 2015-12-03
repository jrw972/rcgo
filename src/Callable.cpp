#include "Callable.hpp"
#include "executor_base.hpp"
#include "ast.hpp"
#include "runtime.hpp"
#include "SymbolVisitor.hpp"
#include "check_types.hpp"
#include "check_mutability.hpp"
#include "compute_receiver_access.hpp"

using namespace ast;

void
Callable::check_types (ast::Node* args) const
{
  semantic::check_types_arguments (args, signature ());
}

void
Callable::check_references (ast::Node* args) const
{
  semantic::require_value_or_variable_list (args);
}

void
Callable::check_mutability (ast::Node* args) const
{
  semantic::check_mutability_arguments (args, signature ());
}

void
Callable::compute_receiver_access (ast::Node* args, ReceiverAccess& receiver_access, bool& flag) const
{
  semantic::compute_receiver_access_arguments (args, signature (), receiver_access, flag);
}

Function::Function (ast_function_t& node_, const Type::Function* ft)
  : Symbol (ast_get_identifier (node_.identifier ()), node_.identifier ())
  , node (node_)
  , functionType_ (ft)
{ }

void
Function::accept (SymbolVisitor& visitor)
{
  visitor.visit (*this);
}

void
Function::accept (ConstSymbolVisitor& visitor) const
{
  visitor.visit (*this);
}

void Function::call (executor_base_t& exec) const
{
  this->node.body ()->operation->execute (exec);
}

void Method::call (executor_base_t& exec) const
{
  this->node->body ()->operation->execute (exec);
}

void Initializer::call (executor_base_t& exec) const
{
  this->node->operation->execute (exec);
}

void Getter::call (executor_base_t& exec) const
{
  this->node->operation->execute (exec);
}

void Getter::call (executor_base_t& exec, const ast_call_expr_t& node, component_t* thisPtr) const
{
  unimplemented;
  // // Create space for the return.
  // exec.stack ().reserve (this->returnSize);

  // // Sample the top of the stack.
  // char* top_before = exec.stack ().top ();

  // // Push this.
  // exec.stack ().push_pointer (thisPtr);

  // // Push the arguments.
  // runtime::evaluate_expression (exec, memoryModel, node.args ());

  // // Push a fake instruction pointer.
  // exec.stack ().push_pointer (NULL);

  // // Sample the top.
  // char* top_after = exec.stack ().top ();

  // // Do the call.
  // exec.stack ().setup (this->memoryModel.LocalsSize ());
  // component_t* old_this = exec.current_instance (thisPtr);
  // runtime::evaluate_statement (exec, this->memoryModel, this->node->body ());
  // exec.current_instance (old_this);
  // exec.stack ().teardown ();

  // // Pop the arguments.
  // exec.stack ().popn (top_after - top_before);
}
