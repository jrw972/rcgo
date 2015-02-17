#include "ast.hpp"
#include "symbol.hpp"
#include <error.h>
#include "semantic.hpp"
#include "function.hpp"
#include "method.hpp"
#include "binding.hpp"
#include "trigger.hpp"
#include "action.hpp"
#include "field.hpp"
#include "parameter.hpp"

static void
enter_symbol (symtab_t* symtab, symbol_t * symbol, symbol_holder& holder)
{
  // Check if the symbol is defined locally.
  string_t identifier = symbol_identifier (symbol);
  symbol_t *s = symtab_find_current (symtab, identifier);
  if (s == NULL)
    {
      symtab_enter (symtab, symbol);
      holder.symbol (symbol);
    }
  else
    {
      const ast_t* node = symbol_defining_node (symbol);
      error_at_line (-1, 0, node->file, node->line,
		     "%s is already defined in this scope", get (identifier));
    }
}

static void
convert_rvalue_to_builtin_types (ast_t::iterator ptr)
{
  ast_expr_t *node = dynamic_cast<ast_expr_t*> (*ptr);

  typed_value_t tv = ast_get_typed_value (node);
  struct visitor : public const_type_visitor_t
  {
    ast_t* node;
    typed_value_t& tv;

    visitor (ast_t* n,
             typed_value_t& t) : node (n), tv (t) { }

    void visit (const named_type_t&)
    {
      // Do nothing.
    }

    void visit (const void_type_t&)
    {
      // Do nothing.
    }

    void visit (const immutable_type_t&)
    {
      // Do nothing.
    }

    void visit (const untyped_boolean_type_t&)
    {
      tv.type = bool_type_t::instance ();
      // No need to process value.
    }

    void visit (const untyped_string_type_t&)
    {
      tv.type = string_type_t::instance ();
      // No need to process value.
    }

    void default_action (const type_t& type)
    {
      not_reached;
    }

  };
  visitor v (node, tv);
  tv.type->accept (v);
  node->set_type (tv);
}

static void check_identifier_expr (ast_identifier_expr_t& node)
{
  ast_t *identifier_node = node.at (UNARY_CHILD);
  string_t identifier = ast_get_identifier (identifier_node);
  symbol_t *symbol = symtab_find (node.symtab, identifier);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, identifier_node->file,
                     identifier_node->line, "%s is not defined",
                     get (identifier));
    }

  switch (symbol_kind (symbol))
    {
    case SymbolFunction:
      {
        type_t* t = function_type (symbol_get_function_function (symbol));
        node.set_type (typed_value_make (t));
      }
      break;

    case SymbolInstance:
      unimplemented;

    case SymbolParameter:
      {
        const type_t* t = symbol_parameter_type (symbol);
        node.set_type (typed_value_make (t));
      }
      break;

    case SymbolType:
      unimplemented;

    case SymbolTypedConstant:
      node.set_type (symbol_typed_constant_value (symbol));
      break;

    case SymbolVariable:
      node.set_type (typed_value_make (symbol_variable_type (symbol)));
      break;
    }

  node.symbol.symbol (symbol);
}

typedef union {
  field_t * field;
  reaction_t * reaction;
} FieldOrReaction;

static FieldOrReaction make_field (field_t * field)
{
  FieldOrReaction foa = { field: field };
  return foa;
}

static FieldOrReaction make_action (reaction_t * reaction)
{
  FieldOrReaction foa = { reaction: reaction };
  return foa;
}

static const type_t* check_index (const type_t* aggregate_type, typed_value_t idx_tv, const ast_t& error_node)
{
  struct visitor : public const_type_visitor_t
  {
    const type_t* retval;
    const typed_value_t& idx_tv;
    const ast_t& node;

    visitor (const typed_value_t& it, const ast_t& n) : retval (NULL), idx_tv (it), node (n) { }

    void visit (const immutable_type_t& type)
    {
      type.base_type ()->accept (*this);
    }

    void visit (const array_type_t& type)
    {
      struct visitor : public const_type_visitor_t
      {
        const typed_value_t& idx_tv;
        const array_type_t& array_type;
        const ast_t& node;

        visitor (const typed_value_t& it, const array_type_t& at, const ast_t& n) : idx_tv (it), array_type (at), node (n) { }

        void default_action (const type_t& type)
        {
          not_reached;
        }

        void visit (const named_type_t& type)
        {
          type.subtype ()->accept (*this);
        }

        void visit (const immutable_type_t& type)
        {
          type.base_type ()->accept (*this);
        }

        void visit (const uint_type_t& type)
        {
          if (idx_tv.has_value)
            {
              // Check that the index is in bounds.
              unimplemented;
            }
        }

        void visit (const iota_type_t& type)
        {
          if (type.bound () > array_type.dimension ())
            {
              error_at_line (-1, 0, node.file, node.line,
                             "iota bound (%zd) exceeds array dimension (%zd)", type.bound (), array_type.dimension ());
            }
        }
      };
      visitor v (idx_tv, type, node);
      idx_tv.type->accept (v);

      retval = type.base_type ();
    }

    void default_action (const type_t& type)
    {
      not_reached;
    }
  };
  visitor v (idx_tv, error_node);
  aggregate_type->accept (v);

  return v.retval;
}

