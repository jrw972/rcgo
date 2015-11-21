#include "Callable.hpp"
#include "executor_base.hpp"
#include "ast.hpp"
#include "runtime.hpp"
#include "SymbolVisitor.hpp"

using namespace ast;

Function::Function (ast_function_t& node_)
  : Symbol (ast_get_identifier (node_.identifier ()), node_.identifier ())
  , node (node_)
  , functionType_ (NULL)
  , returnSymbol_ (NULL)
  , returnSize_ (0)
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

void
Function::set (const Type::Function* functionType,
               const Symbol* returnSymbol)
{
  functionType_ = functionType;
  returnSymbol_ = returnSymbol;
  returnSize_ = functionType->GetReturnType ()->Size ();
  value_ = typed_value_t::make_ref (typed_value_t (this));
}

void Function::call (executor_base_t& exec, const MemoryModel& memoryModel, const ast_call_expr_t& node) const
{
  // Create space for the return.
  exec.stack ().reserve (this->returnSize_);

  // Sample the top of the stack.
  char* top_before = exec.stack ().top ();

  // Push the arguments.
  runtime::evaluate_expr (exec, memoryModel, node.args ());

  // Push a fake instruction pointer.
  exec.stack ().push_pointer (NULL);

  // Sample the top.
  char* top_after = exec.stack ().top ();

  // Do the call.
  exec.stack ().setup (this->memoryModel.LocalsSize ());
  runtime::evaluate_statement (exec, this->memoryModel, this->node.body ());
  exec.stack ().teardown ();

  // Pop the arguments.
  exec.stack ().popn (top_after - top_before);
}

void Method::call (executor_base_t& exec, const MemoryModel& memoryModel, const ast_call_expr_t& node) const
{
  // Create space for the return.
  exec.stack ().reserve (this->returnSize);

  // Sample the top of the stack.
  char* top_before = exec.stack ().top ();

  // Push the arguments.
  runtime::evaluate_expr (exec, memoryModel, node.args ());

  // Push a fake instruction pointer.
  exec.stack ().push_pointer (NULL);

  // Sample the top.
  char* top_after = exec.stack ().top ();

  // Do the call.
  exec.stack ().setup (this->memoryModel.LocalsSize ());
  runtime::evaluate_statement (exec, this->memoryModel, this->node->body ());
  exec.stack ().teardown ();

  // Pop the arguments.
  exec.stack ().popn (top_after - top_before);
}

void Initializer::call (executor_base_t& exec, const MemoryModel& memoryModel, const ast_call_expr_t& node) const
{
  // Create space for the return.
  exec.stack ().reserve (this->returnSize);

  // Sample the top of the stack.
  char* top_before = exec.stack ().top ();

  // Push this.
  runtime::evaluate_expr (exec, memoryModel, node.args ()->At (0));
  component_t* thisPtr = static_cast<component_t*> (exec.stack ().peek_pointer ());

  // Push the arguments.
  for (size_t idx = 1; idx != node.args ()->Size (); ++idx)
    {
      runtime::evaluate_expr (exec, memoryModel, node.args ()->At (idx));
    }

  // Push a fake instruction pointer.
  exec.stack ().push_pointer (NULL);

  // Sample the top.
  char* top_after = exec.stack ().top ();

  // Do the call.
  exec.stack ().setup (this->memoryModel.LocalsSize ());
  component_t* old_this = exec.current_instance (thisPtr);
  runtime::evaluate_statement (exec, this->memoryModel, this->node->body ());
  exec.current_instance (old_this);
  exec.stack ().teardown ();

  // Pop the arguments.
  exec.stack ().popn (top_after - top_before);
}

void Initializer::call (executor_base_t& exec, component_t* thisPtr, const ast::Node* args) const
{
  // Create space for the return.
  exec.stack ().reserve (this->returnSize);

  // Sample the top of the stack.
  char* top_before = exec.stack ().top ();

  // Push this.
  exec.stack ().push_pointer (thisPtr);

  // Push the arguments.
  runtime::evaluate_expr (exec, memoryModel, args);

  // Push a fake instruction pointer.
  exec.stack ().push_pointer (NULL);

  // Sample the top.
  char* top_after = exec.stack ().top ();

  // Do the call.
  exec.stack ().setup (this->memoryModel.LocalsSize ());
  component_t* old_this = exec.current_instance (thisPtr);
  runtime::evaluate_statement (exec, this->memoryModel, this->node->body ());
  exec.current_instance (old_this);
  exec.stack ().teardown ();

  // Pop the arguments.
  exec.stack ().popn (top_after - top_before);
}

void Getter::call (executor_base_t& exec, const MemoryModel& memoryModel, const ast_call_expr_t& node) const
{
  // Create space for the return.
  exec.stack ().reserve (this->returnSize);

  // Sample the top of the stack.
  char* top_before = exec.stack ().top ();

  // Push this.
  runtime::evaluate_expr (exec, memoryModel, node.args ()->At (0));
  component_t* thisPtr = static_cast<component_t*> (exec.stack ().peek_pointer ());

  // Push the arguments.
  for (size_t idx = 1; idx != node.args ()->Size (); ++idx)
    {
      runtime::evaluate_expr (exec, memoryModel, node.args ()->At (idx));
    }

  // Push a fake instruction pointer.
  exec.stack ().push_pointer (NULL);

  // Sample the top.
  char* top_after = exec.stack ().top ();

  // Do the call.
  exec.stack ().setup (this->memoryModel.LocalsSize ());
  component_t* old_this = exec.current_instance (thisPtr);
  runtime::evaluate_statement (exec, this->memoryModel, this->node->body ());
  exec.current_instance (old_this);
  exec.stack ().teardown ();

  // Pop the arguments.
  exec.stack ().popn (top_after - top_before);
}

void Getter::call (executor_base_t& exec, const ast_call_expr_t& node, component_t* thisPtr) const
{
  // Create space for the return.
  exec.stack ().reserve (this->returnSize);

  // Sample the top of the stack.
  char* top_before = exec.stack ().top ();

  // Push this.
  exec.stack ().push_pointer (thisPtr);

  // Push the arguments.
  runtime::evaluate_expr (exec, memoryModel, node.args ());

  // Push a fake instruction pointer.
  exec.stack ().push_pointer (NULL);

  // Sample the top.
  char* top_after = exec.stack ().top ();

  // Do the call.
  exec.stack ().setup (this->memoryModel.LocalsSize ());
  component_t* old_this = exec.current_instance (thisPtr);
  runtime::evaluate_statement (exec, this->memoryModel, this->node->body ());
  exec.current_instance (old_this);
  exec.stack ().teardown ();

  // Pop the arguments.
  exec.stack ().popn (top_after - top_before);
}
