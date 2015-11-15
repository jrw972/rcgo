#include "semantic.hpp"
#include "debug.hpp"
#include "ast.hpp"

#define QUICK_CHECK do {                        \
      if (node.typed_value.component_state) {\
        access = std::max (access, AccessRead);\
        return;\
      } \
      } while (0);

ReceiverAccess
ComputeReceiverAccess (const ast_t* node)
{
  struct visitor : public ast_const_visitor_t
  {
    ReceiverAccess access;

    visitor () : access (AccessNone) { }

    void default_action (const ast_t& node)
    {
      ast_not_reached (node);
    }

    void visit (const ast_implicit_dereference_expr_t& node)
    {
      QUICK_CHECK;
      node.visit_children (*this);
    }

    void visit (const ast_address_of_expr_t& node)
    {
      QUICK_CHECK;
      node.visit_children (*this);
    }

    void visit (const ast_index_expr_t& node)
    {
      QUICK_CHECK;
      node.base ()->accept (*this);
    }

    void visit (const ast_select_expr_t& node)
    {
      QUICK_CHECK;
      node.base ()->accept (*this);
    }

    void visit (const ast_dereference_expr_t& node)
    {
      QUICK_CHECK;
      node.visit_children (*this);
    }

    void visit (const ast_identifier_expr_t& node)
    {
      QUICK_CHECK;
    }

    void visit (const ast_list_statement_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_expression_statement_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_call_expr_t& node)
    {
      QUICK_CHECK;
      if (node.IsCall) {
        // Check if a mutable pointer escapes.
        for (ast_t::const_iterator pos = node.args ()->begin (),
               limit = node.args ()->end ();
             pos != limit;
             ++pos) {
          const typed_value_t& tv = (*pos)->typed_value;
          if (tv.component_state && type_contains_pointer (tv.type) && tv.dereference_mutability == MUTABLE) {
            access = AccessWrite;
            return;
          }
        }
        node.visit_children (*this);
      } else {
        node.args ()->accept (*this);
      }
    }

    void visit (const ast_list_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_implicit_conversion_expr_t& node)
    {
      QUICK_CHECK;
      node.visit_children (*this);
    }

    void visit (const ast_literal_expr_t& node)
    {
      QUICK_CHECK;
      node.visit_children (*this);
    }

    void visit (const ast_assign_statement_t& node)
    {
      if (node.left ()->typed_value.component_state) {
        access = AccessWrite;
        return;
      }
      // Check if a mutable pointer escapes.
      if (node.right ()->typed_value.component_state &&
          type_contains_pointer (node.right ()->typed_value.type) &&
          node.right ()->typed_value.dereference_mutability == MUTABLE) {
        access = AccessWrite;
        return;
      }
      node.left ()->accept (*this);
      node.right ()->accept (*this);
    }

    void visit (const ast_activate_statement_t& node)
    {
      node.expr_list ()->accept (*this);
      // Don't do the body.  It should be analyzed separately.
    }

    void visit (const ast_unary_arithmetic_expr_t& node)
    {
      QUICK_CHECK;
      node.visit_children (*this);
    }

    void visit (const ast_push_port_call_expr_t& node)
    {
      QUICK_CHECK;
      node.args ()->accept (*this);
    }

    void visit (const ast_binary_arithmetic_expr_t& node)
    {
      QUICK_CHECK;
      node.visit_children (*this);
    }

    void visit (const ast_return_statement_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_var_statement_t& node)
    {
      node.expression_list ()->accept (*this);
    }

    void visit (const ast_empty_statement_t& node)
    { }

    void visit (const ast_type_expr_t& node)
    { }

    void visit (const ast_increment_statement_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_change_statement_t& node)
    {
      node.expr ()->accept (*this);
      node.body ()->accept (*this);
    }

    void visit (const ast_indexed_port_call_expr_t& node)
    {
      node.args ()->accept (*this);
    }

    void visit (const ast_if_statement_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_slice_expr_t& node)
    {
      node.visit_children (*this);
    }
  };

  visitor v;
  node->accept (v);
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
// mutates_check_statement (ast_t * node)
// {
//   struct derived_visitor : public ast_visitor_t
//   {
//     bool derived_from_receiver;

//     derived_visitor () : derived_from_receiver (false) { }

//     void default_action (ast_t& node)
//     {
//       not_reached;
//     }

//     void visit (ast_implicit_dereference_expr_t& node)
//     {
//       node.visit_children (*this);
//     }

//     void visit (ast_address_of_expr_t& node)
//     {
//       node.visit_children (*this);
//     }

//     void visit (ast_index_expr_t& node)
//     {
//       node.base ()->accept (*this);
//     }

//     void visit (ast_select_expr_t& node)
//     {
//       node.base ()->accept (*this);
//     }

//     void visit (ast_dereference_expr_t& node)
//     {
//       node.visit_children (*this);
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

//   struct check_visitor : public ast_visitor_t
//   {
//     bool mutates_receiver;

//     check_visitor () : mutates_receiver (false) { }

//     void default_action (ast_t& node)
//     {
//       ast_not_reached (node);
//     }

//     void check_for_pointer_copy (ast_t* node)
//     {
//       typed_value_t tv = node->typed_value;
//       if (type_cast<Pointer> (tv.type))
//         {
//           derived_visitor v;
//           node->accept (v);
//           mutates_receiver = mutates_receiver || v.derived_from_receiver;
//         }
//     }

//     void visit (ast_var_statement_t& node)
//     {
//       node.expression_list ()->accept (*this);
//     }

//     void visit (ast_slice_expr_t& node)
//     {
//       node.visit_children (*this);
//     }

//     void visit (ast_call_expr_t& node)
//     {
//       node.expr ()->accept (*this);

//       if (type_cast<Type::Method> (node.expr ()->typed_value.type) != NULL)
//         {
//           // Invoking a method.
//           check_for_pointer_copy (node.expr ()->at (0)->at (0));
//         }

//       for (ast_t::iterator pos = node.args ()->begin (), limit = node.args ()->end ();
//            pos != limit;
//            ++pos)
//         {
//           check_for_pointer_copy (*pos);
//           (*pos)->accept (*this);
//         }
//     }

//     void visit (ast_literal_expr_t& node)
//     { }

//     void visit (ast_identifier_expr_t& node)
//     { }

//     void visit (ast_implicit_dereference_expr_t& node)
//     {
//       node.visit_children (*this);
//     }

//     void visit (ast_address_of_expr_t& node)
//     {
//       node.visit_children (*this);
//     }

//     void visit (ast_dereference_expr_t& node)
//     {
//       node.visit_children (*this);
//     }

//     void visit (ast_index_expr_t& node)
//     {
//       node.visit_children (*this);
//     }

//     void visit (ast_list_expr_t& node)
//     {
//       node.visit_children (*this);
//     }

//     void visit (ast_select_expr_t& node)
//     {
//       node.base ()->accept (*this);
//     }

//     void visit (ast_unary_arithmetic_expr_t& node)
//     {
//       node.visit_children (*this);
//     }

//     void visit (ast_binary_arithmetic_expr_t& node)
//     {
//       node.visit_children (*this);
//     }

//     void visit (ast_implicit_conversion_expr_t& node)
//     {
//       node.visit_children (*this);
//     }

//     void visit (ast_assign_statement_t& node)
//     {
//       {
//         derived_visitor v;
//         node.left ()->accept (v);
//         mutates_receiver = mutates_receiver || v.derived_from_receiver;
//       }

//       check_for_pointer_copy (node.right ());

//       node.left ()->accept (*this);
//       node.right ()->accept (*this);
//     }

//     void visit (ast_add_assign_statement_t& node)
//     {
//       derived_visitor v;
//       node.left ()->accept (v);
//       mutates_receiver = mutates_receiver || v.derived_from_receiver;

//       node.left ()->accept (*this);
//       node.right ()->accept (*this);
//     }

//     void visit (ast_increment_statement_t& node)
//     {
//       derived_visitor v;
//       node.child ()->accept (v);
//       mutates_receiver = mutates_receiver || v.derived_from_receiver;

//       node.child ()->accept (*this);
//     }

//     void visit (ast_change_statement_t& node)
//     {
//       node.expr ()->accept (*this);
//       node.body ()->accept (*this);
//     }

//     void visit (ast_expression_statement_t& node)
//     {
//       node.child ()->accept (*this);
//     }

//     void visit (ast_if_statement_t& node)
//     {
//       node.visit_children (*this);
//     }

//     void visit (ast_while_statement_t& node)
//     {
//       node.visit_children (*this);
//     }

//     void visit (ast_list_statement_t& node)
//     {
//       for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
//            pos != limit;
//            ++pos)
//         {
//           (*pos)->accept (*this);
//           if (mutates_receiver)
//             {
//               break;
//             }
//         }
//     }
//   };

//   struct visitor : public ast_visitor_t
//   {
//     void visit (ast_change_statement_t& node)
//     {
//       node.body ()->accept (*this);
//     }

//     void visit (ast_if_statement_t& node)
//     {
//       node.true_branch ()->accept (*this);
//       node.false_branch ()->accept (*this);
//     }

//     void visit (ast_while_statement_t& node)
//     {
//       node.body ()->accept (*this);
//     }

//     void visit (ast_list_statement_t& node)
//     {
//       node.visit_children (*this);
//     }

//     void visit (ast_activate_statement_t& node)
//     {
//       check_visitor v;
//       node.body ()->accept (v);
//       if (v.mutates_receiver)
//         {
//           node.activation->mode = ACTIVATION_WRITE;
//         }
//     }
//   };

//   visitor v;
//   node->accept (v);
// }
