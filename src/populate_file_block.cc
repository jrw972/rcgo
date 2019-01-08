// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/populate_file_block.h"

#include <iostream>

#include "src/insert_symbol.h"

namespace rcgo {

void
PopulateFileBlock(
    ast::Node* source_file, MutableBlock* file_block, Package* package,
    ErrorReporter* error_reporter) {
  struct FileBlockVisitor : public ast::DefaultNodeVisitor {
    FileBlockVisitor(Package* a_package, MutableBlock* a_file_block,
                     ErrorReporter* a_error_reporter)
        : package(a_package)
        , file_block(a_file_block)
        , error_reporter(a_error_reporter)
    { }

    Package* package;
    MutableBlock* file_block;
    ErrorReporter* error_reporter;

    void DefaultAction(ast::Node* ast) override { abort(); /* NOT_COVERED */ }

    void Visit(ast::SourceFile* ast) override { VisitAll(ast->imports); }

    void Visit(ast::ImportDecl* ast) override { VisitAll(ast->import_specs); }

    void Visit(ast::ImportSpec* ast) override {
      // Find the package.
      // TODO(jrw972):  These cannot conflict with the package block.
      Package::ImportsType::const_iterator pos =
          package->imports.find(ast->import_path);
      if (pos != package->imports.end()) {
        Package* p = pos->second;

        if (ast->dot) {
          // Copy exported symbols of the package into the file block.
          for (symbol::Table::const_iterator pos = p->Begin(), limit = p->End();
               pos != limit; ++pos) {
            symbol::Symbol* s = pos->second;
            if (s->exported) {
              InsertSymbol(file_block, s, error_reporter);
            }
          }
        } else if (ast->optional_identifier != nullptr) {
          // Add the package using the given identifier.
          ast::Identifier* identifier =
              ast::Cast<ast::Identifier>(ast->optional_identifier);
          symbol::Package* ps = file_block->MakePackage(
              identifier->identifier, ast->location, p);
          InsertSymbol(file_block, ps, error_reporter);
        } else {
          // Add the package using its identifier.
          symbol::Package* ps = file_block->MakePackage(
              p->name(), ast->location, p);
          InsertSymbol(file_block, ps, error_reporter);
        }
      } else {
        // TODO(jrw972): Importing something that was not in config.yaml.
        std::cout << "TODO:  Import " << ast->import_path << " not in config of " << package->path << std::endl;
      }
    }
  };

  FileBlockVisitor visitor(package, file_block, error_reporter);
  source_file->Accept(&visitor);
}

}  // namespace rcgo
