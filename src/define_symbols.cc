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
                   type::Factory* type_factory, ErrorList* error_list) {
  struct Visitor : public ast::DefaultNodeVisitor {
    Visitor(Block* a_file_block, type::Factory* a_type_factory,
            ErrorList* a_error_list)
        : file_block(a_file_block), type_factory(a_type_factory),
          error_list(a_error_list) { }

    Block* file_block;
    type::Factory* type_factory;
    ErrorList* error_list;

    void DefaultAction(ast::Node* ast) override { abort(); /* NOT_COVERED */ }

    void Visit(ast::SourceFile* ast) override { VisitAll(ast->decls); }

    void Visit(ast::ConstSpec* ast) override {
      for (symbol::Constant* constant : ast->constants) {
        DefineSymbol(constant, file_block, type_factory, error_list);
      }
    }

    void Visit(ast::TypeDecl* ast) override { VisitAll(ast->type_specs); }

    void Visit(ast::TypeSpec* ast) override {
      DefineSymbol(ast->type, file_block, type_factory, error_list);
    }

    void Visit(ast::VarSpec* ast) override {
      for (symbol::Variable* variable : ast->variables) {
        DefineSymbol(variable, file_block, type_factory, error_list);
      }
    }

    void Visit(ast::FuncDecl* ast) override {
      DefineSymbol(ast->function, file_block, type_factory, error_list);
    }

    void Visit(ast::MethodDecl* ast) override {
      // TODO(jrw972):  Implement this.
      abort();
    }
  };

  Visitor visitor(file_block, type_factory, error_list);
  source_file->Accept(&visitor);
}

}  // namespace rcgo