static FieldOrReaction
check_lvalue (ast_t::iterator ptr, binding_t* binding, bool output)
{
  struct check_lvalue_visitor_t : public ast_visitor_t
  {
    ast_t::iterator ptr;
    binding_t* binding;
    bool output;
    FieldOrReaction retval;

    check_lvalue_visitor_t (ast_t::iterator p, binding_t* b, bool o) : ptr (p), binding (b), output (o) { }

    void default_action (ast_t& node)
    {
      not_reached;
    }

    void visit (ast_dereference_expr_t& node)
    {
      ast_t::iterator child = node.get_child_ptr (UNARY_CHILD);
      type_check_rvalue (child, binding, output);
      typed_value_t tv = ast_get_typed_value (*child);
      const type_t* type = type_dereference (tv.type);
      if (type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot dereference non-pointer");
        }
      node.set_type (typed_value_make (type));
      retval = make_field (NULL);
    }

    void visit (ast_identifier_expr_t& node)
    {
      check_identifier_expr (node);
      retval = make_field (NULL);
    }

    void visit (ast_index_expr_t& node)
    {
      ast_t::iterator expr = node.base_iter ();
      ast_t::iterator idx = node.index_iter ();

      check_lvalue (expr, binding, output);
      type_check_rvalue (idx, binding, output);

      convert_rvalue_to_builtin_types (idx);

      typed_value_t expr_tv = ast_get_typed_value (*expr);
      typed_value_t idx_tv = ast_get_typed_value (*idx);

      const type_t* result_type = check_index (expr_tv.type, idx_tv, node);

      if (result_type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "incompatible types (%s)[%s]", expr_tv.type->to_string ().c_str (), idx_tv.type->to_string ().c_str ());
        }

      node.set_type (typed_value_make (result_type));
      retval = make_field (NULL);
    }

    void visit (ast_select_expr_t& node)
    {
      ast_t::iterator left = node.get_child_ptr (BINARY_LEFT_CHILD);
      ast_t *right = node.at (BINARY_RIGHT_CHILD);
      string_t identifier = ast_get_identifier (right);
      check_lvalue (left, binding, output);
      typed_value_t tv = ast_get_typed_value (*left);

      const type_t* t = tv.type;
      if (type_to_immutable (t))
        {
          t = dynamic_cast<const immutable_type_t*> (t)->base_type ();
        }
      else if (type_to_foreign (t))
        {
          t = type_to_foreign (t)->base_type ();
        }

      if (type_is_any_pointer (t))
        {
          // Selecting from a pointer.  Insert a dereference.
          ast_dereference_expr_t* deref = new ast_dereference_expr_t (node.line, *left);
          deref->set_type (typed_value_make (type_dereference (tv.type)));
          *left = deref;
          tv = ast_get_typed_value (*left);
        }

      const type_t* type = type_select (tv.type, identifier);
      if (type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot select %s from expression of type %s",
                         get (identifier), tv.type->to_string ().c_str ());
        }

      typed_value_t selected_tv = typed_value_make (type);

      node.set_type (selected_tv);
      node.field = type_select_field (tv.type, identifier);
      node.reaction = type_select_reaction (tv.type, identifier);

      if (binding != NULL)
        {
          if (type_to_component (selected_tv.type) || type_to_port (selected_tv.type))
            {
              field_t *field = type_select_field (tv.type, identifier);
              assert (field != NULL);
              if (output)
                {
                  binding->outputs.push_back (field);
                }
              else
                {
                  binding->inputs.push_back (field);
                }
              retval = make_field (field);
              return;
            }
          else if (type_to_reaction (selected_tv.type))
            {
              reaction_t * reaction = dynamic_cast<const named_type_t*> (tv.type)->get_reaction (identifier);
              assert (reaction != NULL);
              binding->reaction = reaction;
              retval = make_action (reaction);
              return;
            }
        }

      retval = make_field (NULL);
    }
  };
  check_lvalue_visitor_t check_lvalue_visitor (ptr, binding, output);
  (*ptr)->accept (check_lvalue_visitor);
  return check_lvalue_visitor.retval;
}

static void
check_assignment_target (ast_t::iterator left, ast_t* node)
{
  check_lvalue (left, NULL, false);
  typed_value_t tv = ast_get_typed_value (*left);
  if (!type_is_assignable (tv.type))
    {
      error_at_line (-1, 0, node->file, node->line,
                     "cannot assign to read-only location of type %s", tv.type->to_string ().c_str ());
    }
}

static void arithmetic_assign (ast_t* node, const char* symbol)
{
  ast_t::iterator left = node->get_child_ptr (BINARY_LEFT_CHILD);
  check_assignment_target (left, node);
  ast_t::iterator right = node->get_child_ptr (BINARY_RIGHT_CHILD);
  type_check_rvalue (right, NULL, false);
  typed_value_t left_tv = ast_get_typed_value (*left);
  typed_value_t right_tv = ast_get_typed_value (*right);
  typed_value_convert (right_tv, left_tv);
  if (!type_is_equal (left_tv.type, right_tv.type))
    {
      error_at_line (-1, 0, node->file, node->line,
                     "incompatible types (%s) %s (%s)", left_tv.type->to_string ().c_str (), symbol, right_tv.type->to_string ().c_str ());
    }

  struct visitor : public const_type_visitor_t
  {
    ast_t* node;
    const char* symbol;

    visitor (ast_t* n, const char* s) : node (n), symbol (s) { }

    void visit (const named_type_t& type)
    {
      type.subtype ()->accept (*this);
    }

    void visit (const uint_type_t& type)
    {
      // Okay.
    }

    void default_action (const type_t& type)
    {
      error_at_line (-1, 0, node->file, node->line,
                     "incompatible types (%s) %s (%s)", type.to_string ().c_str (), symbol, type.to_string ().c_str ());
    }
  };
  visitor v (node, symbol);
  left_tv.type->accept (v);

  dynamic_cast<ast_expr_t*> (*right)->set_type (right_tv);
}

static void check_rvalue_list (ast_t * node);

static void
check_call (ast_expr_t * node, ast_t * args, const type_t * expr_type)
{
  size_t idx;
  size_t argument_count = args->size ();
  size_t parameter_count = type_parameter_count (expr_type);
  if (argument_count != parameter_count)
    {
      error_at_line (-1, 0, node->file, node->line,
		     "call expects %zd arguments but given %zd",
		     parameter_count, argument_count);
    }

  for (idx = 0; idx != argument_count; ++idx)
    {
      ast_t *arg = args->at (idx);
      typed_value_t argument_tv  = ast_get_typed_value (arg);
      const type_t *parameter_type = type_parameter_type (expr_type, idx);
      typed_value_t parameter_tv = typed_value_make (parameter_type);
      typed_value_convert (argument_tv, parameter_tv);
      if (!type_is_equal (argument_tv.type, parameter_tv.type))
        {
          error_at_line (-1, 0, arg->file,
                         arg->line,
                         "cannot convert %s to %s", argument_tv.type->to_string ().c_str (), parameter_tv.type->to_string ().c_str ());
        }
      dynamic_cast<ast_expr_t*> (arg)->set_type (argument_tv);
    }

  // Set the return type.
  node->set_type (typed_value_make (type_return_type (expr_type)));
}

