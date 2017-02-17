#include "callable.hpp"


#include "node.hpp"
#include "symbol_visitor.hpp"
#include "operation.hpp"
#include "process_type.hpp"
#include "error_reporter.hpp"
#include "scope.hpp"
#include "node_cast.hpp"
#include "identifier.hpp"

namespace decl
{

using namespace ast;
using namespace semantic;

Callable::~Callable () { }

FunctionBase::FunctionBase (const source::Identifier& identifier,
                            const type::Function* a_type)
  : Symbol (identifier)
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
  semantic::require_value_or_const_or_variable_list (args);
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
  : FunctionBase (a_functiondecl->identifier, NULL)
  , operation (NULL)
  , functiondecl (a_functiondecl)
{ }

bool Function::process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table)
{
  const decl::ParameterList* parameter_list;
  const decl::ParameterList* return_parameter_list;
  process_signature (er, symbol_table, functiondecl->parameters,
                     functiondecl->return_parameters, false,
                     parameter_list, return_parameter_list);
  this->type = new type::Function (parameter_list, return_parameter_list);
  return true;
}

void Function::call (runtime::ExecutorBase& exec) const
{
  this->operation->execute (exec);
}

MethodBase::MethodBase (const source::Identifier& identifier,
                        const type::NamedType* a_named_type)
  : Symbol (identifier)
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
  semantic::require_value_or_const_or_variable_list (args);
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
  : MethodBase (a_methoddecl->identifier, a_named_type)
  , methoddecl (a_methoddecl)
{ }

bool Method::process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table)
{
  assert (named_type->state () == Defined);
  Parameter* this_symbol = process_receiver (er, symbol_table, named_type, methoddecl->receiver, false, false);
  const decl::ParameterList* parameter_list;
  const decl::ParameterList* return_parameter_list;
  process_signature (er, symbol_table,
                     methoddecl->parameters, methoddecl->return_parameters, false,
                     parameter_list, return_parameter_list);
  this->type = new type::Method (named_type,
                                 this_symbol,
                                 parameter_list,
                                 return_parameter_list);
  check_unique_parameters (er, this->type->function_type);
  return true;
}

Initializer::Initializer (ast::InitializerDecl* a_initdecl,
                          const type::NamedType* a_named_type)
  : MethodBase (a_initdecl->identifier, a_named_type)
  , initdecl (a_initdecl)
{ }

bool Initializer::process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table)
{
  assert (named_type->state () == Defined);
  Parameter* this_symbol = process_receiver (er, symbol_table, named_type, initdecl->receiver, true, false);
  const decl::ParameterList* parameter_list;
  const decl::ParameterList* return_parameter_list;
  process_signature (er, symbol_table,
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
  : MethodBase (a_getterdecl->identifier, a_named_type)
  , getterdecl (a_getterdecl)
{ }

bool Getter::process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table)
{
  assert (named_type->state () == Defined);
  Parameter* this_symbol = process_receiver (er, symbol_table, named_type, getterdecl->receiver, true, true);
  const decl::ParameterList* parameter_list;
  const decl::ParameterList* return_parameter_list;
  process_signature (er, symbol_table,
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
  : MethodBase (a_actiondecl->identifier, a_named_type)
  , actiondecl (a_actiondecl)
  , receiver_parameter_ (NULL)
  , iota_parameter_ (NULL)
  , dimension_ (-1)
{ }

bool Action::process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table)
{
  assert (named_type->state () == Defined);

  if (node_cast<EmptyExpression> (actiondecl->dimension) == NULL)
    {
      this->iota_parameter_ = Parameter::make (source::Identifier ("IOTA", actiondecl->dimension->location), /*type::Int::instance (),*/ Immutable, Immutable);
      this->dimension_ = semantic::process_array_dimension (er, symbol_table, actiondecl->dimension);
    }

  this->receiver_parameter_ = semantic::process_receiver (er, symbol_table, named_type, actiondecl->receiver, true, true);

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
  : MethodBase (a_reactiondecl->identifier, a_named_type)
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

bool Reaction::process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table)
{
  assert (named_type->state () == Defined);

  if (node_cast<EmptyExpression> (reactiondecl->dimension) == NULL)
    {
      this->iota_ = Parameter::make (source::Identifier ("IOTA", reactiondecl->dimension->location), /*type::Int::instance (),*/ Immutable, Immutable);
      this->dimension_ = process_array_dimension (er, symbol_table, reactiondecl->dimension);
    }

  Parameter* this_symbol = process_receiver (er, symbol_table, named_type, reactiondecl->receiver, true, true);
  const decl::ParameterList* parameter_list;
  const decl::ParameterList* return_parameter_list;
  process_signature (er, symbol_table,
                     reactiondecl->parameters, reactiondecl->return_parameters, true,
                     parameter_list, return_parameter_list);
  this->type = new type::Reaction (named_type,
                                   this_symbol,
                                   parameter_list,
                                   return_parameter_list);
  check_unique_parameters (er, this->type->function_type);
  return true;
}

Bind::Bind (ast::BinderDecl* a_binddecl,
            const type::NamedType* a_named_type)
  : MethodBase (a_binddecl->identifier, a_named_type)
  , binddecl (a_binddecl)
  , receiver_parameter_ (NULL)
{ }

Parameter* Bind::receiver_parameter () const
{
  return receiver_parameter_;
}

bool Bind::process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table)
{
  assert (named_type->state () == Defined);
  this->receiver_parameter_ = semantic::process_receiver (er, symbol_table, named_type, binddecl->receiver, true, false);
  return true;
}

}
