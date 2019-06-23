// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/process_qualified_identifier.h"

#include <cassert>
#include <string>

#include "src/ast.h"
#include "src/package.h"

namespace rcgo {
namespace {

Error doesNotReferToAPackage(const Location& a_location,
                             const std::string& a_identifier) {
  Error error(a_location);
  error.message << "error: " << a_identifier << " does not refer to a package" << std::endl;
  return error;
}

Error isNotExported(const Location& a_location, const std::string& a_identifier) {
  Error error(a_location);
  error.message << "error: " << a_identifier << " is not exported" << std::endl;
  return error;
}

}

symbol::Symbol* ProcessQualifiedIdentifier(ast::Node* ast, const Block& block,
                                   ErrorList* error_list) {
  struct Visitor : public ast::DefaultNodeVisitor {
    const Block& block;
    ErrorList* error_list;
    symbol::Symbol* symbol;

    Visitor(const Block& a_block, ErrorList* a_error_list)
        : block(a_block), error_list(a_error_list), symbol(NULL) {}

    void DefaultAction(ast::Node* ast) override { abort(); /* NOT_COVERED */ }

    void Visit(ast::Selector* ast) override {
      ast::Identifier* p = ast::Cast<ast::Identifier>(ast->operand);
      assert(p != NULL);
      ast::Identifier* m = ast::Cast<ast::Identifier>(ast->member);
      assert(m != NULL);

      // Look up the package.
      symbol = block.FindGlobalSymbol(p->identifier);
      if (symbol == NULL) {
        error_list->push_back(NotDeclared(p->location, p->identifier));
        symbol = NULL;
        return;
      }
      symbol::Package* package = symbol::Cast<symbol::Package>(symbol);
      if (package == NULL) {
        error_list->push_back(
            doesNotReferToAPackage(p->location, p->identifier));
        symbol = NULL;
        return;
      }

      if (!symbol::IsExported(m->identifier)) {
        error_list->push_back(isNotExported(m->location, m->identifier));
        symbol = NULL;
        return;
      }

      symbol = package->the_package->Find(m->identifier);
      if (symbol == NULL) {
        error_list->push_back(NotDeclared(m->location, m->identifier));
        symbol = NULL;
        return;
      }
    }

    void Visit(ast::Identifier* ast) override {
      symbol = block.FindGlobalSymbol(ast->identifier);
      if (symbol == NULL) {
        error_list->push_back(NotDeclared(ast->location, ast->identifier));
      }
    }
  };

  Visitor visitor(block, error_list);
  ast->Accept(&visitor);
  return visitor.symbol;
}

}  // namespace rcgo
