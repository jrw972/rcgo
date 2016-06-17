#include "callable.hpp"
#include "executor_base.hpp"
#include "node.hpp"
#include "runtime.hpp"
#include "symbol_visitor.hpp"
#include "check_types.hpp"
#include "compute_receiver_access.hpp"
#include "parameter_list.hpp"

namespace decl
{

using namespace ast;

void
Callable::check_types (ast::List* args) const
{
  semantic::check_types_arguments (args, parameter_list ());
}

void
Callable::check_references (ast::List* args) const
{
  semantic::require_value_or_variable_list (args);
}

void
Callable::check_mutability (ast::List* args) const
{
  semantic::check_mutability_arguments (args, parameter_list ());
}

void
Callable::compute_receiver_access (const semantic::ExpressionValueList& args, ReceiverAccess& receiver_access, bool& flag) const
{
  semantic::compute_receiver_access_arguments (args, parameter_list (), receiver_access, flag);
}

Function::Function (ast::Function* node_, const type::Function* a_type)
  : Symbol (node_->identifier->identifier, node_->identifier->location)
  , node (node_)
  , type (a_type)
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

void Function::call (runtime::ExecutorBase& exec) const
{
  this->node->body->operation->execute (exec);
}

size_t Function::parameters_size_on_stack () const
{
  return type->parameter_list->size_on_stack ();
}

void Method::call (runtime::ExecutorBase& exec) const
{
  this->node->body->operation->execute (exec);
}

size_t Method::parameters_size_on_stack () const
{
  return methodType->parameter_list->size_on_stack ();
}

const decl::ParameterList* Method::return_parameter_list () const
{
  return methodType->return_parameter_list;
}

void Initializer::call (runtime::ExecutorBase& exec) const
{
  this->node->operation->execute (exec);
}

size_t Initializer::parameters_size_on_stack () const
{
  return initializerType->parameter_list->size_on_stack ();
}

const decl::ParameterList* Initializer::return_parameter_list () const
{
  return initializerType->return_parameter_list;
}

void Getter::call (runtime::ExecutorBase& exec) const
{
  this->node->operation->execute (exec);
}

size_t Getter::parameters_size_on_stack () const
{
  return getterType->parameter_list->size_on_stack ();
}

const decl::ParameterList* Getter::return_parameter_list () const
{
  return getterType->return_parameter_list;
}

}
