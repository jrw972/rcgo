#include "package.hpp"

#include "semantic.hpp"
#include "source_file.hpp"
#include "symbol_table.hpp"

namespace source
{
void
Package::add_source_file (SourceFile* source_file)
{
  source_files_.push_back (source_file);
}

Package::const_source_files_iterator
Package::source_files_begin () const
{
  return source_files_.begin ();
}

Package::const_source_files_iterator
Package::source_files_end () const
{
  return source_files_.end ();
}

void
Package::add_dependency (Package* package)
{
  dependencies_.insert (package);
}

Package::const_adjacency_iterator
Package::adjacency_begin () const
{
  return dependencies_.begin ();
}

Package::const_adjacency_iterator
Package::adjacency_end () const
{
  return dependencies_.end ();
}

void
Package::determine_package_name (util::ErrorReporter& er)
{
  bool package_set = false;

  for (const_source_files_iterator sfpos = source_files_begin (),
       sflimit = source_files_end ();
       sfpos != sflimit;
       ++sfpos)
    {
      SourceFile* source_file = *sfpos;
      if (!package_set)
        {
          package_name_ = source_file->package_name->identifier ();
          package_set = true;
        }
      else if (package_name_ != source_file->package_name->identifier ())
        {
          // Error.  Inconsistent package name.
          UNIMPLEMENTED;
        }
    }
}

void
Package::process_symbols (util::ErrorReporter& er,
                          decl::SymbolTable& symbol_table)
{
  // Package block.
  symbol_table.open_scope ();

  for (const_source_files_iterator pos = source_files_begin (),
       limit = source_files_end ();
       pos != limit;
       ++pos)
    {
      (*pos)->generate_package_symbols (er, symbol_table);
    }

  // Collect symbols.
  for (decl::SymbolTable::const_symbols_iterator pos = symbol_table.symbols_begin (),
       limit = symbol_table.symbols_end ();
       pos != limit;
       ++pos)
    {
      symbols_.push_back (pos->second);
    }

  for (const_source_files_iterator pos = source_files_begin (),
       limit = source_files_end ();
       pos != limit;
       ++pos)
    {
      (*pos)->process_symbols (er, symbol_table);
    }

  // Build a digraph of which symbols depend on other symbols.
  UNIMPLEMENTED;

  symbol_table.close_scope ();
}

const std::string&
Package::package_name () const
{
  return package_name_;
}

Package::const_symbols_iterator
Package::symbols_begin () const
{
  return symbols_.begin ();
}

Package::const_symbols_iterator
Package::symbols_end () const
{
  return symbols_.end ();
}

}
