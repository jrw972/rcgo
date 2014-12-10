#ifndef symtab_h
#define symtab_h

/* Symbol table. */

#include "strtab.h"
#include "typed_value.h"

void symtab_print (const symtab_t* symtab);

symtab_t *symtab_make (symtab_t * parent);

const symtab_t *symtab_parent (symtab_t * symtab);

symtab_t *symtab_get_root (symtab_t * symtab);

void symtab_enter (symtab_t * symtab, symbol_t * symbol);

symbol_t *symtab_find (const symtab_t * symtab, string_t identifier);

symbol_t *symtab_find_current (const symtab_t * symtab, string_t identifier);

symbol_t *symtab_get_this (const symtab_t * symtab);

const type_t *symtab_get_this_type (const symtab_t * symtab);

void symtab_set_current_action (symtab_t * symtab, action_t * action);

action_t *symtab_get_current_action (const symtab_t * symtab);

void symtab_set_current_func (symtab_t * symtab, func_t * func);

func_t *symtab_get_current_func (const symtab_t * symtab);

void symtab_set_current_trigger (symtab_t * symtab, trigger_t * trigger);

trigger_t *symtab_get_current_trigger (const symtab_t * symtab);

void symtab_set_current_receiver_type (symtab_t * symtab, type_t * type);

type_t *symtab_get_current_receiver_type (const symtab_t * symtab);

symbol_t* symtab_get_first_return_parameter (const symtab_t * symtab);

symbol_t** symtab_begin (const symtab_t* symtab);

symbol_t** symtab_end (const symtab_t* symtab);

symbol_t** symtab_next (symbol_t** pos);

#endif /* symtab_h */
