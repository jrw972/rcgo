#include "symbol_table.hpp"

#include <error.h>

#include "symbol.hpp"
#include "symbol_visitor.hpp"
#include "parameter_list.hpp"

namespace decl
{

using namespace type;

struct SymbolTable::Scope
{

  typedef std::vector<Symbol*> SymbolsType;

  Scope (Scope* p = NULL) :
    parent_ (p)
  { }

  void enter_symbol (Symbol* s)
  {
    symbols_.push_back (s);
  }

  Symbol* find_global_symbol (const std::string& identifier) const
  {
    for (SymbolsType::const_iterator pos = symbols_.begin (),
         limit = symbols_.end ();
         pos != limit;
         ++pos)
      {
        Symbol *s = *pos;
        if (identifier == s->identifier)
          {
            return s;
          }
      }

    /* Not found in this scope.  Try the parent. */
    if (parent_)
      {
        return parent_->find_global_symbol (identifier);
      }
    else
      {
        return NULL;
      }
  }

  Symbol* find_local_symbol (const std::string& identifier) const
  {
    for (SymbolsType::const_iterator pos = symbols_.begin (),
         limit = symbols_.end ();
         pos != limit;
         ++pos)
      {
        Symbol *s = *pos;
        if (identifier == s->identifier)
          {
            return s;
          }
      }

    return NULL;
  }

  void activate ()
  {
    // Remove all parameters containing pointers to avoid a leak.
    Scope* s;
    for (s = parent_; s != NULL; s = s->parent_)
      {
        for (SymbolsType::const_iterator ptr = s->symbols_.begin (),
             limit = s->symbols_.end ();
             ptr != limit;
             ++ptr)
          {
            {
              ParameterSymbol* symbol = SymbolCast<ParameterSymbol> (*ptr);
              if (symbol != NULL)
                {
                  if (symbol->kind == ParameterSymbol::Receiver)
                    {
                      enter_symbol (symbol->duplicate (Mutable));
                    }
                  else
                    {
                      if (type_contains_pointer (symbol->type) && symbol->dereference_mutability == Foreign)
                        {
                          // Hide this parameter.
                          enter_symbol (new HiddenSymbol (symbol, symbol->location));
                        }
                    }
                }
            }

            {
              const VariableSymbol* symbol = SymbolCast<VariableSymbol> (*ptr);
              if (symbol != NULL)
                {
                  if (type_contains_pointer (symbol->type) && symbol->dereference_mutability == Foreign)
                    {
                      // Hide this variable.
                      enter_symbol (new HiddenSymbol (symbol, symbol->location));
                    }
                }
            }

          }
      }
  }

  void change ()
  {
    Scope* s;
    for (s = parent_; s != NULL; s = s->parent_)
      {
        for (SymbolsType::const_iterator ptr = s->symbols_.begin (),
             limit = s->symbols_.end ();
             ptr != limit;
             ++ptr)
          {
            {
              ParameterSymbol* symbol = SymbolCast<ParameterSymbol> (*ptr);
              if (symbol != NULL)
                {
                  if (type_contains_pointer (symbol->type))
                    {
                      // Enter as a duplicate.
                      Symbol* dup = symbol->duplicate (Foreign);
                      enter_symbol (dup);
                    }
                }
            }

            {
              VariableSymbol* symbol = SymbolCast<VariableSymbol> (*ptr);
              if (symbol != NULL)
                {
                  if (type_contains_pointer (symbol->type))
                    {
                      // Enter as a duplicate.
                      Symbol* dup = symbol->duplicate ();
                      enter_symbol (dup);
                    }
                }
            }
          }
      }
  }

  Scope* parent_;
  SymbolsType symbols_;
};

void
SymbolTable::open_scope ()
{
  current_scope_ = new Scope (current_scope_);
}

void
SymbolTable::close_scope ()
{
  Scope* s = current_scope_;
  current_scope_ = s->parent_;
  delete s;
}

void
SymbolTable::enter_symbol (Symbol* symbol)
{
  // Check if the symbol is defined locally.
  const std::string& identifier = symbol->identifier;
  Symbol *s = find_local_symbol (identifier);
  if (s == NULL)
    {
      current_scope_->enter_symbol (symbol);
    }
  else
    {
      error_at_line (-1, 0, symbol->location.File.c_str (), symbol->location.Line,
                     "%s is already defined in this scope (E113)", identifier.c_str ());
    }
}

void
SymbolTable::enter_signature (const ParameterList* type)
{
  for (ParameterList::ParametersType::const_iterator pos = type->begin (), limit = type->end ();
       pos != limit; ++pos)
    {
      ParameterSymbol* x = *pos;
      // Check if the symbol is defined locally.
      const std::string& identifier = x->identifier;
      Symbol* s = find_local_symbol (identifier);
      if (s == NULL)
        {
          current_scope_->enter_symbol (x);
        }
      else
        {
          error_at_line (-1, 0, x->location.File.c_str (), x->location.Line,
                         "%s is already defined in this scope (E55)",
                         identifier.c_str ());
        }
    }
}

void
SymbolTable::change ()
{
  current_scope_->change ();
}

void
SymbolTable::activate ()
{
  current_scope_->activate ();
}

Symbol*
SymbolTable::find_global_symbol (const std::string& identifier) const
{
  return current_scope_->find_global_symbol (identifier);
}

Symbol*
SymbolTable::find_local_symbol (const std::string& identifier) const
{
  return current_scope_->find_local_symbol (identifier);
}

}
