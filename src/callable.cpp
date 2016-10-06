#include "callable.hpp"


#include "node.hpp"
#include "symbol_visitor.hpp"
#include "check_types.hpp"
#include "compute_receiver_access.hpp"
#include "operation.hpp"
#include "process_type.hpp"
#include "error_reporter.hpp"
#include "scope.hpp"
#include "node_cast.hpp"

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

Function::Function (ast::FunctionDecl* a_functiondecl)
  : FunctionBase (a_functiondecl->identifier->identifier, a_functiondecl->identifier->location, NULL)
  , operation (NULL)
  , functiondecl (a_functiondecl)
{ }

bool Function::process_declaration_i (util::ErrorReporter& er, Scope* file_scope)
{
  const decl::ParameterList* parameter_list;
  const decl::ParameterList* return_parameter_list;
  process_signature (er, file_scope, functiondecl->parameters,
                     functiondecl->return_parameters, false,
                     parameter_list, return_parameter_list);
  this->type = new type::Function (parameter_list, return_parameter_list);
  return true;
}

void Function::call (runtime::ExecutorBase& exec) const
{
  this->operation->execute (exec);
}

MethodBase::MethodBase (const std::string& a_name,
                        const util::Location& a_location,
                        const type::NamedType* a_named_type)
  : Symbol (a_name, a_location)
  , named_type (a_named_type)
  , type (NULL)
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

void
MethodBase::accept (SymbolVisitor& visitor)
{
  visitor.visit (*this);
}

void
MethodBase::accept (ConstSymbolVisitor& visitor) const
{
  visitor.visit (*this);
}

Method::Method (ast::MethodDecl* a_methoddecl,
                const type::NamedType* a_named_type)
  : MethodBase (a_methoddecl->identifier->identifier, a_methoddecl->identifier->location, a_named_type)
  , methoddecl (a_methoddecl)
{ }

bool Method::process_declaration_i (util::ErrorReporter& er, Scope* file_scope)
{
  assert (named_type->state () == Defined);
  Parameter* this_symbol = process_receiver (er, file_scope, named_type, methoddecl->receiver, false, false);
  const decl::ParameterList* parameter_list;
  const decl::ParameterList* return_parameter_list;
  process_signature (er, file_scope,
                     methoddecl->parameters, methoddecl->return_parameters, false,
                     parameter_list, return_parameter_list);
  this->type = new type::Method (named_type,
                                 this_symbol,
                                 parameter_list,
                                 return_parameter_list);
  check_unique_parameters (er, this->type->function_type);
  return true;
}

Initializer::Initializer (ast::InitDecl* a_initdecl,
                          const type::NamedType* a_named_type)
  : MethodBase (a_initdecl->identifier->identifier, a_initdecl->identifier->location, a_named_type)
  , initdecl (a_initdecl)
{ }

bool Initializer::process_declaration_i (util::ErrorReporter& er, Scope* file_scope)
{
  assert (named_type->state () == Defined);
  Parameter* this_symbol = process_receiver (er, file_scope, named_type, initdecl->receiver, true, false);
  const decl::ParameterList* parameter_list;
  const decl::ParameterList* return_parameter_list;
  process_signature (er, file_scope,
                     initdecl->parameters, initdecl->return_parameters, true,
                     parameter_list, return_parameter_list);
  this->type = new type::Initializer (named_type,
                                      this_symbol,
                                      parameter_list,
                                      return_parameter_list);
  check_unique_parameters (er, this->type->function_type);
  return true;
}

Getter::Getter (ast::GetterDecl* a_getterdecl,
                const type::NamedType* a_named_type)
  : MethodBase (a_getterdecl->identifier->identifier, a_getterdecl->identifier->location, a_named_type)
  , getterdecl (a_getterdecl)
{ }

