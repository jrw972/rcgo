#include "symbol.hpp"

#include <error.h>

#include "symbol_visitor.hpp"
#include "node.hpp"
#include "polymorphic_function.hpp"
#include "callable.hpp"
#include "error_reporter.hpp"
#include "process_type.hpp"

namespace decl
{
using namespace semantic;
using namespace type;

Symbol::Symbol (const std::string& id, const util::Location& loc)
  : name (id)
  , location (loc)
  , state_ (Declared)
  , offset_ (0)
{ }

Symbol::~Symbol() { }

Symbol::State
Symbol::state () const
{
  return state_;
}

bool
Symbol::process_declaration (util::ErrorReporter& er, SymbolTable& symbol_table)
{
  switch (state_)
    {
    case Symbol::Declared:
    {
      state_ = Symbol::In_Progress;
      if (process_declaration_i (er, symbol_table))
        {
          state_ = Symbol::Defined;
          post_process_declaration_i (er, symbol_table);
        }
      else
        {
          state_ = Symbol::Undefined;
        }
    }
    break;

    case Symbol::In_Progress:
      er.defined_recursively (location, name);
      state_ = Symbol::Undefined;
      break;

    case Symbol::Defined:
      break;

    case Symbol::Undefined:
      break;
    }

  return state_ == Symbol::Defined;
}

bool
Symbol::process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table)
{
  return true;
}

void
Symbol::post_process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table)
{ }

void Symbol::offset (ptrdiff_t o)
{
  offset_ = o;
}
ptrdiff_t Symbol::offset () const
{
  return offset_;
}

#define ACCEPT(type) void type::accept (SymbolVisitor& visitor) { visitor.visit (*this); } \
void type::accept (ConstSymbolVisitor& visitor) const { visitor.visit (*this); }

ACCEPT(Instance)
ACCEPT(Parameter)
ACCEPT(Constant)
ACCEPT(Variable)
ACCEPT(Hidden)
ACCEPT(Field)

Instance::Instance (ast::InstanceDecl* a_instancedecl)
  : Symbol (a_instancedecl->identifier->identifier, a_instancedecl->identifier->location)
  , type_ (NULL)
  , initializer_ (NULL)
  , instance (NULL)
  , instancedecl_ (a_instancedecl)
{ }

Instance::Instance (const std::string& id, const util::Location& loc, const type::NamedType* t, Initializer* init)
  : Symbol (id, loc)
  , type_ (t)
  , initializer_ (init)
  , instance (NULL)
  , instancedecl_ (NULL)
{
  state_ = Defined;
}

const type::NamedType*
Instance::type () const
{
  return type_;
}

Initializer*
Instance::initializer () const
{
  return initializer_;
}

bool
Instance::process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table)
{
  const std::string& initializer_identifier = instancedecl_->initializer->identifier;

  process_type (instancedecl_->type, er, symbol_table);
  if (instancedecl_->type->eval.kind == ExpressionValue::Error)
    {
      return false;
    }

  const type::NamedType* type = instancedecl_->type->eval.type->to_named_type ();

  if (type->underlying_type ()->kind () != Component_Kind)
    {
      er.expected_a_component (instancedecl_->type->location);
      return false;
    }

  decl::Initializer* initializer = type->find_initializer (initializer_identifier);
  if (initializer == NULL)
    {
      er.not_declared (instancedecl_->initializer->location, initializer_identifier);
      return false;
    }
  this->type_ = type;
  this->initializer_ = initializer;

  return true;
}

Parameter::Parameter (const util::Location& loc,
                      const std::string& id,
                      const type::Type* t,
                      Mutability im,
                      Mutability dm,
                      Kind k)
  : Symbol (id, loc)
  , type (t)
  , intrinsic_mutability (im)
  , indirection_mutability (t->is_typed_string () ? std::max (dm, Immutable) : dm)
  , kind (k)
  , original_ (NULL)
{ }


Parameter*
Parameter::make (const util::Location& loc,
                 const std::string& name,
                 const type::Type* type,
                 Mutability intrinsic_mutability,
                 Mutability indirection_mutability)
{
  return new Parameter (loc, name, type, intrinsic_mutability, indirection_mutability, Ordinary);
}

