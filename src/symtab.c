#include "symtab.h"
#include <stdlib.h>
#include "strtab.h"
#include <string.h>
#include "debug.h"

struct symbol_t
{
  string_t identifier;
  enum {
    Semval,
    Type,
  } stype;
  union {
    semval_t semval;
    const type_t* type;
  };
  symbol_t *next;
};

static symtab_t *top = NULL;

void
symtab_set_top (symtab_t * symtab)
{
  top = symtab;
}

symtab_t *
symtab_get_top (void)
{
  return top;
}

void
symtab_init (symtab_t * symtab, const symtab_t * parent)
{
  symtab->parent = parent;
  symtab->head = NULL;
}

void
symtab_enter (symtab_t * symtab, symbol_t * symbol)
{
  /* TODO:  Check that the symbol is not already defined. */
  symbol->next = symtab->head;
  symtab->head = symbol;
}

symbol_t *
symtab_find (const symtab_t * symtab, string_t identifier)
{
  if (symtab == NULL)
    {
      return NULL;
    }

  symbol_t *s;
  for (s = symtab->head; s != NULL; s = s->next)
    {
      if (streq (identifier, s->identifier))
	{
	  return s;
	}
    }

  /* Not found in this scope.  Try the parent. */
  return symtab_find (symtab->parent, identifier);
}

symbol_t *
symbol_make_semval (string_t identifier, semval_t semval)
{
  symbol_t *s = malloc (sizeof (symbol_t));
  memset (s, 0, sizeof (symbol_t));
  s->identifier = identifier;
  s->stype = Semval;
  s->semval = semval;
  return s;
}

semval_t
symbol_get_semval (symbol_t * symbol)
{
  return symbol->semval;
}

symbol_t *symbol_make_type (string_t identifier, const type_t* type)
{
  symbol_t *s = malloc (sizeof (symbol_t));
  memset (s, 0, sizeof (symbol_t));
  s->identifier = identifier;
  s->stype = Type;
  s->type = type;
  return s;
}

const type_t* symbol_get_type (symbol_t * symbol)
{
  assert (symbol_is_type (symbol));
  return symbol->type;
}

bool symbol_is_type (symbol_t* symbol)
{
  return symbol->stype == Type;
}
