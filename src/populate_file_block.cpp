/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "AstVisitor.hpp"
#include "Package.hpp"
#include "functions.hpp"

namespace rcgo {

  void
  populate_file_block (Ast* source_file,
                       Block& file_block,
                       PackageCache& package_cache,
                       ErrorReporter& error_reporter)
  {
    struct FileBlockVisitor : public DefaultAstVisitor {
      FileBlockVisitor (PackageCache& a_package_factory,
                        Block* a_file_block,
                        ErrorReporter& a_error_reporter)
        : package_factory (a_package_factory)
        , file_block (a_file_block)
        , error_reporter (a_error_reporter)
      { }

      PackageCache& package_factory;
      Block* file_block;
      ErrorReporter& error_reporter;

      void default_action (Ast& ast) { abort (); /* NOT_COVERED */ }

      void visit (SourceFileAst& ast)
      {
        ast.imports->accept (*this);
      }

      void visit (ImportDeclListAst& ast)
      {
        ast.visit_children (*this);
      }

      void visit (ImportDeclAst& ast)
      {
        ast.visit_children (*this);
      }

      void visit (ImportSpecAst& ast)
      {
        // Find the package.
        // TODO:  These cannot conflict with the package block.
        Package* p = package_factory.find (ast.import_path);
        if (p != NULL)
          {
            if (ast.dot)
              {
                // Copy all of the exported symbols of the package into the file block.
                for (SymbolTable::const_iterator pos = p->begin (),
                       limit = p->end ();
                     pos != limit;
                     ++pos)
                  {
                    Symbol* s = pos->second;
                    if (s->exported)
                      {
                        insert_symbol (file_block, new ImportedSymbol (pos->first, ast.location, file_block->package (), s), error_reporter);
                      }
                  }
              }
            else if (ast.optional_identifier != NULL)
              {
                // Add the package using the given identifier.
                IdentifierAst* identifier = to_identifier (ast.optional_identifier);
                insert_symbol (file_block, new PackageSymbol (identifier->identifier, ast.location, file_block->package (), p), error_reporter);

              }
            else
              {
                // Add the package using its identifier.
                insert_symbol (file_block, new PackageSymbol (p->name (), ast.location, file_block->package (), p), error_reporter);
              }
          }
      }
    };

    FileBlockVisitor visitor (package_cache, &file_block, error_reporter);
    source_file->accept (visitor);
  }
}
