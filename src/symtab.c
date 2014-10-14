#include "symtab.h"
#include <stdlib.h>
#include "strtab.h"
#include <string.h>
#include "debug.h"
#include "util.h"
#include "type.h"
#include "symbol.h"

struct symtab_t
{
  symtab_t *parent;
  VECTOR_DECL (symbols, symbol_t*);
  bool in_trigger_statement;
  action_t* current_action;
  type_t* current_receiver_type;
};

symtab_t* symtab_make (symtab_t * parent)
{
  symtab_t* s = xmalloc (sizeof (symtab_t));
  s->parent = parent;
  VECTOR_INIT (s->symbols, symbol_t*, 0, NULL);
  return s;
}

const symtab_t* symtab_parent (symtab_t* symtab)
{
  return symtab->parent;
}

symtab_t* symtab_get_root (symtab_t* symtab)
{
  for (; symtab->parent != NULL; symtab = symtab->parent)
    ;;
  return symtab;
}

void
symtab_enter (symtab_t * symtab, symbol_t * symbol)
{
  VECTOR_PUSH (symtab->symbols, symbol_t*, symbol);
}

symbol_t *
symtab_find (const symtab_t * symtab, string_t identifier)
{
  if (symtab == NULL)
    {
      return NULL;
    }

  VECTOR_FOREACH (pos, limit, symtab->symbols, symbol_t*)
    {
      symbol_t* s = *pos;
      if (streq (identifier, symbol_identifier (s)))
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
  VECTOR_FOREACH (pos, limit, symtab->symbols, symbol_t*)
    {
      symbol_t* s = *pos;
      if (streq (identifier, symbol_identifier (s)))
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

  VECTOR_FOREACH (pos, limit, symtab->symbols, symbol_t*)
    {
      symbol_t* s = *pos;
      if (symbol_kind (s) == SymbolParameter && symbol_parameter_kind (s) == ParameterThis)
	{
	  return s;
	}
    }

  return symtab_get_this (symtab->parent);
}

const type_t* symtab_get_this_type (const symtab_t* symtab)
{
  return symbol_parameter_type (symtab_get_this (symtab));
}

bool symtab_in_trigger_statement (const symtab_t* symtab)
{
  if (symtab == NULL)
    {
      return false;
    }

  if (symtab->in_trigger_statement == true)
    {
      return true;
    }

  return symtab_in_trigger_statement (symtab->parent);
}

void symtab_set_in_trigger_statement (symtab_t* symtab)
{
  symtab->in_trigger_statement = true;
}

void symtab_set_current_action (symtab_t* symtab,
                                action_t* action)
{
  symtab->current_action = action;
}

action_t* symtab_get_current_action (const symtab_t* symtab)
{
  if (symtab == NULL)
    {
      return NULL;
    }

  if (symtab->current_action != NULL)
    {
      return symtab->current_action;
    }

  return symtab_get_current_action (symtab->parent);
}

void symtab_set_current_receiver_type (symtab_t* symtab, type_t* type)
{
  symtab->current_receiver_type = type;
}

type_t* symtab_get_current_receiver_type (const symtab_t* symtab)
{
  if (symtab == NULL)
    {
      return NULL;
    }

  if (symtab->current_receiver_type != NULL)
    {
      return symtab->current_receiver_type;
    }

  return symtab_get_current_receiver_type (symtab->parent);
}
