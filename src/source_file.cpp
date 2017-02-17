#include "source_file.hpp"

#include "semantic.hpp"
#include "symbol_table.hpp"
#include "node.hpp"
#include "error_reporter.hpp"
#include "node_cast.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include "callable.hpp"
#include "package.hpp"
#include "decl.hpp"

namespace source
{
  Import::Import (const Location& location,
                  const Identifier& a_package_name,
                  const std::string& a_import_path)
    : location_ (location)
    , package_name_ (a_package_name)
    , import_path_ (a_import_path)
    , package_ (NULL)
{ }

  SourceFile::SourceFile (const std::string& a_path)
    : path_ (a_path)
  { }

  const std::string&
  SourceFile::path () const
  {
    return path_;
  }

  Location
  SourceFile::location (unsigned int line) const
  {
    return Location (this, line);
  }

void
SourceFile::add_import (const Location& location,
                        const Identifier& package_name,
                        const std::string& import_path)
{
  imports_.push_back (Import (location, package_name, import_path));
}

SourceFile::const_imports_iterator
SourceFile::imports_begin () const
{
  return imports_.begin ();
}

SourceFile::const_imports_iterator
SourceFile::imports_end () const
{
  return imports_.end ();
}

SourceFile::imports_iterator
SourceFile::imports_begin ()
{
  return imports_.begin ();
}

SourceFile::imports_iterator
SourceFile::imports_end ()
{
  return imports_.end ();
}

void
SourceFile::generate_package_symbols (util::ErrorReporter& er,
                                      decl::SymbolTable& symbol_table) const
{
  decl::generate_package_symbols (er, symbol_table, top_level_decl_list);
}

void
SourceFile::process_symbols (util::ErrorReporter& er,
                             decl::SymbolTable& symbol_table) const
{
  // File block.
  symbol_table.open_scope ();

  // Create symbols for imported packages.
  //
  // The Go Programming Language Specification is ambiguous regarding
  // imports and the file block.  The spec implies that the file block
  // is inside the package block and that the normal scoping rules
  // apply.  That is, it should be possible to redeclare any symbol
  // declared in a previous scope.  However, the go compiler does not
  // allow this behavior and does not permit an imported symbol to
  // interfere with a previously declared symbol.  By construction,
  // this cannot happen for the universal block since all universal
  // identifiers are not exported and all symbols imported from a
  // package will be exported.  An import statement can either declare
  // a new package symbol or import all exported symbols from a
  // package.  In either case, it seems that the go compiler processes
  // the imports first and then checks if package symbols conflict
  // with the import.
  //
  // I think that checking for conflicts between the package and file
  // block is the correct thing to do as it is hard to construct an
  // argument where it makes sense for imported symbols to override
  // package symbols.
  for (const_imports_iterator pos = imports_.begin (),
       limit = imports_.end ();
       pos != limit;
       ++pos)
    {
      const Import& i = *pos;
      if (i.package_name ().identifier () == "")
        {
          // Use the name of the package.
          Identifier id (i.package ()->package_name (), i.location ());
          if (!symbol_table.check_is_declared (er, id))
            {
              symbol_table.enter_symbol (new decl::Package (id, i.package ()));
            }
        }
      else if (i.package_name ().identifier () == ".")
        {
          // Import all symbols into the file scope.
          for (Package::const_symbols_iterator ppos = i.package ()->symbols_begin (),
                 plimit = i.package ()->symbols_end ();
               ppos != plimit;
               ++pos)
            {
              decl::Symbol* s = *ppos;
              if (s->identifier.is_exported ())
                {
                  Identifier id (s->identifier.identifier (), i.location ());
                  if (!symbol_table.check_is_declared (er, id))
                    {
                      symbol_table.enter_symbol (new decl::ImportedSymbol (id, s));
                    }
                }
            }
        }
      else if (i.package_name ().is_blank ())
        {
          // Imported for side-effects.
          // Do nothing.
        }
      else
        {
          // Use the designated name.
          if (!symbol_table.check_is_declared (er, i.package_name ()))
            {
              symbol_table.enter_symbol (new decl::Package (i.package_name (), i.package ()));
            }
        }
    }

  generate_and_lookup_symbols (er, symbol_table, top_level_decl_list);

  // // Enter method-like identifiers into the named types.
  // semantic::enter_method_identifiers (top_level_decl_list, er, symbol_table);

  symbol_table.close_scope ();

  UNIMPLEMENTED;
}

  const SourceFile SourceFile::builtin ("<builtin>");


}
