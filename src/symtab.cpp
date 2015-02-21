#include "symtab.hpp"
#include <stdlib.h>
#include "strtab.hpp"
#include <string.h>
#include "debug.hpp"
#include "util.hpp"
#include "type.hpp"
#include "symbol.hpp"
#include "function.hpp"
#include <vector>
#include "action.hpp"

void symtab_print (const symtab_t* symtab)
{
  if (symtab != NULL)
    {
      printf ("Symtab %p\n", symtab);
      for (std::vector<symbol_t*>::const_iterator ptr = symtab->symbols.begin (),
             limit = symtab->symbols.end ();
           ptr != limit;
           ++ptr)
        {
          const symbol_t* symbol = *ptr;
          const char* name = get (symbol_identifier (symbol));
          const char* kind = symbol_kind_string (symbol_kind (symbol));
          std::string type_str = "(none)";
          size_t offset = symbol_get_offset (symbol);
          switch (symbol_kind (symbol))
            {
            case SymbolFunction:
              {
                function_t* function = symbol_get_function_function (symbol);
                if (function)
                  {
                    const type_t* type = function_type (function);
                    if (type)
                      {
                        type_str = type->to_string ();
                      }
                  }
              }
              break;
            case SymbolInstance:
              type_str = symbol_get_instance_type (symbol)->to_string ();
              break;
            case SymbolParameter:
              type_str = symbol_parameter_type (symbol)->to_string ();
              break;
            case SymbolType:
              type_str = symbol_get_type_type (symbol)->to_string ();
              break;
            case SymbolTypedConstant:
              type_str = symbol_typed_constant_value (symbol).type->to_string ();
              break;
            case SymbolVariable:
              type_str = symbol_variable_type (symbol)->to_string ();
              break;
            }

          printf ("%s\t%s\t%s\t%zd\n", name, kind, type_str.c_str (), offset);
        }

      symtab_print (symtab->parent);
    }
}

symtab_t *
symtab_make (symtab_t * parent)
{
  symtab_t *s = new symtab_t;
  s->parent = parent;
  return s;
}

symtab_t *
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
  symtab->symbols.push_back (symbol);
}

symbol_t *
symtab_find (const symtab_t * symtab, string_t identifier)
{
  if (symtab == NULL)
    {
      return NULL;
    }

  for (std::vector<symbol_t*>::const_iterator pos = symtab->symbols.begin (),
         limit = symtab->symbols.end ();
       pos != limit;
       ++pos)
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
  for (std::vector<symbol_t*>::const_iterator pos = symtab->symbols.begin (),
         limit = symtab->symbols.end ();
       pos != limit;
       ++pos)
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

  for (std::vector<symbol_t*>::const_iterator pos = symtab->symbols.begin (),
         limit = symtab->symbols.end ();
       pos != limit;
       ++pos)
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
symtab_set_current_action (symtab_t * symtab, action_reaction_base_t * action)
{
  symtab->current_action = action;
}

action_reaction_base_t *
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
symtab_set_current_function (symtab_t * symtab, function_t * function)
{
  symtab->current_function = function;
}

function_t *
symtab_get_current_function (const symtab_t * symtab)
{
  if (symtab == NULL)
    {
      return NULL;
    }

  if (symtab->current_function != NULL)
    {
      return symtab->current_function;
    }

  return symtab_get_current_function (symtab->parent);
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
symtab_set_current_receiver_type (symtab_t * symtab, named_type_t * type)
{
  symtab->current_receiver_type = type;
}

named_type_t *
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

  if (!symtab->symbols.empty ())
    {
      symbol_t* symbol = symtab->symbols.at (0);
      if (symbol_kind (symbol) == SymbolParameter &&
          symbol_parameter_kind (symbol) == ParameterReturn)
        {
          return symbol;
        }
    }

  return symtab_get_first_return_parameter (symtab->parent);
}

void symtab_change (symtab_t* symtab)
{
  symtab_t* s;
  for (s = symtab->parent; s != NULL; s = s->parent)
    {
      for (std::vector<symbol_t*>::const_iterator ptr = s->symbols.begin (),
             limit = s->symbols.end ();
           ptr != limit;
           ++ptr)
        {
          symbol_t* symbol = *ptr;
          if (symbol_kind (symbol) == SymbolParameter)
            {
              const type_t* type = symbol_parameter_type (symbol);
              if (type_cast<pointer_type_t> (type))
                {
                  unimplemented;
                  // type_t* base_type = type_pointer_base_type (type);
                  // if (type_to_foreign (base_type) == NULL)
                  //   {
                  //     // Strip immutability.
                  //     if (type_is_immutable (base_type))
                  //       {
                  //         unimplemented;
                  //         //base_type = type_immutable_base_type (base_type);
                  //       }
                  //     // Make a foreign pointer.
                  //     base_type = type_make_foreign (base_type);
                  //     type = type_make_pointer (base_type);
                  //     // Enter as a duplicate.
                  //     symbol_t* dup = symbol_make_parameter_duplicate (symbol, type);
                  //     symtab_enter (symtab, dup);
                  //   }
                }
            }
          else if (symbol_kind (symbol) == SymbolVariable)
            {
              const type_t* base_type = type_dereference (symbol_variable_type (symbol));
              if (base_type != NULL)
                {
                  // Make a foreign pointer.
                  const type_t* t = pointer_to_foreign_type_t::make (base_type);
                  // Enter as a duplicate.
                  symbol_t* dup = symbol_make_variable_duplicate (symbol, t);
                  symtab_enter (symtab, dup);
                }
            }
        }
    }
}
