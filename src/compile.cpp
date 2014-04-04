/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Package.hpp"
#include "ByteStream.hpp"
#include "Parser.hpp"
#include "Error.hpp"
#include "ErrorReporter.hpp"
#include "Type.hpp"
#include "Ast.hpp"
#include "functions.hpp"

namespace rcgo {

  Package*
  compile (ImportLocations& import_locations,
           const Paths& search_paths,
           PackageCache& package_cache,
           ErrorReporter& error_reporter,
           std::ostream& out)
  {
    const ImportLocation import_location = import_locations.back ();

    // Check for recursive import.
    Package* package = package_cache.find (import_location.import_path);
    if (package != NULL) {
      if (package->is_new ()) {
        error_reporter << RecursiveImport (import_locations);
        package->error ();
      }
      return package;
    }

    size_t error_count = error_reporter.count ();

    package = new Package (import_location.import_path);
    package_cache.insert (package);

    // Find the directory.
    std::string package_source_directory_path;
    DIR* package_source_directory;
    if (!find_package_source_directory (import_location, search_paths, error_reporter, package_source_directory_path, package_source_directory)) {
      package->error ();
      return package;
    }

    // Get the files.
    Paths package_source_file_paths = get_package_source_file_paths (package_source_directory_path, package_source_directory, error_reporter);
    closedir (package_source_directory);

    if (package_source_file_paths.empty ()) {
      error_reporter << NoFiles (import_location, package_source_directory_path);
      package->error ();
      return package;
    }

    // Parse the files.
    SourceFiles source_files;
    for (Paths::const_iterator pos = package_source_file_paths.begin (),
           limit = package_source_file_paths.end ();
         pos != limit;
         ++pos)
      {
        FileByteStream byte_stream (pos->c_str ());
        Parser parser (byte_stream, error_reporter, out);
        source_files.push_back (parser.SourceFile ());
      }

    // Set the package name.
    package->name (get_and_check_package_name (source_files, error_reporter));

    // Compile imported packages.
    ImportLocations package_import_locations = get_import_locations (source_files);
    for (ImportLocations::const_iterator pos = package_import_locations.begin (),
           limit = package_import_locations.end ();
         pos != limit;
         ++pos)
      {
        import_locations.push_back (*pos);
        compile (import_locations, search_paths, package_cache, error_reporter, out);
        import_locations.pop_back ();
      }

    {
      TypeFactory type_factory;
      SymbolTable universe_symbol_table;
      // Populate the universe block.
      enter_predeclared_symbols (type_factory, universe_symbol_table);
      Block universe_block (universe_symbol_table, NULL);
      // Populate the package block.
      Block package_block (*package, &universe_block, package);
      populate_package_block (source_files, package_block, error_reporter);

      for (SourceFiles::const_iterator pos = source_files.begin (),
             limit = source_files.end ();
           pos != limit;
           ++pos)
        {
          Ast* source_file = *pos;
          // Populate the file block.
          SymbolTable file_block_st;
          Block file_block (file_block_st, &package_block);
          populate_file_block (source_file, file_block, package_cache, error_reporter);
          define_symbols (source_file, file_block, type_factory, error_reporter);
          // TODO: type checking
          // TODO: control flow analysis and other semantic checks
          // TODO: code generation
        }
    }

    // Clean up.
    for (SourceFiles::const_iterator pos = source_files.begin (),
           limit = source_files.end ();
         pos != limit;
         ++pos)
      {
        Ast* source_file = *pos;
        delete source_file;
      }

    if (error_reporter.count () != error_count) {
      package->error ();
      return package;
    }

    // TODO

    /*
      For each file.
      Parse the file.
      Record its hash.
      Recur on its imports.
      Record if any of the imported packages were recompiled.

      If a compiled package exists already and no packages were recompiled
      Read the package.
      Compare the files and their hashes.
      If everything is consistent, return no change

      Compile the package
      Return compiled.
    */

    package->changed ();

    return package;
  }

}
