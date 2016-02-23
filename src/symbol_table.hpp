#ifndef RC_SRC_SYMBOL_TABLE_HPP
#define RC_SRC_SYMBOL_TABLE_HPP

#include "types.hpp"

namespace decl
{

struct SymbolTable
{

  SymbolTable ()
    : current_scope_ (NULL)
  { }

  void open_scope ();
  void close_scope ();

  void enter_symbol (decl::Symbol* s);
  void enter_signature (const decl::ParameterList* type);

  void change ();
  void activate ();

  Symbol* find_global_symbol (const std::string& identifier) const;
  Symbol* find_local_symbol (const std::string& identifier) const;

  Package* package () const
  {
    return NULL;
  }

private:
  struct Scope;
  Scope* current_scope_;
};

}

#endif // RC_SRC_SYMBOL_TABLE_HPP
