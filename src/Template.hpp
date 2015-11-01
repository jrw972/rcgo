#ifndef Template_hpp
#define Template_hpp

#include "Symbol.hpp"

/*
 * Base class for builtin functions.
 */
struct Template : public Symbol
{
  Template (const std::string& id,
            ast_t* dn,
            const Type::Type* type)
    : Symbol (id, dn)
    , type_ (type)
    // type () is called here
    , typed_value_ (typed_value_t::make_ref (typed_value_t (this)))
  { }

  virtual typed_value_t instantiate (TypedValueListType& tvlist) = 0;

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

  typed_value_t value () const
  {
    return typed_value_;
  }

private:
  const Type::Type* const type_;
  typed_value_t const typed_value_;
};

#endif /* Template_hpp */
