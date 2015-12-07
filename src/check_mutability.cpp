#include "check_mutability.hpp"

#include <error.h>

#include "AstVisitor.hpp"
#include "Symbol.hpp"
#include "SymbolVisitor.hpp"
#include "Type.hpp"

/*
  A reference has:
  - a type
  - a region
  - an intrinsic mutability
  - a dereference mutability
  - the value of the data to which this reference refers (possibly)

  A value has:
  - a type
  - an intrinsic mutability
  - a dereference mutability
  - an actual value (possibly)

  The four operations that contributed to the "const correctness" of
  the language are implicit dereference, dereference (*), address of
  (&), and assignment (=).

  Implicit Dereference
  --------------------

  This operation converts a reference to a value.  The type,
  intrinsic mutability, dereference mutability, and actual value of
  the reference are passed on to the value.

  | REFERENCE | VALUE |
  -----------------------+-----------+-------+
  Intrinsic Mutability |         A |     A |
  Dereference Mutability |         B |     B |


  Dereference (*)
  ---------------

  This operation converts a value to a reference and must be applied
  to a pointer.  The region of the reference is assumed to be the
  heap.  (This is points-to analysis which is undecidable in the
  general case.)  The intrinsic mutability and dereference
  mutability of the reference is determined by dereference
  mutability of the value.  The only actual values for a value with
  pointer type is nil but this could be improved.  Similarly, it is
  not defined how the actual value passes to the reference.

  | VALUE | REFERENCE |
  -----------------------+-------+-----------+
  Intrinsic Mutability |     A |         B |
  Dereference Mutability |     B |         B |


  Address Of (&)
  --------------

  This operation converts a reference to a pointer value.  The type
  and mutability of the pointer is determined by the mutability of
  the reference.

  | REFERENCE | VALUE |
  -----------------------+-----------+-------+
  Intrinsic Mutability |         A |     A |
  Dereference Mutability |         B |     B |

  Assignment (=)
  --------------

  Assignment must check if a value can be copied to the location
  indicated by a reference.  The two simple checks are (1) the types
  must be equivalent and (2) the reference should be mutable.  (All
  references to constant data should be immutable.)

  If the type being assigned does not contain a pointer, then no
  further checks are necessary as new references cannot be generated
  from the value.  However, if the type being assigned does contain
  a pointer, then we must be careful to ensure that any generated
  reference has the correct mutability.  To this end, we check the
  dereference mutability of the reference with the intrinsic
  mutability of the value.

  MUTABLE | IMMUTABLE | FOREIGN (VALUE)
  +---------+-----------+--------
  MUTABLE     |   OK    |     NO    |    NO
  IMMUTABLE   |   OK    |     OK    |    NO
  FOREIGN     |   OK    |     OK    |    OK
  (REFERENCE)

  If the value has FOREIGN dereference mutability then we require that
  the reference refer to the STACK.

  (This check is currently checked externally.)

  Port Calls and Getters
  ----------------------

  Getters (component methods) can only be defined on immutable
  components and can only be called in the immutable phase.  There
  is no restriction on the values going in but the return values
  must have FOREIGN intrinsic mutability.  This allows a component
  to return pointers but prevents them from being saved in the heap.

  Parameters to ports and reactions must have FOREIGN dereference
  mutability if the associated type contains a pointer.
*/

namespace semantic
{
using namespace ast;
using namespace Type;

namespace
{
void fix (Node& node)
{
  if (node.type->underlying_kind () == Type::kStringU)
    {
      node.dereference_mutability = std::max (node.dereference_mutability, IMMUTABLE);
    }
}

// Compute and check the intrinsic and dereference mutability of the expression.
struct MutabilityVisitor : public ast::DefaultVisitor
{
  void default_action (Node& node)
  {
    ast_not_reached (node);
  }

  void visit (ast_call_expr_t& node)
  {
    if (node.expr ()->expression_kind == kType)
      {
        node.args ()->Accept (*this);
        // Conversion.
        node.intrinsic_mutability = IMMUTABLE;
        node.dereference_mutability = node.args ()->At (0)->dereference_mutability;
        if (node.reset_mutability)
          {
            node.dereference_mutability = MUTABLE;
          }
        fix (node);
        return;
      }

    node.VisitChildren (*this);

    if (node.callable != NULL)
      {
        node.callable->check_mutability (node.args ());
      }
    else
      {
        check_mutability_arguments (node.args (), node.signature);
      }
    node.intrinsic_mutability = IMMUTABLE;
    node.dereference_mutability = node.return_parameter->dereference_mutability;
    fix (node);
  }