static bool
in_mutable_section (const ast_t* node)
{
  return get_current_action (node) != NULL &&
    get_current_trigger (node) != NULL;
}

struct extract_method_visitor_t : public ast_const_visitor_t
{
  method_t* retval;

  extract_method_visitor_t () : retval (NULL) { }

  void default_action (const ast_t& node)
  {
    not_reached;
  }

  void visit (const ast_identifier_expr_t& node)
  {
    // Do nothing.
  }

  void visit (const ast_select_expr_t& node)
  {
    ast_t *left = node.at (BINARY_LEFT_CHILD);
    ast_t *right = node.at (BINARY_RIGHT_CHILD);
    string_t identifier = ast_get_identifier (right);

    const type_t* t = ast_get_typed_value (left).type;

    if (type_to_pointer (t))
      {
        // Selecting from a pointer.  Insert a dereference.
        unimplemented;
        //t = type_pointer_base_type (t);
      }

    if (type_to_immutable (t))
      {
        t = dynamic_cast<const immutable_type_t*> (t)->base_type ();
      }

    retval = dynamic_cast<const named_type_t*> (t)->get_method (identifier);
  }
};

// TODO:  Fold this into check rvalue.
static method_t*
extract_method (const ast_t* node)
{
  extract_method_visitor_t extract_method_visitor;
  node->accept (extract_method_visitor);
  return extract_method_visitor.retval;
}

struct extract_function_visitor_t : public ast_const_visitor_t
{
  function_t* retval;

  extract_function_visitor_t () : retval (NULL) { }

  void default_action (const ast_t& node)
  {
    not_reached;
  }

  void visit (const ast_identifier_expr_t& node)
  {
    string_t s = ast_get_identifier (node.at (UNARY_CHILD));
    symbol_t* symbol = symtab_find (node.symtab, s);
    retval = symbol_get_function_function (symbol);
  }

  void visit (const ast_select_expr_t& node)
  {
    retval = NULL;
  }
};

// TODO:  Fold this into check rvalue.
static function_t*
extract_function (const ast_t* node)
{
  extract_function_visitor_t extract_function_visitor;
  node->accept (extract_function_visitor);
  return extract_function_visitor.retval;
}

static bool logic_or (bool x, bool y) { return x || y; }
static bool logic_and (bool x, bool y) { return x && y; }

