/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "AstVisitor.hpp"
#include "Symbol.hpp"
#include "SymbolTable.hpp"
#include "functions.hpp"

namespace rcgo {

  void
  populate_package_block (const SourceFiles& source_files,
                          Block& package_block,
                          ErrorReporter& error_reporter)
  {
    struct PackageBlockVisitor : public DefaultAstVisitor {
      PackageBlockVisitor (Block* a_package_block,
                           ErrorReporter& a_error_reporter)
        : package_block (a_package_block)
        , error_reporter (a_error_reporter)
      { }

      Block* package_block;
      ErrorReporter& error_reporter;

      void default_action (Ast& ast) { abort (); /* NOT_COVERED */ }

      void visit (SourceFileAst& ast)
      {
        ast.decls->visit_children (*this);
      }

      void visit (ListAst& ast)
      {
        ast.visit_children (*this);
      }

      void visit (ConstSpecAst& ast)
      {
        for (Ast::const_iterator pos = ast.identifier_list->begin (),
               limit = ast.identifier_list->end ();
             pos != limit;
             ++pos)
          {
            IdentifierAst* identifier = to_identifier (*pos);
            assert (identifier != NULL);
            insert_symbol (package_block, new ConstantSymbol (identifier->identifier, identifier->location, package_block->package (), &ast), error_reporter);
          }
      }

      void visit (TypeSpecAst& ast)
      {
        IdentifierAst* identifier = to_identifier (ast.identifier);
        assert (identifier != NULL);
        insert_symbol (package_block, new TypeSymbol (identifier->identifier, identifier->location, package_block->package (), &ast), error_reporter);
      }

      void visit (VarSpecAst& ast)
      {
        for (Ast::const_iterator pos = ast.identifier_list->begin (),
               limit = ast.identifier_list->end ();
             pos != limit;
             ++pos)
          {
            IdentifierAst* identifier = to_identifier (*pos);
            assert (identifier != NULL);
            insert_symbol (package_block, new VariableSymbol (identifier->identifier, identifier->location, package_block->package (), &ast), error_reporter);
          }
      }

      void visit (FuncDeclAst& ast)
      {
        IdentifierAst* identifier = to_identifier (ast.identifier);
        assert (identifier != NULL);
        insert_symbol (package_block, new FunctionSymbol (identifier->identifier, identifier->location, package_block->package (), &ast), error_reporter);
      }
    };

    PackageBlockVisitor visitor (&package_block, error_reporter);
    for (SourceFiles::const_iterator pos = source_files.begin (),
           limit = source_files.end ();
         pos != limit;
         ++pos)
      {
        Ast* source_file = *pos;
        source_file->accept (visitor);
      }
  }

}
