/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "functions.hpp"
#include "AstVisitor.hpp"
#include "Error.hpp"
#include "Type.hpp"
#include "TypeVisitor.hpp"
#include "Symbol.hpp"
#include "SymbolVisitor.hpp"

namespace rcgo {

  namespace {
    struct Visitor : public DefaultAstVisitor {
      TypeFactory& factory;
      const Block& block;
      ErrorReporter& error_reporter;
      StructType* struct_type;
      FunctionType* function_type;
      InterfaceType* interface_type;
      bool results;
      const Type* type;

      Visitor (TypeFactory& a_factory,
               const Block& a_block,
               ErrorReporter& a_error_reporter)
        : factory (a_factory)
        , block (a_block)
        , error_reporter (a_error_reporter)
        , struct_type (NULL)
        , function_type (NULL)
        , interface_type (NULL)
        , results (false)
        , type (NULL)
      { }

      void default_action (Ast& ast) { abort (); /* NOT_COVERED */ }

      void visit (ArrayAst& ast)
      {
        const Type* element_type = process_type (ast.element_type, block, factory, error_reporter);
        if (ast.optional_length == NULL)
          {
            error_reporter << ArrayLengthRequired (ast.location);
            type = &ErrorType::instance;
            return;
          }
        Value length = evaluate_constant_expression (ast.optional_length, block, factory, error_reporter);
        if (length.kind == Value::Error)
          {
            type = &ErrorType::instance;
            return;
          }
        length = length.convert (&IntType::instance);
        if (length.kind == Value::Error)
          {
            error_reporter << ArrayLengthNotAnInt (ast.optional_length->location);
            type = &ErrorType::instance;
            return;
          }
        if (length.int_value < 0)
          {
            error_reporter << ArrayLengthIsNegative (ast.optional_length->location);
            type = &ErrorType::instance;
            return;
          }
        type = element_type->get_array (length.int_value);
      }

      void visit (SliceTypeAst& ast)
      {
        const Type* element_type = process_type (ast.element_type, block, factory, error_reporter);
        type = element_type->get_slice ();
      }

      void visit (PointerAst& ast)
      {
        const Type* base_type = process_type (ast.base_type, block, factory, error_reporter);
        type = base_type->get_pointer ();
      }

      void visit (StructAst& ast)
      {
        struct_type = factory.make_struct_type ();
        ast.visit_children (*this);
        type = struct_type;
      }

      void visit (FieldAst& ast)
      {
        // TODO:  Handle recursive types.
        const Type* field_type = process_type (ast.type, block, factory, error_reporter);
        for (Ast::const_iterator pos = ast.identifier_list->begin (), limit = ast.identifier_list->end ();
             pos != limit;
             ++pos)
          {
            IdentifierAst* id = to_identifier (*pos);
            Symbol* previous_field = struct_type->find (id->identifier);
            FieldSymbol* field = new FieldSymbol (id->identifier, id->location, block.package (), field_type, ast.tag, false);
            struct_type->append_field (field);
            if (previous_field != NULL)
              {
                error_reporter << DuplicateSymbol (field, previous_field);
              }
          }
      }

      void visit (EmbeddedFieldAst& ast)
      {
        // TODO:  Handle recursive types.
        Symbol* s = process_qualified_identifier (ast.type_name, block, error_reporter);
        if (s == NULL)
          {
            return;
          }

        TypeSymbol* ts = symbol_cast<TypeSymbol> (s);
        if (ts == NULL)
          {
            error_reporter << DoesNotReferToAType (ast.type_name->location, s->identifier);
            return;
          }

        Symbol* previous_field = struct_type->find (s->identifier);

        const Type* t = ts->type ();
        if (ast.is_pointer)
          {
            t = t->get_pointer ();
          }

        FieldSymbol* field = new FieldSymbol (s->identifier, ast.type_name->location, block.package (), t, ast.tag, true);
        struct_type->append_field (field);

        if (previous_field != NULL)
          {
            error_reporter << DuplicateSymbol (field, previous_field);
          }
      }

      void visit (IdentifierAst& ast)
      {
        Symbol* s = process_qualified_identifier (&ast, block, error_reporter);
        if (s == NULL)
          {
            type = &ErrorType::instance;
            return;
          }
        TypeSymbol* t = symbol_cast<TypeSymbol> (s);
        if (t == NULL)
          {
            error_reporter << DoesNotReferToAType (ast.location, ast.identifier);
            type = &ErrorType::instance;
            return;
          }
        if (define_symbol (t, block, factory, error_reporter))
          {
            type = t->type ();
          }
        else
          {
            type = &ErrorType::instance;
          }
      }

