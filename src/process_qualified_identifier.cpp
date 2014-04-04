/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "AstVisitor.hpp"
#include "Symbol.hpp"
#include "SymbolTable.hpp"
#include "SymbolVisitor.hpp"
#include "Error.hpp"
#include "functions.hpp"
#include "Package.hpp"

namespace rcgo {

  Symbol*
  process_qualified_identifier (Ast* ast,
                                const Block& block,
                                ErrorReporter& error_reporter)
  {
    struct Visitor : public DefaultAstVisitor {
      const Block& block;
      ErrorReporter& error_reporter;
      Symbol* symbol;

      Visitor (const Block& a_block,
               ErrorReporter& a_error_reporter)
        : block (a_block)
        , error_reporter (a_error_reporter)
        , symbol (NULL)
      { }

      void default_action (Ast& ast) { abort (); /* NOT_COVERED */ }

      void visit (SelectorAst& ast)
      {
        IdentifierAst* p = to_identifier (ast.operand);
        assert (p != NULL);
        IdentifierAst* m = to_identifier (ast.member);
        assert (m != NULL);

        // Look up the package.
        symbol = block.find_global_symbol (p->identifier);
        if (symbol == NULL)
          {
            error_reporter << NotDeclared (p->location, p->identifier);
            symbol = NULL;
            return;
          }
        PackageSymbol* package = symbol_cast<PackageSymbol> (symbol);
        if (package == NULL)
          {
            error_reporter << DoesNotReferToAPackage (p->location, p->identifier);
            symbol = NULL;
            return;
          }

        if (!is_exported (m->identifier))
          {
            error_reporter << IsNotExported (m->location, m->identifier);
            symbol = NULL;
            return;
          }

        symbol = package->the_package->find (m->identifier);
        if (symbol == NULL)
          {
            error_reporter << NotDeclared (m->location, m->identifier);
            symbol = NULL;
            return;
          }
      }

      void visit (IdentifierAst& ast)
      {
        symbol = block.find_global_symbol (ast.identifier);
        if (symbol == NULL)
          {
            error_reporter << NotDeclared (ast.location, ast.identifier);
          }
      }
    };

    Visitor visitor (block, error_reporter);
    ast->accept (visitor);
    return visitor.symbol;
  }

}
