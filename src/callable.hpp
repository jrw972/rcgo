#ifndef RC_SRC_CALLABLE_HPP
#define RC_SRC_CALLABLE_HPP

#include "types.hpp"
#include "type.hpp"
#include "memory_model.hpp"
#include "symbol.hpp"
#include "expression_value.hpp"
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

  // TODO:  Use getter/setter.
  const type::Function* type;
};

struct Function : public FunctionBase
{
  Function (const std::string& name,
            const util::Location& location,
            ast::FunctionDecl* a_functiondecl);
  Function (const std::string& name,
            const util::Location& location,
            const type::Function* type);
  virtual bool process_declaration_i (util::ErrorReporter& er, Scope* file_scope);
  virtual void call (runtime::ExecutorBase& exec) const;
  runtime::Operation* operation;
private:
  ast::FunctionDecl* const functiondecl_;
};

struct MethodBase : public Callable
{
  MethodBase (const std::string& a_name,
              const type::MethodBase* a_type);
  virtual const decl::ParameterList* parameter_list () const;
  virtual const decl::ParameterList* return_parameter_list () const;
  virtual const type::Type* callable_type () const;

  virtual void check (ast::List* args) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const;

  virtual void call (runtime::ExecutorBase& exec) const;

  std::string const name;
  const type::MethodBase* const type;
  runtime::Operation* operation;
};

struct Method : public MethodBase
{
  Method (const std::string& name,
          const type::Method* method_type);
};

struct Initializer : public MethodBase
{
  Initializer (const std::string& name,
               const type::Initializer* initializer_type);
};

struct Getter : public MethodBase
{
  Getter (ast::GetterDecl* node,
          const std::string& name,
          const type::Getter* getter_type);
  ast::GetterDecl* const node;
  ReceiverAccess immutable_phase_access;
};

struct Reaction : public MethodBase
{
  Reaction (ast::Node* a_body,
            const std::string& a_name,
            const type::Reaction* a_reaction_type);
  Reaction (ast::Node* a_body,
            const std::string& a_name,
            const type::Reaction* a_reaction_type,
            Symbol* a_iota,
            long a_dimension);

  ast::Node* const body;
  ReceiverAccess immutable_phase_access;
  Symbol* const iota;
  long const dimension;

  bool has_dimension () const;
};

}

#endif // RC_SRC_CALLABLE_HPP
