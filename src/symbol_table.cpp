#include "symbol_table.hpp"
#include "symbol.hpp"
#include "symbol_cast.hpp"
#include "type.hpp"
#include "parameter_list.hpp"
#include "identifier.hpp"
#include "error_reporter.hpp"

namespace decl
{

void
SymbolTable::open_scope ()
{
  scopes_.push_back (Scope ());
}

void
SymbolTable::open_scope (const ParameterList* parameter_list,
                         const ParameterList* return_parameter_list)
{
  scopes_.push_back (Scope ());
  Scope& s = scopes_.back ();
  s.enter_parameter_list (parameter_list);
  s.enter_parameter_list (return_parameter_list);
  s.parameter_list_ = parameter_list;
  s.return_parameter_list_ = return_parameter_list;
}

void
SymbolTable::open_scope (Symbol* iota,
                         const ParameterList* parameter_list,
                         const ParameterList* return_parameter_list)
{
  scopes_.push_back (Scope ());
  Scope& s = scopes_.back ();
  s.enter_symbol (iota);
  s.enter_parameter_list (parameter_list);
  s.enter_parameter_list (return_parameter_list);
  s.parameter_list_ = parameter_list;
  s.return_parameter_list_ = return_parameter_list;
}

void
SymbolTable::close_scope ()
{
  assert (!scopes_.empty ());
  scopes_.pop_back ();
}

void
SymbolTable::enter_symbol (Symbol* a_symbol)
{
  assert (!scopes_.empty ());
  scopes_.back ().enter_symbol (a_symbol);
}

Symbol*
SymbolTable::retrieve_symbol (const source::Identifier& identifier) const
{
  return retrieve_symbol (identifier.identifier ());
}

Symbol*
SymbolTable::retrieve_symbol (const std::string& identifier) const
{
  for (ScopesType::const_reverse_iterator pos = scopes_.rbegin (),
         limit = scopes_.rend ();
       pos != limit;
       ++pos)
    {
      SymbolsType::const_iterator p = pos->symbols.find (identifier);
      if (p != pos->symbols.end ())
        {
          return p->second;
        }
    }
  return NULL;
}

  bool
  SymbolTable::is_declared_locally (const source::Identifier& identifier) const
{
  return is_declared_locally (identifier.identifier ());
}

bool
SymbolTable::is_declared_locally (const std::string& identifier) const
{
  return scopes_.back ().symbols.find (identifier) != scopes_.back ().symbols.end ();
}

  bool
  SymbolTable::check_is_declared_locally (util::ErrorReporter& er,
                                          const source::Identifier& identifier) const
  {
    if (is_declared_locally (identifier))
      {
        decl::Symbol* s = retrieve_symbol (identifier);
        er.already_declared (identifier.location (), identifier.identifier (), s->identifier.location ());
        return true;
      }
    return false;
  }

  bool
  SymbolTable::check_is_declared (util::ErrorReporter& er,
                                  const source::Identifier& identifier) const
  {
    decl::Symbol* s = retrieve_symbol (identifier);
    if (s != NULL)
      {
        er.already_declared (identifier.location (), identifier.identifier (), s->identifier.location ());
        return true;
      }
    return false;
  }

void
SymbolTable::activate ()
{
  ScopesType::const_reverse_iterator pos = scopes_.rbegin (), limit = scopes_.rend ();
  ++pos;
  for (; pos != limit; ++pos)
    {
      for (SymbolsType::const_iterator ptr = pos->symbols.begin (),
           limit = pos->symbols.end ();
           ptr != limit;
           ++ptr)
        {
          // Find because it may be hidden.
          Symbol* x = retrieve_symbol (ptr->second->identifier);
          if (x != NULL && symbol_cast<Hidden> (x) != NULL)
            {
              // Leave hidden symbols hidden.
              continue;
            }

          {
            Parameter* symbol = symbol_cast<Parameter> (ptr->second);
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
                        enter_symbol (new Hidden (symbol));
                      }
                  }
              }
          }

          {
            const Variable* symbol = symbol_cast<Variable> (ptr->second);
            if (symbol != NULL)
              {
                // Hide all variables that may point to foreign data.
                if (symbol->type->contains_pointer ()
                    && symbol->indirection_mutability == Foreign)
                  {
                    enter_symbol (new Hidden (symbol));
                  }
              }
          }

        }
    }
}

void
SymbolTable::change ()
{
  ScopesType::const_reverse_iterator pos = scopes_.rbegin (), limit = scopes_.rend ();
  ++pos;
  for (; pos != limit; ++pos)
    {
      for (SymbolsType::const_iterator ptr = pos->symbols.begin (),
           limit = pos->symbols.end ();
           ptr != limit;
           ++ptr)
        {
          Symbol* x = retrieve_symbol (ptr->second->identifier);
          if (x != NULL && symbol_cast<Hidden> (x) != NULL)
            {
              // Leave hidden symbols hidden.
              continue;
            }

          {
            Parameter* symbol = symbol_cast<Parameter> (ptr->second);
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
            Variable* symbol = symbol_cast<Variable> (ptr->second);
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
SymbolTable::return_parameter_list () const
{
  for (ScopesType::const_reverse_iterator pos = scopes_.rbegin (), limit = scopes_.rend (); pos != limit; ++pos)
    {
      if (pos->return_parameter_list_ != NULL)
        {
          return pos->return_parameter_list_;
        }
    }
  return NULL;
}

source::Package*
SymbolTable::package () const
{
  return NULL;
}

SymbolTable::const_symbols_iterator
SymbolTable::symbols_begin () const
{
  return scopes_.back ().symbols.begin ();
}

SymbolTable::const_symbols_iterator
SymbolTable::symbols_end () const
{
  return scopes_.back ().symbols.end ();
}

void
SymbolTable::Scope::enter_symbol (Symbol* a_symbol)
{
  symbols[a_symbol->identifier.identifier ()] = a_symbol;
}

void
SymbolTable::Scope::enter_parameter_list (const ParameterList* type)
{
  for (ParameterList::ParametersType::const_iterator pos = type->begin (), limit = type->end ();
       pos != limit; ++pos)
    {
      enter_symbol (*pos);
    }
}

}
