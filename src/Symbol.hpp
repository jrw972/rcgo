#ifndef Symbol_hpp
#define Symbol_hpp

#include "types.hpp"
#include "typed_value.hpp"
#include "Type.hpp"
#include "parameter.hpp"

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
  virtual const char* kindString () const = 0;
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
  InstanceSymbol (const std::string& id, ast::Node* dn)
    : Symbol (id, dn)
    , type (NULL)
    , initializer (NULL)
    , instance (NULL)
  { }
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual const char* kindString () const
  {
    return "Instance";
  }

  const Type::NamedType *type;
  Initializer* initializer;
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
    Iota,
  };

  ParameterSymbol (const std::string& id, ast::Node* dn, const Type::Type* t, Mutability im, Mutability dm, Kind k)
    : Symbol (id, dn)
    , type (t)
    , intrinsic_mutability (im)
    , dereference_mutability (dm)
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

  static ParameterSymbol* makeIota ()
  {
    unimplemented;
    //return new ParameterSymbol (parameter->name, parameter->defining_node, typed_value_t::make_ref (parameter->value), Iota);
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
    unimplemented;
    // s->value.dereference_mutability = dereferenceMutability;
    // s->original_ = this;
    // return s;
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

  virtual const char* kindString () const
  {
    return "Parameter";
  }

  void check_foreign_safe () const;

  const Type::Type* const type;
  Mutability const intrinsic_mutability;
  Mutability const dereference_mutability;
  Kind kind;
private:
  Symbol* const original_;
};

struct TypeSymbol : public Symbol
{
  TypeSymbol (const std::string& id, ast::Node* dn, Type::NamedType* t)
    : Symbol (id, dn)
    , type (t)
  { }

  TypeSymbol (const std::string& id, ast::Node* dn)
    : Symbol (id, dn)
    , type (new Type::NamedType (id))
  { }

  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual bool defined () const
  {
    return type->UnderlyingType () != NULL;
  }
  virtual const char* kindString () const
  {
    return "Type";
  }

  Type::NamedType* const type;
};

struct TypedConstantSymbol : public Symbol
{
  TypedConstantSymbol (const std::string& id, ast::Node* dn, const typed_value_t& v)
    : Symbol (id, dn)
    , value (typed_value_t::make_ref (v))
  { }
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual const char* kindString () const
  {
    return "TypedConstant";
  }

  typed_value_t const value;
};

struct VariableSymbol : public Symbol
{
  VariableSymbol (const std::string& id, ast::Node* dn, const typed_value_t& v)
    : Symbol (id, dn)
    , value (v)
    , original_ (NULL)
  { }

  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual const char* kindString () const
  {
    return "Variable";
  }

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
    typed_value_t tv = value;
    tv.intrinsic_mutability = FOREIGN;
    tv.dereference_mutability = FOREIGN;
    VariableSymbol* s = new VariableSymbol (this->identifier, this->definingNode, tv);
    s->original_ = this;
    return s;
  }

  typed_value_t const value;
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
  virtual const char* kindString () const
  {
    return "Hidden";
  }
};

#endif /* Symbol_hpp */
