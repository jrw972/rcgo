#ifndef RC_SRC_TEMPLATE_HPP
#define RC_SRC_TEMPLATE_HPP

#include "symbol.hpp"

namespace decl
{

/*
 * Base class for builtin functions.
 */
struct Template : public Symbol
{
  Template (const std::string& id,
            const util::Location& loc,
            const type::Type* type)
    : Symbol (id, loc)
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

#endif // RC_SRC_TEMPLATE_HPP