Parameter*
Parameter::make_return (const util::Location& loc,
                        const std::string& name,
                        const type::Type* type,
                        Mutability indirection_mutability)
{
  return new Parameter (loc, name, type, Mutable, indirection_mutability, Return);
}

Parameter*
Parameter::make_receiver (const util::Location& loc,
                          const std::string& name,
                          const type::Type* type,
                          Mutability intrinsic_mutability,
                          Mutability indirection_mutability)
{
  return new Parameter (loc, name, type, intrinsic_mutability, indirection_mutability, Receiver);
}

Parameter*
Parameter::duplicate (Mutability dereferenceMutability)
{
  Parameter* s = new Parameter (*this);
  switch (this->kind)
    {
    case Receiver:
      s->kind = Receiver_Duplicate;
      break;
    case Receiver_Duplicate:
      break;
    case Ordinary:
      s->kind = Ordinary_Duplicate;
      break;
    case Ordinary_Duplicate:
      break;
    case Return:
      NOT_REACHED;
    }
  s->indirection_mutability = dereferenceMutability;
  s->original_ = this;
  return s;
}

ptrdiff_t Parameter::offset () const
{
  if (kind == Receiver_Duplicate ||
      kind == Ordinary_Duplicate)
    {
      return original_->offset ();
    }
  else
    {
      return Symbol::offset ();
    }
}

bool
Parameter::is_foreign_safe () const
{
  return !(type->contains_pointer () && indirection_mutability != Foreign);
}

Constant::Constant (const std::string& id, const util::Location& loc, ast::Node* a_type_spec, ast::Node* a_init)
  : Symbol (id, loc)
  , type_spec_ (a_type_spec)
  , init_ (a_init)
{ }

Constant::Constant (const std::string& id, const util::Location& loc, const semantic::ExpressionValue& v)
  : Symbol (id, loc)
  , value_ (v)
  , type_spec_ (NULL)
  , init_ (NULL)
{
  state_ = Defined;
}

bool
Constant::process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table)
{
  // Process the type spec.
  process_type (type_spec_, er, symbol_table);
  if (type_spec_->eval.kind == ExpressionValue::Error)
    {
      return false;
    }

  if (type_spec_->eval.kind == ExpressionValue::Type)
    {
      const type::Type* type = type_spec_->eval.type;

      // Type, expressions.
      if (!check_constant_expression (init_, er, symbol_table))
        {
          return false;
        }
      if (!init_->eval.is_assignable (type))
        {
          er.cannot_convert (init_->location, init_->eval.type, type);
          return false;
        }

      init_->eval.convert (type);
      init_->eval.type = type;

      this->value_ = init_->eval;
      return true;
    }
  else
    {
      // No type, just expressions.
      if (!check_constant_expression (init_, er, symbol_table))
        {
          return false;
        }

      this->value_ = init_->eval;
      return true;
    }
}

semantic::ExpressionValue
Constant::value () const
{
  return value_;
}

Variable::Variable (const std::string& id, const util::Location& loc, const type::Type* t, Mutability im, Mutability dm)
  : Symbol (id, loc)
  , type (t)
  , intrinsic_mutability (im)
  , indirection_mutability (t->is_typed_string () ? std::max (dm, Immutable) : dm)
  , original_ (NULL)
{ }

ptrdiff_t Variable::offset () const
{
  if (original_ != NULL)
    {
      return original_->offset ();
    }
  else
    {
      return Symbol::offset ();
    }
}

Variable* Variable::duplicate()
{
  Variable* s = new Variable (this->name, this->location, this->type, Foreign, Foreign);
  s->original_ = this;
  return s;
}

Hidden::Hidden (const Symbol* s, const util::Location& loc)
  : Symbol (s->name, loc)
{ }

Field::Field (const Struct* a_struct,
              decl::Package* a_package,
              bool a_is_anonymous,
              const std::string& a_name,
              const util::Location& a_location,
              const type::Type* a_type,
              const TagSet& a_tags)
  : Symbol (a_name, a_location)
  , m_struct (a_struct)
  , package (a_package)
  , is_anonymous (a_is_anonymous)
  , type (a_type)
  , tags (a_tags)
{ }

}
