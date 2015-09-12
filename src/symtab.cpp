#include "symtab.hpp"
#include <stdlib.h>
#include <string.h>
#include "debug.hpp"
#include "util.hpp"
#include "type.hpp"
#include "Symbol.hpp"
#include <vector>
#include "action.hpp"

std::ostream& symtab_t::print (std::ostream& o) const
{
  o << "Symtab " << this << '\n';
  for (std::vector<Symbol*>::const_iterator ptr = symbols_.begin (),
         limit = symbols_.end ();
       ptr != limit;
       ++ptr)
    {
      const Symbol* symbol = *ptr;
      const std::string& name = symbol->identifier;
      const char* kind = symbol->kindString ();
      size_t offset = symbol->offset ();

      struct visitor : public ConstSymbolVisitor {
        std::string type_str;
        std::ostream& o;

        visitor (std::ostream& o_) : type_str ("(none)"), o (o_) { }

        void defaultAction (const Symbol& symbol) {
          not_reached;
        }

        void visit (const Function& symbol) {
          typed_value_t tv = symbol.value ();
          if (tv.type != NULL) {
            type_str = tv.type->to_string ();
          }
        }

        void visit (const InstanceSymbol& symbol) {
          if (symbol.type != NULL) {
            type_str = symbol.type->to_string ();
          }
        }

        void visit (const ParameterSymbol& symbol) {
          type_str = symbol.value.type->to_string ();
          switch (symbol.kind)
            {
            case ParameterSymbol::Ordinary:
              o << "Ordinary\n";
              break;
            case ParameterSymbol::Receiver:
              o << "Receiver\n";
              break;
            case ParameterSymbol::ReceiverDuplicate:
              o << "ReceiverDuplicate\n";
              break;
            case ParameterSymbol::Return:
              o << "Return\n";
              break;
            case ParameterSymbol::OrdinaryDuplicate:
              o << "OrdinaryDuplicate\n";
              break;
            }
        }

        void visit (const TypeSymbol& symbol) {
          type_str = symbol.type->to_string ();
        }

        void visit (const TypedConstantSymbol& symbol) {
          type_str = symbol.value.type->to_string ();
        }

        void visit (const VariableSymbol& symbol) {
          type_str = symbol.value.type->to_string ();
        }

        void visit (const HiddenSymbol& symbol) {
          // Do nothing.
        }
      };

      visitor v (o);
      symbol->accept (v);

      o << name << '\t' << kind << '\t' << v.type_str.c_str () << '\t' << offset << '\n';
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
  return SymbolCast<ParameterSymbol> (symtab->get_this ())->value.type;
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
symtab_set_current_function (symtab_t * symtab, Function* function)
{
  symtab->current_function = function;
}

Function*
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
symtab_set_current_method (symtab_t * symtab, Method* method)
{
  symtab->current_method = method;
}

Method*
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
symtab_set_current_initializer (symtab_t * symtab, Initializer* initializer)
{
  symtab->current_initializer = initializer;
}

Initializer*
symtab_get_current_initializer (const symtab_t * symtab)
{
  if (symtab == NULL)
    {
      return NULL;
    }

  if (symtab->current_initializer != NULL)
    {
      return symtab->current_initializer;
    }

  return symtab_get_current_initializer (symtab->parent);
}

void
symtab_set_current_getter (symtab_t * symtab, Getter* getter)
{
  symtab->current_getter = getter;
}

Getter*
symtab_get_current_getter (const symtab_t * symtab)
{
  if (symtab == NULL)
    {
      return NULL;
    }

  if (symtab->current_getter != NULL)
    {
      return symtab->current_getter;
    }

  return symtab_get_current_getter (symtab->parent);
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
  Symbol *this_symbol = get_this ();
  Symbol *new_this_symbol = SymbolCast<ParameterSymbol> (this_symbol)->duplicate ();
  enter (new_this_symbol);
  holder.symbol (new_this_symbol);

  // Remove all parameters containing pointers to avoid a leak.
  symtab_t* s;
  for (s = parent; s != NULL; s = s->parent)
    {
      for (std::vector<Symbol*>::const_iterator ptr = s->symbols_.begin (),
             limit = s->symbols_.end ();
           ptr != limit;
           ++ptr)
        {
          {
            const ParameterSymbol* symbol = SymbolCast<ParameterSymbol> (*ptr);
            if (symbol != NULL && symbol != this_symbol) {
              typed_value_t tv = symbol->value;
              if (type_contains_pointer (tv.type) && tv.dereference_mutability == FOREIGN) {
                // Hide this parameter.
                enter (new HiddenSymbol (symbol, defining_node));
              }
            }
          }

          {
            const VariableSymbol* symbol = SymbolCast<VariableSymbol> (*ptr);
            if (symbol != NULL) {
              typed_value_t tv = symbol->value;
              if (type_contains_pointer (tv.type) && tv.dereference_mutability == FOREIGN) {
                // Hide this variable.
                enter (new HiddenSymbol (symbol, defining_node));
              }
            }
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
      for (std::vector<Symbol*>::const_iterator ptr = s->symbols_.begin (),
             limit = s->symbols_.end ();
           ptr != limit;
           ++ptr)
        {
          {
            ParameterSymbol* symbol = SymbolCast<ParameterSymbol> (*ptr);
            if (symbol != NULL) {
              typed_value_t tv = SymbolCast<ParameterSymbol> (symbol)->value;
              if (type_contains_pointer (tv.type)) {
                // Enter as a duplicate.
                Symbol* dup = SymbolCast<ParameterSymbol> (symbol)->duplicate ();
                enter (dup);
              }
            }
          }

          {
            VariableSymbol* symbol = SymbolCast<VariableSymbol> (*ptr);
            if (symbol != NULL) {
              typed_value_t tv = SymbolCast<VariableSymbol> (symbol)->value;
              if (type_contains_pointer (tv.type)) {
                // Enter as a duplicate.
                Symbol* dup = SymbolCast<VariableSymbol> (symbol)->duplicate ();
                enter (dup);
              }
            }
          }
        }
    }
}
