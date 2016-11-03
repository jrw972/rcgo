#ifndef RC_SRC_CALLABLE_HPP
#define RC_SRC_CALLABLE_HPP

#include "types.hpp"
#include "type.hpp"
#include "memory_model.hpp"
#include "symbol.hpp"
#include "parameter_list.hpp"

namespace decl
{

// Base class for things that can be called.
struct Callable
{
  virtual ~Callable ();
  virtual const decl::ParameterList* parameter_list () const = 0;
  virtual const decl::ParameterList* return_parameter_list () const = 0;
  virtual const type::Type* callable_type () const = 0;
  virtual void check (ast::List* args) const = 0;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const = 0;

  virtual void call (runtime::ExecutorBase& exec) const = 0;

  runtime::MemoryModel memory_model;
};

struct FunctionBase : public Callable, public decl::Symbol
{
  FunctionBase (const std::string& id,
                const util::Location& loc,
                const type::Function* type);

  // Callable
  virtual const decl::ParameterList* parameter_list () const;
  const decl::ParameterList* return_parameter_list () const;
  virtual const type::Type* callable_type () const;
  virtual void check (ast::List* args) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const;

  // Symbol
  virtual void accept (decl::SymbolVisitor& visitor);
  virtual void accept (decl::ConstSymbolVisitor& visitor) const;

  // TODO:  Use getter.
  const type::Function* type;
};

struct Function : public FunctionBase
{
  Function (ast::FunctionDecl* a_functiondecl);
  virtual void call (runtime::ExecutorBase& exec) const;
  runtime::Operation* operation;
  ast::FunctionDecl* const functiondecl;

  virtual bool process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table);
};

struct MethodBase : public Callable, public decl::Symbol
{
  MethodBase (const std::string& a_name,
              const util::Location& a_location,
              const type::NamedType* a_named_type);
  virtual const decl::ParameterList* parameter_list () const;
  virtual const decl::ParameterList* return_parameter_list () const;
  virtual const type::Type* callable_type () const;

  virtual void check (ast::List* args) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const;

  virtual void call (runtime::ExecutorBase& exec) const;

  // Symbol
  virtual void accept (decl::SymbolVisitor& visitor);
  virtual void accept (decl::ConstSymbolVisitor& visitor) const;

  const type::NamedType* const named_type;
  // TODO:  Use getter.
  const type::MethodBase* type;
  runtime::Operation* operation;
};

struct Method : public MethodBase
{
  Method (ast::MethodDecl* a_methoddecl,
          const type::NamedType* a_named_type);
  ast::MethodDecl* const methoddecl;

private:
  virtual bool process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table);
};

struct Initializer : public MethodBase
{
  Initializer (ast::InitDecl* a_initdecl,
               const type::NamedType* a_named_type);
  ast::InitDecl* const initdecl;

private:
  virtual bool process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table);
};

struct Getter : public MethodBase
{
  Getter (ast::GetterDecl* node,
          const type::NamedType* a_named_type);
  ast::GetterDecl* const getterdecl;
  ReceiverAccess immutable_phase_access;

private:
  virtual bool process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table);
};

struct Action : public MethodBase
{
  enum PreconditionKind
  {
    Dynamic,
    Static_True,
    Static_False,
  };

  Action (ast::ActionDecl* a_actiondecl,
          const type::NamedType* a_named_type);

  PreconditionKind precondition_kind;
  ReceiverAccess precondition_access;
  ReceiverAccess immutable_phase_access;
  ast::ActionDecl* const actiondecl;

  Parameter* receiver_parameter () const;
  Parameter* iota_parameter () const;
  long dimension () const;

private:
  Parameter* receiver_parameter_;
  Parameter* iota_parameter_;
  long dimension_;

  virtual bool process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table);
};

struct Reaction : public MethodBase
{
  Reaction (ast::ReactionDecl* a_reaction,
            const type::NamedType* a_named_type);

  ast::ReactionDecl* const reactiondecl;
  ReceiverAccess immutable_phase_access;

  Symbol* iota () const;
  long dimension () const;

private:
  Symbol* iota_;
  long dimension_;
  virtual bool process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table);
};

struct Bind : public MethodBase
{
  Bind (ast::BindDecl* a_binddecl,
        const type::NamedType* a_named_type);

  ast::BindDecl* const binddecl;

  Parameter* receiver_parameter () const;

private:
  Parameter* receiver_parameter_;
  virtual bool process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table);
};

}

#endif // RC_SRC_CALLABLE_HPP
