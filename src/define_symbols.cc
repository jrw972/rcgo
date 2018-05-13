// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/define_symbols.h"

#include <cassert>

#include "src/ast.h"
#include "src/define_symbol.h"

namespace rcgo {

void DefineSymbols(ast::Node* source_file, Block* file_block,
                   type::Factory* type_factory, ErrorReporter* error_reporter) {
  struct Visitor : public ast::DefaultNodeVisitor {
    Visitor(Block* a_file_block, type::Factory* a_type_factory,
            ErrorReporter* a_error_reporter)
        : file_block(a_file_block), type_factory(a_type_factory),
          error_reporter(a_error_reporter) { }

    Block* file_block;
    type::Factory* type_factory;
    ErrorReporter* error_reporter;

    void DefaultAction(ast::Node* ast) override { abort(); /* NOT_COVERED */ }

    void Visit(ast::SourceFile* ast) override { VisitAll(ast->decls); }

    void Visit(ast::ConstSpec* ast) override {
      for (ast::Node* node : ast->identifier_list) {
        ast::Identifier* identifier = ast::Cast<ast::Identifier>(node);
        assert(identifier != NULL);
        Symbol* symbol = file_block->FindGlobalSymbol(identifier->identifier);
        DefineSymbol(symbol, *file_block, type_factory, error_reporter);
      }
    }

    void Visit(ast::TypeDecl* ast) override { VisitAll(ast->type_specs); }

    void Visit(ast::TypeSpec* ast) override {
      ast::Identifier* identifier = ast::Cast<ast::Identifier>(ast->identifier);
      assert(identifier != NULL);
      Symbol* symbol = file_block->FindGlobalSymbol(identifier->identifier);
      DefineSymbol(symbol, *file_block, type_factory, error_reporter);
    }

    void Visit(ast::VarSpec* ast) override {
      for (ast::Node* node : ast->identifier_list) {
        ast::Identifier* identifier = ast::Cast<ast::Identifier>(node);
        assert(identifier != NULL);
        Symbol* symbol = file_block->FindGlobalSymbol(identifier->identifier);
        DefineSymbol(symbol, *file_block, type_factory, error_reporter);
      }
    }

    void Visit(ast::FuncDecl* ast) override {
      ast::Identifier* identifier = ast::Cast<ast::Identifier>(ast->identifier);
      assert(identifier != NULL);
      Symbol* symbol = file_block->FindGlobalSymbol(identifier->identifier);
      DefineSymbol(symbol, *file_block, type_factory, error_reporter);
    }

    void Visit(ast::MethodDecl* ast) override {
      // TODO(jrw972):  Implement this.
      abort();
    }
  };

  Visitor visitor(file_block, type_factory, error_reporter);
  source_file->Accept(&visitor);
}

}  // namespace rcgo
