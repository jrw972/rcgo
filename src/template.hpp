#ifndef rc_src_template_hpp
#define rc_src_template_hpp

#include "symbol.hpp"

namespace decl
{

/*
 * Base class for builtin functions.
 */
struct Template : public Symbol
{
  Template (const std::string& id,
            ast::Node* dn,
            const type::Type* type)
    : Symbol (id, dn)
    , type_ (type)
    // type () is called here
  { }

  virtual Callable* instantiate (const std::vector<const type::Type*>& argument_types) const = 0;

  // Symbol
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual const char* kindString () const
  {
    return "Template";
  }

  const type::Type* type () const
  {
    return type_;
  }

private:
  const type::Type* const type_;
};

}

#endif // rc_src_template_hpp