  void visit (ast_list_expr_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (ast_literal_expr_t& node)
  {
    node.intrinsic_mutability = IMMUTABLE;
    node.dereference_mutability = IMMUTABLE;
    fix (node);
  }

  void visit (ast_identifier_expr_t& node)
  {
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
        node.intrinsic_mutability = IMMUTABLE;
        node.dereference_mutability = IMMUTABLE;
        fix (node);
      }

      void visit (const ::Template& symbol)
      {
        node.intrinsic_mutability = IMMUTABLE;
        node.dereference_mutability = IMMUTABLE;
        fix (node);
      }

      void visit (const ::Function& symbol)
      {
        node.intrinsic_mutability = IMMUTABLE;
        node.dereference_mutability = IMMUTABLE;
        fix (node);
      }

      void visit (const ParameterSymbol& symbol)
      {
        node.intrinsic_mutability = symbol.intrinsic_mutability;
        node.dereference_mutability = symbol.dereference_mutability;
        fix (node);
      }

      void visit (const TypeSymbol& symbol)
      {
        // Do nothing.
      }

      void visit (const ConstantSymbol& symbol)
      {
        node.intrinsic_mutability = IMMUTABLE;
        node.dereference_mutability = IMMUTABLE;
        fix (node);
      }

      void visit (const VariableSymbol& symbol)
      {
        node.intrinsic_mutability = symbol.intrinsic_mutability;
        node.dereference_mutability = symbol.dereference_mutability;
        fix (node);
      }

      void visit (const HiddenSymbol& symbol)
      {
        unimplemented;
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
    node.expression_list ()->Accept (*this);
    check_mutability_arguments (node.expression_list (), node.symbol->initializer->initializerType->signature);
  }

  void visit (ast_initializer_t& node)
  {
    node.body ()->Accept (*this);
  }

  void visit (ast_getter_t& node)
  {
    node.body ()->Accept (*this);
  }

  void visit (ast_action_t& node)
  {
    node.precondition ()->Accept (*this);
    node.body ()->Accept (*this);
  }

  void visit (ast_dimensioned_action_t& node)
  {
    node.precondition ()->Accept (*this);
    node.body ()->Accept (*this);
  }

  void visit (ast_reaction_t& node)
  {
    node.body ()->Accept (*this);
  }

  void visit (ast_dimensioned_reaction_t& node)
  {
    node.body ()->Accept (*this);
  }

  void visit (ast_bind_t& node)
  {
    // Do nothing.
  }

  void visit (ast_function_t& node)
  {
    node.body ()->Accept (*this);
  }

  void visit (ast_method_t& node)
  {
    node.body ()->Accept (*this);
  }

  void visit (ast_const_t& node)
  {
    // Do nothing.
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
    node.VisitChildren (*this);
    if (type_contains_pointer (node.child ()->type) &&
        node.return_symbol->dereference_mutability < node.child ()->dereference_mutability)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "return casts away +const or +foreign (E149)");
      }
  }

  void visit (ast_if_statement_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (ast_while_statement_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (ast_for_iota_statement_t& node)
  {
    node.body ()->Accept (*this);
  }

  void visit (ast_empty_statement_t& node)
  {
    // Do nothing.
  }

  void visit (ast_var_statement_t& node)
  {
    if (!node.expression_list ()->Empty ())
      {
        node.expression_list ()->Accept (*this);
        size_t idx = 0;
        for (Node::ConstIterator pos = node.expression_list ()->Begin (), limit = node.expression_list ()->End ();
             pos != limit;
             ++pos, ++idx)
          {
            Node* n = *pos;
            VariableSymbol* symbol = node.symbols[idx];

            if (type_contains_pointer (n->type) &&
                symbol->dereference_mutability < n->dereference_mutability)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "assignment casts away +const or +foreign (E92)");
              }
          }
      }
  }

  void visit (ast_assign_statement_t& node)
  {
    node.VisitChildren (*this);
    if (node.left ()->intrinsic_mutability != MUTABLE)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "target of assignment is not mutable (E86)");
      }

