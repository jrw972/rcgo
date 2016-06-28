#include "symbol.hpp"

#include <error.h>

#include "symbol_visitor.hpp"
#include "node.hpp"
#include "polymorphic_function.hpp"
#include "callable.hpp"

namespace decl
{

Symbol::Symbol (const std::string& id, const util::Location& loc)
  : name (id)
  , location (loc)
  , in_progress (false)
  , offset_ (0)
{ }
Symbol::~Symbol() { }

bool Symbol::defined () const
{
  return true;
}

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

Instance::Instance (const std::string& id, const util::Location& loc, const type::NamedType* t, Initializer* init)
  : Symbol (id, loc)
  , type (t)
  , initializer (init)
  , instance (NULL)
{ }

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

Constant::Constant (const std::string& id, const util::Location& loc, const type::Type* t, const semantic::Value& v)
  : Symbol (id, loc)
  , type (t)
  , value (v)
{ }

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

}
