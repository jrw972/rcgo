#include <error.h>

#include "semantic.hpp"
#include "ast.hpp"
#include "Symbol.hpp"
#include "semantic.hpp"
#include "action.hpp"
#include "reaction.hpp"
#include "field.hpp"
#include "Callable.hpp"
#include "AstVisitor.hpp"

using namespace Type;
using namespace ast;
using namespace decl;

void
check_assignment (typed_value_t left_tv,
                  typed_value_t right_tv,
                  const Node& node,
                  const char* conversion_message,
                  const char* leak_message)
{
  assert (left_tv.type != NULL);
  assert (right_tv.type != NULL);

  if (left_tv.kind != typed_value_t::REFERENCE)
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     "target of assignment is not an lvalue (E48)");
    }

  if (left_tv.intrinsic_mutability != MUTABLE)
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     "target of assignment is not mutable (E13)");
    }

  if (right_tv.kind != typed_value_t::VALUE)
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     "source of assignment is not an rvalue (E132)");
    }

  if (!(
        Identical (left_tv.type, right_tv.type) ||
        (Type::type_cast<Type::Pointer> (type_strip(left_tv.type)) && right_tv.type == Type::Nil::Instance ())
      ))
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     conversion_message, left_tv.type->ToString ().c_str (), right_tv.type->ToString ().c_str ());
    }

  if (type_contains_pointer (left_tv.type))
    {
      if (left_tv.dereference_mutability < right_tv.dereference_mutability)
        {
          std::cout << left_tv << '\n';
          std::cout << right_tv << '\n';
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "%s", leak_message);
        }
    }
}

typed_value_t
CheckExpectReference (ast::Node* expr)
{
  unimplemented;
  // typed_value_t tv = TypeCheckExpression (expr);
  // tv.RequireReference (expr->location);
  // return tv;
}

static void
check_condition (ast::Node* condition_node)
{
  TypeCheckExpression (condition_node);
  const Type::Type* type = condition_node->type;
  if (!type_is_boolean (type))
    {
      error_at_line (-1, 0, condition_node->location.File.c_str (),
                     condition_node->location.Line,
                     "cannot convert (%s) to boolean expression in condition (E37)", type->ToString ().c_str ());
    }
}

