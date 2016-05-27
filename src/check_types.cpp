#include "check_types.hpp"

#include <error.h>

#include "node.hpp"
#include "node_visitor.hpp"
#include "node_cast.hpp"
#include "template.hpp"
#include "callable.hpp"
#include "symbol_visitor.hpp"
#include "semantic.hpp"
#include "symbol.hpp"
#include "field.hpp"
#include "process_types_and_constants.hpp"
#include "symbol.hpp"
#include "action.hpp"
#include "reaction.hpp"
#include "bind.hpp"
#include "parameter_list.hpp"
#include "builtin_function.hpp"
#include "error_reporter.hpp"

namespace semantic
{
using namespace util;
using namespace ast;
using namespace type;
using namespace decl;

namespace
{

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

static void require_value_or_variable (const Node* node)
{
  assert (!node->eval.is_unknown ());
  if (!node->eval.is_value_or_variable ())
    {
      error_at_line (-1, 0, node->location.file.c_str (), node->location.line,
                     "required a value (E78)");
    }
}

static void require_variable (const Node* node)
{
  assert (node->eval.expression_kind != UnknownExpressionKind);
  if (!(node->eval.expression_kind == VariableExpressionKind))
    {
      error_at_line (-1, 0, node->location.file.c_str (), node->location.line,
                     "required a variable (E2)");
    }
}

static const decl::Reaction* bind (Node& node, ast::Node* port_node, ast::Node* reaction_node)
{
  const type::Function* push_port_type = type::type_cast<type::Function> (port_node->eval.type);

  if (push_port_type == NULL || push_port_type->function_kind != type::Function::PUSH_PORT)
    {
      error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                     "source of bind is not a port (E38)");
    }

  require_variable (port_node);

  const type::Method* reaction_type = type::type_cast<type::Method> (reaction_node->eval.type);
  if (reaction_type == NULL || reaction_type->method_kind != type::Method::REACTION)
    {
      error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                     "target of bind is not a reaction (E39)");
    }

  require_variable (reaction_node);

  type::Function f (type::Function::PUSH_PORT, reaction_type->parameter_list, reaction_type->return_parameter_list);
  if (!are_identical (push_port_type, &f))
    {
      error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                     "cannot bind %s to %s (E40)", push_port_type->to_string ().c_str (), reaction_type->to_string ().c_str ());
    }

  return static_cast<const decl::Reaction*> (reaction_node->callable);
}

static void check_condition (Node* node)
{
  const type::Type* condition = node->eval.type;
  if (!(is_any_boolean (condition)))
    {
      error_at_line (-1, 0,
                     node->location.file.c_str (),
                     node->location.line,
                     "condition is not boolean (E155)");
    }
  node->eval.convert (condition->default_type ());
  require_value_or_variable (node);
}

// Determine the type of an expression.
struct Visitor : public ast::DefaultNodeVisitor
{
  enum Context
  {
    Other,
    Action,
    Reaction,
    Initializer,
    Getter,
  };

  ErrorReporter& er;
  SymbolTable& symtab;
  ParameterSymbol* receiver_parameter;
  Context context;
  bool in_mutable_phase;

  Visitor (ErrorReporter& a_er,
           SymbolTable& st)
    : er (a_er)
    , symtab (st)
    , receiver_parameter (NULL)
    , context (Other)
    , in_mutable_phase (false)
  { }

  void default_action (Node& node)
  {
    AST_NOT_REACHED (node);
  }

