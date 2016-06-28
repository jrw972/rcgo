#ifndef RC_SRC_SYMBOL_HPP
#define RC_SRC_SYMBOL_HPP

#include "types.hpp"
#include "location.hpp"
#include "value.hpp"

namespace decl
{

// Base class for symbols.
struct Symbol
{
  Symbol (const std::string& name, const util::Location& location);
  virtual ~Symbol ();
  virtual void accept (SymbolVisitor& visitor) = 0;
  virtual void accept (ConstSymbolVisitor& visitor) const = 0;
  virtual bool defined () const;
  void offset (ptrdiff_t o);
  virtual ptrdiff_t offset () const;

  std::string const name;
  util::Location const location;
  bool in_progress;

private:
  // TODO:  Should this be here?
  ptrdiff_t offset_;
};

struct Instance : public Symbol
{
  Instance (const std::string& name,
            const util::Location& location,
            const type::NamedType* type,
            Initializer* initializer);
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;

  const type::NamedType* const type;
  Initializer* const initializer;
  // TODO:  Should this be here?
  composition::Instance* instance;
};

struct Parameter : public Symbol
{
  enum Kind
  {
    Ordinary,
    Ordinary_Duplicate,
    Receiver,
    Receiver_Duplicate,
    Return,
  };

  static Parameter*
  make (const util::Location& location,
        const std::string& name,
        const type::Type* type,
        Mutability intrinsic_mutability,
        Mutability indirection_mutability);

  static Parameter*
  make_return (const util::Location& location,
               const std::string& name,
               const type::Type* type,
               Mutability indirection_mutability);

  static Parameter*
  make_receiver (const util::Location& location,
                 const std::string& name,
                 const type::Type* type,
                 Mutability intrinsic_mutability,
                 Mutability indirection_mutability);

  Parameter*
  duplicate (Mutability indirection_mutability);

  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual ptrdiff_t offset () const;
  bool is_foreign_safe () const;

  const type::Type* const type;
  Mutability const intrinsic_mutability;
  Mutability indirection_mutability;
  Kind kind;
private:
  Parameter (const util::Location& location,
             const std::string& name,
             const type::Type* type,
             Mutability intrinsic_mutability,
             Mutability indirection_mutability,
             Kind kind);

  Symbol* original_;
};

struct Constant : public Symbol
{
  Constant (const std::string& name,
            const util::Location& location,
            const type::Type* type,
            const semantic::Value& value);
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  const type::Type* const type;
  semantic::Value const value;
};

struct Variable : public Symbol
{
  Variable (const std::string& name,
            const util::Location& location,
            const type::Type* type,
            Mutability intrinsic_mutability,
            Mutability indirection_mutability);
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual ptrdiff_t offset () const;
  Variable* duplicate();

  const type::Type* const type;
  Mutability const intrinsic_mutability;
  Mutability indirection_mutability;
private:
  Symbol* original_;
};

struct Hidden : public Symbol
{
  Hidden (const Symbol* symbol, const util::Location& location);
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
};

}

#endif // RC_SRC_SYMBOL_HPP