void
type_check_rvalue (ast_t::iterator ptr, binding_t* binding, bool output)
{
  struct check_rvalue_visitor_t : public ast_visitor_t
  {
    ast_t::iterator ptr;
    binding_t* binding;
    bool output;

    check_rvalue_visitor_t (ast_t::iterator p, binding_t* b, bool o) : ptr (p), binding (b), output (o) { }

    void default_action (ast_t& node)
    {
      not_reached;
    }

    void visit (ast_literal_expr_t& node)
    {
      // Do nothing.
    }

    void visit (ast_add_expr_t& node)
    {
      ast_t::iterator left = node.get_child_ptr (BINARY_LEFT_CHILD);
      ast_t::iterator right = node.get_child_ptr (BINARY_RIGHT_CHILD);
      type_check_rvalue (left, binding, output);
      type_check_rvalue (right, binding, output);
      unimplemented;
      // typed_value_t left_tv = ast_get_typed_value (*left);
      // typed_value_t right_tv = ast_get_typed_value (*right);
      // if (!(typed_value_convertible (left_tv, right_tv) ||
      //       typed_value_convertible (right_tv, left_tv)))
      //   {
      //     error_at_line (-1, 0, node->file, node->line,
      //                    "cannot convert %s to %s in addition", right_tv.type->to_string ().c_str (), left_tv.type->to_string ().c_str ());

      //   }
      // convert_if_untyped (left_tv, *right);
      // convert_if_untyped (right_tv, *left);
      // left_tv = ast_get_typed_value (*left);
      // right_tv = ast_get_typed_value (*right);
      // if (!typed_value_can_add (left_tv, right_tv))
      //   {
      //     error_at_line (-1, 0, node->file, node->line,
      //                    "cannot apply '+' to expressions of type %s and %s", left_tv.type->to_string ().c_str (), right_tv.type->to_string ().c_str ());

      //   }
      // ast_set_type (node, typed_value_add (left_tv, right_tv), false);
    }

    void check_address_of (ast_address_of_expr_t& node)
    {
      ast_t::iterator expr = node.get_child_ptr (UNARY_CHILD);
      typed_value_t expr_tv = ast_get_typed_value (*expr);
      const type_t* type;
      // TODO:  Move to typed_value.
      if (type_to_foreign (expr_tv.type))
        {
          type = pointer_to_foreign_type_t::make (type_to_foreign (expr_tv.type)->base_type ());
        }
      else if (type_to_immutable (expr_tv.type))
        {
          type = pointer_to_immutable_type_t::make (dynamic_cast<const immutable_type_t*> (expr_tv.type)->base_type ());
        }
      else
        {
          type = pointer_type_t::make (expr_tv.type);
        }

      node.set_type (typed_value_make (type));
    }

    void visit (ast_address_of_expr_t& node)
    {
      ast_t::iterator expr = node.get_child_ptr (UNARY_CHILD);
      check_lvalue (expr, binding, output);
      check_address_of (node);
    }

    void visit (ast_call_expr_t& node)
    {
      ast_t::iterator expr = node.expr_iter ();
      ast_t *args = node.args ();

      // Analyze the args.
      check_rvalue_list (args);

      // Analyze the callee.
      type_check_rvalue (expr, binding, output);

      typed_value_t expr_tv = ast_get_typed_value (*expr);
      if (!type_is_callable (expr_tv.type))
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot call %s", expr_tv.type->to_string ().c_str ());
        }

      function_t* function = extract_function (*expr);
      method_t* method = extract_method (*expr);

      if (method != NULL)
        {
          // Calling a method.

          // The receiver is either a copy or a pointer.
          bool receiver_is_pointer = type_is_any_pointer (type_parameter_type (expr_tv.type, 0));

          // Transfer the computation for the receiver to the argument list.
          ast_t* receiver_select_expr = (*expr)->at (BINARY_LEFT_CHILD);
          if (receiver_is_pointer)
            {
              ast_address_of_expr_t* e = new ast_address_of_expr_t (node.line, receiver_select_expr);
              check_address_of (*e);
              args->prepend (e);
            }
          else
            {
              args->prepend (receiver_select_expr);
            }

          if (in_mutable_section (&node))
            {
              named_type_t* t = method_named_type (method);
              if (type_to_component (t))
                {
                  // Invoking a method on a component in a mutable section.
                  // Ensure the receiver is this.
                  std::cout << node.line << '\n';
                  unimplemented;
                }
            }

          // Replace the callee with a literal or expression.
          *expr = ast_make_typed_literal (node.line, typed_value_make_method (method));
        }
      else if (function != NULL)
        {
          // Calling a function.

          // Replace the callee with a literal or expression.
          *expr = ast_make_typed_literal (node.line, typed_value_make_function (function));
        }
      else
        {
          unimplemented;
        }

      check_call (&node, args, expr_tv.type);
    }

    void visit (ast_dereference_expr_t& node)
    {
      ast_t::iterator child = node.get_child_ptr (UNARY_CHILD);
      type_check_rvalue (child, binding, output);
      typed_value_t tv = ast_get_typed_value (*child);
      const type_t* type = type_dereference (tv.type);
      if (type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot apply @ to expression of type %s", tv.type->to_string ().c_str ());
        }

      node.set_type (typed_value_make (type));
    }

    void visit (ast_equal_expr_t& node)
    {
      check_comparison (node, "==");
    }

    void visit (ast_identifier_expr_t& node)
    {
      check_identifier_expr (node);
    }

    void visit (ast_index_expr_t& node)
    {
      check_lvalue (ptr, binding, output);
    }

    void visit (ast_logic_and_expr_t& node)
    {
      binary_boolean (node, true, "&&", logic_and);
    }

    void visit (ast_logic_not_expr_t& node)
    {
      ast_t::iterator child = node.get_child_ptr (UNARY_CHILD);
      type_check_rvalue (child, binding, output);
      typed_value_t tv = ast_get_typed_value (*child);

      struct visitor : public const_type_visitor_t
      {
        ast_t& node;
        typed_value_t& tv;

        visitor (ast_t& n, typed_value_t& t) : node (n), tv (t) { }

        void visit (const named_type_t& type)
        {
          type.subtype ()->accept (*this);
        }

        void visit (const immutable_type_t& type)
        {
          type.base_type ()->accept (*this);
        }

        void visit (const foreign_type_t& type)
        {
          type.base_type ()->accept (*this);
        }

        void visit (const bool_type_t& type)
        {
          if (tv.has_value)
            {
              tv.bool_value = !tv.bool_value;
            }
        }

        void visit (const untyped_boolean_type_t& type)
        {
          assert (tv.has_value);
          tv.bool_value = !tv.bool_value;
        }

        void default_action (const type_t& type)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "incompatible types (%s) !", type.to_string ().c_str ());
        }
      };
      visitor v (node, tv);
      tv.type->accept (v);

      node.set_type (tv);
    }

    void visit (ast_logic_or_expr_t& node)
    {
      binary_boolean (node, false, "||", logic_or);
    }

    void visit (ast_merge_expr_t& node)
    {
      ast_t::iterator child = node.get_child_ptr (UNARY_CHILD);
      type_check_rvalue (child, binding, output);
      typed_value_t tv = ast_get_typed_value (*child);
      const type_t* type = type_merge (tv.type);
      if (type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot merge expression of type %s", tv.type->to_string ().c_str ());
        }
      tv = typed_value_make (type);
      node.set_type (tv);
    }

    void visit (ast_move_expr_t& node)
    {
      ast_t::iterator child = node.get_child_ptr (UNARY_CHILD);
      type_check_rvalue (child, binding, output);
      typed_value_t tv = ast_get_typed_value (*child);
      const type_t* type = type_move (tv.type);
      if (type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot move expression of type %s", tv.type->to_string ().c_str ());
        }
      tv = typed_value_make (type);
      node.set_type (tv);
    }

    void visit (ast_new_expr_t& node)
    {
      ast_t* type_spec = node.at (UNARY_CHILD);
      const type_t* t = process_type_spec (type_spec, false);
      const type_t* type = pointer_type_t::make (t);
      node.set_type (typed_value_make (type));
    }

    void visit (ast_not_equal_expr_t& node)
    {
      check_comparison (node, "!=");
    }

    void visit (ast_port_call_expr_t& node)
    {
      ast_t *expr = node.name ();
      string_t port_identifier = ast_get_identifier (expr);
      const type_t *this_type = get_current_receiver_type (&node);
      const type_t *port_type = type_select (this_type, port_identifier);

      if (port_type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "no port named %s", get (port_identifier));
        }

      if (!type_to_port (port_type))
        {
          error_at_line (-1, 0, node.file, node.line,
                         "%s is not a port", get (port_identifier));
        }

      node.field = type_select_field (this_type, port_identifier);

      ast_t *args = node.args ();
      check_rvalue_list (args);
      check_call (&node, args, port_type);
    }

    void visit (ast_indexed_port_call_expr_t& node)
    {
      string_t port_identifier = ast_get_identifier (node.identifier ());
      const type_t *this_type = get_current_receiver_type (&node);
      const type_t *type = type_select (this_type, port_identifier);

      if (type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "no port named %s", get (port_identifier));
        }

      const array_type_t* array_type = type_to_array (type);
      if (!array_type)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "%s is not an array of ports", get (port_identifier));
        }

      const port_type_t* port_type = type_to_port (array_type->base_type ());

      if (!port_type)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "%s is not an array of ports", get (port_identifier));
        }

      ast_t::iterator index = node.index_iter ();
      type_check_rvalue (index, binding, output);
      check_index (array_type, ast_get_typed_value (*index), **index);

      ast_t *args = node.args ();
      check_rvalue_list (args);
      check_call (&node, args, port_type);
    }

    void visit (ast_select_expr_t& node)
    {
      check_lvalue (ptr, binding, output);
    }

    void
    check_comparison (ast_expr_t& node, const char* operation)
    {
      ast_t::iterator left = node.get_child_ptr (BINARY_LEFT_CHILD);
      type_check_rvalue (left, NULL, false);
      typed_value_t left_tv = ast_get_typed_value (*left);
      ast_t::iterator right = node.get_child_ptr (BINARY_RIGHT_CHILD);
      type_check_rvalue (right, NULL, false);
      typed_value_t right_tv = ast_get_typed_value (*right);
      typed_value_convert (left_tv, right_tv);
      typed_value_convert (right_tv, left_tv);
      if (!type_is_equal (left_tv.type, right_tv.type))
        {
          error_at_line (-1, 0, node.file, node.line,
                         "incompatible types (%s) %s (%s)", right_tv.type->to_string ().c_str (), operation, left_tv.type->to_string ().c_str ());

        }

      dynamic_cast<ast_expr_t*> (*left)->set_type (left_tv);
      dynamic_cast<ast_expr_t*> (*right)->set_type (right_tv);

      node.set_type (typed_value_make (untyped_boolean_type_t::instance ()));
    }

    void
    binary_boolean (ast_expr_t& node, bool identity_value, const char* operator_str, bool (*func) (bool, bool))
    {
      typed_value_t tv = typed_value_make_bool (untyped_boolean_type_t::instance (), identity_value);
      for (ast_t::iterator child = node.begin (), limit = node.end ();
           child != limit;
           ++child)
        {
          type_check_rvalue (child, binding, output);
          typed_value_t v = ast_get_typed_value (*child);

          typed_value_convert (tv, v);
          typed_value_convert (v, tv);

          if (!type_is_equal (tv.type, v.type))
            {
              error_at_line (-1, 0, node.file, node.line,
                             "incompatible types (%s) %s (%s)", tv.type->to_string ().c_str (), operator_str, v.type->to_string ().c_str ());
            }

          struct visitor : public const_type_visitor_t
          {
            ast_t& node;
            const char* operator_str;
            bool (*func) (bool, bool);
            typed_value_t& tv;
            typed_value_t& v;

            visitor (ast_t& n, const char* o, bool (*f) (bool, bool), typed_value_t& tv_, typed_value_t& v_) : node (n), operator_str (o), func (f), tv (tv_), v (v_) { }

            void visit (const named_type_t& type)
            {
              type.subtype ()->accept (*this);
            }

            void visit (const bool_type_t&)
            {
              if (tv.has_value && v.has_value)
                {
                  tv.has_value = true;
                  tv.bool_value = func (tv.bool_value, v.bool_value);
                }
              else
                {
                  tv.has_value = false;
                }
            }

            void visit (const untyped_boolean_type_t&)
            {
              assert (tv.has_value);
              assert (v.has_value);
              tv.bool_value = func (tv.bool_value, v.bool_value);
            }

            void default_action (const type_t& type)
            {
              error_at_line (-1, 0, node.file, node.line,
                             "incompatible types (%s) %s (%s)", tv.type->to_string ().c_str (), operator_str, v.type->to_string ().c_str ());
            }
          };
          visitor vis (node, operator_str, func, tv, v);
          tv.type->accept (vis);

          dynamic_cast<ast_expr_t*> (*child)->set_type (v);
        }

      node.set_type (tv);
    }

  };
  check_rvalue_visitor_t check_rvalue_visitor (ptr, binding, output);
  (*ptr)->accept (check_rvalue_visitor);
}

