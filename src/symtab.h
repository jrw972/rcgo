#ifndef symtab_h
#define symtab_h

/* Symbol table. */

#include "strtab.h"
#include "semval.h"

typedef struct symbol_t symbol_t;

/* A symtab_t is designed to be allocated on the stack. */
typedef struct symtab_t symtab_t;
struct symtab_t
{
  const symtab_t *parent;
  symbol_t *head;
};

void symtab_init (symtab_t * symtab, const symtab_t * parent);

void symtab_fini (symtab_t * symtab);

void symtab_enter (symtab_t * symtab, symbol_t * symbol);

symbol_t *symtab_find (const symtab_t * symtab, string_t identifier);

symbol_t *symtab_find_current (const symtab_t * symtab, string_t identifier);

symbol_t *symtab_get_this (const symtab_t * symtab);

string_t symbol_identifier (const symbol_t* symbol);

symbol_t *symbol_make_variable (string_t identifier, const type_t* type);

bool symbol_is_variable (const symbol_t* symbol);

const type_t* symbol_variable_type (const symbol_t* symbol);

symbol_t *symbol_make_type (const type_t* type);

bool symbol_is_type (const symbol_t* symbol);

const type_t* symbol_type_type (const symbol_t* symbol);

symbol_t * symbol_make_constant (string_t identifier, abstract_value_t value);

bool symbol_is_constant (const symbol_t* symbol);

abstract_value_t symbol_constant_value (const symbol_t* symbol);

symbol_t * symbol_make_instance (string_t identifier, const type_t* type);

void symbol_set_as_this (symbol_t* symbol);

#endif /* symtab_h */
