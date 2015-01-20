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
    VECTOR_DECL (symbols, symbol_t *);
  action_t *current_action;
  method_t *current_method;
  trigger_t *current_trigger;
  type_t *current_receiver_type;
};

void symtab_print (const symtab_t* symtab)
{
  VECTOR_FOREACH (ptr, limit, symtab->symbols, symbol_t*)
    {
      const symbol_t* symbol = *ptr;
      const char* name = get (symbol_identifier (symbol));
      const char* kind = symbol_kind_string (symbol_kind (symbol));
      const type_t* type;
      size_t offset = symbol_get_offset (symbol);
      switch (symbol_kind (symbol))
        {
        case SymbolInstance:
          type = symbol_get_instance_type (symbol);
          break;
        case SymbolParameter:
          type = symbol_parameter_type (symbol);
          break;
        case SymbolType:
          type = symbol_get_type_type (symbol);
          break;
        case SymbolTypedConstant:
          type = symbol_typed_constant_value (symbol).type;
          break;
        case SymbolVariable:
          type = symbol_variable_type (symbol);
          break;
        }

      printf ("%s\t%s\t%s\t%zd\n", name, kind, type_to_string (type), offset);
    }
}

symtab_t *
symtab_make (symtab_t * parent)
{
  symtab_t *s = xmalloc (sizeof (symtab_t));
  s->parent = parent;
  VECTOR_INIT (s->symbols, symbol_t *, 0, NULL);
  return s;
}

const symtab_t *
symtab_parent (symtab_t * symtab)
{
  return symtab->parent;
}

symtab_t *
symtab_get_root (symtab_t * symtab)
{
  for (; symtab->parent != NULL; symtab = symtab->parent)
    ;;
  return symtab;
}

void
symtab_enter (symtab_t * symtab, symbol_t * symbol)
{
  VECTOR_PUSH (symtab->symbols, symbol_t *, symbol);
}

symbol_t *
symtab_find (const symtab_t * symtab, string_t identifier)
{
  if (symtab == NULL)
    {
      return NULL;
    }

  VECTOR_FOREACH (pos, limit, symtab->symbols, symbol_t *)
  {
    symbol_t *s = *pos;
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
  VECTOR_FOREACH (pos, limit, symtab->symbols, symbol_t *)
  {
    symbol_t *s = *pos;
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

  VECTOR_FOREACH (pos, limit, symtab->symbols, symbol_t *)
  {
    symbol_t *s = *pos;
    if (symbol_kind (s) == SymbolParameter
	&& symbol_parameter_kind (s) == ParameterReceiver)
      {
	return s;
      }
  }

  return symtab_get_this (symtab->parent);
}

const type_t *
symtab_get_this_type (const symtab_t * symtab)
{
  return symbol_parameter_type (symtab_get_this (symtab));
}

void
symtab_set_current_action (symtab_t * symtab, action_t * action)
{
  symtab->current_action = action;
}

action_t *
symtab_get_current_action (const symtab_t * symtab)
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

void
symtab_set_current_method (symtab_t * symtab, method_t * method)
{
  symtab->current_method = method;
}

method_t *
symtab_get_current_method (const symtab_t * symtab)
{
  if (symtab == NULL)
    {
      return NULL;
    }

  if (symtab->current_method != NULL)
    {
      return symtab->current_method;
    }

  return symtab_get_current_method (symtab->parent);
}

void
symtab_set_current_trigger (symtab_t * symtab, trigger_t * trigger)
{
  symtab->current_trigger = trigger;
}

trigger_t *
symtab_get_current_trigger (const symtab_t * symtab)
{
  if (symtab == NULL)
    {
      return NULL;
    }

  if (symtab->current_trigger != NULL)
    {
      return symtab->current_trigger;
    }

  return symtab_get_current_trigger (symtab->parent);
}

void
symtab_set_current_receiver_type (symtab_t * symtab, type_t * type)
{
  symtab->current_receiver_type = type;
}

type_t *
symtab_get_current_receiver_type (const symtab_t * symtab)
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

symbol_t* symtab_get_first_return_parameter (const symtab_t * symtab)
{
  if (symtab == NULL)
    {
      return NULL;
    }

  if (!VECTOR_EMPTY (symtab->symbols))
    {
      symbol_t* symbol = VECTOR_AT (symtab->symbols, 0);
      if (symbol_kind (symbol) == SymbolParameter &&
          symbol_parameter_kind (symbol) == ParameterReturn)
        {
          return symbol;
        }
    }

  return symtab_get_first_return_parameter (symtab->parent);
}

symbol_t** symtab_begin (const symtab_t* symtab)
{
  return VECTOR_BEGIN (symtab->symbols);
}

symbol_t** symtab_end (const symtab_t* symtab)
{
  return VECTOR_END (symtab->symbols);
}

symbol_t** symtab_next (symbol_t** pos)
{
  return VECTOR_NEXT (pos);
}

void symtab_change (symtab_t* symtab)
{
  symtab_t* s;
  for (s = symtab->parent; s != NULL; s = s->parent)
    {
      VECTOR_FOREACH (ptr, limit, s->symbols, symbol_t*)
        {
          symbol_t* symbol = *ptr;
          if (symbol_kind (symbol) == SymbolParameter)
            {
              type_t* type = symbol_parameter_type (symbol);
              if (type_is_pointer (type))
                {
                  type_t* base_type = type_pointer_base_type (type);
                  if (!type_is_foreign (base_type))
                    {
                      // Strip immutability.
                      if (type_is_immutable (base_type))
                        {
                          base_type = type_immutable_base_type (base_type);
                        }
                      // Make a foreign pointer.
                      base_type = type_make_foreign (base_type);
                      type = type_make_pointer (base_type);
                      // Enter as a duplicate.
                      symbol_t* dup = symbol_make_parameter_duplicate (symbol, type);
                      symtab_enter (symtab, dup);
                    }
                }
            }
          else if (symbol_kind (symbol) == SymbolVariable)
            {
              type_t* type = symbol_variable_type (symbol);
              if (type_is_pointer (type))
                {
                  type_t* base_type = type_pointer_base_type (type);
                  if (!type_is_foreign (base_type))
                    {
                      // Strip immutability.
                      if (type_is_immutable (base_type))
                        {
                          base_type = type_immutable_base_type (base_type);
                        }
                      // Make a foreign pointer.
                      base_type = type_make_foreign (base_type);
                      type = type_make_pointer (base_type);
                      // Enter as a duplicate.
                      symbol_t* dup = symbol_make_variable_duplicate (symbol, type);
                      symtab_enter (symtab, dup);
                    }
                }
            }
        }
    }
}
