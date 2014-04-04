#ifndef RCGO_SRC_FUNCTIONS_HPP
#define RCGO_SRC_FUNCTIONS_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Forward declarations.

#include "types.hpp"
#include "ImportLocation.hpp"
#include "Value.hpp"

namespace rcgo {

  Paths
  get_search_paths ();

  bool
  find_package_source_directory (const ImportLocation& import_location,
                                 const Paths& search_paths,
                                 ErrorReporter& error_reporter,
                                 std::string& package_source_directory_path,
                                 DIR*& package_source_directory);

  Paths
  get_package_source_file_paths (const std::string& package_source_directory_path,
                                 DIR* package_source_directory,
                                 ErrorReporter& error_reporter);

  std::string
  get_and_check_package_name (const SourceFiles& source_files,
                              ErrorReporter& error_reporter);

  ImportLocations
  get_import_locations (const SourceFiles& source_files);

  void
  enter_predeclared_symbols (TypeFactory& factory,
                             SymbolTable& universe_st);

  void
  insert_symbol (Block* block,
                 Symbol* symbol,
                 ErrorReporter& error_reporter);

  void
  populate_package_block (const SourceFiles& source_files,
                          Block& package_block,
                          ErrorReporter& error_reporter);

  void
  populate_file_block (Ast* source_file,
                       Block& file_block,
                       PackageCache& package_cache,
                       ErrorReporter& error_reporter);

  void
  define_symbols (Ast* source_file,
                  Block& file_block,
                  TypeFactory& type_factory,
                  ErrorReporter& error_reporter);

  bool
  define_symbol (Symbol* symbol,
                 const Block& file_block,
                 TypeFactory& type_factory,
                 ErrorReporter& error_reporter);

  Symbol*
  process_qualified_identifier (Ast* ast,
                                const Block& block,
                                ErrorReporter& error_reporter);

  const Type*
  process_type (Ast* ast,
                const Block& block,
                TypeFactory& type_factory,
                ErrorReporter& error_reporter);

  const FunctionType*
  process_function_type (SignatureAst* ast,
                         const Block& block,
                         TypeFactory& type_factory,
                         ErrorReporter& error_reporter);

  Value
  evaluate_constant_expression (Ast* ast,
                                const Block& block,
                                TypeFactory& type_factory,
                                ErrorReporter& error_reporter);

  Package*
  compile (ImportLocations& import_locations,
           const Paths& search_paths,
           PackageCache& package_cache,
           ErrorReporter& error_reporter,
           std::ostream& out);

  void
  process_symbols (PackageCache& package_factory,
                   Package* p,
                   TypeFactory& factory,
                   Block* universe_block,
                   ErrorReporter& er);

  ErrorReporter&
  operator<< (ErrorReporter& error_reporter,
              const Error& error);

}

#endif // RCGO_SRC_FUNCTIONS_HPP
