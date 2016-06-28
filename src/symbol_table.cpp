#include "symbol_table.hpp"

#include "symbol.hpp"
#include "symbol_visitor.hpp"
#include "parameter_list.hpp"
#include "symbol_cast.hpp"
#include "type.hpp"

namespace decl
{

using namespace type;

struct SymbolTable::Scope
{

  typedef std::vector<Symbol*> SymbolsType;

  Scope (Scope* p = NULL)
    : parent (p)
    , parameter_list (NULL)
    , return_parameter_list (NULL)
  { }

  void enter_symbol (Symbol* s)
  {
    symbols.push_back (s);
  }

  Symbol* find_global_symbol (const std::string& identifier) const
  {
    for (SymbolsType::const_iterator pos = symbols.begin (),
         limit = symbols.end ();
         pos != limit;
         ++pos)
      {
        Symbol *s = *pos;
        if (identifier == s->name)
          {
            return s;
          }
      }

    /* Not found in this scope.  Try the parent. */
    if (parent)
      {
        return parent->find_global_symbol (identifier);
      }
    else
      {
        return NULL;
      }
  }

  Symbol* find_local_symbol (const std::string& identifier) const
  {
    for (SymbolsType::const_iterator pos = symbols.begin (),
         limit = symbols.end ();
         pos != limit;
         ++pos)
      {
        Symbol *s = *pos;
        if (identifier == s->name)
          {
            return s;
          }
      }

    return NULL;
  }

  void activate ()
  {
    Scope* s;
    for (s = parent; s != NULL; s = s->parent)
      {
        for (SymbolsType::const_iterator ptr = s->symbols.begin (),
             limit = s->symbols.end ();
             ptr != limit;
             ++ptr)
          {
            Symbol* x = find_global_symbol ((*ptr)->name);
            if (x != NULL && symbol_cast<Hidden> (x) != NULL)
              {
                // Leave hidden symbols hidden.
                continue;
              }

            {
              Parameter* symbol = symbol_cast<Parameter> (*ptr);
              if (symbol != NULL)
                {
                  if (symbol->kind == Parameter::Receiver)
                    {
                      // Change receiver to have mutable indirection mutability.
                      enter_symbol (symbol->duplicate (Mutable));
                    }
                  else
                    {
                      // Hide all parameters containing pointers.
                      if (symbol->type->contains_pointer ())
                        {
                          assert (symbol->is_foreign_safe ());
                          // Hide this parameter.
                          enter_symbol (new Hidden (symbol, symbol->location));
                        }
                    }
                }
            }

            {
              const Variable* symbol = symbol_cast<Variable> (*ptr);
              if (symbol != NULL)
                {
                  // Hide all variables that may point to foreign data.
                  if (symbol->type->contains_pointer ()
                      && symbol->indirection_mutability == Foreign)
                    {
                      enter_symbol (new Hidden (symbol, symbol->location));
                    }
                }
            }

          }
      }
  }

  void change ()
  {
    Scope* s;
    for (s = parent; s != NULL; s = s->parent)
      {
        for (SymbolsType::const_iterator ptr = s->symbols.begin (),
             limit = s->symbols.end ();
             ptr != limit;
             ++ptr)
          {
            Symbol* x = find_global_symbol ((*ptr)->name);
            if (x != NULL && symbol_cast<Hidden> (x) != NULL)
              {
                // Leave hidden symbols hidden.
                continue;
              }

            {
              Parameter* symbol = symbol_cast<Parameter> (*ptr);
              if (symbol != NULL)
                {
                  if (symbol->type->contains_pointer ())
                    {
                      // Enter as a duplicate.
                      Symbol* dup = symbol->duplicate (Foreign);
                      enter_symbol (dup);
                    }
                }
            }

            {
              Variable* symbol = symbol_cast<Variable> (*ptr);
              if (symbol != NULL)
                {
                  if (symbol->type->contains_pointer ())
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

  Scope* parent;
  SymbolsType symbols;
  const ParameterList* parameter_list;
  const ParameterList* return_parameter_list;
};

SymbolTable::SymbolTable ()
  : current_scope_ (NULL)
{ }

void
SymbolTable::open_scope ()
{
  current_scope_ = new Scope (current_scope_);
}

void
SymbolTable::open_scope (const ParameterList* parameter_list,
                         const ParameterList* return_parameter_list)
{
  open_scope ();
  enter_parameter_list (parameter_list);
  enter_parameter_list (return_parameter_list);
  current_scope_->parameter_list = parameter_list;
  current_scope_->return_parameter_list = return_parameter_list;
}

void
SymbolTable::open_scope (Symbol* iota,
                         const ParameterList* parameter_list,
                         const ParameterList* return_parameter_list)
{
  open_scope ();
  enter_symbol (iota);
  enter_parameter_list (parameter_list);
  enter_parameter_list (return_parameter_list);
  current_scope_->parameter_list = parameter_list;
  current_scope_->return_parameter_list = return_parameter_list;
}


void
SymbolTable::close_scope ()
{
  Scope* s = current_scope_;
  current_scope_ = s->parent;
  delete s;
}

void
SymbolTable::enter_symbol (Symbol* symbol)
{
  current_scope_->enter_symbol (symbol);
}

void
SymbolTable::enter_parameter_list (const ParameterList* type)
{
  for (ParameterList::ParametersType::const_iterator pos = type->begin (), limit = type->end ();
       pos != limit; ++pos)
    {
      current_scope_->enter_symbol (*pos);
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

Package*
SymbolTable::package () const
{
  return NULL;
}

const ParameterList*
SymbolTable::return_parameter_list () const
{
  Scope* s = current_scope_;
  while (s != NULL && s->return_parameter_list == NULL)
    {
      s = s->parent;
    }
  if (s != NULL)
    {
      return s->return_parameter_list;
    }
  else
    {
      return NULL;
    }
}

}
