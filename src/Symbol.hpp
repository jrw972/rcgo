#ifndef Symbol_hpp
#define Symbol_hpp

#include "types.hpp"
#include "typed_value.hpp"
#include "type.hpp"
#include "parameter.hpp"

class SymbolVisitor;
class ConstSymbolVisitor;

/*
 * Base class for symbols.
 */
struct Symbol {
  Symbol (const std::string& id, ast_t* dn)
    : identifier (id)
    , definingNode (dn)
    , inProgress (false)
    , offset_ (0)
  { }
  virtual ~Symbol() { }
  virtual void accept (SymbolVisitor& visitor) = 0;
  virtual void accept (ConstSymbolVisitor& visitor) const = 0;
  virtual const char* kindString () const = 0;
  virtual bool defined () const { not_reached; }
  void offset (ptrdiff_t o) { offset_ = o; }
  virtual ptrdiff_t offset () const { return offset_; }

  std::string const identifier;
  ast_t* const definingNode;
  bool inProgress;

private:
  ptrdiff_t offset_;
};

struct BuiltinFunctionSymbol : public Symbol {
  BuiltinFunctionSymbol (const std::string& id, ast_t* dn, const function_type_t* t)
    : Symbol (id, dn)
    , value_ (typed_value_t::make_ref (t, typed_value_t::CONSTANT, IMMUTABLE, IMMUTABLE))
  { }
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual const char* kindString () const { return "BuiltinFunction"; }

  typed_value_t value () const { return value_; }

private:
  typed_value_t const value_;
};

struct InstanceSymbol : public Symbol {
  InstanceSymbol (const std::string& id, ast_t* dn)
    : Symbol (id, dn)
    , type (NULL)
    , initializer (NULL)
  { }
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual const char* kindString () const { return "Instance"; }

  const named_type_t *type;
  Initializer* initializer;
};

struct ParameterSymbol : public Symbol {
  enum Kind {
    Ordinary,
    OrdinaryDuplicate,
    Receiver,
    ReceiverDuplicate,
    Return,
  };

  ParameterSymbol (const std::string& id, ast_t* dn, const typed_value_t& v, Kind k)
    : Symbol (id, dn)
    , value (v)
    , kind (k)
    , original_ (NULL)
  { }

  static ParameterSymbol* make (const parameter_t* parameter) {
    return new ParameterSymbol (parameter->name, parameter->defining_node, typed_value_t::make_ref (parameter->value), Ordinary);
  }

  static ParameterSymbol* makeReturn (const parameter_t* parameter) {
    return new ParameterSymbol (parameter->name, parameter->defining_node, typed_value_t::make_ref (parameter->value), Return);
  }

  static ParameterSymbol* makeReceiver (const parameter_t* parameter) {
    return new ParameterSymbol (parameter->name, parameter->defining_node, typed_value_t::make_ref (parameter->value), Receiver);
  }

  ParameterSymbol* duplicate () {
    ParameterSymbol* s = new ParameterSymbol (*this);
    switch (this->kind) {
    case Receiver:
      s->value.dereference_mutability = MUTABLE;
      s->kind = ReceiverDuplicate;
      break;
    case Ordinary:
      s->value = typed_value_t::make_ref (this->value.type, typed_value_t::STACK, FOREIGN, FOREIGN);
      s->kind = OrdinaryDuplicate;
      break;
    default:
      not_reached;
    }
    s->original_ = this;
    return s;
  }

  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual ptrdiff_t offset () const {
    if (kind == ReceiverDuplicate ||
        kind == OrdinaryDuplicate) {
      return original_->offset ();
    } else {
      return Symbol::offset ();
    }
  }

  virtual const char* kindString () const { return "Parameter"; }

  typed_value_t value;
  Kind kind;
private:
  Symbol* original_;
};

struct TypeSymbol : public Symbol {
  TypeSymbol (const std::string& id, ast_t* dn, named_type_t* t)
    : Symbol (id, dn)
    , type (t)
  { }

  TypeSymbol (const std::string& id, ast_t* dn)
    : Symbol (id, dn)
    , type (new named_type_t (id))
  { }

  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual bool defined () const { return type->subtype () != NULL; }
  virtual const char* kindString () const { return "Type"; }

  named_type_t* const type;
};

struct TypedConstantSymbol : public Symbol {
  TypedConstantSymbol (const std::string& id, ast_t* dn, const typed_value_t& v)
    : Symbol (id, dn)
    , value (typed_value_t::make_ref (v))
  { }
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual const char* kindString () const { return "TypedConstant"; }

  typed_value_t const value;
};

struct VariableSymbol : public Symbol {
  VariableSymbol (const std::string& id, ast_t* dn, const typed_value_t& v)
    : Symbol (id, dn)
    , value (v)
    , original_ (NULL)
  { }

  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual const char* kindString () const { return "Variable"; }

  virtual ptrdiff_t offset () const {
    if (original_ != NULL) {
      return original_->offset ();
    } else {
      return Symbol::offset ();
    }
  }

  VariableSymbol* duplicate() {
    VariableSymbol* s = new VariableSymbol (this->identifier, this->definingNode, typed_value_t::make_ref (this->value.type, typed_value_t::STACK, FOREIGN, FOREIGN));
    s->original_ = this;
    return s;
  }

  typed_value_t const value;
private:
  Symbol* original_;
};

struct HiddenSymbol : public Symbol {
  HiddenSymbol (const Symbol* s, ast_t* dn)
    : Symbol (s->identifier, dn)
  { }
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual const char* kindString () const { return "Hidden"; }
};

#endif /* Symbol_hpp */
