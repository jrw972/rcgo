#include "symbol.hpp"

#include <error.h>

#include "symbol_visitor.hpp"
#include "node.hpp"
#include "polymorphic_function.hpp"
#include "callable.hpp"
#include "error_reporter.hpp"
#include "process_type.hpp"
#include "identifier.hpp"

namespace decl
{
using namespace semantic;
using namespace type;
using namespace source;

Symbol::Symbol (const source::Identifier& a_identifier)
  : identifier (a_identifier)
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
      er.defined_recursively (identifier.location (), identifier.identifier ());
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
ACCEPT(Package)
ACCEPT(ImportedSymbol)

Instance::Instance (ast::InstanceDecl* a_instancedecl)
  : Symbol (a_instancedecl->identifier)
  , type_ (NULL)
  , initializer_ (NULL)
  , instance (NULL)
  , instancedecl_ (a_instancedecl)
{ }

Instance::Instance (const source::Identifier& identifier, const type::NamedType* t, Initializer* init)
  : Symbol (identifier)
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
  const Identifier& initializer_identifier = instancedecl_->initializer;

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

  decl::Initializer* initializer = type->find_initializer (initializer_identifier.identifier ());
  if (initializer == NULL)
    {
      er.not_declared (initializer_identifier);
      return false;
    }
  this->type_ = type;
  this->initializer_ = initializer;

  return true;
}

Parameter::Parameter (const source::Identifier& identifier,
                      const type::Type* a_type,
                      Mutability im,
                      Mutability dm,
                      Kind k)
  : Symbol (identifier)
  , type (a_type)
  , intrinsic_mutability (im)
  , indirection_mutability (a_type != NULL && a_type->is_typed_string () ? std::max (dm, Immutable) : dm)
  , kind (k)
  , original_ (NULL)
{ }


Parameter*
Parameter::make (const source::Identifier& identifier,
                 Mutability intrinsic_mutability,
                 Mutability indirection_mutability,
                 const type::Type* type)
{
  return new Parameter (identifier, type, intrinsic_mutability, indirection_mutability, Ordinary);
}

Parameter*
Parameter::make_return (const source::Identifier& identifier,
                        Mutability indirection_mutability,
                        const type::Type* type)
{
  return new Parameter (identifier, type, Mutable, indirection_mutability, Return);
}

Parameter*
Parameter::make_receiver (const source::Identifier& identifier,
                          Mutability intrinsic_mutability,
                          Mutability indirection_mutability,
                          const type::Type* type)
{
  return new Parameter (identifier, type, intrinsic_mutability, indirection_mutability, Receiver);
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

Constant::Constant (const source::Identifier& identifier, ast::Node* a_type_spec, ast::Node* a_init)
  : Symbol (identifier)
  , type_spec_ (a_type_spec)
  , init_ (a_init)
{ }

Constant::Constant (const source::Identifier& identifier, const semantic::ExpressionValue& v)
  : Symbol (identifier)
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

  Variable::Variable (const source::Identifier& identifier, Mutability im, Mutability dm, const type::Type* a_type)
  : Symbol (identifier)
  , type (a_type)
  , intrinsic_mutability (im)
  , indirection_mutability (a_type != NULL && a_type->is_typed_string () ? std::max (dm, Immutable) : dm)
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
  Variable* s = new Variable (this->identifier, Foreign, Foreign, this->type);
  s->original_ = this;
  return s;
}

Hidden::Hidden (const Symbol* s)
  : Symbol (s->identifier)
{ }

Field::Field (const Struct* a_struct,
              source::Package* a_package,
              bool a_is_anonymous,
              const source::Identifier& identifier,
              const type::Type* a_type,
              const TagSet& a_tags)
  : Symbol (identifier)
  , m_struct (a_struct)
  , package (a_package)
  , is_anonymous (a_is_anonymous)
  , type (a_type)
  , tags (a_tags)
{ }

Package::Package (const source::Identifier& a_identifier, const source::Package* a_package)
  : Symbol (a_identifier)
  , package (a_package)
{ }

  ImportedSymbol::ImportedSymbol (const source::Identifier& a_identifier, Symbol* a_symbol)
    : Symbol (a_identifier)
    , symbol (a_symbol)
  { }

}