bool Getter::process_declaration_i (util::ErrorReporter& er, Scope* file_scope)
{
  assert (named_type->state () == Defined);
  Parameter* this_symbol = process_receiver (er, file_scope, named_type, getterdecl->receiver, true, true);
  const decl::ParameterList* parameter_list;
  const decl::ParameterList* return_parameter_list;
  process_signature (er, file_scope,
                     getterdecl->parameters, getterdecl->return_parameters, true,
                     parameter_list, return_parameter_list);
  this->type = new type::Getter (named_type,
                                 this_symbol,
                                 parameter_list,
                                 return_parameter_list);
  check_unique_parameters (er, this->type->function_type);
  return true;
}

Action::Action (ast::ActionDecl* a_actiondecl,
                const type::NamedType* a_named_type)
  : MethodBase (a_actiondecl->identifier->identifier, a_actiondecl->identifier->location, a_named_type)
  , actiondecl (a_actiondecl)
  , receiver_parameter_ (NULL)
  , iota_parameter_ (NULL)
  , dimension_ (-1)
{ }

bool Action::process_declaration_i (util::ErrorReporter& er, Scope* file_scope)
{
  assert (named_type->state () == Defined);

  if (node_cast<EmptyExpression> (actiondecl->dimension) == NULL)
    {
      this->iota_parameter_ = Parameter::make (actiondecl->dimension->location, "IOTA", type::Int::instance (), Immutable, Immutable);
      this->dimension_ = semantic::process_array_dimension (er, file_scope, actiondecl->dimension);
    }

  this->receiver_parameter_ = semantic::process_receiver (er, file_scope, named_type, actiondecl->receiver, true, true);

  return true;
}

Parameter* Action::receiver_parameter () const
{
  return receiver_parameter_;
}

Parameter* Action::iota_parameter () const
{
  return iota_parameter_;
}

long Action::dimension () const
{
  return dimension_;
}

Reaction::Reaction (ast::ReactionDecl* a_reactiondecl,
                    const type::NamedType* a_named_type)
  : MethodBase (a_reactiondecl->identifier->identifier, a_reactiondecl->identifier->location, a_named_type)
  , reactiondecl (a_reactiondecl)
  , iota_ (NULL)
  , dimension_ (-1)
{ }

Symbol* Reaction::iota () const
{
  return iota_;
}

long Reaction::dimension () const
{
  return dimension_;
}

bool Reaction::process_declaration_i (util::ErrorReporter& er, Scope* file_scope)
{
  assert (named_type->state () == Defined);

  if (node_cast<EmptyExpression> (reactiondecl->dimension) == NULL)
    {
      this->iota_ = Parameter::make (reactiondecl->dimension->location, "IOTA", type::Int::instance (), Immutable, Immutable);
      this->dimension_ = process_array_dimension (er, file_scope, reactiondecl->dimension);
    }

  Parameter* this_symbol = process_receiver (er, file_scope, named_type, reactiondecl->receiver, true, true);
  const decl::ParameterList* parameter_list;
  const decl::ParameterList* return_parameter_list;
  process_signature (er, file_scope,
                     reactiondecl->parameters, reactiondecl->return_parameters, true,
                     parameter_list, return_parameter_list);
  this->type = new type::Reaction (named_type,
                                   this_symbol,
                                   parameter_list,
                                   return_parameter_list);
  check_unique_parameters (er, this->type->function_type);
  return true;
}

Bind::Bind (ast::BindDecl* a_binddecl,
            const type::NamedType* a_named_type)
  : MethodBase (a_binddecl->identifier->identifier, a_binddecl->identifier->location, a_named_type)
  , binddecl (a_binddecl)
  , receiver_parameter_ (NULL)
{ }

Parameter* Bind::receiver_parameter () const
{
  return receiver_parameter_;
}

bool Bind::process_declaration_i (util::ErrorReporter& er, Scope* file_scope)
{
  assert (named_type->state () == Defined);
  this->receiver_parameter_ = semantic::process_receiver (er, file_scope, named_type, binddecl->receiver, true, false);
  return true;
}

}
