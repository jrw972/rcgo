#ifndef RC_SRC_SYMBOL_TABLE_HPP
#define RC_SRC_SYMBOL_TABLE_HPP

#include "types.hpp"

namespace decl
{

struct SymbolTable
{
  SymbolTable ();

  void open_scope ();
  void open_scope (const ParameterList* parameter_list,
                   const ParameterList* return_parameter_list);
  void open_scope (Symbol* iota,
                   const ParameterList* parameter_list,
                   const ParameterList* return_parameter_list);
  void close_scope ();

  void enter_symbol (Symbol* s);

  void change ();
  void activate ();

  Symbol* find_global_symbol (const std::string& identifier) const;
  Symbol* find_local_symbol (const std::string& identifier) const;
  Package* package () const;
  const ParameterList* return_parameter_list () const;

  struct Scope;
  const Scope* current_scope () const;

private:
  void enter_parameter_list (const ParameterList* type);
  Scope* current_scope_;
};

}

#endif // RC_SRC_SYMBOL_TABLE_HPP
