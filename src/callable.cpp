#include "callable.hpp"
#include "executor_base.hpp"
#include "ast.hpp"
#include "runtime.hpp"
#include "symbol_visitor.hpp"
#include "check_types.hpp"
#include "compute_receiver_access.hpp"

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
Callable::compute_receiver_access (ast::List* args, ReceiverAccess& receiver_access, bool& flag) const
{
  semantic::compute_receiver_access_arguments (args, signature (), receiver_access, flag);
}

Function::Function (ast::Function& node_, const type::Function* ft)
  : Symbol (node_.identifier->identifier, node_.identifier->location)
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

void Function::call (runtime::executor_base_t& exec) const
{
  this->node.body->operation->execute (exec);
}

void Method::call (runtime::executor_base_t& exec) const
{
  this->node->body->operation->execute (exec);
}

void Initializer::call (runtime::executor_base_t& exec) const
{
  this->node->operation->execute (exec);
}

void Getter::call (runtime::executor_base_t& exec) const
{
  this->node->operation->execute (exec);
}

}
