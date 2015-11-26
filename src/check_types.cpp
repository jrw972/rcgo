#include "check_types.hpp"

#include <error.h>

#include "AstVisitor.hpp"
#include "Template.hpp"
#include "Callable.hpp"
#include "SymbolVisitor.hpp"
#include "semantic.hpp"
#include "Symbol.hpp"
#include "field.hpp"

namespace semantic
{

  using namespace ast;
  using namespace Type;

  namespace
  {
    static void check_types_arguments (ast::Node* node, const Type::Signature* signature)
    {
      ast_list_expr_t* args = ast_cast<ast_list_expr_t> (node);

      if (args->Size () != signature->Arity ())
        {
          error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                         "call expects %lu arguments but given %lu (E150)", signature->Arity (), args->Size ());
        }

      size_t i = 0;
      for (Node::ConstIterator pos = args->Begin (), limit = args->End ();
           pos != limit;
           ++pos, ++i)
        {
          const Type::Type*& arg = (*pos)->type;
          value_t& val = (*pos)->value;
          const Type::Type* param = signature->At (i)->type;
          if (!Type::assignable (arg, val, param))
            {
              error_at_line (-1, 0, (*pos)->location.File.c_str (), (*pos)->location.Line,
                             "cannot assign %s to %s in call(E151)", arg->ToString ().c_str (), param->ToString ().c_str ());
            }
        }
    }

    // Determine the type of an expression.
    struct Visitor : public ast::DefaultVisitor
    {
      void default_action (Node& node)
      {
        ast_not_reached (node);
      }

      void visit (ast_call_expr_t& node)
      {
        node.expr ()->Accept (*this);
        node.args ()->Accept (*this);

        // Collect the argument types.
        TypeList argument_types;
        ast_list_expr_t* args = ast_cast<ast_list_expr_t> (node.args ());
        for (Node::ConstIterator pos = args->Begin (), limit = args->End ();
             pos != limit;
             ++pos)
          {
            argument_types.push_back ((*pos)->type);
          }

        if (node.expr ()->temp != NULL)
          {
            node.callable = node.expr ()->temp->instantiate (argument_types);
            node.expr ()->type = node.callable->type ();
          }

        node.function_type = type_cast<Type::Function> (node.expr ()->type);
        node.method_type = type_cast<Type::Method> (node.expr ()->type);

        if (node.function_type)
          {
            node.signature = node.function_type->GetSignature ();
            node.return_parameter = node.function_type->GetReturnParameter ();
          }
        else if (node.method_type)
          {
            node.signature = node.method_type->signature;
            node.return_parameter = node.method_type->return_parameter;
          }
        else
          {
            not_reached;
          }

        check_types_arguments (args, node.signature);
        node.type = node.return_parameter->type;
      }

      void visit (ast_list_expr_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_literal_expr_t& node)
      {
        // Do nothing.  Type already set.
      }

      void visit (ast_identifier_expr_t& node)
      {
        Node *identifier_node = node.child ();
        const std::string& identifier = ast_get_identifier (identifier_node);
        node.symbol = node.FindGlobalSymbol (identifier);
        if (node.symbol == NULL)
          {
            error_at_line (-1, 0, identifier_node->location.File.c_str (),
                           identifier_node->location.Line, "%s is not defined (E18)",
                           identifier.c_str ());
          }

        struct visitor : public ConstSymbolVisitor
        {
          ast_identifier_expr_t& node;

          visitor (ast_identifier_expr_t& n)
            : node (n)
          { }

          void defaultAction (const Symbol& symbol)
          {
            not_reached;
          }

          void visit (const BuiltinFunction& symbol)
          {
            node.type = symbol.value ().type;
          }

          void visit (const ::Template& symbol)
          {
            node.type = symbol.value ().type;
            node.temp = symbol.value ().value.template_value ();
          }

          void visit (const ::Function& symbol)
          {
            node.type = symbol.type ();
          }

          void visit (const ParameterSymbol& symbol)
          {
            node.type = symbol.type;
          }

          void visit (const TypeSymbol& symbol)
          {
            node.type = typed_value_t (symbol.type).type;
          }

          void visit (const TypedConstantSymbol& symbol)
          {
            node.type = symbol.value.type;
          }

          void visit (const VariableSymbol& symbol)
          {
            node.type = symbol.type;
          }

          void visit (const HiddenSymbol& symbol)
          {
            error_at_line (-1, 0, node.location.File.c_str (),
                           node.location.Line, "%s is not accessible in this scope (E19)",
                           symbol.identifier.c_str ());
          }
        };
        visitor v (node);
        node.symbol->accept (v);
      }

      void visit (SourceFile& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast::Type& node)
      {
        // Do nothing.
      }

      void visit (ast_instance_t& node)
      {
        // Lookup the initialization function.
        InstanceSymbol* symbol = node.symbol;
        const NamedType* type = symbol->type;
        ast::Node* initializer_node = node.initializer ();
        Initializer* initializer = type->GetInitializer (ast_get_identifier (initializer_node));
        if (initializer == NULL)
          {
            error_at_line (-1, 0, initializer_node->location.File.c_str (),
                           initializer_node->location.Line,
                           "no initializer named %s (E56)",
                           ast_get_identifier (initializer_node).c_str ());
          }

        // Check the arguments.
        node.expression_list ()->Accept (*this);
        check_types_arguments (node.expression_list (), initializer->initializerType->signature);
        symbol->initializer = initializer;
      }