  void visit (CallExpr& node)
  {
    node.visit_children (*this);
    assert (node.expr->eval.expression_kind != UnknownExpressionKind);
    if (node.expr->eval.expression_kind == TypeExpressionKind)
      {
        // Conversion.
        if (node.args->size () != 1)
          {
            error_at_line (-1, 0, node.location.file.c_str (),
                           node.location.line,
                           "conversion requires one argument (E8)");

          }

        conversion (node, node.expr, node.args->at (0));
        return;
      }

    // Collect the arguments.
    ExpressionValueList arguments;
    List* args = node.args;
    for (List::ConstIterator pos = args->begin (), limit = args->end ();
         pos != limit;
         ++pos)
      {
        arguments.push_back ((*pos)->eval);
      }

    if (node.expr->temp != NULL)
      {
        node.temp = node.expr->temp;
        node.temp->check (er, node.location, node.eval, arguments);
        List::ConstIterator pos, limit;
        ExpressionValueList::const_iterator pos2;
        for (pos = args->begin (), limit = args->end (), pos2 = arguments.begin ();
             pos != limit;
             ++pos, ++pos2)
          {
            (*pos)->eval = *pos2;
          }

        return;
      }
    else
      {
        node.callable = node.expr->callable;
      }

    node.function_type = type_cast<type::Function> (node.expr->eval.type);
    node.method_type = type_cast<type::Method> (node.expr->eval.type);

    if (node.function_type)
      {
        switch (node.function_type->function_kind)
          {
          case type::Function::FUNCTION:
            // No restrictions on caller.
            break;

          case type::Function::PUSH_PORT:
            error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                           "push ports cannot be called (E202)");
            break;

          case type::Function::PULL_PORT:
            // Must be called from either a getter, an action, or reaction.
            if (!(context == Getter ||
                  context == Action ||
                  context == Reaction))
              {
                error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                               "pull ports may only be called from a getter, an action, or a reaction (E201)");
              }
            if (in_mutable_phase)
              {
                error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                               "cannot call pull port in mutable section (E198)");
              }
            break;
          }

        node.signature = node.function_type->parameter_list;
        node.return_parameter = node.function_type->GetReturnParameter ();
      }
    else if (node.method_type)
      {
        switch (node.method_type->method_kind)
          {
          case type::Method::METHOD:
            // No restrictions on caller.
            break;
          case type::Method::INITIALIZER:
            // Caller must be an initializer.
            if (context != Initializer)
              {
                error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                               "initializers may only be called from initializers (E197)");
              }
            break;
          case type::Method::GETTER:
          {
            // Must be called from either a getter, action, reaction, or initializer.
            if (!(context == Getter ||
                  context == Action ||
                  context == Reaction ||
                  context == Initializer))
              {
                error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                               "getters may only be called from a getter, an action, a reaction, or an initializer (E196)");
              }
            if (in_mutable_phase)
              {
                error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                               "cannot call getter in mutable section (E34)");
              }
          }
          break;
          case type::Method::REACTION:
          {
            UNIMPLEMENTED;
          }
          break;
          }

        node.signature = node.method_type->parameter_list;
        node.return_parameter = node.method_type->return_parameter_list->at (0);
      }
    else
      {
        NOT_REACHED;
      }

    if (node.callable != NULL)
      {
        require_value_or_variable (node.expr);
        // TODO:  Merge check_references into check_types.
        node.callable->check_types (args);
        node.callable->check_references (node.args);
        node.callable->check_mutability (node.args);
      }
    else
      {
        node.field = node.expr->field;
        check_types_arguments (args, node.signature);
        require_value_or_variable (node.expr);
        require_value_or_variable_list (node.args);
        check_mutability_arguments (node.args, node.signature);
      }

    node.eval.type = node.return_parameter->type;
    node.eval.expression_kind = ValueExpressionKind;
    node.eval.intrinsic_mutability = Immutable;
    node.eval.indirection_mutability = node.return_parameter->dereference_mutability;
    node.eval.fix_string_indirection_mutability ();
  }

  void conversion (Node& node,
                   Node* type_expr,
                   Node* expr)
  {
    if (!require_type (er, node.location, node.eval, type_expr->eval))
      {
        return;
      }
    require_value_or_variable (expr);

    const type::Type* to = type_expr->eval.type;
    const type::Type*& from = expr->eval.type;
    Value& x = expr->eval.value;

    if (x.present)
      {
        if (x.representable (from, to))
          {
            UNIMPLEMENTED;
          }
        else if (is_typed_float (from) && is_typed_float (to))
          {
            UNIMPLEMENTED;
          }
        else if (is_typed_integer (from) && is_any_string (to))
          {
            UNIMPLEMENTED;
            node.reset_mutability = true;
          }
        else if (is_any_string (from) && is_slice_of_bytes (to))
          {
            if (from->is_untyped ())
              {
                x.convert (from, from->default_type ());
                from = from->default_type ();
              }
            node.reset_mutability = true;
          }
        else
          {
            error_at_line (-1, 0, node.location.file.c_str (),
                           node.location.line,
                           "illegal conversion (E156)");
          }
      }
    else
      {
        if (assignable (from, x, to))
          {
            // Okay.
          }
        else if (are_identical (from->underlying_type (), to->underlying_type ()))
          {
            // Okay.
          }
        else if (from->to_pointer () &&
                 to->to_pointer () &&
                 are_identical (from->to_pointer (), to->to_pointer ()))
          {
            // Okay.
          }
        else if ((is_typed_integer (from) || is_typed_float (from)) &&
                 (is_typed_integer (to) || is_typed_float (to)))
          {
            // Okay.
          }
        else if (is_typed_complex (from) && is_typed_complex (to))
          {
            // Okay.
          }
        else if ((is_typed_integer (from) || is_slice_of_bytes (from) || is_slice_of_runes (from)) && is_typed_string (to))
          {
            // Okay.
            node.reset_mutability = true;
          }
        else if (is_typed_string (from) && (is_slice_of_bytes (to) || is_slice_of_runes (to)))
          {
            // Okay.
            node.reset_mutability = true;
          }
        else
          {
            error_at_line (-1, 0, node.location.file.c_str (),
                           node.location.line,
                           "illegal conversion (E122)");
          }
      }

    node.eval.type = to;
    node.eval.expression_kind = ValueExpressionKind;
    node.eval.intrinsic_mutability = Immutable;
    node.eval.indirection_mutability = expr->eval.indirection_mutability;
    if (node.reset_mutability)
      {
        node.eval.indirection_mutability = Mutable;
      }
    node.eval.fix_string_indirection_mutability ();
  }

  void visit (ConversionExpr& node)
  {
    node.visit_children (*this);
    conversion (node, node.type_expr, node.expr);
  }

  void visit (ListExpr& node)
  {
    node.visit_children (*this);
  }

  void visit (LiteralExpr& node)
  {
    assert (node.eval.value.present);
    node.eval.expression_kind = ValueExpressionKind;
    node.eval.intrinsic_mutability = Immutable;
    node.eval.indirection_mutability = Immutable;
  }

  void visit (IdentifierExpr& node)
  {
    Identifier* identifier_node = node.child;
    const std::string& identifier = identifier_node->identifier;
    node.symbol = symtab.find_global_symbol (identifier);
    if (node.symbol == NULL)
      {
        er.undefined (node.location, identifier);
        node.eval.expression_kind = ErrorExpressionKind;
        return;
      }

    node.eval.type = node.symbol->symbol_type ();

    struct visitor : public ConstSymbolVisitor
    {
      IdentifierExpr& node;
      ErrorReporter& er;

      visitor (IdentifierExpr& n,
               ErrorReporter& a_er)
        : node (n)
        , er (a_er)
      { }

      void default_action (const Symbol& symbol)
      {
        NOT_REACHED;
      }

      void visit (const BuiltinFunction& symbol)
      {
        node.callable = &symbol;
        node.eval.expression_kind = ValueExpressionKind;
        node.eval.intrinsic_mutability = Immutable;
        node.eval.indirection_mutability = Immutable;
      }

      void visit (const decl::TemplateSymbol& symbol)
      {
        node.temp = &symbol;
        node.eval.expression_kind = ValueExpressionKind;
        node.eval.intrinsic_mutability = Immutable;
        node.eval.indirection_mutability = Immutable;
      }

      void visit (const decl::Function& symbol)
      {
        node.callable = &symbol;
        node.eval.expression_kind = ValueExpressionKind;
        node.eval.intrinsic_mutability = Immutable;
        node.eval.indirection_mutability = Immutable;
      }

      void visit (const ParameterSymbol& symbol)
      {
        node.eval.expression_kind = VariableExpressionKind;
        node.eval.intrinsic_mutability = symbol.intrinsic_mutability;
        node.eval.indirection_mutability = symbol.dereference_mutability;
      }

      void visit (const TypeSymbol& symbol)
      {
        node.eval.expression_kind = TypeExpressionKind;
      }

      void visit (const ConstantSymbol& symbol)
      {
        node.eval.value = symbol.value;
        assert (node.eval.value.present);
        node.eval.expression_kind = ValueExpressionKind;
        node.eval.intrinsic_mutability = Immutable;
        node.eval.indirection_mutability = Mutable;
      }

      void visit (const VariableSymbol& symbol)
      {
        node.eval.expression_kind = VariableExpressionKind;
        node.eval.intrinsic_mutability = symbol.intrinsic_mutability;
        node.eval.indirection_mutability = symbol.dereference_mutability;
      }

      void visit (const HiddenSymbol& symbol)
      {
        er.hidden (symbol.location, symbol.identifier);
        node.eval.expression_kind = ErrorExpressionKind;
      }
    };
    visitor v (node, er);
    node.symbol->accept (v);
    node.eval.fix_string_indirection_mutability ();
  }

  void visit (UnaryArithmeticExpr& node)
  {
    node.visit_children (*this);

    if (node.child->eval.expression_kind == ErrorExpressionKind)
      {
        node.eval.expression_kind = ErrorExpressionKind;
        return;
      }

    ExpressionValueList evals;
    evals.push_back (node.child->eval);

    node.temp->check (er, node.location, node.eval, evals);
  }

  void visit (BinaryArithmeticExpr& node)
  {
    node.visit_children (*this);

    if (node.left->eval.expression_kind == ErrorExpressionKind ||
        node.right->eval.expression_kind == ErrorExpressionKind)
      {
        node.eval.expression_kind = ErrorExpressionKind;
        return;
      }

    ExpressionValueList evals;
    evals.push_back (node.left->eval);
    evals.push_back (node.right->eval);

    node.temp->check (er, node.location, node.eval, evals);

    node.left->eval = evals[0];
    node.right->eval = evals[1];
  }

  void visit (SourceFile& node)
  {
    node.visit_children (*this);
  }

  void visit (ast::Type& node)
  {
    // Do nothing.
  }

  void visit (Instance& node)
  {
    // Check the arguments.
    node.expression_list->accept (*this);
    check_types_arguments (node.expression_list, node.symbol->initializer->initializerType->parameter_list);
    require_value_or_variable_list (node.expression_list);
    check_mutability_arguments (node.expression_list, node.symbol->initializer->initializerType->parameter_list);
  }

  void visit (ast::Initializer& node)
  {
    symtab.open_scope ();
    symtab.enter_symbol (node.initializer->receiver_parameter ());
    symtab.enter_signature (node.initializer->parameter_list ());
    symtab.enter_symbol (node.initializer->return_parameter ());
    Visitor v (*this);
    v.receiver_parameter = node.initializer->initializerType->receiver_parameter;
    v.context = Initializer;
    node.body->accept (v);
    symtab.close_scope ();
  }

  void visit (ast::Getter& node)
  {
    symtab.open_scope ();
    symtab.enter_symbol (node.getter->receiver_parameter ());
    symtab.enter_signature (node.getter->parameter_list ());
    symtab.enter_symbol (node.getter->return_parameter ());
    Visitor v (*this);
    v.receiver_parameter = node.getter->getterType->receiver_parameter;
    v.context = Getter;
    node.body->accept (v);
    symtab.close_scope ();
  }

  void visit (ast::Action& node)
  {
    symtab.open_scope ();
    symtab.enter_symbol (node.action->receiver_parameter);
    Visitor v (*this);
    v.receiver_parameter = node.action->receiver_parameter;
    v.context = Action;
    node.precondition->accept (v);
    check_condition (node.precondition);
    node.body->accept (v);
    node.action->precondition = node.precondition;

    if (node.precondition->eval.value.present)
      {
        if (node.precondition->eval.value.bool_value)
          {
            node.action->precondition_kind = decl::Action::StaticTrue;
          }
        else
          {
            node.action->precondition_kind = decl::Action::StaticFalse;
          }
      }
    symtab.close_scope ();
  }

  void visit (DimensionedAction& node)
  {
    symtab.open_scope ();
    symtab.enter_symbol (node.action->receiver_parameter);
    symtab.enter_symbol (node.action->iota_parameter);
    Visitor v (*this);
    v.receiver_parameter = node.action->receiver_parameter;
    v.context = Action;
    node.precondition->accept (v);
    check_condition (node.precondition);
    node.body->accept (v);
    node.action->precondition = node.precondition;

    if (node.precondition->eval.value.present)
      {
        if (node.precondition->eval.value.bool_value)
          {
            node.action->precondition_kind = decl::Action::StaticTrue;
          }
        else
          {
            node.action->precondition_kind = decl::Action::StaticFalse;
          }
      }
    symtab.close_scope ();
  }

  void visit (ast::Reaction& node)
  {
    symtab.open_scope ();
    symtab.enter_symbol (node.reaction->reaction_type->receiver_parameter);
    symtab.enter_signature (node.reaction->parameter_list ());
    // No return type.
    Visitor v (*this);
    v.receiver_parameter = node.reaction->reaction_type->receiver_parameter;
    v.context = Reaction;
    node.body->accept (v);
    symtab.close_scope ();
  }

  void visit (DimensionedReaction& node)
  {
    symtab.open_scope ();
    symtab.enter_symbol (node.reaction->reaction_type->receiver_parameter);
    symtab.enter_symbol (node.reaction->iota);
    symtab.enter_signature (node.reaction->parameter_list ());

    // No return type.
    Visitor v (*this);
    v.receiver_parameter = node.reaction->reaction_type->receiver_parameter;
    v.context = Reaction;
    node.body->accept (v);
    symtab.close_scope ();
  }

  void visit (ast::Bind& node)
  {
    symtab.open_scope ();
    symtab.enter_symbol (node.bind->receiver_parameter);
    node.body->accept (*this);
    symtab.close_scope ();
  }

  void visit (ast::Function& node)
  {
    symtab.open_scope ();
    symtab.enter_signature (node.function->parameter_list ());
    symtab.enter_symbol (node.function->return_parameter ());
    node.body->accept (*this);
    symtab.close_scope ();
  }

  void visit (ast::Method& node)
  {
    symtab.open_scope ();
    symtab.enter_symbol (node.method->receiver_parameter ());
    symtab.enter_signature (node.method->parameter_list ());
    symtab.enter_symbol (node.method->return_parameter ());
    Visitor v (*this);
    v.receiver_parameter = node.method->methodType->receiver_parameter;
    node.body->accept (v);
    symtab.close_scope ();
  }

  void visit (ListStatement& node)
  {
    symtab.open_scope ();
    node.visit_children (*this);
    symtab.close_scope ();
  }

  void visit (ExpressionStatement& node)
  {
    node.visit_children (*this);
    require_value_or_variable (node.child);
  }

  void visit (ReturnStatement& node)
  {
    // Check the expression.
    node.visit_children (*this);

    // Get the return symbol.
    node.return_symbol = symbol_cast<ParameterSymbol> (symtab.find_global_symbol (ReturnSymbol));
    assert (node.return_symbol != NULL);

    if (!assignable (node.child->eval.type, node.child->eval.value, node.return_symbol->type))
      {
        error_at_line (-1, 0, node.location.file.c_str (),
                       node.location.line, "cannot convert %s to %s in return (E160)",
                       node.child->eval.type->to_string ().c_str (), node.return_symbol->type->to_string ().c_str ());
      }
    node.child->eval.convert (node.return_symbol->type);

    require_value_or_variable (node.child);

    if (type_contains_pointer (node.child->eval.type) &&
        node.return_symbol->dereference_mutability < node.child->eval.indirection_mutability)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "return casts away +const or +foreign (E149)");
      }
  }

  void visit (IfStatement& node)
  {
    symtab.open_scope ();
    node.visit_children (*this);
    check_condition (node.condition);
    symtab.close_scope ();
  }

  void visit (WhileStatement& node)
  {
    symtab.open_scope ();
    node.visit_children (*this);
    check_condition (node.condition);
    symtab.close_scope ();
  }

  void visit (ForIotaStatement& node)
  {
    const std::string& identifier = node.identifier->identifier;
    node.limit = process_array_dimension (node.limit_node, er, symtab);
    node.symbol = new VariableSymbol (identifier, node.identifier->location, Int::instance (), Immutable, Immutable);
    symtab.open_scope ();
    symtab.enter_symbol (node.symbol);
    node.body->accept (*this);
    symtab.close_scope ();
  }

  void visit (ChangeStatement& node)
  {
    node.expr->accept (*this);

    const type::Type* root_type = node.expr->eval.type->merge_change ();
    if (root_type == NULL)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "cannot change expression of type %s (E96)", node.expr->eval.type->to_string ().c_str ());
      }

    require_value_or_variable (node.expr);

    // Enter all parameters and variables in scope that are pointers as pointers to foreign.
    symtab.open_scope ();
    symtab.change ();

    // Enter the new heap root.
    const std::string& identifier = node.identifier->identifier;
    // Don't know dereference mutability yet.
    node.root_symbol = new VariableSymbol (identifier, node.location, root_type, Immutable, Foreign);
    node.root_symbol->dereference_mutability = node.expr->eval.indirection_mutability;
    symtab.enter_symbol (node.root_symbol);

    // Check the body.
    node.body->accept (*this);
    symtab.close_scope ();
  }

  void visit (ActivateStatement& node)
  {
    if (!(context == Action ||
          context == Reaction))
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "activation outside of action or reaction (E53)");
      }

    if (in_mutable_phase)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "activations within activations are not allowed (E54)");
      }

    // Check the activations.
    node.expr_list->accept (*this);

    Visitor v (*this);
    v.in_mutable_phase = true;

    // Re-insert this as a pointer to mutable.
    symtab.open_scope ();
    symtab.activate ();
    // Check the body.
    node.body->accept (v);
    symtab.close_scope ();

    node.in_action = context == Action;
  }

  void visit (Const& node)
  {
    if (!node.done)
      {
        process_types_and_constants (&node, er, symtab);
      }
  }

  void visit (EmptyStatement& node)
  {
    // Do nothing.
  }

  void visit (VarStatement& node)
  {
    ast::List* identifier_list = node.identifier_list;
    ast::Node* type_spec = node.type_spec;
    ast::List* expression_list = node.expression_list;

    if (expression_list->size () != 0 &&
        identifier_list->size () != expression_list->size ())
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "wrong number of initializers (E184)");
      }

    // Process the type spec.
    const type::Type* type = process_type (type_spec, er, symtab, true);

    if (expression_list->size () == 0)
      {
        // Type, no expressions.

        if (type_cast<Void> (type) != NULL)
          {
            error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                           "missing type (E183)");

          }

        // Enter each symbol.
        for (List::ConstIterator id_pos = identifier_list->begin (),
             id_limit = identifier_list->end ();
             id_pos != id_limit;
             ++id_pos)
          {
            const std::string& name = node_cast<Identifier> (*id_pos)->identifier;
            VariableSymbol* symbol = new VariableSymbol (name, (*id_pos)->location, type, node.mutability, node.dereferenceMutability);
            symtab.enter_symbol (symbol);
            node.symbols.push_back (symbol);
          }

        return;
      }

    if (type_cast<Void> (type) == NULL)
      {
        // Type, expressions.

        // Enter each symbol.
        for (List::ConstIterator id_pos = identifier_list->begin (),
             id_limit = identifier_list->end (),
             init_pos = expression_list->begin ();
             id_pos != id_limit;
             ++id_pos, ++init_pos)
          {
            Node* n = *init_pos;
            n->accept (*this);

            if (!assignable (n->eval.type, n->eval.value, type))
              {
                error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                               "cannot assign %s to %s in initialization (E62)", n->eval.type->to_string ().c_str (), type->to_string ().c_str ());
              }
            n->eval.convert (type);
            require_value_or_variable (n);

            const std::string& name = node_cast<Identifier> (*id_pos)->identifier;
            VariableSymbol* symbol = new VariableSymbol (name, (*id_pos)->location, type, node.eval.intrinsic_mutability, node.dereferenceMutability);
            symtab.enter_symbol (symbol);
            node.symbols.push_back (symbol);
          }

        goto done;
      }

    // No type, expressions.

    // Enter each symbol.
    for (List::ConstIterator id_pos = identifier_list->begin (),
         id_limit = identifier_list->end (),
         init_pos = expression_list->begin ();
         id_pos != id_limit;
         ++id_pos, ++init_pos)
      {
        Node* n = *init_pos;
        n->accept (*this);

        if (n->eval.type->is_untyped ())
          {
            n->eval.value.convert (n->eval.type, n->eval.type->default_type ());
            n->eval.type = n->eval.type->default_type ();
          }
        require_value_or_variable (n);

        const std::string& name = node_cast<Identifier> (*id_pos)->identifier;
        VariableSymbol* symbol = new VariableSymbol (name, (*id_pos)->location, n->eval.type, node.eval.intrinsic_mutability, node.dereferenceMutability);
        symtab.enter_symbol (symbol);
        node.symbols.push_back (symbol);
      }