static void
type_check_statement (Node * node)
{
  struct visitor : public ast::DefaultVisitor
  {
    void default_action (Node& node)
    {
      ast_not_reached (node);
    }

    void visit (ast_const_t& node)
    {
      ProcessDeclarations (&node);
    }

    void visit (ast_empty_statement_t& node)
    { }

    typed_value_t bind (Node& node, ast::Node* port_node, ast::Node*& reaction_node)
    {
      CheckExpectReference (port_node);
      unimplemented;
      // CheckAndImplicitlyDereference (reaction_node);

      // unimplemented;
      // // typed_value_t port_tv = port_node->typed_value;
      // // typed_value_t reaction_tv = reaction_node->typed_value;

      // // const Type::Function *push_port_type = Type::type_cast<Type::Function> (port_tv.type);

      // // if (push_port_type == NULL || push_port_type->kind != Type::Function::PUSH_PORT)
      // //   {
      // //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      // //                    "source of bind is not a port (E38)");
      // //   }

      // // const Type::Method* reaction_type = Type::type_cast<Type::Method> (reaction_tv.type);
      // // if (reaction_type == NULL || reaction_type->kind != Type::Method::REACTION)
      // //   {
      // //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      // //                    "target of bind is not a reaction (E39)");
      // //   }

      // // if (!type_is_equal (push_port_type->GetSignature (), reaction_type->signature))
      // //   {
      // //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      // //                    "cannot bind %s to %s (E40)", push_port_type->ToString ().c_str (), reaction_type->ToString ().c_str ());
      // //   }

      // // return reaction_tv;
    }

    void visit (ast_bind_push_port_statement_t& node)
    {
      bind (node, node.left (), node.right_ref ());
    }

    void visit (ast_bind_push_port_param_statement_t& node)
    {
      unimplemented;
      // typed_value_t reaction_tv = bind (node, node.left (), node.right_ref ());
      // typed_value_t param_tv = CheckAndImplicitlyDereference (node.param_ref ());
      // assert (reaction_tv.value.present);
      // reaction_t* reaction = reaction_tv.value.reaction_value ();
      // if (!reaction->has_dimension ())
      //   {
      //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      //                    "parameter specified for non-parameterized reaction (E41)");
      //   }
      // Type::Int::ValueType dimension = reaction->dimension ();

      // if (!(param_tv.type->IsInteger () || param_tv.type->IsUntyped ()))
      //   {
      //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      //                    "reaction array index must be an integer (E74)");

      //   }
      // if (param_tv.value.present)
      //   {
      //     if (!param_tv.RepresentableBy (Type::Int::Instance ()))
      //       {
      //         error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      //                        "reaction array index cannot be represented by an integer (E75)");
      //       }
      //     typed_value_t index_int_tv = param_tv.Convert (node.location, Type::Int::Instance (), node);
      //     Type::Int::ValueType v = index_int_tv.value.ref (*Type::Int::Instance ());
      //     if (v < 0)
      //       {
      //         error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      //                        "reaction array index is negative (E76)");
      //       }
      //     if (v >= dimension)
      //       {
      //         error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      //                        "reaction array index is out of range (E77)");
      //       }
      //   }
    }

    void visit (ast_bind_pull_port_statement_t& node)
    {
      unimplemented;
      // typed_value_t pull_port_tv = CheckExpectReference (node.left ());
      // typed_value_t getter_tv = CheckAndImplicitlyDereference (node.right_ref ());

      // const Type::Function* pull_port_type = type_cast<Type::Function> (pull_port_tv.type);

      // if (pull_port_type == NULL || pull_port_type->kind != Type::Function::PULL_PORT)
      //   {
      //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      //                    "target of bind is not a pull port (E42)");
      //   }

      // const Type::Method* getter_type = type_cast<Type::Method> (getter_tv.type);

      // if (getter_type == NULL)
      //   {
      //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      //                    "source of bind is not a getter (E43)");
      //   }

      // Type::Function g (Type::Function::FUNCTION, getter_type->signature, getter_type->return_parameter);
      // if (!type_is_equal (pull_port_type, &g))
      //   {
      //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      //                    "cannot bind %s to %s (E44)", pull_port_type->ToString ().c_str (), getter_type->ToString ().c_str ());
      //   }
    }

    void visit (ast_for_iota_statement_t& node)
    {
      unimplemented;
      // const std::string& identifier = ast_get_identifier (node.identifier ());
      // typed_value_t limit = process_array_dimension (node.limit_node_ref ());
      // typed_value_t zero = limit;
      // zero.zero ();
      // unimplemented;
      // Symbol* symbol = new VariableSymbol (identifier, node.identifier (), typed_value_t::make_ref (typed_value_t::make_range (zero, limit, IMMUTABLE, IMMUTABLE)));
      // node.symbol = enter_symbol (node, symbol);
      // type_check_statement (node.body ());
      // node.limit = limit;
    }

    static typed_value_t
    check_assignment_target (ast::Node* left)
    {
      typed_value_t tv = CheckExpectReference (left);
      if (tv.intrinsic_mutability != MUTABLE)
        {
          error_at_line (-1, 0, left->location.File.c_str (), left->location.Line,
                         "cannot assign to read-only location of type %s (E45)", tv.type->ToString ().c_str ());
        }

      return tv;
    }

    static void arithmetic_assign (ast_binary_t* node, const char* symbol)
    {
      unimplemented;
      // typed_value_t left_tv = check_assignment_target (node->left ());
      // typed_value_t right_tv = CheckAndImplicitlyDereference (node->right_ref ());
      // if (!type_is_equal (left_tv.type, right_tv.type))
      //   {
      //     error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
      //                    "incompatible types (%s) %s (%s) (E46)", left_tv.type->ToString ().c_str (), symbol, right_tv.type->ToString ().c_str ());
      //   }

      // struct visitor : public Type::DefaultVisitor
      // {
      //   ast::Node* node;
      //   const char* symbol;

      //   visitor (ast::Node* n, const char* s) : node (n), symbol (s) { }

      //   void visit (const NamedType& type)
      //   {
      //     type.UnderlyingType ()->Accept (*this);
      //   }

      //   void visit (const Int& type)
      //   {
      //     // Okay.
      //   }

      //   void visit (const Uint& type)
      //   {
      //     // Okay.
      //   }

      //   void default_action (const Type::Type& type)
      //   {
      //     error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
      //                    "incompatible types (%s) %s (%s) (E47)", type.ToString ().c_str (), symbol, type.ToString ().c_str ());
      //   }
      // };
      // visitor v (node, symbol);
      // left_tv.type->Accept (v);
    }

    void visit (ast_assign_statement_t& node)
    {
      typed_value_t left_tv = check_assignment_target (node.left ());
      typed_value_t right_tv = CheckAndImplicitlyDereferenceAndConvert (node.right_ref (), left_tv.type);
      check_assignment (left_tv, right_tv, node,
                        "incompatible types (%s) = (%s) (E122)",
                        "assignment leaks mutable pointers (E123)");
    }

    void visit (ast_change_statement_t& node)
    {
      unimplemented;
      // // Process the expression.
      // typed_value_t tv = CheckAndImplicitlyDereference (node.expr_ref ());
      // tv = typed_value_t::change (node.location, tv);

      // // Enter the new heap root.
      // const std::string& identifier = ast_get_identifier (node.identifier ());
      // Symbol* symbol = new VariableSymbol (identifier, &node, typed_value_t::make_ref (tv));
      // node.root_symbol = enter_symbol (node, symbol);

      // // Enter all parameters and variables in scope that are pointers as pointers to foreign.
      // node.Change ();

      // // Check the body.
      // type_check_statement (node.body ());
    }

    void visit (ast_expression_statement_t& node)
    {
      TypeCheckExpression (node.child ());
    }

    void visit (ast_if_statement_t& node)
    {
      check_condition (node.condition_ref ());
      type_check_statement (node.true_branch ());
      type_check_statement (node.false_branch ());
    }

    void visit (ast_while_statement_t& node)
    {
      check_condition (node.condition_ref ());
      type_check_statement (node.body ());
    }

    void visit (ast_add_assign_statement_t& node)
    {
      arithmetic_assign (&node, "+=");
    }

    void visit (ast_subtract_assign_statement_t& node)
    {
      arithmetic_assign (&node, "-=");
    }

    void visit (ast_list_statement_t& node)
    {
      for (Node::ConstIterator pos = node.Begin (), limit = node.End ();
           pos != limit;
           ++pos)
        {
          type_check_statement (*pos);
        }
    }

    void visit (ast_return_statement_t& node)
    {
      // Get the return symbol.
      node.return_symbol = SymbolCast<ParameterSymbol> (node.FindGlobalSymbol (ReturnSymbol));
      assert (node.return_symbol != NULL);

      // Check the expression.
      TypeCheckExpression (node.child ());
      unimplemented;
      // if (!assignable (node.child ()->type, node.return_symbol->type))
      //   {
      //     unimplemented;
      //   }

      unimplemented;
      // typed_value_t expr_tv = CheckAndImplicitlyDereferenceAndConvert (node.child_ref (), node.return_symbol->value.type);

      // // Check that it matches with the return type.
      // check_assignment (node.return_symbol->value, expr_tv, node,
      //                   "cannot convert to (%s) from (%s) in return (E124)",
      //                   "return leaks mutable pointers (E125)");
    }

    void visit (ast_increment_statement_t& node)
    {
      ast::Node* expr = node.child ();
      check_assignment_target (expr);
      struct visitor : public Type::DefaultVisitor
      {
        Node& node;

        visitor (Node& n) : node (n) { }

        void default_action (const Type::Type& type)
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "cannot increment location of type %s (E50)", type.ToString ().c_str ());
        }

        void visit (const NamedType& type)
        {
          type.UnderlyingType ()->Accept (*this);
        }

        void visit (const Int& type)
        {
          // Okay.
        }

        void visit (const Uint& type)
        {
          // Okay.
        }
      };
      visitor v (node);
      unimplemented;
      //expr->typed_value.type->Accept (v);
    }

    void visit (ast_decrement_statement_t& node)
    {
      unimplemented;
    }

    void visit (ast_activate_statement_t& node)
    {
      Node *expression_list_node = node.expr_list ();
      Node *body_node = node.body ();

      /* Check the activations. */
      TypeCheckExpression (expression_list_node);

      /* Re-insert this as a pointer to mutable. */
      node.Activate ();

      /* Check the body. */
      type_check_statement (body_node);
      node.mutable_phase_access = ComputeReceiverAccess (body_node);
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
              typed_value_t left_tv = typed_value_t::make_ref (type, MUTABLE, node.dereferenceMutability, false);
              typed_value_t right_tv = CheckAndImplicitlyDereferenceAndConvert (*init_pos, left_tv.type);
              check_assignment (left_tv, right_tv, node,
                                "incompatible types (%s) = (%s) (E126)",
                                "assignment leaks mutable pointers (E127)");
              // Convert to specified mutability.
              left_tv.intrinsic_mutability = node.mutability;
              unimplemented;
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
  };

  visitor v;
  node->Accept (v);
}