    if (type_contains_pointer (node.right ()->type) &&
        node.left ()->dereference_mutability < node.right ()->dereference_mutability)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "assignment casts away +const or +foreign (E161)");
      }
  }

  void visit (ast_add_assign_statement_t& node)
  {
    node.VisitChildren (*this);
    if (node.left ()->intrinsic_mutability != MUTABLE)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "target of assignment is not mutable (E15)");
      }
  }

  void visit (ast_increment_statement_t& node)
  {
    node.VisitChildren (*this);
    if (node.child ()->intrinsic_mutability != MUTABLE)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "target of increment is not mutable (E177)");
      }
  }

  void visit (ast_change_statement_t& node)
  {
    node.expr ()->Accept (*this);
    node.root_symbol->dereference_mutability = node.expr ()->dereference_mutability;
    node.body ()->Accept (*this);
  }

  void visit (ast_activate_statement_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (ast_dereference_expr_t& node)
  {
    node.VisitChildren (*this);
    node.intrinsic_mutability = node.child ()->dereference_mutability;
    node.dereference_mutability = node.child ()->dereference_mutability;
    fix (node);
  }

  void visit (ast_address_of_expr_t& node)
  {
    node.VisitChildren (*this);
    node.intrinsic_mutability = node.child ()->intrinsic_mutability;
    node.dereference_mutability = std::max (node.child ()->intrinsic_mutability, node.child ()->dereference_mutability);
    fix (node);
  }

  void visit (ast_select_expr_t& node)
  {
    node.base ()->Accept (*this);

    if (type_dereference (node.base ()->type))
      {
        node.intrinsic_mutability = node.base ()->dereference_mutability;
        node.dereference_mutability = node.base ()->dereference_mutability;
        fix (node);
      }
    else
      {
        node.intrinsic_mutability = node.base ()->intrinsic_mutability;
        node.dereference_mutability = node.base ()->dereference_mutability;
        fix (node);
      }
  }

  void visit (ast_index_expr_t& node)
  {
    node.VisitChildren (*this);
    if (node.array_type != NULL)
      {
        node.intrinsic_mutability = node.base ()->intrinsic_mutability;
        node.dereference_mutability = node.base ()->dereference_mutability;
        fix (node);
        return;
      }
    if (node.slice_type != NULL)
      {
        node.intrinsic_mutability = node.base ()->intrinsic_mutability;
        node.dereference_mutability = node.base ()->dereference_mutability;
        fix (node);
        return;
      }
    not_reached;
  }

  void visit (ast_slice_expr_t& node)
  {
    node.VisitChildren (*this);
    if (node.array_type != NULL)
      {
        node.intrinsic_mutability = IMMUTABLE;
        node.dereference_mutability = node.base ()->dereference_mutability;
        fix (node);
        return;
      }
    if (node.slice_type != NULL)
      {
        node.intrinsic_mutability = IMMUTABLE;
        node.dereference_mutability = node.base ()->dereference_mutability;
        fix (node);
        return;
      }
    not_reached;
  }

  void visit (ast_unary_arithmetic_expr_t& node)
  {
    node.VisitChildren (*this);
    node.intrinsic_mutability = IMMUTABLE;
    node.dereference_mutability = IMMUTABLE;
    fix (node);
  }

  void visit (ast_binary_arithmetic_expr_t& node)
  {
    node.VisitChildren (*this);
    node.intrinsic_mutability = IMMUTABLE;
    node.dereference_mutability = IMMUTABLE;
    fix (node);
  }

  void visit (TypeExpression& node)
  {
    // Do nothing.
  }

  void visit (ast_push_port_call_expr_t& node)
  {
    node.args ()->Accept (*this);
  }

  void visit (ast_indexed_port_call_expr_t& node)
  {
    node.args ()->Accept (*this);
    node.index ()->Accept (*this);
  }
};

}

void check_mutability_arguments (ast::Node* node, const Type::Signature* signature)
{
  ast_list_expr_t* args = ast_cast<ast_list_expr_t> (node);

  size_t i = 0;
  for (Node::ConstIterator pos = args->Begin (), limit = args->End ();
       pos != limit;
       ++pos, ++i)
    {
      const Type::Type* arg = (*pos)->type;
      if (type_contains_pointer (arg))
        {
          if (signature->At (i)->dereference_mutability < (*pos)->dereference_mutability)
            {
              error_at_line (-1, 0, (*pos)->location.File.c_str (), (*pos)->location.Line,
                             "argument %zd casts away +const or +foreign (E85)", i + 1);
            }
        }
    }
}

void check_mutability (ast::Node* root)
{
  MutabilityVisitor visitor;
  root->Accept (visitor);
}
}