// TODO:  Fold into check_rvalue.
static void
check_rvalue_list (ast_t * node)
{
  struct check_rvalue_list_visitor_t : public ast_visitor_t
  {
    void default_action (ast_t& node)
    {
      not_reached;
    }

    void visit (ast_list_expr_t& node)
    {
      for (ast_t::iterator child = node.begin (), limit = node.end ();
           child != limit;
           ++child)
        {
          type_check_rvalue (child, NULL, false);
        }
    }
  };

  check_rvalue_list_visitor_t check_rvalue_list_visitor;
  node->accept (check_rvalue_list_visitor);
}

static void
convert_rvalue_list_to_builtin_types (ast_t * node)
{
  struct convert_rvalue_list_to_builtin_types_visitor_t : public ast_visitor_t
  {
    void default_action (ast_t& node)
    {
      not_reached;
    }

    void visit (ast_list_expr_t& node)
    {
      for (ast_t::iterator child = node.begin (), limit = node.end ();
           child != limit;
           ++child)
        {
          convert_rvalue_to_builtin_types (child);
        }
    }
  };
  convert_rvalue_list_to_builtin_types_visitor_t convert_rvalue_list_to_builtin_types_visitor;
  node->accept (convert_rvalue_list_to_builtin_types_visitor);
}

static void
check_bind_statement (ast_t * node)
{
  struct visitor : public ast_visitor_t
  {
    void default_action (ast_t& node)
    {
      not_reached;
    }

    void visit (ast_bind_statement_t& node)
    {
      binding_t *binding = new binding_t;

      ast_t::iterator port_node = node.get_child_ptr (BINARY_LEFT_CHILD);
      field_t *port_field = check_lvalue (port_node, binding, true).field;
      const port_type_t *port_type = dynamic_cast<const port_type_t*> (field_type (port_field));

      if (port_type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "source of bind is not a port");
        }

      ast_t::iterator reaction_node = node.get_child_ptr (BINARY_RIGHT_CHILD);
      reaction_t *reaction = check_lvalue (reaction_node, binding, false).reaction;
      const reaction_type_t *reaction_typ = reaction->reaction_type ();

      if (!type_is_compatible_port_reaction (port_type, reaction_typ))
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot bind %s to %s", port_type->to_string ().c_str (), reaction_typ->to_string ().c_str ());
        }

      get_current_receiver_type (&node)->add_binding (binding);
    }

    void visit (ast_bind_statement_list_t& node)
    {
      AST_FOREACH (child, &node)
        {
          check_bind_statement (child);
        }
    }
  };

  visitor v;
  node->accept (v);
}

