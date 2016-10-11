#include "check_types.hpp"

#include <error.h>

#include "node.hpp"
#include "node_visitor.hpp"
#include "node_cast.hpp"
#include "polymorphic_function.hpp"
#include "callable.hpp"
#include "symbol_visitor.hpp"
#include "semantic.hpp"
#include "symbol.hpp"
#include "symbol.hpp"
#include "parameter_list.hpp"
#include "error_reporter.hpp"
#include "enter_top_level_identifiers.hpp"
#include "process_top_level_identifiers.hpp"
#include "process_type.hpp"

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

static void require_value_or_variable_or_void (const Node* node)
{
  assert (!node->eval.is_unknown ());
  if (!node->eval.is_value_or_variable_or_void ())
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
  const type::PushPort* push_port_type = port_node->eval.type->to_push_port ();

  if (push_port_type == NULL)
    {
      error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                     "source of bind is not a port (E38)");
    }

  require_variable (port_node);

  const type::Reaction* reaction_type = reaction_node->eval.type->to_reaction ();
  if (reaction_type == NULL)
    {
      error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                     "target of bind is not a reaction (E39)");
    }

  require_variable (reaction_node);

  type::PushPort f (reaction_type->parameter_list, reaction_type->return_parameter_list);
  if (!are_identical (push_port_type, &f))
    {
      error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                     "cannot bind %s to %s (E40)", push_port_type->to_error_string ().c_str (), reaction_type->to_error_string ().c_str ());
    }

  return static_cast<const decl::Reaction*> (reaction_node->callable);
}

