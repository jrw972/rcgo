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

std::ostream& symtab_t::print (std::ostream& o) const
{
  o << "Symtab " << this << '\n';
  for (std::vector<symbol_t*>::const_iterator ptr = symbols_.begin (),
         limit = symbols_.end ();
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
                type_str = function->function_type->to_string ();
              }
          }
          break;
        case SymbolInstance:
          if (symbol_get_instance_type (symbol))
            {
              type_str = symbol_get_instance_type (symbol)->to_string ();
            }
          break;
        case SymbolParameter:
          type_str = symbol_parameter_type (symbol)->to_string ();
          switch (symbol_parameter_kind (symbol))
            {
            case ParameterOrdinary:
              o << "Ordinary\n";
              break;
            case ParameterReceiver:
              o << "Receiver\n";
              break;
            case ParameterReceiverDuplicate:
              o << "ReceiverDuplicate\n";
              break;
            case ParameterReturn:
              o << "Return\n";
              break;
            case ParameterDuplicate:
              o << "Duplicate\n";
              break;
            }
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
        case SymbolHidden:
          break;
        }

      std::cout << name << '\t' << kind << '\t' << type_str.c_str () << '\t' << offset << '\n';
    }

  if (parent)
    {
      parent->print (o);
    }

  return o;
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

const type_t *
symtab_get_this_type (const symtab_t * symtab)
{
  return symbol_parameter_type (symtab->get_this ());
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

void
symtab_t::trigger (symbol_holder& holder, ast_t* defining_node)
{
  symbol_t *this_symbol = get_this ();
  symbol_t *new_this_symbol =
    symbol_make_receiver_duplicate (this_symbol);
  enter (new_this_symbol);
  holder.symbol (new_this_symbol);

  // Remove all parameters containing pointers to avoid a leak.
  symtab_t* s;
  for (s = parent; s != NULL; s = s->parent)
    {
      for (std::vector<symbol_t*>::const_iterator ptr = s->symbols_.begin (),
             limit = s->symbols_.end ();
           ptr != limit;
           ++ptr)
        {
          const symbol_t* symbol = *ptr;

          switch (symbol_kind (symbol))
            {
            case SymbolFunction:
              // Do nothing.
              break;
            case SymbolInstance:
              // Do nothing.
              break;
            case SymbolParameter:
              {
                if (symbol != this_symbol)
                  {
                    typed_value_t tv = symbol_parameter_value (symbol);
                    if (type_contains_pointer (tv.type) && tv.dereference_mutability == FOREIGN)
                      {
                        // Hide this parameter.
                        enter (symbol_make_hidden (symbol, defining_node));
                      }
                  }
              }
              break;
              unimplemented;
            case SymbolType:
              // Do nothing.
              break;
            case SymbolTypedConstant:
              // Do nothing.
              break;
            case SymbolVariable:
              {
                typed_value_t tv = symbol_variable_value (symbol);
                if (type_contains_pointer (tv.type) && tv.dereference_mutability == FOREIGN)
                  {
                    // Hide this variable.
                    enter (symbol_make_hidden (symbol, defining_node));
                  }
              }
              break;
            case SymbolHidden:
              unimplemented;
            }
        }
    }
}

void
symtab_t::change ()
{
  symtab_t* s;
  for (s = parent; s != NULL; s = s->parent)
    {
      for (std::vector<symbol_t*>::const_iterator ptr = s->symbols_.begin (),
             limit = s->symbols_.end ();
           ptr != limit;
           ++ptr)
        {
          symbol_t* symbol = *ptr;
          if (symbol_kind (symbol) == SymbolParameter)
            {
              typed_value_t tv = symbol_parameter_value (symbol);
              if (type_contains_pointer (tv.type))
                {
                  // Enter as a duplicate.
                  symbol_t* dup = symbol_make_parameter_duplicate (symbol);
                  enter (dup);
                }
            }
          else if (symbol_kind (symbol) == SymbolVariable)
            {
              typed_value_t tv = symbol_variable_value (symbol);
              if (type_contains_pointer (tv.type))
                {
                  // Enter as a duplicate.
                  symbol_t* dup = symbol_make_variable_duplicate (symbol);
                  enter (dup);
                }
            }
        }
    }
}
