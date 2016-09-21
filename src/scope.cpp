#include "scope.hpp"

#include "symbol.hpp"
#include "symbol_visitor.hpp"
#include "parameter_list.hpp"
#include "symbol_cast.hpp"
#include "type.hpp"

namespace decl
{

using namespace type;

Scope::Scope ()
  : parent_ (NULL)
  , parameter_list_ (NULL)
  , return_parameter_list_ (NULL)
{ }

Scope::Scope (Scope* p)
  : parent_ (p)
  , parameter_list_ (NULL)
  , return_parameter_list_ (NULL)
{ }

Scope*
Scope::open ()
{
  return new Scope (this);
}

Scope*
Scope::open (const ParameterList* parameter_list,
             const ParameterList* return_parameter_list)
{
  Scope* s = new Scope (this);
  s->enter_parameter_list (parameter_list);
  s->enter_parameter_list (return_parameter_list);
  s->parameter_list_ = parameter_list;
  s->return_parameter_list_ = return_parameter_list;
  return s;
}

Scope*
Scope::open (Symbol* iota,
             const ParameterList* parameter_list,
             const ParameterList* return_parameter_list)
{
  Scope* s = new Scope (this);
  s->enter_symbol (iota);
  s->enter_parameter_list (parameter_list);
  s->enter_parameter_list (return_parameter_list);
  s->parameter_list_ = parameter_list;
  s->return_parameter_list_ = return_parameter_list;
  return s;
}

Scope*
Scope::close ()
{
  Scope* s = this->parent_;
  delete this;
  return s;
}

void Scope::enter_symbol (Symbol* s)
{
  symbols_.push_back (s);
}

Symbol* Scope::find_global_symbol (const std::string& identifier) const
{
  for (SymbolsType::const_iterator pos = symbols_.begin (),
       limit = symbols_.end ();
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
  if (parent_)
    {
      return parent_->find_global_symbol (identifier);
    }
  else
    {
      return NULL;
    }
}

Symbol* Scope::find_local_symbol (const std::string& identifier) const
{
  for (SymbolsType::const_iterator pos = symbols_.begin (),
       limit = symbols_.end ();
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

void Scope::activate ()
{
  for (Scope* s = parent_; s != NULL; s = s->parent_)
    {
      for (SymbolsType::const_iterator ptr = s->symbols_.begin (),
           limit = s->symbols_.end ();
           ptr != limit;
           ++ptr)
        {
          // Find because it may be hidden.
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

void Scope::change ()
{
  Scope* s;
  for (s = parent_; s != NULL; s = s->parent_)
    {
      for (SymbolsType::const_iterator ptr = s->symbols_.begin (),
           limit = s->symbols_.end ();
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

const ParameterList*
Scope::return_parameter_list () const
{
  const Scope* s = this;
  while (s != NULL && s->return_parameter_list_ == NULL)
    {
      s = s->parent_;
    }
  if (s != NULL)
    {
      return s->return_parameter_list_;
    }
  else
    {
      return NULL;
    }
}

Package*
Scope::package () const
{
  return NULL;
}

void
Scope::enter_parameter_list (const ParameterList* type)
{
  for (ParameterList::ParametersType::const_iterator pos = type->begin (), limit = type->end ();
       pos != limit; ++pos)
    {
      enter_symbol (*pos);
    }
}

}
