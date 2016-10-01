#include "callable.hpp"


#include "node.hpp"
#include "symbol_visitor.hpp"
#include "check_types.hpp"
#include "compute_receiver_access.hpp"
#include "operation.hpp"
#include "process_type.hpp"

namespace decl
{

using namespace ast;
using namespace semantic;

Callable::~Callable () { }

FunctionBase::FunctionBase (const std::string& id,
                            const util::Location& loc,
                            const type::Function* a_type)
  : Symbol (id, loc)
  , type (a_type)
{
  if (a_type)
    {
      state_ = Defined;
    }
}

const type::Type* FunctionBase::callable_type () const
{
  return type;
}
const decl::ParameterList* FunctionBase::parameter_list () const
{
  return type->parameter_list;
}
const decl::ParameterList* FunctionBase::return_parameter_list () const
{
  return type->return_parameter_list;
}

void
FunctionBase::accept (SymbolVisitor& visitor)
{
  visitor.visit (*this);
}

void
FunctionBase::accept (ConstSymbolVisitor& visitor) const
{
  visitor.visit (*this);
}

void
FunctionBase::check (ast::List* args) const
{
  semantic::check_types_arguments (args, parameter_list ());
  semantic::require_value_or_variable_list (args);
  semantic::check_mutability_arguments (args, parameter_list ());
}

void
FunctionBase::compute_receiver_access (const semantic::ExpressionValueList& args, ReceiverAccess& receiver_access, bool& flag) const
{
  semantic::compute_receiver_access_arguments (args, parameter_list (), receiver_access, flag);
}

Function::Function (const std::string& name,
                    const util::Location& location,
                    ast::FunctionDecl* a_functiondecl)
  : FunctionBase (name, location, NULL)
  , operation (NULL)
  , functiondecl_ (a_functiondecl)
{ }

Function::Function (const std::string& name,
                    const util::Location& location,
                    const type::Function* type)
  : FunctionBase (name, location, type)
  , operation (NULL)
  , functiondecl_ (NULL)
{ }

bool Function::process_declaration_i (util::ErrorReporter& er, Scope* file_scope)
{
  const decl::ParameterList* parameter_list;
  const decl::ParameterList* return_parameter_list;
  // TODO:  Handle error.
  process_signature (er, file_scope, functiondecl_->parameters, functiondecl_->return_parameters, false,
                     parameter_list, return_parameter_list);
  this->type = new type::Function (parameter_list, return_parameter_list);
  return true;
}

void Function::call (runtime::ExecutorBase& exec) const
{
  this->operation->execute (exec);
}

MethodBase::MethodBase (const std::string& a_name,
                        const type::MethodBase* a_type)
  : name (a_name)
  , type (a_type)
  , operation (NULL)
{ }

const decl::ParameterList* MethodBase::parameter_list () const
{
  return type->function_type->parameter_list;
}
const decl::ParameterList* MethodBase::return_parameter_list () const
{
  return type->function_type->return_parameter_list;
}
const type::Type* MethodBase::callable_type () const
{
  return type;
}

void
MethodBase::check (ast::List* args) const
{
  semantic::check_types_arguments (args, type->parameter_list);
  semantic::require_value_or_variable_list (args);
  semantic::check_mutability_arguments (args, type->parameter_list);
}

void
MethodBase::compute_receiver_access (const semantic::ExpressionValueList& args, ReceiverAccess& receiver_access, bool& flag) const
{
  semantic::compute_receiver_access_arguments (args, type->parameter_list, receiver_access, flag);
}

void MethodBase::call (runtime::ExecutorBase& exec) const
{
  this->operation->execute (exec);
}

Method::Method (const std::string& name,
                const type::Method* method_type)
  : MethodBase (name, method_type)
{ }

Initializer::Initializer (const std::string& name,
                          const type::Initializer* initializer_type)
  : MethodBase (name, initializer_type)
{ }

Getter::Getter (ast::GetterDecl* n,
                const std::string& name,
                const type::Getter* getter_type)
  : MethodBase (name, getter_type)
  , node (n)
{ }

Reaction::Reaction (ast::Node* a_body,
                    const std::string& a_name,
                    const type::Reaction* a_reaction_type)
  : MethodBase (a_name, a_reaction_type)
  , body (a_body)
  , iota (NULL)
  , dimension (-1)
{ }

Reaction::Reaction (ast::Node* a_body,
                    const std::string& a_name,
                    const type::Reaction* a_reaction_type,
                    Symbol* a_iota,
                    long a_dimension)
  : MethodBase (a_name, a_reaction_type)
  , body (a_body)
  , iota (a_iota)
  , dimension (a_dimension)
{ }

bool Reaction::has_dimension () const
{
  return iota != NULL;
}

}
