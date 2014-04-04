/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "AstVisitor.hpp"
#include "Symbol.hpp"
#include "SymbolTable.hpp"
#include "functions.hpp"

namespace rcgo {

  void
  define_symbols (Ast* source_file,
                  Block& file_block,
                  TypeFactory& type_factory,
                  ErrorReporter& error_reporter)
  {
    struct Visitor : public DefaultAstVisitor {
      Visitor (Block& a_file_block,
               TypeFactory& a_type_factory,
               ErrorReporter& a_error_reporter)
        : file_block (a_file_block)
        , type_factory (a_type_factory)
        , error_reporter (a_error_reporter)
      { }

      Block& file_block;
      TypeFactory& type_factory;
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
            Symbol* symbol = file_block.find_global_symbol (identifier->identifier);
            define_symbol (symbol, file_block, type_factory, error_reporter);
          }
      }

      void visit (TypeSpecAst& ast)
      {
        IdentifierAst* identifier = to_identifier (ast.identifier);
        assert (identifier != NULL);
        Symbol* symbol = file_block.find_global_symbol (identifier->identifier);
        define_symbol (symbol, file_block, type_factory, error_reporter);
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
            Symbol* symbol = file_block.find_global_symbol (identifier->identifier);
            define_symbol (symbol, file_block, type_factory, error_reporter);
          }
      }

      void visit (FuncDeclAst& ast)
      {
        IdentifierAst* identifier = to_identifier (ast.identifier);
        assert (identifier != NULL);
        Symbol* symbol = file_block.find_global_symbol (identifier->identifier);
        define_symbol (symbol, file_block, type_factory, error_reporter);
      }

      void visit (MethodDeclAst& ast)
      {
        // TODO
        abort ();
      }
    };

    Visitor visitor (file_block, type_factory, error_reporter);
    source_file->accept (visitor);
  }

}
