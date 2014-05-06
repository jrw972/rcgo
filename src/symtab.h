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

void symtab_set_top (symtab_t * symtab);

symtab_t *symtab_get_top (void);

void symtab_init (symtab_t * symtab, const symtab_t * parent);

void symtab_enter (symtab_t * symtab, symbol_t * symbol);

symbol_t *symtab_find (const symtab_t * symtab, string_t identifier);

symbol_t *symbol_make_semval (string_t identifier, semval_t semval);

semval_t symbol_get_semval (symbol_t * symbol);

symbol_t *symbol_make_type (string_t identifier, const type_t* type);

const type_t* symbol_get_type (symbol_t * symbol);

bool symbol_is_type (symbol_t* symbol);

#endif /* symtab_h */