static void
type_check_statement (ast_t * node)
{
  struct visitor : public ast_visitor_t
  {
    void default_action (ast_t& node)
    {
      not_reached;
    }

    void visit (ast_assign_statement_t& node)
    {
      ast_t::iterator left = node.get_child_ptr (BINARY_LEFT_CHILD);
      check_assignment_target (left, &node);
      ast_t::iterator right = node.get_child_ptr (BINARY_RIGHT_CHILD);
      type_check_rvalue (right, NULL, false);
      typed_value_t left_tv = ast_get_typed_value (*left);
      typed_value_t right_tv = ast_get_typed_value (*right);
      typed_value_convert (right_tv, left_tv);
      if (!type_is_equal (left_tv.type, right_tv.type))
        {
          error_at_line (-1, 0, node.file, node.line,
                         "incompatible types (%s) = (%s)", left_tv.type->to_string ().c_str (), right_tv.type->to_string ().c_str ());
        }
      dynamic_cast<ast_expr_t*> (*right)->set_type (right_tv);
    }

    void visit (ast_change_statement_t& node)
    {
      ast_t::iterator expr = node.expr_iter ();
      string_t identifier = ast_get_identifier (node.identifier ());
      ast_t* type = node.type ();
      ast_t* body = node.body ();

      // Process the expression.
      type_check_rvalue (expr, NULL, false);
      typed_value_t tv = ast_get_typed_value (*expr);

      const type_t* root_type = type_change (tv.type);
      if (root_type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot change expression of type %s", tv.type->to_string ().c_str ());
        }

      // Process the root variable.
      const type_t* proposed_root_type = process_type_spec (type, false);

      if (!type_is_convertible (root_type, proposed_root_type))
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot convert %s to %s in change", root_type->to_string ().c_str (), proposed_root_type->to_string ().c_str ());
        }

      // Enter the new heap root.
      symbol_t* symbol = symbol_make_variable (identifier, root_type, &node);
      enter_symbol (node.symtab, symbol, node.root_symbol);

      // Enter all parameters and variables in scope that are pointers as pointers to foreign.
      symtab_change (node.symtab);

      // Check the body.
      type_check_statement (body);
    }

    void visit (ast_expression_statement_t& node)
    {
      ast_t::iterator child = node.get_child_ptr (UNARY_CHILD);
      type_check_rvalue (child, NULL, false);
      convert_rvalue_to_builtin_types (child);
    }

    void visit (ast_if_statement_t& node)
    {
      ast_t::iterator condition_node = node.get_child_ptr (IF_CONDITION);
      ast_t::iterator true_branch = node.get_child_ptr (IF_TRUE_BRANCH);

      // Check the condition.
      type_check_rvalue (condition_node, NULL, false);

      convert_rvalue_to_builtin_types (condition_node);

      typed_value_t tv = ast_get_typed_value (*condition_node);
      if (type_to_bool (tv.type) == NULL)
        {
          error_at_line (-1, 0, (*condition_node)->file,
                         (*condition_node)->line,
                         "cannot convert (%s) to boolean expression in condition", tv.type->to_string ().c_str ());
        }

      // Check the branch.
      type_check_statement (*true_branch);
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
      AST_FOREACH (child, &node)
        {
          type_check_statement (child);
        }
    }

    void visit (ast_return_statement_t& node)
    {
      // Check the expression.
      ast_t::iterator expr = node.get_child_ptr (UNARY_CHILD);
      type_check_rvalue (expr, NULL, false);
      typed_value_t expr_tv = ast_get_typed_value (*expr);

      // Check that it matches with the return type.
      const type_t* return_type = get_current_return_type (&node);
      assert (return_type != NULL);
      typed_value_t return_tv = typed_value_make (return_type);
      typed_value_convert (expr_tv, return_tv);
      if (!type_is_equal (expr_tv.type, return_tv.type))
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot convert %s to %s in return", expr_tv.type->to_string ().c_str (), return_tv.type->to_string ().c_str ());
        }

      static_cast<ast_expr_t*> (*expr)->set_type (expr_tv);
    }

    void visit (ast_trigger_statement_t& node)
    {
      action_reaction_base_t *action = get_current_action (&node);
      ast_t *expression_list_node = node.expr_list ();
      ast_t *body_node = node.body ();

      trigger_t *trigger = new trigger_t (node);
      action->add_trigger (trigger);
      symtab_set_current_trigger (body_node->symtab, trigger);

      /* Check the triggers. */
      check_rvalue_list (expression_list_node);

      /* Re-insert this as a pointer to mutable. */
      symbol_t *this_symbol = symtab_get_this (node.symtab);
      symbol_t *new_this_symbol =
        symbol_make_receiver_duplicate (this_symbol);
      enter_symbol (node.symtab, new_this_symbol, node.this_symbol);

      /* Check the body. */
      type_check_statement (body_node);
    }

    void visit (ast_var_statement_t& node)
    {
      ast_t::iterator identifier_list = node.get_child_ptr (VAR_IDENTIFIER_LIST);
      ast_t::iterator type_spec = node.get_child_ptr (VAR_TYPE_SPEC);

      // Process the type spec.
      const type_t* type = process_type_spec (*type_spec, true);

      // Enter each symbol.
      AST_FOREACH (child, *identifier_list)
        {
          string_t name = ast_get_identifier (child);
          symbol_t* symbol = symbol_make_variable (name, type, child);
          node.symbols.push_back (symbol_holder ());
          enter_symbol (node.symtab, symbol, node.symbols.back ());
        }
    }

    void visit (ast_println_statement_t& node)
    {
      ast_t::iterator child = node.get_child_ptr (UNARY_CHILD);
      check_rvalue_list (*child);
      // Convert to type so we can evaluate it.
      convert_rvalue_list_to_builtin_types (*child);
    }
  };

  visitor v;
  node->accept (v);
}

static void
control_check_statement (ast_t * node)
{
  struct visitor : public ast_visitor_t
  {
    bool in_trigger_statement;

    visitor () : in_trigger_statement (false) { }

    void visit (ast_change_statement_t& node)
    {
      node.body ()->accept (*this);
    }

    void visit (ast_if_statement_t& node)
    {
      node.at (IF_TRUE_BRANCH)->accept (*this);
    }

    void visit (ast_list_statement_t& node)
    {
      AST_FOREACH (child, &node)
        {
          child->accept (*this);
        }
    }

    void visit (ast_return_statement_t& node)
    {
      // TODO: Maybe.
    }

    void visit (ast_trigger_statement_t& node)
    {
      ast_t *body_node = node.body ();
      action_reaction_base_t *action = get_current_action (&node);

      if (action == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "trigger outside of action or reaction");
        }

      if (in_trigger_statement)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "triggers within triggers are not allowed");
        }

      in_trigger_statement = true;
      body_node->accept (*this);
      in_trigger_statement = false;
    }
  };

  visitor v;
  node->accept (v);
}

