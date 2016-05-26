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
  semantic::check_types_arguments (args, signature ());
}

void
Callable::check_references (ast::List* args) const
{
  semantic::require_value_or_variable_list (args);
}

void
Callable::check_mutability (ast::List* args) const
{
  semantic::check_mutability_arguments (args, signature ());
}

void
Callable::compute_receiver_access (const semantic::ExpressionValueList& args, ReceiverAccess& receiver_access, bool& flag) const
{
  semantic::compute_receiver_access_arguments (args, signature (), receiver_access, flag);
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

size_t Function::arguments_size () const
{
  return type->parameter_list->size_on_stack ();
}

void Method::call (runtime::ExecutorBase& exec) const
{
  this->node->body->operation->execute (exec);
}

size_t Method::arguments_size () const
{
  return methodType->parameter_list->size_on_stack ();
}

decl::ParameterSymbol* Method::return_parameter () const
{
  return methodType->return_parameter_list->at (0);
}

void Initializer::call (runtime::ExecutorBase& exec) const
{
  this->node->operation->execute (exec);
}

size_t Initializer::arguments_size () const
{
  return initializerType->parameter_list->size_on_stack ();
}

decl::ParameterSymbol* Initializer::return_parameter () const
{
  return initializerType->return_parameter_list->at (0);
}

void Getter::call (runtime::ExecutorBase& exec) const
{
  this->node->operation->execute (exec);
}

size_t Getter::arguments_size () const
{
  return getterType->parameter_list->size_on_stack ();
}

decl::ParameterSymbol* Getter::return_parameter () const
{
  return getterType->return_parameter_list->at (0);
}

}
