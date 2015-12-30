#ifndef RC_SRC_SYMBOL_HPP
#define RC_SRC_SYMBOL_HPP

#include "types.hpp"
#include "location.hpp"
#include "value.hpp"

namespace decl
{
class SymbolVisitor;
class ConstSymbolVisitor;

extern std::string const ReturnSymbol;

/*
 * Base class for symbols.
 */
struct Symbol
{
  Symbol (const std::string& id, const util::Location& loc)
    : identifier (id)
    , location (loc)
    , inProgress (false)
    , offset_ (0)
  { }
  virtual ~Symbol() { }
  virtual void accept (SymbolVisitor& visitor) = 0;
  virtual void accept (ConstSymbolVisitor& visitor) const = 0;
  virtual bool defined () const
  {
    NOT_REACHED;
  }
  void offset (ptrdiff_t o)
  {
    offset_ = o;
  }
  virtual ptrdiff_t offset () const
  {
    return offset_;
  }

  std::string const identifier;
  util::Location const location;
  bool inProgress;

private:
  ptrdiff_t offset_;
};

struct InstanceSymbol : public Symbol
{
  InstanceSymbol (const std::string& id, const util::Location& loc, const type::NamedType* t, Initializer* init)
    : Symbol (id, loc)
    , type (t)
    , initializer (init)
    , instance (NULL)
  { }
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;

  const type::NamedType* const type;
  Initializer* const initializer;
  composition::Instance* instance;
};

struct ParameterSymbol : public Symbol
{
  enum Kind
  {
    Ordinary,
    OrdinaryDuplicate,
    Receiver,
    ReceiverDuplicate,
    Return,
  };

  ParameterSymbol (const std::string& id, const util::Location& loc, const type::Type* t, Mutability im, Mutability dm, Kind k)
    : Symbol (id, loc)
    , type (t)
    , intrinsic_mutability (im)
    , dereference_mutability (is_typed_string (t) ? std::max (dm, Immutable) : dm)
    , kind (k)
    , original_ (NULL)
  { }

  static ParameterSymbol* make (const util::Location& loc,
                                const std::string& name,
                                const type::Type* type,
                                Mutability intrinsic_mutability,
                                Mutability dereference_mutability)
  {
    return new ParameterSymbol (name, loc, type, intrinsic_mutability, dereference_mutability, Ordinary);
  }

  static ParameterSymbol* makeReturn (const util::Location& loc,
                                      const std::string& name,
                                      const type::Type* type,
                                      Mutability dereference_mutability)
  {
    return new ParameterSymbol (name, loc, type, Mutable, dereference_mutability, Return);
  }

  static ParameterSymbol* makeReceiver (const util::Location& loc,
                                        const std::string& name,
                                        const type::Type* type,
                                        Mutability intrinsic_mutability,
                                        Mutability dereference_mutability)
  {
    return new ParameterSymbol (name, loc, type, intrinsic_mutability, dereference_mutability, Receiver);
  }

  ParameterSymbol* duplicate (Mutability dereferenceMutability)
  {
    ParameterSymbol* s = new ParameterSymbol (*this);
    switch (this->kind)
      {
      case Receiver:
        s->kind = ReceiverDuplicate;
        break;
      case ReceiverDuplicate:
        break;
      case Ordinary:
        s->kind = OrdinaryDuplicate;
        break;
      case OrdinaryDuplicate:
        break;
      default:
        NOT_REACHED;
      }
    s->dereference_mutability = dereferenceMutability;
    s->original_ = this;
    return s;
  }

  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual ptrdiff_t offset () const
  {
    if (kind == ReceiverDuplicate ||
        kind == OrdinaryDuplicate)
      {
        return original_->offset ();
      }
    else
      {
        return Symbol::offset ();
      }
  }

  void check_foreign_safe () const;

  const type::Type* const type;
  Mutability const intrinsic_mutability;
  Mutability dereference_mutability;
  Kind kind;
private:
  Symbol* original_;
};

struct TypeSymbol : public Symbol
{
  TypeSymbol (const std::string& id, const util::Location& loc, type::NamedType* t)
    : Symbol (id, loc)
    , type (t)
  { }

  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual bool defined () const
  {
    return type->UnderlyingType () != NULL;
  }

  type::NamedType* const type;
};

struct ConstantSymbol : public Symbol
{
  ConstantSymbol (const std::string& id, const util::Location& loc, const type::Type* t, const semantic::Value& v)
    : Symbol (id, loc)
    , type (t)
    , value (v)
  { }
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  const type::Type* const type;
  semantic::Value const value;
};

struct VariableSymbol : public Symbol
{
  VariableSymbol (const std::string& id, const util::Location& loc, const type::Type* t, Mutability im, Mutability dm)
    : Symbol (id, loc)
    , type (t)
    , intrinsic_mutability (im)
    , dereference_mutability (is_typed_string (t) ? std::max (dm, Immutable) : dm)
    , original_ (NULL)
  { }

  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;

  virtual ptrdiff_t offset () const
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

  VariableSymbol* duplicate()
  {
    VariableSymbol* s = new VariableSymbol (this->identifier, this->location, this->type, Foreign, Foreign);
    s->original_ = this;
    return s;
  }

  const type::Type* const type;
  Mutability const intrinsic_mutability;
  Mutability dereference_mutability;
private:
  Symbol* original_;
};

struct HiddenSymbol : public Symbol
{
  HiddenSymbol (const Symbol* s, const util::Location& loc)
    : Symbol (s->identifier, loc)
  { }
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
};

std::ostream&
operator<< (std::ostream& out, const Symbol& s);

}

#define SYMBOL_NOT_REACHED(s) do { std::cerr << s << '\n'; NOT_REACHED; } while (0);

#endif // RC_SRC_SYMBOL_HPP
