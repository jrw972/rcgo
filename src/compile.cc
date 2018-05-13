// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/compile.h"

#include <string>

#include "src/byte_stream.h"
#include "src/define_symbols.h"
#include "src/enter_predeclared_symbols.h"
#include "src/get_and_check_package_name.h"
#include "src/get_import_locations.h"
#include "src/parser.h"
#include "src/populate_file_block.h"
#include "src/populate_package_block.h"
#include "src/type.h"

namespace rcgo {

Package* Compile(
    ImportLocations* import_locations, const Paths& search_paths,
    PackageCache* package_cache, ErrorReporter* error_reporter,
    std::ostream& out) {
  const ImportLocation import_location = import_locations->back();

  // Check for recursive import.
  Package* package = package_cache->Find(import_location.import_path);
  if (package != NULL) {
    if (package->IsNew()) {
      error_reporter->Insert(RecursiveImport(*import_locations));
      package->Error();
    }
    return package;
  }

  size_t error_count = error_reporter->Count();

  package = new Package(import_location.import_path);
  package_cache->Insert(package);

  // Find the directory.
  std::string package_source_directory_path;
  DIR* package_source_directory;
  if (!FindPackageSourceDirectory(import_location, search_paths,
                                  &package_source_directory_path,
                                  &package_source_directory,
                                  error_reporter)) {
    package->Error();
    return package;
  }

  // Get the files.
  Paths package_source_file_paths = GetPackageSourceFilePaths(
      package_source_directory_path, package_source_directory,
      error_reporter);
  closedir(package_source_directory);

  if (package_source_file_paths.empty()) {
    error_reporter->Insert(
        NoFiles(import_location, package_source_directory_path));
    package->Error();
    return package;
  }

  // Parse the files.
  ast::SourceFiles source_files;
  for (const std::string& path : package_source_file_paths) {
    FileByteStream byte_stream(path.c_str());
    Parser parser(&byte_stream, error_reporter, out);
    source_files.push_back(parser.SourceFile());
  }

  // Set the package name.
  package->name(GetAndCheckPackageName(source_files, error_reporter));

  // Compile imported packages.
  ImportLocations package_import_locations = GetImportLocations(source_files);
  for (const ImportLocations::value_type& location : package_import_locations) {
    import_locations->push_back(location);
    Compile(import_locations, search_paths, package_cache, error_reporter, out);
    import_locations->pop_back();
  }

  {
    type::Factory type_factory;
    SymbolTable universe_symbol_table;
    // Populate the universe block.
    EnterPredeclaredSymbols(&type_factory, &universe_symbol_table);
    Block universe_block(&universe_symbol_table, NULL);
    // Populate the package block.
    Block package_block(package, &universe_block, package);
    PopulatePackageBlock(source_files, &package_block, error_reporter);

    for (ast::Node* source_file : source_files) {
      // Populate the file block.
      SymbolTable file_block_st;
      Block file_block(&file_block_st, &package_block);
      PopulateFileBlock(
          source_file, &file_block, package_cache, error_reporter);
      DefineSymbols(source_file, &file_block, &type_factory, error_reporter);
      // TODO(jrw972):  type checking
      // TODO(jrw972):  control flow analysis and other semantic checks
      // TODO(jrw972):  code generation
    }
  }

  // Clean up.
  for (ast::Node* source_file : source_files) delete source_file;

  if (error_reporter->Count() != error_count) {
    package->Error();
    return package;
  }

  /*
    TODO(jrw972)
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

  package->Changed();

  return package;
}

}  // namespace rcgo
