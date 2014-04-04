/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Symbol.hpp"
#include "AstVisitor.hpp"
#include "SymbolVisitor.hpp"
#include "Type.hpp"
#include "functions.hpp"
#include "Error.hpp"
#include "ErrorReporter.hpp"

#include <iostream>

namespace rcgo {

  bool
  define_symbol (Symbol* symbol,
                 const Block& file_block,
                 TypeFactory& factory,
                 ErrorReporter& er)
  {
    struct DefinePackageSymbolVisitor : public DefaultAstVisitor {
      TypeFactory& factory;
      const Block& block;
      ErrorReporter& error_reporter;
      Symbol* symbol;

      DefinePackageSymbolVisitor (TypeFactory& a_factory, const Block& a_block, ErrorReporter& a_error_reporter, Symbol* a_symbol)
        : factory (a_factory)
        , block (a_block)
        , error_reporter (a_error_reporter)
        , symbol (a_symbol)
      { }

      void default_action (Ast& ast)
      {
        std::cout << "TODO: DefinedPackageSymbolVisitor::default_action " << ast << std::endl;
        abort ();
      }

      void visit (TypeSpecAst& ast)
      {
        TypeSymbol* s = symbol_cast<TypeSymbol> (symbol);
        assert (s != NULL);
        if (ast.is_alias)
          {
            s->type (factory.make_alias_type (process_type (ast.type, block, factory, error_reporter)));
          }
        else
          {
            s->type (factory.make_defined_type (process_type (ast.type, block, factory, error_reporter)));
          }
      }

      void visit (ConstSpecAst& ast)
      {
        ConstantSymbol* s = symbol_cast<ConstantSymbol> (symbol);

        // TODO: Handle case when no expressions given.
        assert (ast.optional_expression_list != NULL);
        if (ast.type == NULL && ast.optional_type != NULL)
          {
            ast.type = process_type (ast.optional_type, block, factory, error_reporter);
          }

        // Find the identifier.
        Ast::const_iterator pos = std::find_if (ast.identifier_list->begin (), ast.identifier_list->end (), IdentifierEqual (s->identifier));
        size_t idx = std::distance (ast.identifier_list->begin (), pos);
        if (idx >= ast.optional_expression_list->size ())
          {
            // Value for the symbol indicates error.
            // Error has already been reported.
            s->value (Value ());
            return;
          }

        Ast* e = ast.optional_expression_list->at (idx);
        s->value (evaluate_constant_expression (e, block, factory, error_reporter));
        if (s->value ().kind != Value::Error && ast.type != NULL)
          {
            s->value (s->value ().convert (ast.type));
            if (s->value ().kind == Value::Error)
              {
                // TODO:  Conversion error.
                abort ();
              }
          }
      }

      void visit (FuncDeclAst& ast)
      {
        FunctionSymbol* s = symbol_cast<FunctionSymbol> (symbol);
        assert (s != NULL);
        const FunctionType* function_type = process_function_type (ast_cast<SignatureAst> (ast.signature), block, factory, error_reporter);
        s->type (function_type);
      }
    };

    if (symbol->is_new ())
      {
        symbol->in_progress ();
        DefinePackageSymbolVisitor visitor (factory, file_block, er, symbol);
        symbol->ast->accept (visitor);
      }
    else if (symbol->is_in_progress ())
      {
        er << SymbolIsDefinedRecursively (symbol);
      }
    else
      {
        assert (symbol->is_defined ());
      }
    return symbol->is_defined ();
  }

}