/*
  Determine if each trigger mutates the state of a component.
  This requires pointer analysis and could be improved.
  However, pointer analysis solves the halting problem so we must draw the line somewhere.

  The state of a component is only mutable in the body of a trigger statement so we can exclude all other activity.
  The cases to check:
  1.  Assignment to an lvalue derived from the receiver.
  2.  Copying a pointer (to mutable) whose value is derived from the receiver.
      a.  In assignment
      a.  In a call

 */
static void
mutates_check_statement (ast_t * node)
{
  struct derived_visitor : public ast_visitor_t
  {
    bool derived_from_receiver;

    derived_visitor () : derived_from_receiver (false) { }

    void default_action (ast_t& node)
    {
      not_reached;
    }

    void visit (ast_address_of_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_index_expr_t& node)
    {
      node.base ()->accept (*this);
    }

    void visit (ast_select_expr_t& node)
    {
      node.base ()->accept (*this);
    }

    void visit (ast_dereference_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_identifier_expr_t& node)
    {
      symbol_t* symbol = node.symbol.symbol ();

      switch (symbol_kind (symbol))
        {
        case SymbolParameter:
          assert (symbol_parameter_kind (symbol) != ParameterReceiver);
          derived_from_receiver = (symbol_parameter_kind (symbol) == ParameterReceiverDuplicate);
          break;

        case SymbolFunction:
        case SymbolInstance:
        case SymbolType:
        case SymbolTypedConstant:
        case SymbolVariable:
          break;
        }
    }
  };

  struct check_visitor : public ast_visitor_t
  {
    bool mutates_receiver;

    check_visitor () : mutates_receiver (false) { }

    void default_action (ast_t& node)
    {
      not_reached;
    }

    void check_for_pointer_copy (ast_t* node)
    {
      typed_value_t tv = ast_get_typed_value (node);
      if (type_to_pointer (tv.type))
        {
          derived_visitor v;
          node->accept (v);
          mutates_receiver = mutates_receiver || v.derived_from_receiver;
        }
    }

    void visit (ast_call_expr_t& node)
    {
      node.expr ()->accept (*this);

      for (ast_t::iterator pos = node.args ()->begin (), limit = node.args ()->end ();
           pos != limit;
           ++pos)
        {
          check_for_pointer_copy (*pos);
          (*pos)->accept (*this);
        }
    }

    void visit (ast_literal_expr_t& node)
    { }

    void visit (ast_identifier_expr_t& node)
    { }

    void visit (ast_address_of_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_dereference_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_index_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_list_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_select_expr_t& node)
    {
      node.base ()->accept (*this);
    }

    void visit (ast_assign_statement_t& node)
    {
      {
        derived_visitor v;
        node.left ()->accept (v);
        mutates_receiver = mutates_receiver || v.derived_from_receiver;
      }

      check_for_pointer_copy (node.right ());

      node.left ()->accept (*this);
      node.right ()->accept (*this);
    }

    void visit (ast_add_assign_statement_t& node)
    {
      derived_visitor v;
      node.left ()->accept (v);
      mutates_receiver = mutates_receiver || v.derived_from_receiver;

      node.left ()->accept (*this);
      node.right ()->accept (*this);
    }

    void visit (ast_change_statement_t& node)
    {
      node.expr ()->accept (*this);
      node.body ()->accept (*this);
    }

    void visit (ast_expression_statement_t& node)
    {
      node.at (UNARY_CHILD)->accept (*this);
    }

    void visit (ast_if_statement_t& node)
    {
      node.at (IF_CONDITION)->accept (*this);
      node.at (IF_TRUE_BRANCH)->accept (*this);
    }

    void visit (ast_list_statement_t& node)
    {
      AST_FOREACH (child, &node)
        {
          child->accept (*this);
          if (mutates_receiver)
            {
              break;
            }
        }
    }

    void visit (ast_println_statement_t& node)
    {
      node.at (UNARY_CHILD)->accept (*this);
    }
  };

  struct visitor : public ast_visitor_t
  {
    void visit (ast_change_statement_t& node)
    {
      node.body ()->accept (*this);
    }

    void visit (ast_if_statement_t& node)
    {
      node.at (IF_TRUE_BRANCH)->accept (*this);
    }

    void visit (ast_list_statement_t& node)
    {
      AST_FOREACH (child, &node)
        {
          child->accept (*this);
        }
    }

    void visit (ast_trigger_statement_t& node)
    {
      check_visitor v;
      node.body ()->accept (v);
      if (v.mutates_receiver)
        {
          trigger_t *trigger = get_current_trigger (node.body ());
          trigger->action = TRIGGER_WRITE;
        }
    }
  };

  visitor v;
  node->accept (v);
}

static void
enter_signature (ast_t * node, const signature_type_t * type)
{
  for (signature_type_t::ParametersType::const_iterator pos = type->begin (), limit = type->end ();
       pos != limit; ++pos)
    {
      parameter_t* parameter = *pos;
      // Check if the symbol is defined locally.
      string_t identifier = parameter_name (parameter);
      const type_t *type = parameter_type (parameter);
      symbol_t *s = symtab_find_current (node->symtab, identifier);
      if (s == NULL)
        {
          if (parameter_is_receiver (parameter))
            {
              s = symbol_make_receiver (identifier, type, node);
            }
          else
            {
              s = symbol_make_parameter (identifier, type, node);
            }
          symtab_enter (node->symtab, s);
        }
      else
        {
          error_at_line (-1, 0, node->file, node->line,
        		 "%s is already defined in this scope",
        		 get (identifier));
        }
    }
}

