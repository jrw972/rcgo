#include "semantic.hpp"
#include "debug.hpp"
#include "ast.hpp"
#include "AstVisitor.hpp"

using namespace ast;

#define QUICK_CHECK do {                        \
      if (node.typed_value.component_state) {\
        access = std::max (access, AccessRead);\
        return;\
      } \
      } while (0);

ReceiverAccess
ComputeReceiverAccess (const ast::Node* node)
{
  struct visitor : public DefaultConstVisitor
  {
    ReceiverAccess access;

    visitor () : access (AccessNone) { }

    void default_action (const Node& node)
    {
      ast_not_reached (node);
    }

    void visit (const ast_implicit_dereference_expr_t& node)
    {
      QUICK_CHECK;
      node.VisitChildren (*this);
    }

    void visit (const ast_address_of_expr_t& node)
    {
      QUICK_CHECK;
      node.VisitChildren (*this);
    }

    void visit (const ast_index_expr_t& node)
    {
      QUICK_CHECK;
      node.base ()->Accept (*this);
    }

    void visit (const ast_select_expr_t& node)
    {
      QUICK_CHECK;
      node.base ()->Accept (*this);
    }

    void visit (const ast_dereference_expr_t& node)
    {
      QUICK_CHECK;
      node.VisitChildren (*this);
    }

    void visit (const ast_identifier_expr_t& node)
    {
      QUICK_CHECK;
    }

    void visit (const ast_list_statement_t& node)
    {
      node.VisitChildren (*this);
    }

    void visit (const ast_expression_statement_t& node)
    {
      node.VisitChildren (*this);
    }

    void visit (const ast_call_expr_t& node)
    {
      QUICK_CHECK;
      if (node.IsCall)
        {
          // Check if a mutable pointer escapes.
          for (Node::ConstIterator pos = node.args ()->Begin (),
               limit = node.args ()->End ();
               pos != limit;
               ++pos)
            {
              const typed_value_t& tv = (*pos)->typed_value;
              if (tv.component_state && type_contains_pointer (tv.type) && tv.dereference_mutability == MUTABLE)
                {
                  access = AccessWrite;
                  return;
                }
            }
          node.VisitChildren (*this);
        }
      else
        {
          node.args ()->Accept (*this);
        }
    }

    void visit (const ast_list_expr_t& node)
    {
      node.VisitChildren (*this);
    }

    void visit (const ast_implicit_conversion_expr_t& node)
    {
      QUICK_CHECK;
      node.VisitChildren (*this);
    }

    void visit (const ast_literal_expr_t& node)
    {
      QUICK_CHECK;
      node.VisitChildren (*this);
    }

    void visit (const ast_assign_statement_t& node)
    {
      if (node.left ()->typed_value.component_state)
        {
          access = AccessWrite;
          return;
        }
      // Check if a mutable pointer escapes.
      if (node.right ()->typed_value.component_state &&
          type_contains_pointer (node.right ()->typed_value.type) &&
          node.right ()->typed_value.dereference_mutability == MUTABLE)
        {
          access = AccessWrite;
          return;
        }
      node.left ()->Accept (*this);
      node.right ()->Accept (*this);
    }

    void visit (const ast_activate_statement_t& node)
    {
      node.expr_list ()->Accept (*this);
      // Don't do the body.  It should be analyzed separately.
    }

    void visit (const ast_unary_arithmetic_expr_t& node)
    {
      QUICK_CHECK;
      node.VisitChildren (*this);
    }

    void visit (const ast_push_port_call_expr_t& node)
    {
      QUICK_CHECK;
      node.args ()->Accept (*this);
    }

    void visit (const ast_binary_arithmetic_expr_t& node)
    {
      QUICK_CHECK;
      node.VisitChildren (*this);
    }

    void visit (const ast_return_statement_t& node)
    {
      node.VisitChildren (*this);
    }

    void visit (const ast_var_statement_t& node)
    {
      node.expression_list ()->Accept (*this);
    }

    void visit (const ast_empty_statement_t& node)
    { }

    void visit (const TypeExpression& node)
    { }

    void visit (const ast_increment_statement_t& node)
    {
      node.VisitChildren (*this);
    }

    void visit (const ast_change_statement_t& node)
    {
      node.expr ()->Accept (*this);
      node.body ()->Accept (*this);
    }

    void visit (const ast_indexed_port_call_expr_t& node)
    {
      node.args ()->Accept (*this);
    }

    void visit (const ast_if_statement_t& node)
    {
      node.VisitChildren (*this);
    }

    void visit (const ast_slice_expr_t& node)
    {
      node.VisitChildren (*this);
    }
  };

  visitor v;
  node->Accept (v);
  return v.access;
}

/*
  Determine if each activation mutates the state of a component.
  This requires pointer analysis and could be improved.
  However, pointer analysis solves the halting problem so we must draw the line somewhere.

  The state of a component is only mutable in the body of a activation statement so we can exclude all other activity.
  The cases to check:
  1.  Assignment to an lvalue derived from the receiver.
  2.  Copying a pointer (to mutable) whose value is derived from the receiver.
      a.  In assignment
      a.  In a call

 */
// static void
// mutates_check_statement (Node * node)
// {
//   struct derived_visitor : public Visitor
//   {
//     bool derived_from_receiver;

//     derived_visitor () : derived_from_receiver (false) { }

//     void default_action (Node& node)
//     {
//       not_reached;
//     }

//     void visit (ast_implicit_dereference_expr_t& node)
//     {
//       node.VisitChildren (*this);
//     }

//     void visit (ast_address_of_expr_t& node)
//     {
//       node.VisitChildren (*this);
//     }

//     void visit (ast_index_expr_t& node)
//     {
//       node.base ()->Accept (*this);
//     }

