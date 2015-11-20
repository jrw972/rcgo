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
  stack_frame_reserve (exec.stack (), this->returnSize_);

  // Sample the top of the stack.
  char* top_before = stack_frame_top (exec.stack ());

  // Push the arguments.
  runtime::evaluate_expr (exec, memoryModel, node.args ());

  // Push a fake instruction pointer.
  stack_frame_push_pointer (exec.stack (), NULL);

  // Sample the top.
  char* top_after = stack_frame_top (exec.stack ());

  // Do the call.
  stack_frame_push_base_pointer (exec.stack (), this->memoryModel.LocalsSize ());
  runtime::evaluate_statement (exec, this->memoryModel, this->node.body ());
  stack_frame_pop_base_pointer (exec.stack ());

  // Pop the arguments.
  stack_frame_popn (exec.stack (), top_after - top_before);
}

void Method::call (executor_base_t& exec, const MemoryModel& memoryModel, const ast_call_expr_t& node) const
{
  // Create space for the return.
  stack_frame_reserve (exec.stack (), this->returnSize);

  // Sample the top of the stack.
  char* top_before = stack_frame_top (exec.stack ());

  // Push the arguments.
  runtime::evaluate_expr (exec, memoryModel, node.args ());

  // Push a fake instruction pointer.
  stack_frame_push_pointer (exec.stack (), NULL);

  // Sample the top.
  char* top_after = stack_frame_top (exec.stack ());

  // Do the call.
  stack_frame_push_base_pointer (exec.stack (), this->memoryModel.LocalsSize ());
  runtime::evaluate_statement (exec, this->memoryModel, this->node->body ());
  stack_frame_pop_base_pointer (exec.stack ());

  // Pop the arguments.
  stack_frame_popn (exec.stack (), top_after - top_before);
}

void Initializer::call (executor_base_t& exec, const MemoryModel& memoryModel, const ast_call_expr_t& node) const
{
  // Create space for the return.
  stack_frame_reserve (exec.stack (), this->returnSize);

  // Sample the top of the stack.
  char* top_before = stack_frame_top (exec.stack ());

  // Push this.
  runtime::evaluate_expr (exec, memoryModel, node.args ()->At (0));
  component_t* thisPtr = static_cast<component_t*> (stack_frame_read_pointer (exec.stack ()));

  // Push the arguments.
  for (size_t idx = 1; idx != node.args ()->Size (); ++idx)
    {
      runtime::evaluate_expr (exec, memoryModel, node.args ()->At (idx));
    }

  // Push a fake instruction pointer.
  stack_frame_push_pointer (exec.stack (), NULL);

  // Sample the top.
  char* top_after = stack_frame_top (exec.stack ());

  // Do the call.
  stack_frame_push_base_pointer (exec.stack (), this->memoryModel.LocalsSize ());
  component_t* old_this = exec.current_instance (thisPtr);
  runtime::evaluate_statement (exec, this->memoryModel, this->node->body ());
  exec.current_instance (old_this);
  stack_frame_pop_base_pointer (exec.stack ());

  // Pop the arguments.
  stack_frame_popn (exec.stack (), top_after - top_before);
}

void Initializer::call (executor_base_t& exec, component_t* thisPtr, const ast::Node* args) const
{
  // Create space for the return.
  stack_frame_reserve (exec.stack (), this->returnSize);

  // Sample the top of the stack.
  char* top_before = stack_frame_top (exec.stack ());

  // Push this.
  stack_frame_push_pointer (exec.stack (), thisPtr);

  // Push the arguments.
  runtime::evaluate_expr (exec, memoryModel, args);

  // Push a fake instruction pointer.
  stack_frame_push_pointer (exec.stack (), NULL);

  // Sample the top.
  char* top_after = stack_frame_top (exec.stack ());

  // Do the call.
  stack_frame_push_base_pointer (exec.stack (), this->memoryModel.LocalsSize ());
  component_t* old_this = exec.current_instance (thisPtr);
  runtime::evaluate_statement (exec, this->memoryModel, this->node->body ());
  exec.current_instance (old_this);
  stack_frame_pop_base_pointer (exec.stack ());

  // Pop the arguments.
  stack_frame_popn (exec.stack (), top_after - top_before);
}

void Getter::call (executor_base_t& exec, const MemoryModel& memoryModel, const ast_call_expr_t& node) const
{
  // Create space for the return.
  stack_frame_reserve (exec.stack (), this->returnSize);

  // Sample the top of the stack.
  char* top_before = stack_frame_top (exec.stack ());

  // Push this.
  runtime::evaluate_expr (exec, memoryModel, node.args ()->At (0));
  component_t* thisPtr = static_cast<component_t*> (stack_frame_read_pointer (exec.stack ()));

  // Push the arguments.
  for (size_t idx = 1; idx != node.args ()->Size (); ++idx)
    {
      runtime::evaluate_expr (exec, memoryModel, node.args ()->At (idx));
    }

  // Push a fake instruction pointer.
  stack_frame_push_pointer (exec.stack (), NULL);

  // Sample the top.
  char* top_after = stack_frame_top (exec.stack ());

  // Do the call.
  stack_frame_push_base_pointer (exec.stack (), this->memoryModel.LocalsSize ());
  component_t* old_this = exec.current_instance (thisPtr);
  runtime::evaluate_statement (exec, this->memoryModel, this->node->body ());
  exec.current_instance (old_this);
  stack_frame_pop_base_pointer (exec.stack ());

  // Pop the arguments.
  stack_frame_popn (exec.stack (), top_after - top_before);
}

void Getter::call (executor_base_t& exec, const ast_call_expr_t& node, component_t* thisPtr) const
{
  // Create space for the return.
  stack_frame_reserve (exec.stack (), this->returnSize);

  // Sample the top of the stack.
  char* top_before = stack_frame_top (exec.stack ());

  // Push this.
  stack_frame_push_pointer (exec.stack (), thisPtr);

  // Push the arguments.
  runtime::evaluate_expr (exec, memoryModel, node.args ());

  // Push a fake instruction pointer.
  stack_frame_push_pointer (exec.stack (), NULL);

  // Sample the top.
  char* top_after = stack_frame_top (exec.stack ());

  // Do the call.
  stack_frame_push_base_pointer (exec.stack (), this->memoryModel.LocalsSize ());
  component_t* old_this = exec.current_instance (thisPtr);
  runtime::evaluate_statement (exec, this->memoryModel, this->node->body ());
  exec.current_instance (old_this);
  stack_frame_pop_base_pointer (exec.stack ());

  // Pop the arguments.
  stack_frame_popn (exec.stack (), top_after - top_before);
}