done:
    if (!node.expression_list->empty ())
      {
        size_t idx = 0;
        for (List::ConstIterator pos = node.expression_list->begin (), limit = node.expression_list->end ();
             pos != limit;
             ++pos, ++idx)
          {
            Node* n = *pos;
            VariableSymbol* symbol = node.symbols[idx];

            if (type_contains_pointer (n->eval.type) &&
                symbol->dereference_mutability < n->eval.indirection_mutability)
              {
                error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                               "assignment casts away +const or +foreign (E92)");
              }
          }
      }
  }

  void visit (AssignStatement& node)
  {
    node.visit_children (*this);
    const type::Type* to = node.left->eval.type;
    const type::Type*& from = node.right->eval.type;
    Value& val = node.right->eval.value;
    if (!assignable (from, val, to))
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "cannot assign value of type %s to variable of type %s (E199)",
                       from->to_string ().c_str (),
                       to->to_string ().c_str ());
      }
    node.right->eval.convert (to);
    require_variable (node.left);
    require_value_or_variable (node.right);

    if (node.left->eval.intrinsic_mutability != Mutable)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "target of assignment is not mutable (E86)");
      }

    if (type_contains_pointer (node.right->eval.type) &&
        node.left->eval.indirection_mutability < node.right->eval.indirection_mutability)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "assignment casts away +const or +foreign (E161)");
      }
  }

  void visit (AddAssignStatement& node)
  {
    node.visit_children (*this);

    const type::Type* to = node.left->eval.type;
    const type::Type*& from = node.right->eval.type;
    Value& val = node.right->eval.value;
    if (!arithmetic (to))
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "+= cannot be applied to %s (E200)",
                       to->to_string ().c_str ());
      }
    if (!assignable (from, val, to))
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "cannot assign value of type %s to variable of type %s (E76)",
                       from->to_string ().c_str (),
                       to->to_string ().c_str ());
      }
    node.right->eval.convert (to);
    require_variable (node.left);
    require_value_or_variable (node.right);

    if (node.left->eval.intrinsic_mutability != Mutable)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "target of assignment is not mutable (E15)");
      }
  }

  void visit (IncrementDecrementStatement& node)
  {
    const char* op = "";
    switch (node.kind)
      {
      case IncrementDecrementStatement::Increment:
        op = "++";
        break;
      case IncrementDecrementStatement::Decrement:
        op = "--";
        break;
      }

    node.visit_children (*this);
    if (!arithmetic (node.child->eval.type))
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "%s cannot be applied to %s (E77)",
                       op, node.child->eval.type->to_string ().c_str ());
      }
    require_variable (node.child);

    if (node.child->eval.intrinsic_mutability != Mutable)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "target of %s is not mutable (E177)", op);
      }
  }

  void visit (BindPushPortStatement& node)
  {
    node.visit_children (*this);
    bind (node, node.left, node.right);
  }

  void visit (BindPushPortParamStatement& node)
  {
    node.visit_children (*this);
    const decl::Reaction* reaction = bind (node, node.left, node.right);
    if (!reaction->has_dimension ())
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "parameter specified for non-parameterized reaction (E41)");
      }
    type::Int::ValueType dimension = reaction->dimension;
    check_array_index (reaction->reaction_type->get_array (dimension), node.param, false);
  }

  void visit (BindPullPortStatement& node)
  {
    node.visit_children (*this);

    const type::Function* pull_port_type = type_cast<type::Function> (node.left->eval.type);

    if (pull_port_type == NULL || pull_port_type->function_kind != type::Function::PULL_PORT)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "target of bind is not a pull port (E193)");
      }

    require_variable (node.left);

    const type::Method* getter_type = type_cast<type::Method> (node.right->eval.type);

    if (getter_type == NULL || getter_type->method_kind != type::Method::GETTER)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "source of bind is not a getter (E192)");
      }

    require_variable (node.right);

    type::Function g (type::Function::PULL_PORT, getter_type->parameter_list, getter_type->return_parameter_list);
    if (!are_identical (pull_port_type, &g))
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "cannot bind %s to %s (E191)", pull_port_type->to_string ().c_str (), getter_type->to_string ().c_str ());
      }
  }

  void visit (DereferenceExpr& node)
  {
    node.visit_children (*this);
    const type::Type* t = node.child->eval.type;
    const Pointer* p = type_cast<Pointer> (t->underlying_type ());
    if (p == NULL)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "* cannot be applied to %s (E21)", t->to_string ().c_str ());
      }
    require_value_or_variable (node.child);
    node.eval.type = p->base_type;
    node.eval.expression_kind = VariableExpressionKind;
    node.eval.intrinsic_mutability = node.child->eval.indirection_mutability;
    node.eval.indirection_mutability = node.child->eval.indirection_mutability;
    node.eval.fix_string_indirection_mutability ();
  }

  void visit (AddressOfExpr& node)
  {
    node.visit_children (*this);
    require_variable (node.child);
    node.eval.type = node.child->eval.type->get_pointer ();
    node.eval.expression_kind = ValueExpressionKind;
    node.eval.intrinsic_mutability = node.child->eval.intrinsic_mutability;
    node.eval.indirection_mutability = std::max (node.child->eval.intrinsic_mutability, node.child->eval.indirection_mutability);
    node.eval.fix_string_indirection_mutability ();
  }

  void visit (SelectExpr& node)
  {
    const std::string& identifier = node.identifier->identifier;
    node.base->accept (*this);
    const type::Type* base_type = node.base->eval.type;
    require_value_or_variable (node.base);

    node.field = base_type->find_field (identifier);
    node.callable = base_type->find_callable (identifier);
    if (node.field != NULL)
      {
        node.eval.type = node.field->type;
      }
    else if (node.callable != NULL)
      {
        node.eval.type = node.callable->callable_type ();
      }
    else
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "cannot select %s from expression of type %s (E154)",
                       identifier.c_str (), base_type->to_string ().c_str ());
      }

    if (base_type->underlying_type ()->to_pointer ())
      {
        // Selecting through pointer always yields a variable.
        node.eval.expression_kind = VariableExpressionKind;
        node.eval.intrinsic_mutability = node.base->eval.indirection_mutability;
        node.eval.indirection_mutability = node.base->eval.indirection_mutability;
        node.eval.fix_string_indirection_mutability ();
      }
    else
      {
        // Otherwise, use the base kind.
        node.eval.expression_kind = node.base->eval.expression_kind;
        node.eval.intrinsic_mutability = node.base->eval.intrinsic_mutability;
        node.eval.indirection_mutability = node.base->eval.indirection_mutability;
        node.eval.fix_string_indirection_mutability ();
      }
  }

  void check_array_index (const Array* array_type, Node* index, bool allow_equal)
  {
    const type::Type*& index_type = index->eval.type;
    Value& index_value = index->eval.value;

    if (is_untyped_numeric (index_type))
      {
        if (!index_value.representable (index_type, Int::instance ()))
          {
            error_at_line (-1, 0, index->location.file.c_str (), index->location.line,
                           "array index is not an integer (E20)");
          }
        index_value.convert (index_type, Int::instance ());
        index_type = Int::instance ();
        Int::ValueType idx = index_value.int_value;
        if (idx < 0)
          {
            error_at_line (-1, 0, index->location.file.c_str (), index->location.line,
                           "array index is negative (E162)");
          }
        if (array_type)
          {
            if ((!allow_equal && idx >= array_type->dimension) ||
                (allow_equal && idx > array_type->dimension))
              {
                error_at_line (-1, 0, index->location.file.c_str (), index->location.line,
                               "array index is out of bounds (E163)");
              }
          }
      }
    else if (is_typed_integer (index_type))
      {
        if (index_value.present)
          {
            Int::ValueType idx = index_value.to_int (index_type);
            if (idx < 0)
              {
                error_at_line (-1, 0, index->location.file.c_str (), index->location.line,
                               "array index is negative (E164)");
              }
            if (array_type)
              {
                if (idx >= array_type->dimension)
                  {
                    error_at_line (-1, 0, index->location.file.c_str (), index->location.line,
                                   "array index is out of bounds (E165)");
                  }
              }
          }
      }
    else
      {
        error_at_line (-1, 0, index->location.file.c_str (), index->location.line,
                       "array index is not an integer (E203)");
      }
    require_value_or_variable (index);
  }

  void visit (IndexExpr& node)
  {
    node.visit_children (*this);
    const type::Type* base_type = node.base->eval.type;
    Node* index_node = node.index;
    Value& index_value = index_node->eval.value;
    const type::Type*& index_type = node.index->eval.type;

    node.array_type = type_cast<Array> (base_type->underlying_type ());
    if (node.array_type != NULL)
      {
        check_array_index (node.array_type, node.index, false);
        require_value_or_variable (node.base);
        node.eval.type = node.array_type->base_type;
        node.eval.expression_kind = node.base->eval.expression_kind;
        node.eval.intrinsic_mutability = node.base->eval.intrinsic_mutability;
        node.eval.indirection_mutability = node.base->eval.indirection_mutability;
        node.eval.fix_string_indirection_mutability ();
        return;
      }

    node.slice_type = type_cast<Slice> (base_type->underlying_type ());
    if (node.slice_type != NULL)
      {
        if (is_untyped_numeric (index_type))
          {
            if (!index_value.representable (index_type, Int::instance ()))
              {
                error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                               "slice index is not an integer (E204)");
              }
            index_value.convert (index_type, Int::instance ());
            index_type = Int::instance ();
            Int::ValueType idx = index_value.int_value;
            if (idx < 0)
              {
                error_at_line (-1, 0, index_node->location.file.c_str (), index_node->location.line,
                               "slice index is negative (E166)");
              }
          }
        else if (is_typed_integer (index_type))
          {
            if (index_value.present)
              {
                Int::ValueType idx = index_value.to_int (index_type);
                if (idx < 0)
                  {
                    error_at_line (-1, 0, index_node->location.file.c_str (), index_node->location.line,
                                   "slice index is negative (E167)");
                  }
              }
          }
        else
          {
            error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                           "slice index is not an integer (E205)");
          }

        require_value_or_variable (node.base);
        require_value_or_variable (node.index);

        node.eval.type = node.slice_type->base_type;
        node.eval.expression_kind = VariableExpressionKind;
        node.eval.intrinsic_mutability = node.base->eval.intrinsic_mutability;
        node.eval.indirection_mutability = node.base->eval.indirection_mutability;
        node.eval.fix_string_indirection_mutability ();
        return;
      }

    error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                   "cannot index expression of type %s (E168)",
                   base_type->to_string ().c_str ());
  }

  void visit (SliceExpr& node)
  {
    Node* base = node.base;
    base->accept (*this);
    require_value_or_variable (base);
    const type::Type* base_type = node.base->eval.type;

    Node* low_node = node.low;
    if (node.low_present)
      {
        low_node->accept (*this);
      }
    const type::Type*& low_type = low_node->eval.type;
    Value& low_value = low_node->eval.value;

    Node* high_node = node.high;
    if (node.high_present)
      {
        high_node->accept (*this);
      }
    const type::Type*& high_type = high_node->eval.type;
    Value& high_value = high_node->eval.value;

    Node* max_node = node.max;
    if (node.max_present)
      {
        max_node->accept (*this);
      }
    const type::Type*& max_type = max_node->eval.type;
    Value& max_value = max_node->eval.value;

    node.string_type = type_cast<String> (base_type->underlying_type ());
    if (node.string_type != NULL)
      {
        UNIMPLEMENTED;
        return;
      }

    node.pointer_to_array_type = pointer_to_array (base_type->underlying_type ());
    if (node.pointer_to_array_type)
      {
        UNIMPLEMENTED;
        return;
      }

    node.array_type = type_cast<Array> (base_type->underlying_type ());
    if (node.array_type != NULL)
      {
        if (node.low_present)
          {
            check_array_index (node.array_type, low_node, true);
          }
        if (node.high_present)
          {
            check_array_index (node.array_type, high_node, true);
          }
        if (node.max_present)
          {
            check_array_index (node.array_type, max_node, true);
          }

        node.eval.type = node.array_type->base_type->get_slice ();
        node.eval.expression_kind = ValueExpressionKind;
        node.eval.intrinsic_mutability = Immutable;
        node.eval.indirection_mutability = node.base->eval.indirection_mutability;
        node.eval.fix_string_indirection_mutability ();
        goto done;
      }

    node.slice_type = type_cast<Slice> (base_type->underlying_type ());
    if (node.slice_type != NULL)
      {
        if (node.low_present)
          {
            check_array_index (NULL, low_node, true);
          }
        if (node.high_present)
          {
            check_array_index (NULL, high_node, true);
          }
        if (node.max_present)
          {
            check_array_index (NULL, max_node, true);
          }

        node.eval.type = node.slice_type;
        node.eval.expression_kind = ValueExpressionKind;
        node.eval.intrinsic_mutability = Immutable;
        node.eval.indirection_mutability = node.base->eval.indirection_mutability;
        node.eval.fix_string_indirection_mutability ();
        goto done;
      }

    error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                   "cannot slice expression of type %s (E225)",
                   base_type->to_string ().c_str ());

