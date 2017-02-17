#ifndef RC_SRC_SCOPE_HPP
#define RC_SRC_SCOPE_HPP

#include "types.hpp"

namespace decl
{

// TODO:  Remove this class.
struct Scope
{
  typedef std::vector<Symbol*> SymbolsType;
  Scope ();

  Scope* open ();
  Scope* open (const ParameterList* parameter_list,
               const ParameterList* return_parameter_list);
  Scope* open (Symbol* iota,
               const ParameterList* parameter_list,
               const ParameterList* return_parameter_list);
  Scope* close ();

  void enter_symbol (Symbol* s);
  Symbol* find_global_symbol (const std::string& identifier) const;
  Symbol* find_local_symbol (const std::string& identifier) const;
  void activate ();
  void change ();
  const ParameterList* return_parameter_list () const;
  source::Package* package () const;

private:
  Scope (Scope* p);
  void enter_parameter_list (const ParameterList* type);

  Scope* parent_;
  SymbolsType symbols_;
  const ParameterList* parameter_list_;
  const ParameterList* return_parameter_list_;
};

}

#endif // RC_SRC_SCOPE_HPP
