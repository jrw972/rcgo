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
  enum State
  {
    Declared,
    In_Progress,
    Defined,
    Undefined,
  };

  Symbol (const std::string& name, const util::Location& location);
  virtual ~Symbol ();
  virtual void accept (SymbolVisitor& visitor) = 0;
  virtual void accept (ConstSymbolVisitor& visitor) const = 0;
  State state () const;
  bool process_declaration (util::ErrorReporter& er, Scope* file_scope);
  void offset (ptrdiff_t o);
  virtual ptrdiff_t offset () const;

  std::string const name;
  util::Location const location;

protected:
  State state_;
  virtual bool process_declaration_i (util::ErrorReporter& er, Scope* file_scope);
  virtual void post_process_declaration_i (util::ErrorReporter& er, Scope* file_scope);
private:
  // TODO:  Should this be here?
  ptrdiff_t offset_;
};

struct Instance : public Symbol
{
  Instance (ast::InstanceDecl* a_instancedecl);
  Instance (const std::string& name,
            const util::Location& location,
            const type::NamedType* type,
            Initializer* initializer);
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual bool process_declaration_i (util::ErrorReporter& er, Scope* file_scope);

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
            ast::Node* a_type_spec,
            ast::Node* a_init);
  Constant (const std::string& name,
            const util::Location& location,
            const type::Type* type,
            const semantic::Value& value);
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual bool process_declaration_i (util::ErrorReporter& er, Scope* file_scope);

  const type::Type* type () const;
  semantic::Value value () const;

private:
  const type::Type* type_;
  semantic::Value value_;
  ast::Node* const type_spec_;
  ast::Node* const init_;
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

struct Field : public Symbol
{
  const type::Struct* const m_struct;
  decl::Package* const package;
  bool const is_anonymous;
  const type::Type* const type;
  type::TagSet const tags;

  Field (const type::Struct* a_struct,
         decl::Package* a_package,
         bool a_is_anonymous,
         const std::string& a_name,
         const util::Location& a_location,
         const type::Type* a_type,
         const type::TagSet& a_tags);
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
};

}

#endif // RC_SRC_SYMBOL_HPP
