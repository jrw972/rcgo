// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/populate_package_block.h"

#include <cassert>

#include "src/insert_symbol.h"

namespace rcgo {

void PopulatePackageBlock(const ast::SourceFiles& source_files,
                          Block* package_block, ErrorReporter* error_reporter) {
  struct PackageBlockVisitor : public ast::DefaultNodeVisitor {
    PackageBlockVisitor(Block* a_package_block, ErrorReporter* a_error_reporter)
        : package_block(a_package_block) , error_reporter(a_error_reporter) { }

    Block* package_block;
    ErrorReporter* error_reporter;

    void DefaultAction(ast::Node* ast) override { abort(); /* NOT_COVERED */ }

    void Visit(ast::SourceFile* ast) override {
      VisitAll(ast->decls);
    }

    void Visit(ast::ConstSpec* ast) override {
      for (ast::Node* node : ast->identifier_list) {
        ast::Identifier* identifier = ast::Cast<ast::Identifier>(node);
        assert(identifier != NULL);
        ConstantSymbol* cs = new ConstantSymbol(
            identifier->identifier, identifier->location,
            package_block->GetPackage(), ast);
        InsertSymbol(package_block, cs, error_reporter);
      }
    }

    void Visit(ast::TypeDecl* ast) override {
      VisitAll(ast->type_specs);
    }

    void Visit(ast::TypeSpec* ast) override {
      ast::Identifier* identifier = ast::Cast<ast::Identifier>(ast->identifier);
      assert(identifier != NULL);
      TypeSymbol* ts = new TypeSymbol(
          identifier->identifier, identifier->location,
          package_block->GetPackage(), ast);
      InsertSymbol(package_block, ts, error_reporter);
    }

    void Visit(ast::VarSpec* ast) override {
      for (ast::Node* node : ast->identifier_list) {
        ast::Identifier* identifier = ast::Cast<ast::Identifier>(node);
        assert(identifier != NULL);
        VariableSymbol* vs = new VariableSymbol(
            identifier->identifier, identifier->location,
            package_block->GetPackage(), ast);
        InsertSymbol(package_block, vs, error_reporter);
      }
    }

    void Visit(ast::FuncDecl* ast) override {
      ast::Identifier* identifier = ast::Cast<ast::Identifier>(ast->identifier);
      assert(identifier != NULL);
      FunctionSymbol* fs = new FunctionSymbol(
          identifier->identifier, identifier->location,
          package_block->GetPackage(), ast);
      InsertSymbol(package_block, fs, error_reporter);
    }
  };

  PackageBlockVisitor visitor(package_block, error_reporter);
  for (ast::Node* source_file : source_files) {
    source_file->Accept(&visitor);
  }
}

}  // namespace rcgo