static void check_condition (Node* node)
{
  const type::Type* condition = node->eval.type;
  if (!(condition->is_any_boolean ()))
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
  Scope* scope;
  Parameter* receiver_parameter;
  Context context;
  bool in_mutable_phase;

  Visitor (ErrorReporter& a_er,
           Scope* a_scope)
    : er (a_er)
    , scope (a_scope)
    , receiver_parameter (NULL)
    , context (Other)
    , in_mutable_phase (false)
  { }

  void default_action (Node& node)
  {
    AST_NOT_REACHED (node);
  }

  void visit (Call& node)
  {
    node.visit_children (*this);
    assert (node.expression->eval.expression_kind != UnknownExpressionKind);
    if (node.expression->eval.expression_kind == TypeExpressionKind)
      {
        // Conversion.
        if (node.arguments->size () != 1)
          {
            error_at_line (-1, 0, node.location.file.c_str (),
                           node.location.line,
                           "conversion requires one argument (E8)");

          }

        conversion (node, node.expression, node.arguments->at (0));
        return;
      }

    // Collect the arguments.
    ExpressionValueList arguments;
    List* args = node.arguments;
    for (List::ConstIterator pos = args->begin (), limit = args->end ();
         pos != limit;
         ++pos)
      {
        arguments.push_back ((*pos)->eval);
      }

    if (node.expression->polymorphic_function != NULL)
      {
        node.polymorphic_function = node.expression->polymorphic_function;
        node.polymorphic_function->check (er, node.location, node.eval, arguments);
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
        node.callable = node.expression->callable;
      }

    node.function_type = node.expression->eval.type->to_function ();
    node.push_port_type = node.expression->eval.type->to_push_port ();
    node.pull_port_type = node.expression->eval.type->to_pull_port ();
    node.method_type = node.expression->eval.type->to_method ();
    node.initializer_type = node.expression->eval.type->to_initializer ();
    node.getter_type = node.expression->eval.type->to_getter ();
    node.reaction_type = node.expression->eval.type->to_reaction ();

    if (node.function_type)
      {
        // No restrictions on caller.
        node.parameters = node.function_type->parameter_list;
        node.return_parameters = node.function_type->return_parameter_list;
      }
    else if (node.push_port_type)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "push ports cannot be called (E202)");

      }
    else if (node.pull_port_type)
      {
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
        node.parameters = node.pull_port_type->parameter_list;
        node.return_parameters = node.pull_port_type->return_parameter_list;
      }
    else if (node.method_type)
      {
        // No restrictions on caller.
        node.parameters = node.method_type->parameter_list;
        node.return_parameters = node.method_type->return_parameter_list;
      }
    else if (node.initializer_type)
      {
        // Caller must be an initializer.
        if (context != Initializer)
          {
            error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                           "initializers may only be called from initializers (E197)");
          }
        node.parameters = node.initializer_type->parameter_list;
        node.return_parameters = node.initializer_type->return_parameter_list;
      }
    else if (node.getter_type)
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
        node.parameters = node.getter_type->parameter_list;
        node.return_parameters = node.getter_type->return_parameter_list;
      }
    else if (node.reaction_type)
      {
        UNIMPLEMENTED;
      }
    else
      {
        NOT_REACHED;
      }

    if (node.callable != NULL)
      {
        require_value_or_variable (node.expression);
        node.callable->check (args);
      }
    else
      {
        node.field = node.expression->field;
        check_types_arguments (args, node.parameters);
        require_value_or_variable (node.expression);
        require_value_or_variable_list (node.arguments);
        check_mutability_arguments (node.arguments, node.parameters);
      }

    if (node.return_parameters->empty ())
      {
        node.eval.expression_kind = VoidExpressionKind;
      }
    else if (node.return_parameters->size () == 1)
      {
        node.eval.type = node.return_parameters->at (0)->type;
        node.eval.expression_kind = ValueExpressionKind;
        node.eval.intrinsic_mutability = Immutable;
        node.eval.indirection_mutability = node.return_parameters->at (0)->indirection_mutability;
        node.eval.fix_string_indirection_mutability ();
      }
    else
      {
        UNIMPLEMENTED;
      }
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
        else if (from->is_typed_float () && to->is_typed_float ())
          {
            UNIMPLEMENTED;
          }
        else if (from->is_typed_integer () && to->is_any_string ())
          {
            UNIMPLEMENTED;
            node.reset_mutability = true;
          }
        else if (from->is_any_string () && to->is_slice_of_bytes ())
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
        if (are_assignable (from, x, to))
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
        else if ((from->is_typed_integer () || from->is_typed_float ()) &&
                 (to->is_typed_integer () || to->is_typed_float ()))
          {
            // Okay.
          }
        else if (from->is_typed_complex () && to->is_typed_complex ())
          {
            // Okay.
          }
        else if ((from->is_typed_integer () || from->is_slice_of_bytes () || from->is_slice_of_runes ()) && to->is_typed_string ())
          {
            // Okay.
            node.reset_mutability = true;
          }
        else if (from->is_typed_string () && (to->is_slice_of_bytes () || to->is_slice_of_runes ()))
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

  void visit (Conversion& node)
  {
    node.visit_children (*this);
    conversion (node, node.type, node.argument);
  }

  void visit (ExpressionList& node)
  {
    node.visit_children (*this);
  }

  void visit (Literal& node)
  {
    assert (node.eval.value.present);
    node.eval.expression_kind = ValueExpressionKind;
    node.eval.intrinsic_mutability = Immutable;
    node.eval.indirection_mutability = Immutable;
  }

  void visit (IdentifierExpression& node)
  {
    Identifier* identifier_node = node.child;
    const std::string& identifier = identifier_node->identifier;
    node.symbol = scope->find_global_symbol (identifier);
    if (node.symbol == NULL)
      {
        er.not_declared (node.location, identifier);
        node.eval.expression_kind = ErrorExpressionKind;
        return;
      }

    if (!node.symbol->process_declaration (er, scope))
      {
        // process_declaration reports error.
        node.eval.expression_kind = ErrorExpressionKind;
        return;
      }

    struct visitor : public ConstSymbolVisitor
    {
      IdentifierExpression& node;
      ErrorReporter& er;

      visitor (IdentifierExpression& n,
               ErrorReporter& a_er)
        : node (n)
        , er (a_er)
      { }

      void default_action (const Symbol& symbol)
      {
        NOT_REACHED;
      }

      void visit (const decl::PolymorphicFunction& symbol)
      {
        node.polymorphic_function = &symbol;
        node.eval.type = type::PolymorphicFunction::instance ();
        node.eval.expression_kind = ValueExpressionKind;
        node.eval.intrinsic_mutability = Immutable;
        node.eval.indirection_mutability = Immutable;
      }

      void visit (const decl::FunctionBase& symbol)
      {
        node.callable = &symbol;
        node.eval.type = symbol.type;
        node.eval.expression_kind = ValueExpressionKind;
        node.eval.intrinsic_mutability = Immutable;
        node.eval.indirection_mutability = Immutable;
      }

      void visit (const Parameter& symbol)
      {
        node.eval.type = symbol.type;
        node.eval.expression_kind = VariableExpressionKind;
        node.eval.intrinsic_mutability = symbol.intrinsic_mutability;
        node.eval.indirection_mutability = symbol.indirection_mutability;
      }

      void visit (const NamedType& symbol)
      {
        node.eval.type = &symbol;
        node.eval.expression_kind = TypeExpressionKind;
      }

      void visit (const Constant& symbol)
      {
        node.eval.type = symbol.type ();
        node.eval.value = symbol.value ();
        assert (node.eval.value.present);
        node.eval.expression_kind = ValueExpressionKind;
        node.eval.intrinsic_mutability = Immutable;
        node.eval.indirection_mutability = Mutable;
      }

      void visit (const Variable& symbol)
      {
        node.eval.type = symbol.type;
        node.eval.expression_kind = VariableExpressionKind;
        node.eval.intrinsic_mutability = symbol.intrinsic_mutability;
        node.eval.indirection_mutability = symbol.indirection_mutability;
      }

      void visit (const decl::Hidden& symbol)
      {
        er.hidden_symbol (symbol.location, symbol.name);
        node.eval.expression_kind = ErrorExpressionKind;
      }
    };
    visitor v (node, er);
    node.symbol->accept (v);
    node.eval.fix_string_indirection_mutability ();
  }

  void visit (ast::UnaryArithmetic& node)
  {
    node.visit_children (*this);

    if (node.child->eval.expression_kind == ErrorExpressionKind)
      {
        node.eval.expression_kind = ErrorExpressionKind;
        return;
      }

    ExpressionValueList evals;
    evals.push_back (node.child->eval);

    node.polymorphic_function->check (er, node.location, node.eval, evals);
  }

  void visit (ast::BinaryArithmetic& node)
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

    node.polymorphic_function->check (er, node.location, node.eval, evals);

    node.left->eval = evals[0];
    node.right->eval = evals[1];
  }

  void visit (SourceFile& node)
  {
    node.top_level_decl_list->accept (*this);
  }

  void visit (TopLevelDeclList& node)
  {
    node.visit_children (*this);
  }

  void visit (ast::TypeDecl& node)
  {
    // Do nothing.
  }

  void visit (ast::InstanceDecl& node)
  {
    // Check the arguments.
    node.arguments->accept (*this);
    check_types_arguments (node.arguments, node.symbol->initializer ()->type->parameter_list);
    require_value_or_variable_list (node.arguments);
    check_mutability_arguments (node.arguments, node.symbol->initializer ()->type->parameter_list);
  }

  void visit (ast::InitDecl& node)
  {
    scope = scope->open (node.initializer->parameter_list (),
                         node.initializer->return_parameter_list ());
    Visitor v (*this);
    v.receiver_parameter = node.initializer->type->receiver_parameter;
    v.context = Initializer;
    node.body->accept (v);
    scope = scope->close ();
  }

  void visit (ast::GetterDecl& node)
  {
    scope = scope->open (node.getter->parameter_list (),
                         node.getter->return_parameter_list ());
    Visitor v (*this);
    v.receiver_parameter = node.getter->type->receiver_parameter;
    v.context = Getter;
    node.body->accept (v);
    scope = scope->close ();
  }

  void visit (ast::ActionDecl& node)
  {
    scope = scope->open ();

    if (node.action->dimension () != -1)
      {
        scope->enter_symbol (node.action->iota_parameter ());
      }
    scope->enter_symbol (node.action->receiver_parameter ());
    Visitor v (*this);
    v.receiver_parameter = node.action->receiver_parameter ();
    v.context = Action;
    node.precondition->accept (v);
    check_condition (node.precondition);
    node.body->accept (v);

    if (node.precondition->eval.value.present)
      {
        if (node.precondition->eval.value.bool_value)
          {
            node.action->precondition_kind = decl::Action::Static_True;
          }
        else
          {
            node.action->precondition_kind = decl::Action::Static_False;
          }
      }
    scope = scope->close ();
  }

  void visit (ast::ReactionDecl& node)
  {
    if (node.reaction->dimension () != -1)
      {
        scope = scope->open (node.reaction->iota (),
                             node.reaction->parameter_list (),
                             node.reaction->return_parameter_list ());
      }
    else
      {
        scope = scope->open (node.reaction->parameter_list (),
                             node.reaction->return_parameter_list ());
      }
    // No return type.
    Visitor v (*this);
    v.receiver_parameter = node.reaction->type->receiver_parameter;
    v.context = Reaction;
    node.body->accept (v);
    scope = scope->close ();
  }

  void visit (ast::BindDecl& node)
  {
    scope = scope->open ();
    scope->enter_symbol (node.bind->receiver_parameter ());
    node.body->accept (*this);
    scope = scope->close ();
  }

  void visit (ast::FunctionDecl& node)
  {
    scope = scope->open (node.symbol->parameter_list (),
                         node.symbol->return_parameter_list ());
    node.body->accept (*this);
    scope = scope->close ();
  }

  void visit (ast::MethodDecl& node)
  {
    scope = scope->open (node.method->parameter_list (),
                         node.method->return_parameter_list ());
    Visitor v (*this);
    v.receiver_parameter = node.method->type->receiver_parameter;
    node.body->accept (v);
    scope = scope->close ();
  }

  void visit (StatementList& node)
  {
    scope = scope->open ();
    node.visit_children (*this);
    scope = scope->close ();
  }

  void visit (ExpressionStatement& node)
  {
    node.visit_children (*this);
    require_value_or_variable_or_void (node.child);
  }

  void visit (Return& node)
  {
    // Check the expression.
    node.visit_children (*this);

    // Get the return symbol.
    const decl::ParameterList* return_parameter_list =
      scope->return_parameter_list ();

    if (return_parameter_list == NULL)
      {
        UNIMPLEMENTED;
      }

    if (return_parameter_list->size () != 1)
      {
        UNIMPLEMENTED;
      }

    node.return_symbol = return_parameter_list->at (0);

    if (!are_assignable (node.child->eval.type, node.child->eval.value, node.return_symbol->type))
      {
        error_at_line (-1, 0, node.location.file.c_str (),
                       node.location.line, "cannot convert %s to %s in return (E160)",
                       node.child->eval.type->to_error_string ().c_str (), node.return_symbol->type->to_error_string ().c_str ());
      }
    node.child->eval.convert (node.return_symbol->type);

    require_value_or_variable (node.child);

    if (node.child->eval.type->contains_pointer () &&
        node.return_symbol->indirection_mutability < node.child->eval.indirection_mutability)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "return casts away +const or +foreign (E149)");
      }
  }

  void visit (If& node)
  {
    scope = scope->open ();
    node.visit_children (*this);
    check_condition (node.condition);
    scope = scope->close ();
  }

  void visit (While& node)
  {
    scope = scope->open ();
    node.visit_children (*this);
    check_condition (node.condition);
    scope = scope->close ();
  }

  void visit (ForIota& node)
  {
    const std::string& identifier = node.identifier->identifier;
    // TODO:  Check for error.
    node.limit_value = process_array_dimension (er, scope, node.limit);
    node.symbol = new Variable (identifier, node.identifier->location, Int::instance (), Immutable, Immutable);
    scope = scope->open ();
    scope->enter_symbol (node.symbol);
    node.body->accept (*this);
    scope = scope->close ();
  }

  void visit (Change& node)
  {
    node.argument->accept (*this);

    const type::Type* root_type = node.argument->eval.type->merge_change ();
    if (root_type == NULL)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "cannot change expression of type %s (E96)", node.argument->eval.type->to_error_string ().c_str ());
      }

    require_value_or_variable (node.argument);

    // Enter all parameters and variables in scope that are pointers as pointers to foreign.
    scope = scope->open ();
    scope->change ();

    // Enter the new heap root.
    const std::string& identifier = node.identifier->identifier;
    // Don't know dereference mutability yet.
    node.root_symbol = new Variable (identifier, node.location, root_type, Immutable, Foreign);
    node.root_symbol->indirection_mutability = node.argument->eval.indirection_mutability;
    scope->enter_symbol (node.root_symbol);

    // Check the body.
    node.body->accept (*this);
    scope = scope->close ();
  }

  void visit (Activate& node)
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
    node.arguments->accept (*this);

    // Re-insert this as a pointer to mutable.
    scope = scope->open ();
    scope->activate ();
    // Check the body.
    Visitor v (*this);
    v.in_mutable_phase = true;
    node.body->accept (v);
    scope = scope->close ();

    node.in_action = context == Action;
  }

  void visit (ConstDecl& node)
  {
    if (node.symbols.empty ())
      {
        // Not at top level so process.
        enter_top_level_identifiers (&node, er, scope, scope);
        process_top_level_declarations (&node, er, scope);
      }
  }

  void visit (EmptyStatement& node)
  {
    // Do nothing.
  }

  void visit (Var& node)
  {
    ast::List* identifier_list = node.identifiers;
    ast::Node* type_spec = node.type;
    ast::List* expression_list = node.expressions;

    if (expression_list->size () != 0 &&
        identifier_list->size () != expression_list->size ())
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "wrong number of initializers (E184)");
      }

    // Process the type spec.
    process_type (type_spec, er, scope);
    if (type_spec->eval.expression_kind == ErrorExpressionKind)
      {
        // TODO:  Check for failure.
        UNIMPLEMENTED;
      }

    if (expression_list->size () == 0)
      {
        // Type, no expressions.
        if (type_spec->eval.expression_kind == EmptyTypeExpressionKind)
          {
            error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                           "missing type (E183)");

          }

        const type::Type* type = type_spec->eval.type;

        // Enter each symbol.
        for (List::ConstIterator id_pos = identifier_list->begin (),
             id_limit = identifier_list->end ();
             id_pos != id_limit;
             ++id_pos)
          {
            const std::string& name = node_cast<Identifier> (*id_pos)->identifier;
            Variable* symbol = new Variable (name, (*id_pos)->location, type, node.mutability, node.indirection_mutability);
            scope->enter_symbol (symbol);
            node.symbols.push_back (symbol);
          }

        return;
      }

    if (type_spec->eval.expression_kind == TypeExpressionKind)
      {
        // Type, expressions.
        const type::Type* type = type_spec->eval.type;

        // Enter each symbol.
        for (List::ConstIterator id_pos = identifier_list->begin (),
             id_limit = identifier_list->end (),
             init_pos = expression_list->begin ();
             id_pos != id_limit;
             ++id_pos, ++init_pos)
          {
            Node* n = *init_pos;
            n->accept (*this);

            if (!are_assignable (n->eval.type, n->eval.value, type))
              {
                error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                               "cannot assign %s to %s in initialization (E62)", n->eval.type->to_error_string ().c_str (), type->to_error_string ().c_str ());
              }
            n->eval.convert (type);
            require_value_or_variable (n);

            const std::string& name = node_cast<Identifier> (*id_pos)->identifier;
            Variable* symbol = new Variable (name, (*id_pos)->location, type, node.eval.intrinsic_mutability, node.indirection_mutability);
            scope->enter_symbol (symbol);
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
        Variable* symbol = new Variable (name, (*id_pos)->location, n->eval.type, node.eval.intrinsic_mutability, node.indirection_mutability);
        scope->enter_symbol (symbol);
        node.symbols.push_back (symbol);
      }

