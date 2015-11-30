#include "check_types.hpp"

#include <error.h>

#include "AstVisitor.hpp"
#include "Template.hpp"
#include "Callable.hpp"
#include "SymbolVisitor.hpp"
#include "semantic.hpp"
#include "Symbol.hpp"
#include "field.hpp"
#include "process_types_and_constants.hpp"
#include "Symbol.hpp"
#include "action.hpp"
#include "reaction.hpp"

namespace semantic
{

  using namespace ast;
  using namespace Type;

  namespace
  {
    static void bind (Node& node, ast::Node* port_node, ast::Node* reaction_node)
    {
      const Type::Function* push_port_type = Type::type_cast<Type::Function> (port_node->type);

      if (push_port_type == NULL || push_port_type->function_kind != Type::Function::PUSH_PORT)
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "source of bind is not a port (E38)");
        }

      const Type::Method* reaction_type = Type::type_cast<Type::Method> (reaction_node->type);
      if (reaction_type == NULL || reaction_type->method_kind != Type::Method::REACTION)
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "target of bind is not a reaction (E39)");
        }

      if (!type_is_equal (push_port_type->GetSignature (), reaction_type->signature))
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "cannot bind %s to %s (E40)", push_port_type->ToString ().c_str (), reaction_type->ToString ().c_str ());
        }
    }

    static void check_condition (Node* node)
    {
      const Type::Type* condition = node->type;
      if (!(is_bool (condition) || is_untyped_boolean (condition)))
        {
          error_at_line (-1, 0, node->location.File.c_str (),
                         node->location.Line,
                         "condition is not boolean");
        }

      if (node->value.present && is_untyped_boolean (condition))
        {
          node->value.convert (node->type, node->type->DefaultType ());
          node->type = node->type->DefaultType ();
        }
    }

    template <typename T>
    static void process_comparable (const char* s, ast_binary_expr_t& node)
    {
      if (!(assignable (node.left ()->type, node.left ()->value, node.right ()->type) ||
            assignable (node.right ()->type, node.right ()->value, node.left ()->type)))
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s cannot be applied to %s and %s (E19)",
                         s,
                         node.left ()->type->ToString ().c_str (),
                         node.right ()->type->ToString ().c_str ());
        }

      const Type::Type* in_type = Choose (node.left ()->type, node.right ()->type);

      if (!comparable (in_type))
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s and %s are not comparable (E124)",
                         node.left ()->type->ToString ().c_str (),
                         node.right ()->type->ToString ().c_str ());
        }

      if (node.left ()->type != in_type)
        {
          // Convert.
          unimplemented;
        }

      if (node.right ()->type != in_type)
        {
          // Convert.
          node.right ()->value.convert (node.right ()->type, in_type);
          node.right ()->type = in_type;
        }

      if (node.left ()->value.present &&
          node.right ()->value.present)
        {
          node.type = Boolean::Instance ();
          node.value.present = true;
          switch (in_type->underlying_kind ())
            {
            case kBool:
              node.value.bool_value_ = T () (node.left ()->value.bool_value_, node.right ()->value.bool_value_);
              break;
            case kUint8:
              node.value.uint8_value_ = T () (node.left ()->value.uint8_value_, node.right ()->value.uint8_value_);
              break;
            case kUint16:
              node.value.uint16_value_ = T () (node.left ()->value.uint16_value_, node.right ()->value.uint16_value_);
              break;
            case kUint32:
              node.value.uint32_value_ = T () (node.left ()->value.uint32_value_, node.right ()->value.uint32_value_);
              break;
            case kUint64:
              node.value.uint64_value_ = T () (node.left ()->value.uint64_value_, node.right ()->value.uint64_value_);
              break;
            case kInt8:
              node.value.int8_value_ = T () (node.left ()->value.int8_value_, node.right ()->value.int8_value_);
              break;
            case kInt16:
              node.value.int16_value_ = T () (node.left ()->value.int16_value_, node.right ()->value.int16_value_);
              break;
            case kInt32:
              node.value.int32_value_ = T () (node.left ()->value.int32_value_, node.right ()->value.int32_value_);
              break;
            case kInt64:
              node.value.int64_value_ = T () (node.left ()->value.int64_value_, node.right ()->value.int64_value_);
              break;
            case kUint:
              node.value.uint_value_ = T () (node.left ()->value.uint_value_, node.right ()->value.uint_value_);
              break;
            case kInt:
              node.value.int_value_ = T () (node.left ()->value.int_value_, node.right ()->value.int_value_);
              break;
            case kUintptr:
              node.value.uintptr_value_ = T () (node.left ()->value.uintptr_value_, node.right ()->value.uintptr_value_);
              break;
            case kBoolean:
              node.value.boolean_value_ = T () (node.left ()->value.boolean_value_, node.right ()->value.boolean_value_);
              break;
            case kRune:
              node.value.boolean_value_ = T () (node.left ()->value.rune_value_, node.right ()->value.rune_value_);
              break;
            case kInteger:
              node.value.boolean_value_ = T () (node.left ()->value.integer_value_, node.right ()->value.integer_value_);
              break;
            case kFloat:
              node.value.boolean_value_ = T () (node.left ()->value.float_value_, node.right ()->value.float_value_);
              break;
            case kComplex:
              node.value.boolean_value_ = T () (node.left ()->value.complex_value_, node.right ()->value.complex_value_);
              break;
            case kString:
              node.value.boolean_value_ = T () (node.left ()->value.string_value_, node.right ()->value.string_value_);
              break;
            default:
              type_not_reached (*in_type);
            }
        }
      else
        {
          node.type = Bool::Instance ();
        }
    }

    template <typename T>
    static void process_orderable (ast_binary_expr_t& node)
    {
      if (!(assignable (node.left ()->type, node.left ()->value, node.right ()->type) ||
            assignable (node.right ()->type, node.right ()->value, node.left ()->type)))
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s and %s are not orderable (E19)",
                         node.left ()->type->ToString ().c_str (),
                         node.right ()->type->ToString ().c_str ());
        }

      if (!orderable (node.left ()->type))
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s and %s are not orderable (E90)",
                         node.left ()->type->ToString ().c_str (),
                         node.right ()->type->ToString ().c_str ());
        }

      const Type::Type* in_type = Choose (node.left ()->type, node.right ()->type);

      if (node.left ()->type != in_type)
        {
          // Convert.
          unimplemented;
        }

      if (node.right ()->type != in_type)
        {
          // Convert.
          unimplemented;
        }

      if (node.left ()->value.present &&
          node.right ()->value.present)
        {
          node.type = Boolean::Instance ();
          node.value.present = true;
          switch (in_type->underlying_kind ())
            {
            case kUint8:
              node.value.boolean_value_ = T () (node.left ()->value.uint8_value_, node.right ()->value.uint8_value_);
              break;
            case kUint16:
              node.value.boolean_value_ = T () (node.left ()->value.uint16_value_, node.right ()->value.uint16_value_);
              break;
            case kUint32:
              node.value.boolean_value_ = T () (node.left ()->value.uint32_value_, node.right ()->value.uint32_value_);
              break;
            case kUint64:
              node.value.boolean_value_ = T () (node.left ()->value.uint64_value_, node.right ()->value.uint64_value_);
              break;
            case kInt8:
              node.value.boolean_value_ = T () (node.left ()->value.int8_value_, node.right ()->value.int8_value_);
              break;
            case kInt16:
              node.value.boolean_value_ = T () (node.left ()->value.int16_value_, node.right ()->value.int16_value_);
              break;
            case kInt32:
              node.value.boolean_value_ = T () (node.left ()->value.int32_value_, node.right ()->value.int32_value_);
              break;
            case kInt64:
              node.value.boolean_value_ = T () (node.left ()->value.int64_value_, node.right ()->value.int64_value_);
              break;
            case kUint:
              node.value.boolean_value_ = T () (node.left ()->value.uint_value_, node.right ()->value.uint_value_);
              break;
            case kInt:
              node.value.boolean_value_ = T () (node.left ()->value.int_value_, node.right ()->value.int_value_);
              break;
            case kUintptr:
              node.value.boolean_value_ = T () (node.left ()->value.uintptr_value_, node.right ()->value.uintptr_value_);
              break;
            case kRune:
              node.value.boolean_value_ = T () (node.left ()->value.rune_value_, node.right ()->value.rune_value_);
              break;
            case kInteger:
              node.value.boolean_value_ = T () (node.left ()->value.integer_value_, node.right ()->value.integer_value_);
              break;
            case kFloat:
              node.value.boolean_value_ = T () (node.left ()->value.float_value_, node.right ()->value.float_value_);
              break;
            case kComplex:
              node.value.boolean_value_ = T () (node.left ()->value.complex_value_, node.right ()->value.complex_value_);
              break;
            case kString:
              node.value.boolean_value_ = T () (node.left ()->value.string_value_, node.right ()->value.string_value_);
              break;
            default:
              type_not_reached (*in_type);
            }
        }
      else
        {
          node.type = Bool::Instance ();
        }
    }

    template <typename T>
    static void process_arithmetic (ast_binary_expr_t& node)
    {
      if (!(assignable (node.left ()->type, node.left ()->value, node.right ()->type) ||
            assignable (node.right ()->type, node.right ()->value, node.left ()->type)))
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s and %s are not arithmetic (E125)",
                         node.left ()->type->ToString ().c_str (),
                         node.right ()->type->ToString ().c_str ());
        }

      if (!arithmetic (node.left ()->type))
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s and %s are not arithmetic (E156)",
                         node.left ()->type->ToString ().c_str (),
                         node.right ()->type->ToString ().c_str ());
        }

      const Type::Type* in_type = Choose (node.left ()->type, node.right ()->type);

      if (node.left ()->type != in_type)
        {
          // Convert.
          unimplemented;
        }

      if (node.right ()->type != in_type)
        {
          // Convert.
          unimplemented;
        }

      if (node.left ()->value.present &&
          node.right ()->value.present)
        {
          node.type = in_type;
          node.value.present = true;
          switch (in_type->underlying_kind ())
            {
            case kUint8:
              node.value.uint8_value_ = T () (node.left ()->value.uint8_value_, node.right ()->value.uint8_value_);
              break;
            case kUint16:
              node.value.uint16_value_ = T () (node.left ()->value.uint16_value_, node.right ()->value.uint16_value_);
              break;
            case kUint32:
              node.value.uint32_value_ = T () (node.left ()->value.uint32_value_, node.right ()->value.uint32_value_);
              break;
            case kUint64:
              node.value.uint64_value_ = T () (node.left ()->value.uint64_value_, node.right ()->value.uint64_value_);
              break;
            case kInt8:
              node.value.int8_value_ = T () (node.left ()->value.int8_value_, node.right ()->value.int8_value_);
              break;
            case kInt16:
              node.value.int16_value_ = T () (node.left ()->value.int16_value_, node.right ()->value.int16_value_);
              break;
            case kInt32:
              node.value.int32_value_ = T () (node.left ()->value.int32_value_, node.right ()->value.int32_value_);
              break;
            case kInt64:
              node.value.int64_value_ = T () (node.left ()->value.int64_value_, node.right ()->value.int64_value_);
              break;
            case kUint:
              node.value.uint_value_ = T () (node.left ()->value.uint_value_, node.right ()->value.uint_value_);
              break;
            case kInt:
              node.value.int_value_ = T () (node.left ()->value.int_value_, node.right ()->value.int_value_);
              break;
            case kUintptr:
              node.value.uintptr_value_ = T () (node.left ()->value.uintptr_value_, node.right ()->value.uintptr_value_);
              break;
            case kRune:
              node.value.rune_value_ = T () (node.left ()->value.rune_value_, node.right ()->value.rune_value_);
              break;
            case kInteger:
              node.value.integer_value_ = T () (node.left ()->value.integer_value_, node.right ()->value.integer_value_);
              break;
            case kFloat:
              node.value.float_value_ = T () (node.left ()->value.float_value_, node.right ()->value.float_value_);
              break;
            case kComplex:
              node.value.complex_value_ = T () (node.left ()->value.complex_value_, node.right ()->value.complex_value_);
              break;
            default:
              type_not_reached (*in_type);
            }
        }
      else
        {
          node.type = in_type;
        }
    }

    template <typename T>
    static void process_integral (ast_binary_expr_t& node)
    {
      if (!(assignable (node.left ()->type, node.left ()->value, node.right ()->type) ||
            assignable (node.right ()->type, node.right ()->value, node.left ()->type)))
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s and %s are not integral (E125)",
                         node.left ()->type->ToString ().c_str (),
                         node.right ()->type->ToString ().c_str ());
        }

      if (!integral (node.left ()->type))
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s and %s are not integral (E156)",
                         node.left ()->type->ToString ().c_str (),
                         node.right ()->type->ToString ().c_str ());
        }

      const Type::Type* in_type = Choose (node.left ()->type, node.right ()->type);

      if (node.left ()->type != in_type)
        {
          // Convert.
          unimplemented;
        }

      if (node.right ()->type != in_type)
        {
          // Convert.
          unimplemented;
        }

      if (node.left ()->value.present &&
          node.right ()->value.present)
        {
          node.type = in_type;
          node.value.present = true;
          switch (in_type->underlying_kind ())
            {
            case kUint8:
              node.value.uint8_value_ = T () (node.left ()->value.uint8_value_, node.right ()->value.uint8_value_);
              break;
            case kUint16:
              node.value.uint16_value_ = T () (node.left ()->value.uint16_value_, node.right ()->value.uint16_value_);
              break;
            case kUint32:
              node.value.uint32_value_ = T () (node.left ()->value.uint32_value_, node.right ()->value.uint32_value_);
              break;
            case kUint64:
              node.value.uint64_value_ = T () (node.left ()->value.uint64_value_, node.right ()->value.uint64_value_);
              break;
            case kInt8:
              node.value.int8_value_ = T () (node.left ()->value.int8_value_, node.right ()->value.int8_value_);
              break;
            case kInt16:
              node.value.int16_value_ = T () (node.left ()->value.int16_value_, node.right ()->value.int16_value_);
              break;
            case kInt32:
              node.value.int32_value_ = T () (node.left ()->value.int32_value_, node.right ()->value.int32_value_);
              break;
            case kInt64:
              node.value.int64_value_ = T () (node.left ()->value.int64_value_, node.right ()->value.int64_value_);
              break;
            case kUint:
              node.value.uint_value_ = T () (node.left ()->value.uint_value_, node.right ()->value.uint_value_);
              break;
            case kInt:
              node.value.int_value_ = T () (node.left ()->value.int_value_, node.right ()->value.int_value_);
              break;
            case kUintptr:
              node.value.uintptr_value_ = T () (node.left ()->value.uintptr_value_, node.right ()->value.uintptr_value_);
              break;
            case kRune:
              node.value.rune_value_ = T () (node.left ()->value.rune_value_, node.right ()->value.rune_value_);
              break;
            case kInteger:
              node.value.integer_value_ = T () (node.left ()->value.integer_value_, node.right ()->value.integer_value_);
              break;
            default:
              type_not_reached (*in_type);
            }
        }
      else
        {
          node.type = in_type;
        }
    }

    template <typename T>
    static void process_shift (ast_binary_expr_t& node)
    {
      if (!(is_unsigned_integral (node.right ()->type) || is_untyped_numeric (node.right ()->type)))
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s is not integral (E156)",
                         node.right ()->type->ToString ().c_str ());
        }

      if (node.right ()->value.present)
        {
          if (!node.right ()->value.representable (node.right ()->type, Type::Uint::Instance ()))
            {
              error_at_line (-1, 0, node.location.File.c_str (),
                             node.location.Line,
                             "shift amount %s is not a uint (E156)",
                             node.right ()->type->ToString ().c_str ());
            }
          node.right ()->value.convert (node.right ()->type, Type::Uint::Instance ());
          node.right ()->type = Type::Uint::Instance ();
        }

      if (!integral (node.left ()->type))
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s is not integral (E156)",
                         node.left ()->type->ToString ().c_str ());
        }

      node.type = node.left ()->type;

      if (node.left ()->value.present &&
          node.right ()->value.present)
        {
          node.value.present = true;
          switch (node.type->underlying_kind ())
            {
            case kUint8:
              node.value.uint8_value_ = T () (node.left ()->value.uint8_value_, node.right ()->value.uint_value_);
              break;
            case kUint16:
              node.value.uint16_value_ = T () (node.left ()->value.uint16_value_, node.right ()->value.uint_value_);
              break;
            case kUint32:
              node.value.uint32_value_ = T () (node.left ()->value.uint32_value_, node.right ()->value.uint_value_);
              break;
            case kUint64:
              node.value.uint64_value_ = T () (node.left ()->value.uint64_value_, node.right ()->value.uint_value_);
              break;
            case kInt8:
              node.value.int8_value_ = T () (node.left ()->value.int8_value_, node.right ()->value.uint_value_);
              break;
            case kInt16:
              node.value.int16_value_ = T () (node.left ()->value.int16_value_, node.right ()->value.uint_value_);
              break;
            case kInt32:
              node.value.int32_value_ = T () (node.left ()->value.int32_value_, node.right ()->value.uint_value_);
              break;
            case kInt64:
              node.value.int64_value_ = T () (node.left ()->value.int64_value_, node.right ()->value.uint_value_);
              break;
            case kUint:
              node.value.uint_value_ = T () (node.left ()->value.uint_value_, node.right ()->value.uint_value_);
              break;
            case kInt:
              node.value.int_value_ = T () (node.left ()->value.int_value_, node.right ()->value.uint_value_);
              break;
            case kUintptr:
              node.value.uintptr_value_ = T () (node.left ()->value.uintptr_value_, node.right ()->value.uint_value_);
              break;
            case kRune:
              node.value.rune_value_ = T () (node.left ()->value.rune_value_, node.right ()->value.uint_value_);
              break;
            case kInteger:
              node.value.integer_value_ = T () (node.left ()->value.integer_value_, node.right ()->value.uint_value_);
              break;
            default:
              type_not_reached (*node.type);
            }
        }
    }

    // Determine the type of an expression.
    struct Visitor : public ast::DefaultVisitor
    {
      ParameterSymbol* receiver_parameter;

      Visitor () : receiver_parameter (NULL) { }

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
        Node* args = node.args ();
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
        else
          {
            node.callable = node.expr ()->callable;
          }

        node.callable->check_types (args);

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

        node.type = node.return_parameter->type;
      }

      void visit (ast_list_expr_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_literal_expr_t& node)
      {
        // Do nothing.  Type already set.
        assert (node.value.present);
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
            assert (node.value.present);
          }

          void visit (const VariableSymbol& symbol)
          {
            node.type = symbol.type;
          }

          void visit (const HiddenSymbol& symbol)
          {
            error_at_line (-1, 0, node.location.File.c_str (),
                           node.location.Line, "%s is not accessible in this scope (E157)",
                           symbol.identifier.c_str ());
          }
        };
        visitor v (node);
        node.symbol->accept (v);
      }

      void visit (ast_unary_arithmetic_expr_t& node)
      {
        node.VisitChildren (*this);
        switch (node.arithmetic)
          {
          case LogicNot:
          {
            if (!(is_bool (node.child ()->type) ||
                  is_untyped_boolean (node.child ()->type)))
              {
                error_at_line (-1, 0, node.location.File.c_str (),
                               node.location.Line,
                               "! cannot be applied to %s (E158)",
                               node.child ()->type->ToString ().c_str ());
              }
            node.type = node.child ()->type;
            if (node.child ()->value.present)
              {
                node.value.present = true;
                switch (node.type->underlying_kind ())
                  {
                  case kBool:
                    node.value.bool_value_ = !node.child ()->value.bool_value_;
                    break;
                  case kBoolean:
                    node.value.boolean_value_ = !node.child ()->value.boolean_value_;
                    break;
                  default:
                    not_reached;
                  }
              }
          }
          break;
          case Negate:
          {
            if (!(is_numeric (node.child ()->type) ||
                  is_untyped_numeric (node.child ()->type)))
              {
                error_at_line (-1, 0, node.location.File.c_str (),
                               node.location.Line,
                               "- cannot be applied to %s (E159)",
                               node.child ()->type->ToString ().c_str ());
              }
            node.type = node.child ()->type;
            if (node.child ()->value.present)
              {
                node.value.present = true;
                switch (node.type->underlying_kind ())
                  {
                  case kUint8:
                    node.value.uint8_value_ = -node.child ()->value.uint8_value_;
                    break;
                  case kUint16:
                    node.value.uint16_value_ = -node.child ()->value.uint16_value_;
                    break;
                  case kUint32:
                    node.value.uint32_value_ = -node.child ()->value.uint32_value_;
                    break;
                  case kUint64:
                    node.value.uint32_value_ = -node.child ()->value.uint32_value_;
                    break;
                  case kInt8:
                    node.value.int8_value_ = -node.child ()->value.int8_value_;
                    break;
                  case kInt16:
                    node.value.int16_value_ = -node.child ()->value.int16_value_;
                    break;
                  case kInt32:
                    node.value.int32_value_ = -node.child ()->value.int32_value_;
                    break;
                  case kInt64:
                    node.value.int64_value_ = -node.child ()->value.int64_value_;
                    break;
                  case kFloat32:
                    node.value.float32_value_ = -node.child ()->value.float32_value_;
                    break;
                  case kFloat64:
                    node.value.float64_value_ = -node.child ()->value.float64_value_;
                    break;
                  case kComplex64:
                    node.value.complex64_value_ = -node.child ()->value.complex64_value_;
                    break;
                  case kComplex128:
                    node.value.complex128_value_ = -node.child ()->value.complex128_value_;
                    break;
                  case kUint:
                    node.value.uint_value_ = -node.child ()->value.uint_value_;
                    break;
                  case kInt:
                    node.value.int_value_ = -node.child ()->value.int_value_;
                    break;
                  case kUintptr:
                    node.value.uintptr_value_ = -node.child ()->value.uintptr_value_;
                    break;
                  case kRune:
                    node.value.rune_value_ = -node.child ()->value.rune_value_;
                    break;
                  case kInteger:
                    node.value.integer_value_ = -node.child ()->value.integer_value_;
                    break;
                  case kFloat:
                    node.value.float_value_ = -node.child ()->value.float_value_;
                    break;
                  case kComplex:
                    node.value.complex_value_ = -node.child ()->value.complex_value_;
                    break;
                  default:
                    not_reached;
                  }
              }
          }
          break;
          }
      }

      void visit (ast_binary_arithmetic_expr_t& node)
      {
        node.VisitChildren (*this);
        switch (node.arithmetic)
          {
          case Multiply:
            process_arithmetic<Multiplier> (node);
            break;
          case Divide:
            process_arithmetic<Divider> (node);
            break;
          case Modulus:
            process_integral<Modulizer> (node);
            break;
          case LeftShift:
            process_shift<LeftShifter> (node);
            break;
          case RightShift:
            process_shift<RightShifter> (node);
            break;
          case BitAnd:
            process_integral<BitAnder> (node);
            break;
          case BitAndNot:
            process_integral<BitAndNotter> (node);
            break;
          case Add:
            process_arithmetic<Adder> (node);
            break;
          case Subtract:
            process_arithmetic<Subtracter> (node);
            break;
          case BitOr:
            process_integral<BitOrer> (node);
            break;
          case BitXor:
            process_integral<BitXorer> (node);
            break;
          case Equal:
            process_comparable<Equalizer> ("==", node);
            break;
          case NotEqual:
            process_comparable<NotEqualizer> ("!=", node);
            break;
          case LessThan:
            process_orderable<LessThaner> (node);
            break;
          case LessEqual:
            process_orderable<LessEqualizer> (node);
            break;
          case MoreThan:
            process_orderable<MoreThaner> (node);
            break;
          case MoreEqual:
            process_orderable<MoreEqualizer> (node);
            break;
          case LogicOr:
          {
            if (!(assignable (node.left ()->type, node.left ()->value, node.right ()->type) ||
                  assignable (node.right ()->type, node.right ()->value, node.left ()->type)))
              {
                error_at_line (-1, 0, node.location.File.c_str (),
                               node.location.Line,
                               "|| cannot be applied to %s and %s (E63)",
                               node.left ()->type->ToString ().c_str (),
                               node.right ()->type->ToString ().c_str ());
              }

            if (!(is_bool (node.left ()->type) ||
                  is_untyped_boolean (node.left ()->type)))
              {
                error_at_line (-1, 0, node.location.File.c_str (),
                               node.location.Line,
                               "|| cannot be applied to %s and %s (E65)",
                               node.left ()->type->ToString ().c_str (),
                               node.right ()->type->ToString ().c_str ());
              }

            if (node.left ()->type->IsUntyped () &&
                node.right ()->type->IsUntyped ())
              {
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

            if (node.left ()->type->IsUntyped ())
              {
                // Convert to right.
                unimplemented;
              }

            if (node.right ()->type->IsUntyped ())
              {
                // Convert to left.
                unimplemented;
              }

            node.type = node.left ()->type;

            if (node.left ()->value.present &&
                node.left ()->value.ref (*Bool::Instance ()))
              {
                node.value.present = true;
                node.value.ref (*Bool::Instance ()) = true;
                // Done.
                break;
              }

            if (node.left ()->value.present &&
                node.right ()->value.present)
              {
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
                  assignable (node.right ()->type, node.right ()->value, node.left ()->type)))
              {
                error_at_line (-1, 0, node.location.File.c_str (),
                               node.location.Line,
                               "&& cannot be applied to %s and %s (E63)",
                               node.left ()->type->ToString ().c_str (),
                               node.right ()->type->ToString ().c_str ());
              }

            if (!(is_bool (node.left ()->type) ||
                  is_untyped_boolean (node.left ()->type)))
              {
                error_at_line (-1, 0, node.location.File.c_str (),
                               node.location.Line,
                               "&& cannot be applied to %s and %s (E65)",
                               node.left ()->type->ToString ().c_str (),
                               node.right ()->type->ToString ().c_str ());
              }

            if (node.left ()->type->IsUntyped () &&
                node.right ()->type->IsUntyped ())
              {
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

            if (node.left ()->type->IsUntyped ())
              {
                // Convert to right.
                unimplemented;
              }

            if (node.right ()->type->IsUntyped ())
              {
                // Convert to left.
                unimplemented;
              }

            node.type = node.left ()->type;

            if (node.left ()->value.present &&
                !node.left ()->value.ref (*Bool::Instance ()))
              {
                node.value.present = true;
                node.value.ref (*Bool::Instance ()) = false;
                // Done.
                break;
              }

            if (node.left ()->value.present &&
                node.right ()->value.present)
              {
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
        receiver_parameter = node.initializer->initializerType->receiver_parameter;
        node.body ()->Accept (*this);
      }

      void visit (ast_action_t& node)
      {
        receiver_parameter = node.receiver_symbol;
        node.precondition ()->Accept (*this);
        check_condition (node.precondition_ref ());
        node.body ()->Accept (*this);
        node.action->precondition = node.precondition ();

        if (node.precondition ()->value.present)
          {
            if (node.precondition ()->value.bool_value_)
              {
                node.action->precondition_kind = decl::Action::StaticTrue;
              }
            else
              {
                node.action->precondition_kind = decl::Action::StaticFalse;
              }
          }
      }

      void visit (ast_reaction_t& node)
      {
        receiver_parameter = node.reaction->reaction_type->receiver_parameter;
        node.body ()->Accept (*this);
      }

      void visit (ast_bind_t& node)
      {
        node.body ()->Accept (*this);
      }

      void visit (ast_function_t& node)
      {
        node.body ()->Accept (*this);
      }

      void visit (ast_method_t& node)
      {
        receiver_parameter = node.method->methodType->receiver_parameter;
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
                           node.location.Line, "cannot convert %s to %s in return (E160)",
                           node.child ()->type->ToString ().c_str (), node.return_symbol->type->ToString ().c_str ());
          }
      }

      void visit (ast_if_statement_t& node)
      {
        node.VisitChildren (*this);
        check_condition (node.condition ());
      }

      void visit (ast_change_statement_t& node)
      {
        node.expr ()->Accept (*this);

        const Type::Type* root_type = type_change (node.expr ()->type);
        if (root_type == NULL)
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "cannot change expression of type %s (E96)", node.expr ()->type->ToString ().c_str ());
          }

        // Enter all parameters and variables in scope that are pointers as pointers to foreign.
        node.Change ();

        // Enter the new heap root.
        const std::string& identifier = ast_get_identifier (node.identifier ());
        // Don't know dereference mutability yet.
        VariableSymbol* symbol = new VariableSymbol (identifier, &node, root_type, IMMUTABLE, FOREIGN);
        node.root_symbol = enter_symbol (node, symbol);

        // Check the body.
        node.body ()->Accept (*this);
      }

      void visit (ast_activate_statement_t& node)
      {
        // Check the activations.
        node.expr_list ()->Accept (*this);
        // Re-insert this as a pointer to mutable.
        node.Activate ();
        // Check the body.
        node.body ()->Accept (*this);
      }

      void visit (ast_const_t& node)
      {
        process_types_and_constants (&node);
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
                Node* n = *init_pos;
                check_types (n);

                if (!assignable (n->type, n->value, type))
                  {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "cannot assign %s to %s in initialization (E62)", n->type->ToString ().c_str (), type->ToString ().c_str ());
                  }

                if (n->value.present &&
                    n->type->IsUntyped ())
                  {
                    n->value.convert (n->type, type);
                    n->type = type;
                  }

                const std::string& name = ast_get_identifier (*id_pos);
                VariableSymbol* symbol = new VariableSymbol (name, *id_pos, type, node.intrinsic_mutability, node.dereferenceMutability);
                node.symbols.push_back (enter_symbol (*node.GetParent (), symbol));
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

      void visit (ast_increment_statement_t& node)
      {
        node.VisitChildren (*this);
        if (!arithmetic (node.child ()->type))
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "++ cannot be applied to %s (E30)",
                           node.child ()->type->ToString ().c_str ());
          }
      }

      void visit (ast_bind_push_port_statement_t& node)
      {
        node.VisitChildren (*this);
        bind (node, node.left (), node.right_ref ());
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
        if (node.array_type != NULL)
          {
            const Type::Type* index_type = node.index ()->type;
            if (!(is_integral (index_type) || is_untyped_numeric (index_type)))
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "array index is not an integer (E20)");
              }

            Node* index_node = node.index ();
            value_t& index_value = index_node->value;

            if (is_untyped_numeric (index_type))
              {
                // Convert to int.
                if (!index_value.representable (index_type, &NamedInt))
                  {
                    error_at_line (-1, 0, index_node->location.File.c_str (), index_node->location.Line,
                                   "array index is not an integer (E108)");
                  }

                index_value.convert (index_type, &NamedInt);
                index_node->type = &NamedInt;
                index_type = &NamedInt;
              }

            if (index_value.present)
              {
                // Convert to int.
                if (!index_value.representable (index_type, &NamedInt))
                  {
                    error_at_line (-1, 0, index_node->location.File.c_str (), index_node->location.Line,
                                   "array index is not an integer (E153)");
                  }

                index_value.convert (index_type, &NamedInt);
                index_node->type = &NamedInt;
                index_type = &NamedInt;

                Int::ValueType x = index_value.ref (*Int::Instance ());
                if (x < 0)
                  {
                    error_at_line (-1, 0, index_node->location.File.c_str (), index_node->location.Line,
                                   "array index is negative (E154)");
                  }

                if (x >= node.array_type->dimension)
                  {
                    error_at_line (-1, 0, index_node->location.File.c_str (), index_node->location.Line,
                                   "array index is out of range (E155)");
                  }
              }

            node.type = node.array_type->Base ();
            return;
          }

        not_reached;
      }

      void visit (TypeExpression& node)
      {
        node.type = process_type (node.type_spec (), true);
      }

      void visit (ast_push_port_call_expr_t& node)
      {
        node.receiver_parameter = receiver_parameter;
        const std::string& port_identifier = ast_get_identifier (node.identifier ());
        const Type::Type* this_type = node.GetReceiverType ();
        const Type::Function* push_port_type = Type::type_cast<Type::Function> (type_select (this_type, port_identifier));
        if (push_port_type == NULL || push_port_type->function_kind != Type::Function::PUSH_PORT)
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "no port named %s (E34)", port_identifier.c_str ());
          }

        node.args ()->Accept (*this);
        check_types_arguments (node.args (), push_port_type->GetSignature ());
        node.field = type_select_field (this_type, port_identifier);
      }
    };
  }

  void check_types_arguments (ast::Node* node, const Type::Signature* signature)
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

  void check_types (ast::Node* root)
  {
    Visitor visitor;
    root->Accept (visitor);
  }
}