      void visit (ast_initializer_t& node)
      {
        node.body ()->Accept (*this);
      }

      void visit (ast_function_t& node)
      {
        node.body ()->Accept (*this);
      }

      void visit (ast_list_statement_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_expression_statement_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_var_statement_t& node)
      {
        ast::Node* identifier_list = node.identifier_list ();
        ast::Node* type_spec = node.type_spec ();
        ast::Node* expression_list = node.expression_list ();

        if (expression_list->Size () != 0 &&
            identifier_list->Size () != expression_list->Size ())
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "wrong number of initializers (E51)");
          }

        // Process the type spec.
        const Type::Type* type = process_type (type_spec, true);

        if (expression_list->Size () == 0)
          {
            // Type, no expressions.

            if (type_cast<Void> (type) != NULL)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "missing type (E52)");

              }

            // Enter each symbol.
            for (Node::Iterator id_pos = identifier_list->Begin (),
                 id_limit = identifier_list->End ();
                 id_pos != id_limit;
                 ++id_pos)
              {
                const std::string& name = ast_get_identifier (*id_pos);
                VariableSymbol* symbol = new VariableSymbol (name, *id_pos, type, node.mutability, node.dereferenceMutability);
                node.symbols.push_back (enter_symbol (*node.GetParent (), symbol));
              }

            return;
          }

        if (type_cast<Void> (type) == NULL)
          {
            // Type, expressions.

            // Enter each symbol.
            for (Node::Iterator id_pos = identifier_list->Begin (),
                 id_limit = identifier_list->End (),
                 init_pos = expression_list->Begin ();
                 id_pos != id_limit;
                 ++id_pos, ++init_pos)
              {
                // Assume left is mutable.
                unimplemented;
                // typed_value_t left_tv = typed_value_t::make_ref (type, MUTABLE, node.dereferenceMutability, false);
                // typed_value_t right_tv = CheckAndImplicitlyDereferenceAndConvert (*init_pos, left_tv.type);
                // check_assignment (left_tv, right_tv, node,
                //                   "incompatible types (%s) = (%s) (E126)",
                //                   "assignment leaks mutable pointers (E127)");
                // // Convert to specified mutability.
                // left_tv.intrinsic_mutability = node.mutability;
                // const std::string& name = ast_get_identifier (*id_pos);
                // Symbol* symbol = new VariableSymbol (name, *id_pos, left_tv);
                // node.symbols.push_back (enter_symbol (*node.GetParent (), symbol));
              }

            return;
          }

        // No type, expressions.

        // Enter each symbol.
        for (Node::Iterator id_pos = identifier_list->Begin (),
             id_limit = identifier_list->End (),
             init_pos = expression_list->Begin ();
             id_pos != id_limit;
             ++id_pos, ++init_pos)
          {
            unimplemented;
            // // Process the initializer.
            // typed_value_t right_tv = CheckAndImplicitlyDereferenceAndConvertToDefault (*init_pos);
            // typed_value_t left_tv = typed_value_t::make_ref (right_tv);
            // left_tv.intrinsic_mutability = MUTABLE;
            // left_tv.dereference_mutability = node.dereferenceMutability;
            // check_assignment (left_tv, right_tv, node,
            //                   "incompatible types (%s) = (%s) (E128)",
            //                   "assignment leaks mutable pointers (E129)");
            // // Convert to specified mutability.
            // left_tv.intrinsic_mutability = node.mutability;
            // const std::string& name = ast_get_identifier (*id_pos);
            // Symbol* symbol = new VariableSymbol (name, *id_pos, left_tv);
            // node.symbols.push_back (enter_symbol (*node.GetParent (), symbol));
          }
      }

      void visit (ast_assign_statement_t& node)
      {
        node.VisitChildren (*this);
        const Type::Type* left = node.left ()->type;
        const Type::Type*& right = node.right ()->type;
        value_t& val = node.right ()->value;
        if (!assignable (right, val, left))
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "cannot assign value of type %s to variable of type %s (E30)",
                           right->ToString ().c_str (),
                           left->ToString ().c_str ());
          }
      }

      void visit (ast_dereference_expr_t& node)
      {
        node.VisitChildren (*this);
        const Type::Type* t = node.child ()->type;
        const Pointer* p = type_cast<Pointer> (t->UnderlyingType ());
        if (p == NULL)
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "* cannot be applied to %s (E21)", t->ToString ().c_str ());
          }
        node.type = p->Base ();
      }

      void visit (ast_address_of_expr_t& node)
      {
        node.VisitChildren (*this);
        node.type = node.child ()->type->GetPointer ();
      }

      void visit (ast_select_expr_t& node)
      {
        const std::string& identifier = ast_get_identifier (node.identifier ());
        node.base ()->Accept (*this);
        const Type::Type* base_type = node.base ()->type;
        node.field = base_type->select_field (identifier);
        if (node.field != NULL)
          {
            node.type = node.field->type;
            return;
          }
        node.callable = base_type->select_callable (identifier);
        if (node.callable != NULL)
          {
            node.type = node.callable->type ();
            return;
          }

        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "cannot select %s from expression of type %s (E20)",
                       identifier.c_str (), base_type->ToString ().c_str ());
      }

    };
  }

  void check_types (ast::Node* root)
  {
    Visitor visitor;
    root->Accept (visitor);
  }
}