/* Check the semantics of all executable code. */
void
process_definitions (ast_t * node)
{
  struct visitor : public ast_visitor_t
  {
    void visit (ast_action_t& node)
    {
      ast_t *receiver_node = node.receiver ();
      ast_t::iterator precondition_node = node.precondition_iter ();
      ast_t *body_node = node.body ();

      /* Insert "this" into the symbol table. */
      // TODO:  Unify with others as entering signature and return value.
      ast_t *this_node = receiver_node->at (RECEIVER_THIS_IDENTIFIER);
      string_t this_identifier = ast_get_identifier (this_node);
      enter_symbol (node.symtab,
                    symbol_make_receiver (this_identifier,
                                          pointer_to_immutable_type_t::make (get_current_receiver_type
                                                                             (&node)), this_node), node.this_symbol);

      /* Check the precondition. */
      type_check_rvalue (precondition_node, NULL, false);

      /* Must be typed since it will be evaluated. */
      convert_rvalue_to_builtin_types (precondition_node);

      typed_value_t tv = ast_get_typed_value (*precondition_node);
      if (type_to_bool (tv.type) == NULL)
        {
          error_at_line (-1, 0, (*precondition_node)->file,
                         (*precondition_node)->line,
                         "cannot convert (%s) to boolean expression in precondition", tv.type->to_string ().c_str ());
        }

      type_check_statement (body_node);
      control_check_statement (body_node);
      mutates_check_statement (body_node);
    }

    void visit (ast_dimensioned_action_t& node)
    {
      ast_t *dimension_node = node.dimension ();
      ast_t *receiver_node = node.receiver ();
      ast_t::iterator precondition_node = node.precondition_iter ();
      ast_t *body_node = node.body ();
      /* Insert "iota" into the symbol table. */
      enter_symbol (node.symtab,
                    symbol_make_parameter (enter ("IOTA"), new iota_type_t (node.action->dimension ()), dimension_node),
                    node.iota_symbol);
      /* Insert "this" into the symbol table. */
      ast_t *this_node = receiver_node->at (RECEIVER_THIS_IDENTIFIER);
      string_t this_identifier = ast_get_identifier (this_node);
      enter_symbol (node.symtab,
                    symbol_make_receiver (this_identifier,
                                          pointer_to_immutable_type_t::make (get_current_receiver_type
                                                                             (&node)), this_node), node.this_symbol);

      /* Check the precondition. */
      type_check_rvalue (precondition_node, NULL, false);

      /* Must be typed since it will be evaluated. */
      convert_rvalue_to_builtin_types (precondition_node);

      typed_value_t tv = ast_get_typed_value (*precondition_node);
      if (type_to_bool (tv.type) == NULL)
        {
          error_at_line (-1, 0, (*precondition_node)->file,
                         (*precondition_node)->line,
                         "cannot convert (%s) to boolean expression in precondition", tv.type->to_string ().c_str ());
        }

      type_check_statement (body_node);
      control_check_statement (body_node);
      mutates_check_statement (body_node);
    }

    void visit (ast_bind_t& node)
    {
      ast_t *receiver_node = node.receiver ();
      ast_t *body_node = node.body ();
      /* Insert "this" into the symbol table. */
      ast_t *this_node =
        receiver_node->at (RECEIVER_THIS_IDENTIFIER);
      string_t this_identifier = ast_get_identifier (this_node);
      enter_symbol (node.symtab,
                    symbol_make_receiver (this_identifier,
                                          pointer_type_t::make
                                          (get_current_receiver_type
                                           (&node)), this_node), node.this_symbol);
      /* Check the body. */
      check_bind_statement (body_node);
      control_check_statement (body_node);
    }

    void visit (ast_function_t& node)
    {
      ast_t *signature_node = node.at (FUNCTION_SIGNATURE);
      ast_t *body_node = node.at (FUNCTION_BODY);
      ast_t *return_type_node = node.at (FUNCTION_RETURN_TYPE);
      function_t *function = get_current_function (&node);

      /* Enter the return type into the symbol table. */
      enter_symbol (node.symtab, symbol_make_return_parameter (enter ("0return"), function_return_type (function), return_type_node), node.return_symbol);

      /* Enter the signature into the symbol table. */
      enter_signature (signature_node, function_signature (function));

      type_check_statement (body_node);
      control_check_statement (body_node);
    }

    void visit (ast_method_t& node)
    {
      ast_t *signature_node = node.at (METHOD_SIGNATURE);
      ast_t *body_node = node.at (METHOD_BODY);
      ast_t *return_type_node = node.at (METHOD_RETURN_TYPE);
      method_t *method = get_current_method (&node);

      /* Enter the return type into the symbol table. */
      enter_symbol (node.symtab, symbol_make_return_parameter (enter ("0return"), method_return_type (method), return_type_node), node.return_symbol);

      /* Enter the signature into the symbol table. */
      enter_signature (signature_node, method_signature (method));

      type_check_statement (body_node);
      control_check_statement (body_node);
    }

    void visit (ast_instance_t& node)
    {
      // Check the initialization function.
      symbol_t* symbol = node.symbol.symbol ();
      const named_type_t* type = symbol_get_instance_type (symbol);
      ast_t* initializer = node.at (INSTANCE_INITIALIZER);
      method_t* method = type->get_method (ast_get_identifier (initializer));
      if (method == NULL)
        {
          error_at_line (-1, 0, initializer->file,
                         initializer->line,
                         "no method named %s",
                         get (ast_get_identifier (initializer)));
        }
      if (method_arity (method) != 1)
        {
          error_at_line (-1, 0, initializer->file,
                         initializer->line,
                         "named method is not null-ary");
        }
      symbol_set_instance_method (symbol, method);
    }

    void visit (ast_reaction_t& node)
    {
      ast_t *receiver_node = node.receiver ();
      ast_t *signature_node = node.signature ();
      ast_t *body_node = node.body ();
      reaction_t *reaction = dynamic_cast<reaction_t*> (get_current_action (&node));
      /* Insert "this" into the symbol table. */
      ast_t *this_node =
        receiver_node->at (RECEIVER_THIS_IDENTIFIER);
      string_t this_identifier = ast_get_identifier (this_node);
      enter_symbol (node.symtab,
                    symbol_make_receiver (this_identifier,
                                          pointer_to_immutable_type_t::make (reaction->component_type ()),
                                          this_node), node.this_symbol);
      /* Enter the signature into the symbol table. */
      enter_signature (signature_node, reaction->reaction_signature ());

      type_check_statement (body_node);
      control_check_statement (body_node);
      mutates_check_statement (body_node);
    }

    void visit (ast_top_level_list_t& node)
    {
      AST_FOREACH (child, &node)
        {
          child->accept (*this);
        }
    }
  };

  visitor v;
  node->accept (v);
}
