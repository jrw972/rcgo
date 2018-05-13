// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/populate_file_block.h"

#include "src/insert_symbol.h"

namespace rcgo {

void
PopulateFileBlock(ast::Node* source_file, Block* file_block,
                  PackageCache* package_cache, ErrorReporter* error_reporter) {
  struct FileBlockVisitor : public ast::DefaultNodeVisitor {
    FileBlockVisitor(PackageCache* a_package_cache,
                      Block* a_file_block,
                      ErrorReporter* a_error_reporter)
        : package_cache(a_package_cache)
        , file_block(a_file_block)
        , error_reporter(a_error_reporter)
    { }

    PackageCache* package_cache;
    Block* file_block;
    ErrorReporter* error_reporter;

    void DefaultAction(ast::Node* ast) override { abort(); /* NOT_COVERED */ }

    void Visit(ast::SourceFile* ast) override { VisitAll(ast->imports); }

    void Visit(ast::ImportDecl* ast) override { VisitAll(ast->import_specs); }

    void Visit(ast::ImportSpec* ast) override {
      // Find the package.
      // TODO(jrw972):  These cannot conflict with the package block.
      Package* p = package_cache->Find(ast->import_path);
      if (p != NULL) {
        if (ast->dot) {
          // Copy exported symbols of the package into the file block.
          for (SymbolTable::const_iterator pos = p->Begin(), limit = p->End();
               pos != limit; ++pos) {
            Symbol* s = pos->second;
            if (s->exported) {
              ImportedSymbol* is =
                  new ImportedSymbol(pos->first, ast->location,
                                     file_block->GetPackage(), s);
              InsertSymbol(file_block, is, error_reporter);
            }
          }
        } else if (ast->optional_identifier != NULL) {
          // Add the package using the given identifier.
          ast::Identifier* identifier =
              ast::Cast<ast::Identifier>(ast->optional_identifier);
          PackageSymbol* ps =
              new PackageSymbol(identifier->identifier, ast->location,
                                file_block->GetPackage(), p);
          InsertSymbol(file_block, ps, error_reporter);
        } else {
          // Add the package using its identifier.
          PackageSymbol* ps =
              new PackageSymbol(p->name(), ast->location,
                                file_block->GetPackage(), p);
          InsertSymbol(file_block, ps, error_reporter);
        }
      }
    }
  };

  FileBlockVisitor visitor(package_cache, file_block, error_reporter);
  source_file->Accept(&visitor);
}

}  // namespace rcgo
