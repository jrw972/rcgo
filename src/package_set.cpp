#include "package_set.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <error.h>

#include "package.hpp"
#include "location.hpp"
#include "yyparse.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include "source_file.hpp"
#include "symbol_table.hpp"
#include "decl.hpp"

namespace source
{

Package*
PackageSet::load (const std::string& rcgo_root, const std::string& import_path)
{
  MapType::const_iterator pos = map_.find (import_path);
  if (pos != map_.end ())
    {
      return pos->second;
    }

  // Form a path to a directory.
  const std::string p = rcgo_root + "/" + import_path;

  // Open the directory and scan it.
  DIR* d = opendir (p.c_str ());
  if (d == NULL)
    {
      UNIMPLEMENTED;
    }
  Package* pkg = new Package ();
  map_[import_path] = pkg;
  list_.push_back (pkg);
  struct dirent* entry;
  struct stat statbuf;
  while ((entry = readdir (d)) != NULL)
    {
      const std::string f = p + "/" + entry->d_name;
      stat (f.c_str (), &statbuf);
      if (S_ISREG (statbuf.st_mode))
        {
          // Open the input file.
          yyscan_t scanner;
          yylex_init (&scanner);
          yyset_in (fopen (f.c_str (), "r"), scanner);
          if (yyget_in (scanner) == NULL)
            {
              error (EXIT_FAILURE, errno, "Could not open '%s'", f.c_str ());
            }

          SourceFile* source_file = new SourceFile (f);
          if (yyparse (scanner, source_file) != 0)
            {
              error (EXIT_FAILURE, 0, "yyparse failed");
            }
          yylex_destroy(scanner);

          pkg->add_source_file (source_file);
        }
    }
  closedir (d);

  for (Package::const_source_files_iterator fpos = pkg->source_files_begin (),
       flimit = pkg->source_files_end ();
       fpos != flimit;
       ++fpos)
    {
      SourceFile* source_file = *fpos;
      for (SourceFile::imports_iterator ipos = source_file->imports_begin (),
           ilimit = source_file->imports_end ();
           ipos != ilimit;
           ++ipos)
        {
          Import& i = *ipos;
          Package* p = load (rcgo_root, i.import_path ());
          i.set_package (p);
          pkg->add_dependency (p);
        }
    }

  return pkg;
}

bool
PackageSet::empty () const
{
  return map_.empty ();
}

PackageSet::packages_iterator
PackageSet::packages_begin ()
{
  return list_.begin ();
}

PackageSet::packages_iterator
PackageSet::packages_end ()
{
  return list_.end ();
}

PackageSet::const_packages_iterator
PackageSet::packages_begin () const
{
  return list_.begin ();
}

PackageSet::const_packages_iterator
PackageSet::packages_end () const
{
  return list_.end ();
}

void
PackageSet::determine_package_names (util::ErrorReporter& er)
{
  for (packages_iterator ppos = packages_begin (),
       plimit = packages_end ();
       ppos != plimit;
       ++ppos)
    {
      (*ppos)->determine_package_name (er);
    }
}

void
PackageSet::process_symbols (util::ErrorReporter& er) const
{
  // Perform all symbol processing.
  decl::SymbolTable symbol_table;

  // Universal block.
  symbol_table.open_scope ();
  decl::populate_universe_block (symbol_table);

  for (const_packages_iterator ppos = packages_begin (),
       plimit = packages_end ();
       ppos != plimit;
       ++ppos)
    {
      (*ppos)->process_symbols (er, symbol_table);
    }

  symbol_table.close_scope ();
}

}