done:
    if (low_value.present && high_value.present)
      {
        if (!(low_value.to_int (low_type) <= high_value.to_int (high_type)))
          {
            error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                           "slice indices are out of range (E224)");
          }
      }

    if (low_value.present && max_value.present)
      {
        if (!(low_value.to_int (low_type) <= max_value.to_int (max_type)))
          {
            error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                           "slice indices are out of range (E6)");
          }
      }

    if (high_value.present && max_value.present)
      {
        if (!(high_value.to_int (high_type) <= max_value.to_int (max_type)))
          {
            error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                           "slice indices are out of range (E7)");
          }
      }
  }

  void visit (TypeExpression& node)
  {
    node.eval.type = process_type (node.type_spec, er, symtab, true);
    node.eval.expression_kind = TypeExpressionKind;
  }

  void visit (PushPortCallExpr& node)
  {
    node.receiver_parameter = receiver_parameter;
    const std::string& port_identifier = node.identifier->identifier;
    const type::Type* this_type = receiver_parameter->type;
    node.field = this_type->find_field (port_identifier);
    if (node.field == NULL)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "no port named %s (E194)", port_identifier.c_str ());
      }
    const type::Function* push_port_type = type::type_cast<type::Function> (node.field->type);
    if (push_port_type == NULL || push_port_type->function_kind != type::Function::PUSH_PORT)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "no port named %s (E195)", port_identifier.c_str ());
      }

    node.args->accept (*this);
    check_types_arguments (node.args, push_port_type->parameter_list);
    require_value_or_variable_list (node.args);

    node.eval.type = type::Void::instance ();
    node.eval.expression_kind = ValueExpressionKind;
    node.eval.intrinsic_mutability = Immutable;
    node.eval.indirection_mutability = Immutable;
  }

  void visit (IndexedPushPortCallExpr& node)
  {
    node.receiver_parameter = receiver_parameter;
    const std::string& port_identifier = node.identifier->identifier;
    const type::Type* this_type = receiver_parameter->type;
    node.field = this_type->find_field (port_identifier);
    if (node.field == NULL)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "no port named %s (E74)", port_identifier.c_str ());
      }
    node.array_type = type::type_cast<type::Array> (node.field->type);
    if (node.array_type == NULL)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "%s is not an array of ports (E16)", port_identifier.c_str ());
      }
    const type::Function* push_port_type = type::type_cast<type::Function> (node.array_type->base_type);
    if (push_port_type == NULL || push_port_type->function_kind != type::Function::PUSH_PORT)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "%s is not an array of ports (E17)", port_identifier.c_str ());
      }

    node.index->accept (*this);
    check_array_index (node.array_type, node.index, false);

    node.args->accept (*this);
    check_types_arguments (node.args, push_port_type->parameter_list);
    require_value_or_variable_list (node.args);

    node.eval.type = type::Void::instance ();
    node.eval.expression_kind = ValueExpressionKind;
    node.eval.intrinsic_mutability = Immutable;
    node.eval.indirection_mutability = Immutable;
  }

  void visit (CompositeLiteral& node)
  {
    node.eval.type = process_type (node.literal_type, er, symtab, true);
    node.eval.expression_kind = VariableExpressionKind;

    switch (node.eval.type->underlying_kind ())
      {
      case Struct_Kind:
      {
        for (List::ConstIterator pos = node.literal_value->begin (),
             limit = node.literal_value->end ();
             pos != limit;
             ++pos)
          {
            //Node* element = *pos;
            UNIMPLEMENTED;
          }
      }
      break;

      case Array_Kind:
        UNIMPLEMENTED;
        break;

      case Slice_Kind:
        UNIMPLEMENTED;
        break;

      default:
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "cannot define composite literals for %s (E5)", node.eval.type->to_string ().c_str ());
      }
  }
};
}