done:
    if (!node.expressions->empty ())
      {
        size_t idx = 0;
        for (List::ConstIterator pos = node.expressions->begin (), limit = node.expressions->end ();
             pos != limit;
             ++pos, ++idx)
          {
            Node* n = *pos;
            Variable* symbol = node.symbols[idx];

            if (n->eval.type->contains_pointer () &&
                symbol->indirection_mutability < n->eval.indirection_mutability)
              {
                error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                               "assignment casts away +const or +foreign (E92)");
              }
          }
      }
  }

  void visit (Assign& node)
  {
    node.visit_children (*this);
    const type::Type* to = node.left->eval.type;
    const type::Type*& from = node.right->eval.type;
    Value& val = node.right->eval.value;
    if (!are_assignable (from, val, to))
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "cannot assign value of type %s to variable of type %s (E199)",
                       from->to_error_string ().c_str (),
                       to->to_error_string ().c_str ());
      }
    node.right->eval.convert (to);
    require_variable (node.left);
    require_value_or_variable (node.right);

    if (node.left->eval.intrinsic_mutability != Mutable)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "target of assignment is not mutable (E86)");
      }

    if (node.right->eval.type->contains_pointer () &&
        node.left->eval.indirection_mutability < node.right->eval.indirection_mutability)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "assignment casts away +const or +foreign (E161)");
      }
  }

  void visit (AddAssign& node)
  {
    node.visit_children (*this);

    const type::Type* to = node.left->eval.type;
    const type::Type*& from = node.right->eval.type;
    Value& val = node.right->eval.value;
    if (!to->is_arithmetic ())
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "+= cannot be applied to %s (E200)",
                       to->to_error_string ().c_str ());
      }
    if (!are_assignable (from, val, to))
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "cannot assign value of type %s to variable of type %s (E76)",
                       from->to_error_string ().c_str (),
                       to->to_error_string ().c_str ());
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

  void visit (IncrementDecrement& node)
  {
    const char* op = "";
    switch (node.kind)
      {
      case IncrementDecrement::Increment:
        op = "++";
        break;
      case IncrementDecrement::Decrement:
        op = "--";
        break;
      }

    node.visit_children (*this);
    if (!node.child->eval.type->is_arithmetic ())
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "%s cannot be applied to %s (E77)",
                       op, node.child->eval.type->to_error_string ().c_str ());
      }
    require_variable (node.child);

    if (node.child->eval.intrinsic_mutability != Mutable)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "target of %s is not mutable (E177)", op);
      }
  }

  void visit (BindPushPort& node)
  {
    node.visit_children (*this);
    bind (node, node.left, node.right);
  }

  void visit (BindPushPortParameter& node)
  {
    node.visit_children (*this);
    const decl::Reaction* reaction = bind (node, node.left, node.right);
    long dimension = reaction->dimension ();
    if (dimension == -1)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "parameter specified for non-parameterized reaction (E41)");
      }

    check_array_index (reaction->type->get_array (dimension), node.param, false);
  }

  void visit (BindPullPort& node)
  {
    node.visit_children (*this);

    const type::PullPort* pull_port_type = node.left->eval.type->to_pull_port ();

    if (pull_port_type == NULL)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "target of bind is not a pull port (E193)");
      }

    require_variable (node.left);

    const type::Getter* getter_type = node.right->eval.type->to_getter ();

    if (getter_type == NULL)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "source of bind is not a getter (E192)");
      }

    require_variable (node.right);

    type::PullPort g (getter_type->parameter_list, getter_type->return_parameter_list);
    if (!are_identical (pull_port_type, &g))
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "cannot bind %s to %s (E191)", pull_port_type->to_error_string ().c_str (), getter_type->to_error_string ().c_str ());
      }
  }

  void visit (Dereference& node)
  {
    node.visit_children (*this);
    const type::Type* t = node.child->eval.type;
    const type::Pointer* p = t->underlying_type ()->to_pointer ();
    if (p == NULL)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "* cannot be applied to %s (E21)", t->to_error_string ().c_str ());
      }
    require_value_or_variable (node.child);
    node.eval.type = p->base_type;
    node.eval.expression_kind = VariableExpressionKind;
    node.eval.intrinsic_mutability = node.child->eval.indirection_mutability;
    node.eval.indirection_mutability = node.child->eval.indirection_mutability;
    node.eval.fix_string_indirection_mutability ();
  }

  void visit (AddressOf& node)
  {
    node.visit_children (*this);
    require_variable (node.child);
    node.eval.type = node.child->eval.type->get_pointer ();
    node.eval.expression_kind = ValueExpressionKind;
    node.eval.intrinsic_mutability = node.child->eval.intrinsic_mutability;
    node.eval.indirection_mutability = std::max (node.child->eval.intrinsic_mutability, node.child->eval.indirection_mutability);
    node.eval.fix_string_indirection_mutability ();
  }

  void visit (Select& node)
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
                       identifier.c_str (), base_type->to_error_string ().c_str ());
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

  void check_array_index (const type::Array* array_type, Node* index, bool allow_equal)
  {
    const type::Type*& index_type = index->eval.type;
    Value& index_value = index->eval.value;

    if (index_type->is_untyped_numeric ())
      {
        if (!index_value.representable (index_type, Int::instance ()))
          {
            error_at_line (-1, 0, index->location.file.c_str (), index->location.line,
                           "array index is not an integer (E20)");
          }
        index_value.convert (index_type, Int::instance ());
        index_type = Int::instance ();
        long idx = index_value.int_value;
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
    else if (index_type->is_typed_integer ())
      {
        if (index_value.present)
          {
            long idx = index_value.to_int (index_type);
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

  void visit (Index& node)
  {
    node.visit_children (*this);
    const type::Type* base_type = node.base->eval.type;
    Node* index_node = node.index;
    Value& index_value = index_node->eval.value;
    const type::Type*& index_type = node.index->eval.type;

    node.array_type = base_type->underlying_type ()->to_array ();
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

    node.slice_type = base_type->underlying_type ()->to_slice ();
    if (node.slice_type != NULL)
      {
        if (index_type->is_untyped_numeric ())
          {
            if (!index_value.representable (index_type, Int::instance ()))
              {
                error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                               "slice index is not an integer (E204)");
              }
            index_value.convert (index_type, Int::instance ());
            index_type = Int::instance ();
            long idx = index_value.int_value;
            if (idx < 0)
              {
                error_at_line (-1, 0, index_node->location.file.c_str (), index_node->location.line,
                               "slice index is negative (E166)");
              }
          }
        else if (index_type->is_typed_integer ())
          {
            if (index_value.present)
              {
                long idx = index_value.to_int (index_type);
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
                   base_type->to_error_string ().c_str ());
  }

  void visit (IndexSlice& node)
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

    if (base_type->underlying_type ()->kind () == Untyped_String_Kind)
      {
        UNIMPLEMENTED;
        return;
      }

    node.pointer_to_array_type = base_type->pointer_to_array ();
    if (node.pointer_to_array_type)
      {
        UNIMPLEMENTED;
        return;
      }

    node.array_type = base_type->underlying_type ()->to_array ();
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

    node.slice_type = base_type->underlying_type ()->to_slice ();
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
                   base_type->to_error_string ().c_str ());

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
    process_type (node.child, er, scope);
    node.eval.expression_kind = node.child->eval.expression_kind;
    node.eval.type = node.child->eval.type;
  }

  void visit (PushPortCall& node)
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
    node.push_port_type = node.field->type->to_push_port ();
    if (node.push_port_type == NULL)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "no port named %s (E195)", port_identifier.c_str ());
      }

    node.arguments->accept (*this);
    check_types_arguments (node.arguments, node.push_port_type->parameter_list);
    require_value_or_variable_list (node.arguments);

    node.eval.expression_kind = VoidExpressionKind;
    node.eval.intrinsic_mutability = Immutable;
    node.eval.indirection_mutability = Immutable;
  }

  void visit (IndexedPushPortCall& node)
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
    node.array_type = node.field->type->to_array ();
    if (node.array_type == NULL)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "%s is not an array of ports (E16)", port_identifier.c_str ());
      }
    node.push_port_type = node.array_type->base_type->to_push_port ();
    if (node.push_port_type == NULL)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "%s is not an array of ports (E17)", port_identifier.c_str ());
      }

    node.index->accept (*this);
    check_array_index (node.array_type, node.index, false);

    node.arguments->accept (*this);
    check_types_arguments (node.arguments, node.push_port_type->parameter_list);
    require_value_or_variable_list (node.arguments);

    node.eval.expression_kind = VoidExpressionKind;
    node.eval.intrinsic_mutability = Immutable;
    node.eval.indirection_mutability = Immutable;
  }

  void visit (CompositeLiteral& node)
  {
    // TODO:  Check for failure.
    node.eval.type = NULL;
    process_type (node.type, er, scope);
    assert (false);
    node.eval.expression_kind = VariableExpressionKind;

    switch (node.eval.type->underlying_kind ())
      {
      case Struct_Kind:
      {
        for (List::ConstIterator pos = node.value->begin (),
             limit = node.value->end ();
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
                       "cannot define composite literals for %s (E5)", node.eval.type->to_error_string ().c_str ());
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
      if (!type::are_assignable (arg, val, param))
        {
          error_at_line (-1, 0, (*pos)->location.file.c_str (), (*pos)->location.line,
                         "cannot assign %s to %s in call (E151)", arg->to_error_string ().c_str (), param->to_error_string ().c_str ());
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

void check_types (ast::Node* root, ErrorReporter& er, Scope* scope)
{
  Visitor visitor (er, scope);
  root->accept (visitor);
}

bool check_constant_expression (ast::Node* root, util::ErrorReporter& er, decl::Scope* scope)
{
  check_types (root, er, scope);
  if (!root->eval.value.present)
    {
      er.expression_is_not_constant (root->location);
      return false;
    }
  return true;
}

void check_mutability_arguments (ast::Node* node, const decl::ParameterList* signature)
{
  ExpressionList* args = node_cast<ExpressionList> (node);

  size_t i = 0;
  for (List::ConstIterator pos = args->begin (), limit = args->end ();
       pos != limit;
       ++pos, ++i)
    {
      const type::Type* arg = (*pos)->eval.type;
      if (arg->contains_pointer ())
        {
          if (signature->at (i)->indirection_mutability < (*pos)->eval.indirection_mutability)
            {
              error_at_line (-1, 0, (*pos)->location.file.c_str (), (*pos)->location.line,
                             "argument %zd casts away +const or +foreign (E85)", i + 1);
            }
        }
    }
}

}