static void
control_check_statement (Node * node)
{
  struct visitor : public ast::DefaultVisitor
  {
    bool in_activation_statement;

    visitor () : in_activation_statement (false) { }

    void visit (ast_change_statement_t& node)
    {
      node.body ()->Accept (*this);
    }

    void visit (ast_if_statement_t& node)
    {
      node.true_branch ()->Accept (*this);
      node.false_branch ()->Accept (*this);
    }

    void visit (ast_while_statement_t& node)
    {
      node.body ()->Accept (*this);
    }

    void visit (ast_list_statement_t& node)
    {
      for (Node::ConstIterator pos = node.Begin (), limit = node.End ();
           pos != limit;
           ++pos)
        {
          (*pos)->Accept (*this);
        }
    }

    void visit (ast_return_statement_t& node)
    {
      // TODO: Maybe.
    }

    void visit (ast_activate_statement_t& node)
    {
      unimplemented;
      // Node::Context context = node.GetContext ();
      // Node *body_node = node.body ();

      // if (!(context == Node::Action ||
      //       context == Node::Reaction))
      //   {
      //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      //                    "activation outside of action or reaction (E53)");
      //   }

      // if (in_activation_statement)
      //   {
      //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      //                    "activations within activations are not allowed (E54)");
      //   }

      // in_activation_statement = true;
      // body_node->Accept (*this);
      // in_activation_statement = false;
    }
  };

  visitor v;
  node->Accept (v);
}