void check_types_arguments (ast::List* args, const decl::ParameterList* signature)
{
  if (args->size () != signature->size ())
    {
      error_at_line (-1, 0, args->location.file.c_str (), args->location.line,
                     "call expects %lu arguments but given %lu (E150)", signature->size (), args->size ());
    }

  size_t i = 0;
  for (List::ConstIterator pos = args->begin (), limit = args->end ();
       pos != limit;
       ++pos, ++i)
    {
      const type::Type*& arg = (*pos)->eval.type;
      Value& val = (*pos)->eval.value;
      const type::Type* param = signature->at (i)->type;
      if (!type::assignable (arg, val, param))
        {
          error_at_line (-1, 0, (*pos)->location.file.c_str (), (*pos)->location.line,
                         "cannot assign %s to %s in call (E151)", arg->to_string ().c_str (), param->to_string ().c_str ());
        }
      (*pos)->eval.convert (param);
    }
}

void require_value_or_variable_list (const List* node)
{
  for (List::ConstIterator pos = node->begin (), limit = node->end ();
       pos != limit;
       ++pos)
    {
      require_value_or_variable (*pos);
    }
}

void check_types (ast::Node* root, ErrorReporter& er, SymbolTable& symtab)
{
  Visitor visitor (er, symtab);
  root->accept (visitor);
}

void check_mutability_arguments (ast::Node* node, const decl::ParameterList* signature)
{
  ListExpr* args = node_cast<ListExpr> (node);

  size_t i = 0;
  for (List::ConstIterator pos = args->begin (), limit = args->end ();
       pos != limit;
       ++pos, ++i)
    {
      const type::Type* arg = (*pos)->eval.type;
      if (type_contains_pointer (arg))
        {
          if (signature->at (i)->dereference_mutability < (*pos)->eval.indirection_mutability)
            {
              error_at_line (-1, 0, (*pos)->location.file.c_str (), (*pos)->location.line,
                             "argument %zd casts away +const or +foreign (E85)", i + 1);
            }
        }
    }
}

}
