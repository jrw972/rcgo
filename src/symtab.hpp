#ifndef symtab_h
#define symtab_h

/* Symbol table. */

#include "strtab.hpp"
#include "typed_value.hpp"
#include "type.hpp"

struct symtab_t
{
  symtab_t ()
    : parent (NULL)
    , current_action (NULL)
    , current_function (NULL)
    , current_method (NULL)
    , current_trigger (NULL)
    , current_receiver_type (NULL)
  { }

  typedef std::vector<symbol_t*> SymbolsType;

  SymbolsType::const_iterator begin () const { return symbols.begin (); }
  SymbolsType::const_iterator end () const { return symbols.end (); }

  symtab_t *parent;
  SymbolsType symbols;
  action_reaction_base_t *current_action;
  function_t* current_function;
  method_t *current_method;
  trigger_t *current_trigger;
  named_type_t *current_receiver_type;
};

void symtab_print (const symtab_t* symtab);

symtab_t *symtab_make (symtab_t * parent);

symtab_t *symtab_parent (symtab_t * symtab);

symtab_t *symtab_get_root (symtab_t * symtab);

void symtab_enter (symtab_t * symtab, symbol_t * symbol);

symbol_t *symtab_find (const symtab_t * symtab, string_t identifier);

symbol_t *symtab_find_current (const symtab_t * symtab, string_t identifier);

symbol_t *symtab_get_this (const symtab_t * symtab);

const type_t *symtab_get_this_type (const symtab_t * symtab);

void symtab_set_current_action (symtab_t * symtab, action_reaction_base_t * action);

action_reaction_base_t *symtab_get_current_action (const symtab_t * symtab);

void symtab_set_current_function (symtab_t * symtab, function_t * function);

function_t *symtab_get_current_function (const symtab_t * symtab);

void symtab_set_current_method (symtab_t * symtab, method_t * method);

method_t *symtab_get_current_method (const symtab_t * symtab);

void symtab_set_current_trigger (symtab_t * symtab, trigger_t * trigger);

trigger_t *symtab_get_current_trigger (const symtab_t * symtab);

void symtab_set_current_receiver_type (symtab_t * symtab, named_type_t * type);

named_type_t *symtab_get_current_receiver_type (const symtab_t * symtab);

symbol_t* symtab_get_first_return_parameter (const symtab_t * symtab);

// Enter all parameters and variables that are in scope and pointers as pointers to foreign.
void symtab_change (symtab_t* symtab);

#endif /* symtab_h */