// TODO: Replace node with its symbol table.
void
enter_signature (Node& node, const Signature * type)
{
  for (Signature::ParametersType::const_iterator pos = type->Begin (), limit = type->End ();
       pos != limit; ++pos)
    {
      ParameterSymbol* x = *pos;
      // Check if the symbol is defined locally.
      const std::string& identifier = x->identifier;
      Symbol* s = node.FindLocalSymbol (identifier);
      if (s == NULL)
        {
          node.EnterSymbol (x);
        }
      else
        {
          error_at_line (-1, 0, x->definingNode->location.File.c_str (), x->definingNode->location.Line,
                         "%s is already defined in this scope (E55)",
                         identifier.c_str ());
        }
    }
}

/* Check the semantics of all executable code. */
void
process_definitions (Node * node)
{
  struct visitor : public ast::DefaultVisitor
  {
    void default_action (Node& node)
    {
      ast_not_reached (node);
    }

    void visit (ast::Type& node)
    { }

    void visit (ast_const_t& node)
    { }

    void visit (ast_action_t& node)
    {
      check_condition (node.precondition_ref ());
      node.action->precondition = node.precondition ();
      Node *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
      node.action->precondition_access = ComputeReceiverAccess (node.precondition ());
      node.action->immutable_phase_access = ComputeReceiverAccess (node.body ());

      std::cout << "TODO:  Static preconditions\n";
      // if (tv.value.present)
      //   {
      //     if (tv.value.ref (*Bool::Instance ()))
      //       {
      //         node.action->precondition_kind = Action::StaticTrue;
      //       }
      //     else
      //       {
      //         node.action->precondition_kind = Action::StaticFalse;
      //       }
      //   }
    }

    void visit (ast_dimensioned_action_t& node)
    {
      check_condition (node.precondition_ref ());
      node.action->precondition = node.precondition ();
      Node *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
      node.action->precondition_access = ComputeReceiverAccess (node.precondition ());
      node.action->immutable_phase_access = ComputeReceiverAccess (node.body ());

      std::cout << "TODO:  Static preconditions\n";
      // if (tv.value.present)
      //   {
      //     if (tv.value.ref (*Bool::Instance ()))
      //       {
      //         node.action->precondition_kind = Action::StaticTrue;
      //       }
      //     else
      //       {
      //         node.action->precondition_kind = Action::StaticFalse;
      //       }
      //   }
    }

    void visit (ast_bind_t& node)
    {
      Node *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
    }

    void visit (ast_function_t& node)
    {
      Node *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
    }

    void visit (ast_method_t& node)
    {
      Node *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
    }

    void visit (ast_initializer_t& node)
    {
      Node *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
    }

    void visit (ast_getter_t& node)
    {
      Node *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
      node.getter->immutable_phase_access = ComputeReceiverAccess (body_node);
    }

    void visit (ast_instance_t& node)
    {
      unimplemented;
    }

    void visit (ast_reaction_t& node)
    {
      Node *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
      node.reaction->immutable_phase_access = ComputeReceiverAccess (body_node);
    }

    void visit (ast_dimensioned_reaction_t& node)
    {
      Node *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
      node.reaction->immutable_phase_access = ComputeReceiverAccess (body_node);
    }

    void visit (SourceFile& node)
    {
      node.VisitChildren (*this);
    }
  };

  visitor v;
  node->Accept (v);
}
