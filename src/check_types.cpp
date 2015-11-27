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
    template <template <typename T> class S>
    static void alpha (const Type::Type* type, value_t& out, const value_t& left, const value_t& right)
    {
      assert (left.present);
      assert (right.present);
      out.present = true;

      struct visitor : public Type::DefaultVisitor {
        value_t& out;
        const value_t& left;
        const value_t& right;

        visitor (value_t& o, const value_t& l, const value_t& r) : out (o), left (l), right (r) { }

        void default_action (const Type::Type& type) {
          type_not_reached (type);
        }

        void visit (const Boolean& type) {
          out.ref (type) = S<Boolean::ValueType> () (left.ref (type), right.ref (type));
        }
      };
      visitor v (out, left, right);
      type->Accept (v);
    }

    template <template <typename S> class T>
    static void process_comparable (ast_binary_expr_t& node)
    {
      if (!(assignable (node.left ()->type, node.left ()->value, node.right ()->type) ||
            assignable (node.right ()->type, node.right ()->value, node.left ()->type))) {
        error_at_line (-1, 0, node.location.File.c_str (),
                       node.location.Line,
                       "%s and %s are not comparable (E19)",
                       node.left ()->type->ToString ().c_str (),
                       node.right ()->type->ToString ().c_str ());
      }

      if (!comparable (node.left ()->type)) {
        error_at_line (-1, 0, node.location.File.c_str (),
                       node.location.Line,
                       "%s and %s are not comparable (E19)",
                       node.left ()->type->ToString ().c_str (),
                       node.right ()->type->ToString ().c_str ());
      }

      if (node.left ()->type->IsUntyped () &&
          node.right ()->type->IsUntyped ()) {
        assert (node.left ()->type == node.right ()->type);
        assert (node.left ()->value.present);
        assert (node.right ()->value.present);

        node.type = node.left ()->type;
        alpha<T> (node.type, node.value, node.left ()->value, node.right ()->value);
        // Done.
        return;
      }

      if (node.left ()->type->IsUntyped ()) {
        // Convert to right.
        unimplemented;
      }

      if (node.right ()->type->IsUntyped ()) {
        // Convert to left.
        unimplemented;
      }

      node.type = Bool::Instance ();

      if (node.left ()->value.present &&
          node.right ()->value.present ) {
        // Do the comparison.
        unimplemented;
      }
    }

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
        else {
          node.callable = node.expr ()->callable;
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
            node.callable = &symbol;
          }

          void visit (const ParameterSymbol& symbol)
          {
            node.type = symbol.type;
          }

          void visit (const TypeSymbol& symbol)
          {
            node.type = typed_value_t (symbol.type).type;
          }

          void visit (const ConstantSymbol& symbol)
          {
            node.type = symbol.type;
            node.value = symbol.value;
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

      void visit (ast_unary_arithmetic_expr_t& node)
      {
        node.VisitChildren (*this);
        switch (node.arithmetic) {
        case LogicNot:
          {
            if (!(is_bool (node.child ()->type) ||
                  is_untyped_boolean (node.child ()->type))) {
              error_at_line (-1, 0, node.location.File.c_str (),
                             node.location.Line,
                             "! cannot be applied to %s (E19)",
                             node.child ()->type->ToString ().c_str ());

            }
            node.type = node.child ()->type;
            if (node.child ()->value.present) {
              if (is_bool (node.child ()->type)) {
                node.value.ref (*Type::Bool::Instance ()) = !node.child ()->value.ref (*Type::Bool::Instance ());
              } else {
                node.value.ref (*Type::Boolean::Instance ()) = !node.child ()->value.ref (*Type::Boolean::Instance ());
              }
              node.value.present = true;
            }
          }
          break;
        case Negate:
          unimplemented;
        }
      }

      void visit (ast_binary_arithmetic_expr_t& node)
      {
        node.VisitChildren (*this);
        switch (node.arithmetic) {
        case Multiply:
          unimplemented;
        case Divide:
          unimplemented;
        case Modulus:
          unimplemented;
        case LeftShift:
          unimplemented;
        case RightShift:
          unimplemented;
        case BitAnd:
          unimplemented;
        case BitAndNot:
          unimplemented;
        case Add:
          unimplemented;
        case Subtract:
          unimplemented;
        case BitOr:
          unimplemented;
        case BitXor:
          unimplemented;
        case Equal:
          process_comparable<Equalizer> (node);
          break;
        case NotEqual:
          process_comparable<NotEqualizer> (node);
          break;
        case LessThan:
          unimplemented;
        case LessEqual:
          unimplemented;
        case MoreThan:
          unimplemented;
        case MoreEqual:
          unimplemented;
        case LogicOr:
          {
            if (!(assignable (node.left ()->type, node.left ()->value, node.right ()->type) ||
                  assignable (node.right ()->type, node.right ()->value, node.left ()->type))) {
              error_at_line (-1, 0, node.location.File.c_str (),
                             node.location.Line,
                             "|| cannot be applied to %s and %s (E63)",
                             node.left ()->type->ToString ().c_str (),
                             node.right ()->type->ToString ().c_str ());
            }

            if (!(is_bool (node.left ()->type) ||
                  is_untyped_boolean (node.left ()->type))) {
              error_at_line (-1, 0, node.location.File.c_str (),
                             node.location.Line,
                             "|| cannot be applied to %s and %s (E65)",
                             node.left ()->type->ToString ().c_str (),
                             node.right ()->type->ToString ().c_str ());
            }

            if (node.left ()->type->IsUntyped () &&
                node.right ()->type->IsUntyped ()) {
              assert (node.left ()->type == node.right ()->type);
              assert (node.left ()->value.present);
              assert (node.right ()->value.present);

              node.type = node.left ()->type;
              node.value.present = true;
              node.value.ref (*Boolean::Instance ()) =
                node.left ()->value.ref (*Boolean::Instance ()) ||
                node.right ()->value.ref (*Boolean::Instance ());
              // Done.
              break;
            }

            if (node.left ()->type->IsUntyped ()) {
              // Convert to right.
              unimplemented;
            }

            if (node.right ()->type->IsUntyped ()) {
              // Convert to left.
              unimplemented;
            }

            node.type = node.left ()->type;

            if (node.left ()->value.present &&
                node.left ()->value.ref (*Bool::Instance ())) {
              node.value.present = true;
              node.value.ref (*Bool::Instance ()) = true;
              // Done.
              break;
            }

            if (node.left ()->value.present &&
                node.right ()->value.present) {
              node.value.present = true;
              node.value.ref (*Bool::Instance ()) = node.right ()->value.ref (*Bool::Instance ());
              // Done.
              break;
            }
          }
          break;
        case LogicAnd:
          {
            if (!(assignable (node.left ()->type, node.left ()->value, node.right ()->type) ||
                  assignable (node.right ()->type, node.right ()->value, node.left ()->type))) {
              error_at_line (-1, 0, node.location.File.c_str (),
                             node.location.Line,
                             "&& cannot be applied to %s and %s (E63)",
                             node.left ()->type->ToString ().c_str (),
                             node.right ()->type->ToString ().c_str ());
            }

            if (!(is_bool (node.left ()->type) ||
                  is_untyped_boolean (node.left ()->type))) {
              error_at_line (-1, 0, node.location.File.c_str (),
                             node.location.Line,
                             "&& cannot be applied to %s and %s (E65)",
                             node.left ()->type->ToString ().c_str (),
                             node.right ()->type->ToString ().c_str ());
            }

            if (node.left ()->type->IsUntyped () &&
                node.right ()->type->IsUntyped ()) {
              assert (node.left ()->type == node.right ()->type);
              assert (node.left ()->value.present);
              assert (node.right ()->value.present);

              node.type = node.left ()->type;
              node.value.present = true;
              node.value.ref (*Boolean::Instance ()) =
                node.left ()->value.ref (*Boolean::Instance ()) &&
                node.right ()->value.ref (*Boolean::Instance ());
              // Done.
              break;
            }

            if (node.left ()->type->IsUntyped ()) {
              // Convert to right.
              unimplemented;
            }

            if (node.right ()->type->IsUntyped ()) {
              // Convert to left.
              unimplemented;
            }

            node.type = node.left ()->type;

            if (node.left ()->value.present &&
                !node.left ()->value.ref (*Bool::Instance ())) {
              node.value.present = true;
              node.value.ref (*Bool::Instance ()) = false;
              // Done.
              break;
            }

            if (node.left ()->value.present &&
                node.right ()->value.present) {
              node.value.present = true;
              node.value.ref (*Bool::Instance ()) = node.right ()->value.ref (*Bool::Instance ());
              // Done.
              break;
            }
          }
          break;
        }
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
        // Check the arguments.
        node.expression_list ()->Accept (*this);
        check_types_arguments (node.expression_list (), node.symbol->initializer->initializerType->signature);
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

      void visit (ast_return_statement_t& node)
      {
        // Check the expression.
        node.VisitChildren (*this);

        // Get the return symbol.
        node.return_symbol = SymbolCast<ParameterSymbol> (node.FindGlobalSymbol (ReturnSymbol));
        assert (node.return_symbol != NULL);

        if (!assignable (node.child ()->type, node.child ()->value, node.return_symbol->type))
          {
            error_at_line (-1, 0, node.location.File.c_str (),
                           node.location.Line, "cannot convert %s to %s in return (E19)",
                           node.child ()->type->ToString ().c_str (), node.return_symbol->type->ToString ().c_str ());
          }
      }

      void visit (ast_if_statement_t& node)
      {
        node.VisitChildren (*this);
        const Type::Type* condition = node.condition ()->type;
        if (!(is_bool (condition) || is_untyped_boolean (condition))) {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "condition is not boolean");
        }

        if (node.condition ()->value.present && is_untyped_boolean (condition)) {
          node.condition ()->value.convert (node.condition ()->type, node.condition ()->type->DefaultType ());
          node.condition ()->type = node.condition ()->type->DefaultType ();
        }

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
                       "cannot select %s from expression of type %s (E152)",
                       identifier.c_str (), base_type->ToString ().c_str ());
      }

      void visit (ast_index_expr_t& node)
      {
        node.VisitChildren (*this);
        const Type::Type* base_type = node.base ()->type;
        node.array_type = type_cast<Array> (base_type->UnderlyingType ());
        if (node.array_type != NULL) {
          const Type::Type* index_type = node.index ()->type;
          if (!(is_integral (index_type) || is_untyped_numeric (index_type))) {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "array index is not an integer (E20)");
          }

          Node* index_node = node.index ();
          value_t& index_value = index_node->value;

          if (is_untyped_numeric (index_type)) {
            // Convert to int.
            if (!index_value.representable (index_type, &NamedInt)) {
              error_at_line (-1, 0, index_node->location.File.c_str (), index_node->location.Line,
                             "array index is not an integer (E108)");
            }

            index_value.convert (index_type, &NamedInt);
            index_node->type = &NamedInt;
            index_type = &NamedInt;
          }

          if (index_value.present) {
            // Convert to int.
            if (!index_value.representable (index_type, &NamedInt)) {
              error_at_line (-1, 0, index_node->location.File.c_str (), index_node->location.Line,
                             "array index is not an integer (E153)");
            }

            index_value.convert (index_type, &NamedInt);
            index_node->type = &NamedInt;
            index_type = &NamedInt;

            Int::ValueType x = index_value.ref (*Int::Instance ());
            if (x < 0) {
              error_at_line (-1, 0, index_node->location.File.c_str (), index_node->location.Line,
                             "array index is negative (E154)");
            }

            if (x >= node.array_type->dimension) {
              error_at_line (-1, 0, index_node->location.File.c_str (), index_node->location.Line,
                             "array index is out of range (E155)");
            }
          }

          node.type = node.array_type->Base ();
          return;
        }

        not_reached;
      }
    };
  }

  void check_types (ast::Node* root)
  {
    Visitor visitor;
    root->Accept (visitor);
  }
}