      void visit (SelectorAst& ast)
      {
        Symbol* s = process_qualified_identifier (&ast, block, error_reporter);
        if (s == NULL)
          {
            type = &ErrorType::instance;
            return;
          }
        TypeSymbol* t = symbol_cast<TypeSymbol> (s);
        if (t == NULL)
          {
            error_reporter << DoesNotReferToAType (ast.location, s->identifier);
            type = &ErrorType::instance;
            return;
          }
        if (define_symbol (t, block, factory, error_reporter))
          {
            type = t->type ();
          }
        else
          {
            type = &ErrorType::instance;
          }
      }

      void visit (FunctionTypeAst& ast)
      {
        function_type = factory.make_function_type ();
        ast.signature->accept (*this);
        type = function_type;
      }

      void visit (SignatureAst& ast)
      {
        results = false;
        ast.parameters->visit_children (*this);
        results = true;
        ast.results->visit_children (*this);
      }

      void visit (ParameterDeclAst& ast)
      {
        assert (function_type != NULL);

        const Type* parameter_type = process_type (ast.type, block, factory, error_reporter);
        if (ast.optional_identifier_list != NULL)
          {
            for (Ast::const_iterator pos = ast.optional_identifier_list->begin (), limit = ast.optional_identifier_list->end ();
                 pos != limit;
                 ++pos)
              {
                IdentifierAst* id = to_identifier (*pos);
                Symbol* previous_parameter = function_type->find (id->identifier);
                ParameterSymbol* parameter = new ParameterSymbol (id->identifier, id->location, block.package (), parameter_type, ast.variadic);
                if (results)
                  {
                    function_type->append_result (parameter);
                  }
                else
                  {
                    function_type->append_parameter (parameter);
                  }
                if (previous_parameter != NULL)
                  {
                    error_reporter << DuplicateSymbol (parameter, previous_parameter);
                  }
              }
          }
        else
          {
            ParameterSymbol* parameter = new ParameterSymbol ("", ast.location, block.package (), parameter_type, ast.variadic);
            if (results)
              {
                function_type->append_result (parameter);
              }
            else
              {
                function_type->append_parameter (parameter);
              }
          }
      }

      void visit (InterfaceAst& ast)
      {
        interface_type = factory.make_interface_type ();
        ast.visit_children (*this);
        type = interface_type;
      }

      void visit (MethodSpecAst& ast)
      {
        assert (interface_type != NULL);
        IdentifierAst* id = to_identifier (ast.identifier);

        function_type = factory.make_function_type ();
        ast.signature->accept (*this);

        Symbol* previous_method = interface_type->find (id->identifier);
        InterfaceMethodSymbol* method = new InterfaceMethodSymbol (id->identifier, id->location, block.package (), function_type);
        interface_type->append_method (method);
        if (previous_method != NULL)
          {
            error_reporter << DuplicateSymbol (method, previous_method);
          }
      }

      void visit (AnonymousMethodSpecAst& ast)
      {
        Symbol* s = process_qualified_identifier (ast.type_name, block, error_reporter);
        if (s == NULL)
          {
            return;
          }

        TypeSymbol* ts = symbol_cast<TypeSymbol> (s);
        if (ts == NULL)
          {
            error_reporter << DoesNotReferToAType (ast.type_name->location, s->identifier);
            return;
          }

        if (!define_symbol (ts, block, factory, error_reporter))
          {
            return;
          }

        const Type* t = ts->type ();
        const InterfaceType* it = type_cast<InterfaceType> (t->underlying_type ());
        if (it == NULL)
          {
            error_reporter << DoesNotReferToAnInterface (ast.type_name->location, s->identifier);
            return;
          }

        for (InterfaceType::const_method_iterator pos = it->method_begin (), limit = it->method_end ();
             pos != limit;
             ++pos)
          {
            const InterfaceMethodSymbol* method = *pos;
            Symbol* previous_method = interface_type->find (method->identifier);
            InterfaceMethodSymbol* new_method = new InterfaceMethodSymbol (method->identifier, ast.location, block.package (), method->type);
            interface_type->append_method (new_method);
            if (previous_method != NULL)
              {
                error_reporter << DuplicateSymbol (new_method, previous_method);
              }
          }
      }

      void visit (MapAst& ast)
      {
        const Type* key_type = process_type (ast.key_type, block, factory, error_reporter);
        const Type* value_type = process_type (ast.value_type, block, factory, error_reporter);
        if (!comparable (key_type))
          {
            error_reporter << KeyTypeNotComparable (ast.location);
          }
        type = value_type->get_map (key_type);
      }
    };
  }

  const Type*
  process_type (Ast* ast,
                const Block& block,
                TypeFactory& factory,
                ErrorReporter& error_reporter)
  {
    Visitor visitor (factory, block, error_reporter);
    ast->accept (visitor);
    return visitor.type;
  }

  const FunctionType*
  process_function_type (SignatureAst* ast,
                         const Block& block,
                         TypeFactory& type_factory,
                         ErrorReporter& error_reporter)
  {
    Visitor tv (type_factory, block, error_reporter);
    tv.function_type = type_factory.make_function_type ();
    ast->accept (tv);
    return tv.function_type;
  }

}
