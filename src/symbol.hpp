#ifndef RC_SRC_SYMBOL_HPP
#define RC_SRC_SYMBOL_HPP

#include "types.hpp"
#include "location.hpp"
#include "semantic.hpp"
#include "identifier.hpp"

namespace decl
{

// Base class for symbols.
struct Symbol
{
  enum State
  {
    Declared,
    In_Progress,
    Defined,
    Undefined,
  };

  Symbol (const source::Identifier& a_identifier);
  virtual ~Symbol ();
  virtual void accept (SymbolVisitor& visitor) = 0;
  virtual void accept (ConstSymbolVisitor& visitor) const = 0;
  State state () const;
  bool process_declaration (util::ErrorReporter& er, SymbolTable& symbol_table);
  void offset (ptrdiff_t o);
  virtual ptrdiff_t offset () const;

  source::Identifier const identifier;

protected:
  State state_;
  virtual bool process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table);
  virtual void post_process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table);
private:
  // TODO:  Should this be here?
  ptrdiff_t offset_;
};

struct Instance : public Symbol
{
  Instance (ast::InstanceDecl* a_instancedecl);
  Instance (const source::Identifier& a_identifier,
            const type::NamedType* type,
            Initializer* initializer);
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual bool process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table);

  const type::NamedType* type () const;
  Initializer* initializer () const;

private:
  const type::NamedType* type_;
  Initializer* initializer_;
public:
  // TODO:  Should this be here?
  composition::Instance* instance;
private:
  ast::InstanceDecl* const instancedecl_;
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
  make (const source::Identifier& identifier,
        Mutability intrinsic_mutability,
        Mutability indirection_mutability,
        const type::Type* type = NULL);

  static Parameter*
  make_return (const source::Identifier& identifier,
               Mutability indirection_mutability,
               const type::Type* type = NULL);

  static Parameter*
  make_receiver (const source::Identifier& identifier,
                 Mutability intrinsic_mutability,
                 Mutability indirection_mutability,
                 const type::Type* type = NULL);

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
  Parameter (const source::Identifier& identifier,
             const type::Type* type,
             Mutability intrinsic_mutability,
             Mutability indirection_mutability,
             Kind kind);

  Symbol* original_;
};

struct Constant : public Symbol
{
  Constant (const source::Identifier& identifier,
            ast::Node* a_type_spec,
            ast::Node* a_init);
  Constant (const source::Identifier& identifier,
            const semantic::ExpressionValue& ev);
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual bool process_declaration_i (util::ErrorReporter& er, SymbolTable& symbol_table);

  semantic::ExpressionValue value () const;

private:
  semantic::ExpressionValue value_;
  ast::Node* const type_spec_;
  ast::Node* const init_;
};

struct Variable : public Symbol
{
  Variable (const source::Identifier& identifier,
            Mutability intrinsic_mutability,
            Mutability indirection_mutability,
            const type::Type* type = NULL);
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
  Hidden (const Symbol* symbol);
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
};

struct Field : public Symbol
{
  const type::Struct* const m_struct;
  source::Package* const package;
  bool const is_anonymous;
  const type::Type* const type;
  type::TagSet const tags;

  Field (const type::Struct* a_struct,
         source::Package* a_package,
         bool a_is_anonymous,
         const source::Identifier& identifier,
         const type::Type* a_type,
         const type::TagSet& a_tags);
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
};

struct Package : public Symbol
{
  Package (const source::Identifier& a_identifier, const source::Package* a_package);
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;

  const source::Package* const package;
};

  struct ImportedSymbol : public Symbol
  {
    ImportedSymbol (const source::Identifier& a_identifier, Symbol* a_symbol);
    virtual void accept (SymbolVisitor& visitor);
    virtual void accept (ConstSymbolVisitor& visitor) const;

    Symbol* const symbol;
  };


}

#endif // RC_SRC_SYMBOL_HPP