//     void visit (ast_select_expr_t& node)
//     {
//       node.base ()->Accept (*this);
//     }

//     void visit (ast_dereference_expr_t& node)
//     {
//       node.VisitChildren (*this);
//     }

//     void visit (ast_identifier_expr_t& node)
//     {
//       ParameterSymbol* symbol = SymbolCast<ParameterSymbol> (node.symbol);
//       if (symbol != NULL)
//         {
//           assert (symbol->kind != ParameterSymbol::Receiver);
//           derived_from_receiver = symbol->kind == ParameterSymbol::ReceiverDuplicate;
//         }
//     }
//   };

//   struct check_visitor : public Visitor
//   {
//     bool mutates_receiver;

//     check_visitor () : mutates_receiver (false) { }

//     void default_action (Node& node)
//     {
//       ast_not_reached (node);
//     }

//     void check_for_pointer_copy (ast::Node* node)
//     {
//       typed_value_t tv = node->typed_value;
//       if (type_cast<Pointer> (tv.type))
//         {
//           derived_visitor v;
//           node->Accept (v);
//           mutates_receiver = mutates_receiver || v.derived_from_receiver;
//         }
//     }

//     void visit (ast_var_statement_t& node)
//     {
//       node.expression_list ()->Accept (*this);
//     }

//     void visit (ast_slice_expr_t& node)
//     {
//       node.VisitChildren (*this);
//     }

//     void visit (ast_call_expr_t& node)
//     {
//       node.expr ()->Accept (*this);

//       if (type_cast<Type::Method> (node.expr ()->typed_value.type) != NULL)
//         {
//           // Invoking a method.
//           check_for_pointer_copy (node.expr ()->at (0)->at (0));
//         }

//       for (Node::iterator pos = node.args ()->begin (), limit = node.args ()->end ();
//            pos != limit;
//            ++pos)
//         {
//           check_for_pointer_copy (*pos);
//           (*pos)->Accept (*this);
//         }
//     }

//     void visit (ast_literal_expr_t& node)
//     { }

//     void visit (ast_identifier_expr_t& node)
//     { }

//     void visit (ast_implicit_dereference_expr_t& node)
//     {
//       node.VisitChildren (*this);
//     }

//     void visit (ast_address_of_expr_t& node)
//     {
//       node.VisitChildren (*this);
//     }

//     void visit (ast_dereference_expr_t& node)
//     {
//       node.VisitChildren (*this);
//     }

//     void visit (ast_index_expr_t& node)
//     {
//       node.VisitChildren (*this);
//     }

//     void visit (ast_list_expr_t& node)
//     {
//       node.VisitChildren (*this);
//     }

//     void visit (ast_select_expr_t& node)
//     {
//       node.base ()->Accept (*this);
//     }

//     void visit (ast_unary_arithmetic_expr_t& node)
//     {
//       node.VisitChildren (*this);
//     }

//     void visit (ast_binary_arithmetic_expr_t& node)
//     {
//       node.VisitChildren (*this);
//     }

//     void visit (ast_implicit_conversion_expr_t& node)
//     {
//       node.VisitChildren (*this);
//     }

//     void visit (ast_assign_statement_t& node)
//     {
//       {
//         derived_visitor v;
//         node.left ()->Accept (v);
//         mutates_receiver = mutates_receiver || v.derived_from_receiver;
//       }

//       check_for_pointer_copy (node.right ());

//       node.left ()->Accept (*this);
//       node.right ()->Accept (*this);
//     }

//     void visit (ast_add_assign_statement_t& node)
//     {
//       derived_visitor v;
//       node.left ()->Accept (v);
//       mutates_receiver = mutates_receiver || v.derived_from_receiver;

//       node.left ()->Accept (*this);
//       node.right ()->Accept (*this);
//     }

//     void visit (ast_increment_statement_t& node)
//     {
//       derived_visitor v;
//       node.child ()->Accept (v);
//       mutates_receiver = mutates_receiver || v.derived_from_receiver;

//       node.child ()->Accept (*this);
//     }

//     void visit (ast_change_statement_t& node)
//     {
//       node.expr ()->Accept (*this);
//       node.body ()->Accept (*this);
//     }

//     void visit (ast_expression_statement_t& node)
//     {
//       node.child ()->Accept (*this);
//     }

//     void visit (ast_if_statement_t& node)
//     {
//       node.VisitChildren (*this);
//     }

//     void visit (ast_while_statement_t& node)
//     {
//       node.VisitChildren (*this);
//     }

//     void visit (ast_list_statement_t& node)
//     {
//       for (Node::const_iterator pos = node.begin (), limit = node.end ();
//            pos != limit;
//            ++pos)
//         {
//           (*pos)->Accept (*this);
//           if (mutates_receiver)
//             {
//               break;
//             }
//         }
//     }
//   };

//   struct visitor : public Visitor
//   {
//     void visit (ast_change_statement_t& node)
//     {
//       node.body ()->Accept (*this);
//     }

//     void visit (ast_if_statement_t& node)
//     {
//       node.true_branch ()->Accept (*this);
//       node.false_branch ()->Accept (*this);
//     }

//     void visit (ast_while_statement_t& node)
//     {
//       node.body ()->Accept (*this);
//     }

//     void visit (ast_list_statement_t& node)
//     {
//       node.VisitChildren (*this);
//     }

//     void visit (ast_activate_statement_t& node)
//     {
//       check_visitor v;
//       node.body ()->Accept (v);
//       if (v.mutates_receiver)
//         {
//           node.activation->mode = ACTIVATION_WRITE;
//         }
//     }
//   };

//   visitor v;
//   node->Accept (v);
// }
