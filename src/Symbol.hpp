#ifndef Symbol_hpp
#define Symbol_hpp

#include "types.hpp"
#include "typed_value.hpp"
#include "Type.hpp"

class SymbolVisitor;
class ConstSymbolVisitor;

extern std::string const ReturnSymbol;

/*
 * Base class for symbols.
 */
struct Symbol
{
  Symbol (const std::string& id, ast::Node* dn)
    : identifier (id)
    , definingNode (dn)
    , inProgress (false)
    , offset_ (0)
  { }
  virtual ~Symbol() { }
  virtual void accept (SymbolVisitor& visitor) = 0;
  virtual void accept (ConstSymbolVisitor& visitor) const = 0;
  virtual bool defined () const
  {
    not_reached;
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
  ast::Node* const definingNode;
  bool inProgress;

private:
  ptrdiff_t offset_;
};

struct InstanceSymbol : public Symbol
{
  InstanceSymbol (const std::string& id, ast::Node* dn, const Type::NamedType* t, Initializer* init)
    : Symbol (id, dn)
    , type (t)
    , initializer (init)
    , instance (NULL)
  { }
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;

  const Type::NamedType* const type;
  Initializer* const initializer;
  Composition::Instance* instance;
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

  ParameterSymbol (const std::string& id, ast::Node* dn, const Type::Type* t, Mutability im, Mutability dm, Kind k)
    : Symbol (id, dn)
    , type (t)
    , intrinsic_mutability (im)
    , dereference_mutability (is_typed_string (t) ? std::max (dm, IMMUTABLE) : dm)
    , kind (k)
    , original_ (NULL)
  { }

  static ParameterSymbol* make (ast::Node* defining_node,
                                const std::string& name,
                                const Type::Type* type,
                                Mutability intrinsic_mutability,
                                Mutability dereference_mutability)
  {
    return new ParameterSymbol (name, defining_node, type, intrinsic_mutability, dereference_mutability, Ordinary);
  }

  static ParameterSymbol* makeReturn (ast::Node* defining_node,
                                      const std::string& name,
                                      const Type::Type* type,
                                      Mutability dereference_mutability)
  {
    return new ParameterSymbol (name, defining_node, type, MUTABLE, dereference_mutability, Return);
  }

  static ParameterSymbol* makeReceiver (ast::Node* defining_node,
                                        const std::string& name,
                                        const Type::Type* type,
                                        Mutability intrinsic_mutability,
                                        Mutability dereference_mutability)
  {
    return new ParameterSymbol (name, defining_node, type, intrinsic_mutability, dereference_mutability, Receiver);
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
        not_reached;
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

  const Type::Type* const type;
  Mutability const intrinsic_mutability;
  Mutability dereference_mutability;
  Kind kind;
private:
  Symbol* original_;
};

struct TypeSymbol : public Symbol
{
  TypeSymbol (const std::string& id, ast::Node* dn, Type::NamedType* t)
    : Symbol (id, dn)
    , type (t)
  { }

  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual bool defined () const
  {
    return type->UnderlyingType () != NULL;
  }

  Type::NamedType* const type;
};

struct ConstantSymbol : public Symbol
{
  ConstantSymbol (const std::string& id, ast::Node* dn, const Type::Type* t, const value_t& v)
    : Symbol (id, dn)
    , type (t)
    , value (v)
  { }
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  const Type::Type* const type;
  value_t const value;
};

struct VariableSymbol : public Symbol
{
  VariableSymbol (const std::string& id, ast::Node* dn, const Type::Type* t, Mutability im, Mutability dm)
    : Symbol (id, dn)
    , type (t)
    , intrinsic_mutability (im)
    , dereference_mutability (is_typed_string (t) ? std::max (dm, IMMUTABLE) : dm)
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
    VariableSymbol* s = new VariableSymbol (this->identifier, this->definingNode, this->type, FOREIGN, FOREIGN);
    s->original_ = this;
    return s;
  }

  const Type::Type* const type;
  Mutability const intrinsic_mutability;
  Mutability dereference_mutability;
private:
  Symbol* original_;
};

struct HiddenSymbol : public Symbol
{
  HiddenSymbol (const Symbol* s, ast::Node* dn)
    : Symbol (s->identifier, dn)
  { }
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
};

std::ostream&
operator<< (std::ostream& out, const Symbol& s);

#define symbol_not_reached(s) do { std::cerr << s << '\n'; not_reached; } while (0);

#endif /* Symbol_hpp */
