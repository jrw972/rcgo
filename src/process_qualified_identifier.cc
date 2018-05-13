// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/process_qualified_identifier.h"

#include <cassert>

#include "src/ast.h"
#include "src/package.h"

namespace rcgo {

Symbol* ProcessQualifiedIdentifier(ast::Node* ast, const Block& block,
                                   ErrorReporter* error_reporter) {
  struct Visitor : public ast::DefaultNodeVisitor {
    const Block& block;
    ErrorReporter* error_reporter;
    Symbol* symbol;

    Visitor(const Block& a_block, ErrorReporter* a_error_reporter)
        : block(a_block), error_reporter(a_error_reporter), symbol(NULL) {}

    void DefaultAction(ast::Node* ast) override { abort(); /* NOT_COVERED */ }

    void Visit(ast::Selector* ast) override {
      ast::Identifier* p = ast::Cast<ast::Identifier>(ast->operand);
      assert(p != NULL);
      ast::Identifier* m = ast::Cast<ast::Identifier>(ast->member);
      assert(m != NULL);

      // Look up the package.
      symbol = block.FindGlobalSymbol(p->identifier);
      if (symbol == NULL) {
        error_reporter->Insert(NotDeclared(p->location, p->identifier));
        symbol = NULL;
        return;
      }
      PackageSymbol* package = symbol_cast<PackageSymbol>(symbol);
      if (package == NULL) {
        error_reporter->Insert(
            DoesNotReferToAPackage(p->location, p->identifier));
        symbol = NULL;
        return;
      }

      if (!IsExported(m->identifier)) {
        error_reporter->Insert(IsNotExported(m->location, m->identifier));
        symbol = NULL;
        return;
      }

      symbol = package->the_package->Find(m->identifier);
      if (symbol == NULL) {
        error_reporter->Insert(NotDeclared(m->location, m->identifier));
        symbol = NULL;
        return;
      }
    }

    void Visit(ast::Identifier* ast) override {
      symbol = block.FindGlobalSymbol(ast->identifier);
      if (symbol == NULL) {
        error_reporter->Insert(NotDeclared(ast->location, ast->identifier));
      }
    }
  };

  Visitor visitor(block, error_reporter);
  ast->Accept(&visitor);
  return visitor.symbol;
}

}  // namespace rcgo
