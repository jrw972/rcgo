#ifndef RC_SRC_SYMBOL_TABLE_HPP
#define RC_SRC_SYMBOL_TABLE_HPP

#include "types.hpp"

namespace decl
{

struct SymbolTable
{
  void open_scope ();
  // TODO:  Remove this.
  void open_scope (const ParameterList* parameter_list,
                   const ParameterList* return_parameter_list);
  // TODO:  Remove this.
  void open_scope (Symbol* iota,
                   const ParameterList* parameter_list,
                   const ParameterList* return_parameter_list);
  void close_scope ();
  void enter_symbol (Symbol* a_symbol);
  Symbol* retrieve_symbol (const source::Identifier& identifier) const;
  Symbol* retrieve_symbol (const std::string& identifier) const;
  bool is_declared_locally (const source::Identifier& identifier) const;
  bool is_declared_locally (const std::string& identifier) const;
  bool check_is_declared_locally (util::ErrorReporter& er,
                                  const source::Identifier& identifier) const;
  bool check_is_declared (util::ErrorReporter& er,
                          const source::Identifier& identifier) const;


  // TODO:  Remove this.
  void activate ();
  // TODO:  Remove this.
  void change ();
  // TODO:  Remove this.
  const ParameterList* return_parameter_list () const;
  source::Package* package () const;
  typedef std::map<std::string, Symbol*> SymbolsType;
  typedef SymbolsType::const_iterator const_symbols_iterator;
  const_symbols_iterator symbols_begin () const;
  const_symbols_iterator symbols_end () const;

private:
  struct Scope
  {
    SymbolsType symbols;
    void enter_symbol (Symbol* s);
    void enter_parameter_list (const ParameterList* type);
    const ParameterList* parameter_list_;
    const ParameterList* return_parameter_list_;
  };
  typedef std::vector<Scope> ScopesType;
  ScopesType scopes_;
};

}

#endif // RC_SRC_SYMBOL_TABLE_HPP
