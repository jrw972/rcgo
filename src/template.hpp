#ifndef rc_src_template_hpp
#define rc_src_template_hpp

#include "symbol.hpp"

/*
 * Base class for builtin functions.
 */
struct Template : public Symbol
{
  Template (const std::string& id,
            ast::Node* dn,
            const Type::Type* type)
    : Symbol (id, dn)
    , type_ (type)
    // type () is called here
  { }

  virtual Callable* instantiate (const std::vector<const Type::Type*>& argument_types) const = 0;

  // Symbol
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual const char* kindString () const
  {
    return "Template";
  }

  const Type::Type* type () const
  {
    return type_;
  }

private:
  const Type::Type* const type_;
};

#endif // rc_src_template_hpp
