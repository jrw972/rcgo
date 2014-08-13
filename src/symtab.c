#include "symtab.h"
#include <stdlib.h>
#include "strtab.h"
#include <string.h>
#include "debug.h"

typedef enum
{
  Constant,
  Instance,
  Variable,
  Type
} SymbolKind;

struct symbol_t
{
  string_t identifier;
  SymbolKind kind;
  union
  {
    struct
    {
      abstract_value_t value;
    } constant;
    struct
    {
      const type_t *type;
    } instance;
    struct
    {
      const type_t *type;
    } variable;
    struct
    {
      const type_t *type;
    } type;
  };
  bool is_this;
  symbol_t *next;
};

void
symtab_init (symtab_t * symtab, const symtab_t * parent)
{
  symtab->parent = parent;
  symtab->head = NULL;
}

void
symtab_fini (symtab_t * symtab)
{
  while (symtab->head != NULL)
    {
      symbol_t *sym = symtab->head;
      symtab->head = sym->next;
      free (sym);
    }
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
symtab_find_current (const symtab_t * symtab, string_t identifier)
{
  symbol_t *s;
  for (s = symtab->head; s != NULL; s = s->next)
    {
      if (streq (identifier, s->identifier))
	{
	  return s;
	}
    }

  return NULL;
}

symbol_t *
symtab_get_this (const symtab_t * symtab)
{
  if (symtab == NULL)
    {
      return NULL;
    }

  symbol_t *s;
  for (s = symtab->head; s != NULL; s = s->next)
    {
      if (s->is_this)
	{
	  return s;
	}
    }

  return symtab_get_this (symtab->parent);
}

string_t
symbol_identifier (const symbol_t * symbol)
{
  return symbol->identifier;
}

static symbol_t *
make (string_t identifier, SymbolKind kind)
{
  symbol_t *s = malloc (sizeof (symbol_t));
  memset (s, 0, sizeof (symbol_t));
  s->identifier = identifier;
  s->kind = kind;
  return s;
}

symbol_t *
symbol_make_variable (string_t identifier, const type_t * type)
{
  symbol_t *s = make (identifier, Variable);
  s->variable.type = type;
  return s;
}

bool
symbol_is_variable (const symbol_t * symbol)
{
  return symbol->kind == Variable;
}

const type_t *
symbol_variable_type (const symbol_t * symbol)
{
  assert (symbol_is_variable (symbol));
  return symbol->variable.type;
}

symbol_t *
symbol_make_type (const type_t * type)
{
  symbol_t *s = make (type_get_name (type), Type);
  s->type.type = type;
  return s;
}

bool
symbol_is_type (const symbol_t * symbol)
{
  return symbol->kind == Type;
}

const type_t *
symbol_type_type (const symbol_t * symbol)
{
  assert (symbol_is_type (symbol));
  return symbol->type.type;
}

symbol_t *
symbol_make_constant (string_t identifier, abstract_value_t value)
{
  symbol_t *s = make (identifier, Constant);
  s->constant.value = value;
  return s;
}

bool
symbol_is_constant (const symbol_t * symbol)
{
  return symbol->kind == Constant;
}

abstract_value_t
symbol_constant_value (const symbol_t * symbol)
{
  assert (symbol_is_constant (symbol));
  return symbol->constant.value;
}

symbol_t *
symbol_make_instance (string_t identifier, const type_t * type)
{
  symbol_t *s = make (identifier, Instance);
  s->instance.type = type;
  return s;
}

void
symbol_set_as_this (symbol_t * symbol)
{
  symbol->is_this = true;
}
