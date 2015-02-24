#include "ast.hpp"
#include "symbol.hpp"
#include <error.h>
#include "semantic.hpp"
#include "function.hpp"
#include "method.hpp"
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
      node.set_type (typed_value_make_function (symbol_get_function_function (symbol)));
      break;

    case SymbolInstance:
      unimplemented;

    case SymbolParameter:
      node.set_type (typed_value_make (symbol_parameter_type (symbol), TV_MUTABLE));
      break;

    case SymbolType:
      unimplemented;

    case SymbolTypedConstant:
      node.set_type (symbol_typed_constant_value (symbol));
      break;

    case SymbolVariable:
      node.set_type (typed_value_make (symbol_variable_type (symbol), TV_MUTABLE));
      break;
    }

  node.symbol.symbol (symbol);
}

static const type_t* check_index (const type_t* aggregate_type, typed_value_t idx_tv, const ast_t& error_node)
{
  struct visitor : public const_type_visitor_t
  {
    const type_t* retval;
    const typed_value_t& idx_tv;
    const ast_t& node;

    visitor (const typed_value_t& it, const ast_t& n) : retval (NULL), idx_tv (it), node (n) { }


    void default_action (const type_t& type)
    {
      not_reached;
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

        void visit (const uint_type_t& type)
        {
          if (idx_tv.has_value)
            {
              // Check that the index is in bounds.
              unimplemented;
            }
        }

        void visit (const int_type_t& type)
        {
          if (idx_tv.has_value)
            {
              // Check that the index is in bounds.
              if (idx_tv.int_value < 0)
                {
                  error_at_line (-1, 0, node.file, node.line,
                                 "array index is negative");

                }

              if (static_cast<size_t> (idx_tv.int_value) >= array_type.dimension ())
                {
                  error_at_line (-1, 0, node.file, node.line,
                                 "array index out of bounds");
                }
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

  };
  visitor v (idx_tv, error_node);
  aggregate_type->accept (v);

  return v.retval;
}

static void
type_check_lvalue (ast_t::iterator ptr)
{
  struct check_lvalue_visitor_t : public ast_visitor_t
  {
    ast_t::iterator ptr;

    check_lvalue_visitor_t (ast_t::iterator p) : ptr (p) { }

    void default_action (ast_t& node)
    {
      not_reached;
    }

    void visit (ast_dereference_expr_t& node)
    {
      ast_t::iterator child = node.get_child_ptr (UNARY_CHILD);
      type_check_rvalue (child);
      typed_value_t in = ast_get_typed_value (*child);
      typed_value_t out = typed_value_dereference (in);
      if (out.type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "incompatible types: @(%s)", in.type->to_string ().c_str ());
        }
      node.set_type (out);
    }

    void visit (ast_identifier_expr_t& node)
    {
      check_identifier_expr (node);
    }

    void visit (ast_index_expr_t& node)
    {
      ast_t::iterator expr = node.base_iter ();
      ast_t::iterator idx = node.index_iter ();

      type_check_lvalue (expr);
      type_check_rvalue (idx);

      typed_value_t expr_tv = ast_get_typed_value (*expr);
      typed_value_t idx_tv = ast_get_typed_value (*idx);

      const type_t* result_type = check_index (expr_tv.type, idx_tv, node);

      if (result_type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "incompatible types (%s)[%s]", expr_tv.type->to_string ().c_str (), idx_tv.type->to_string ().c_str ());
        }

      node.set_type (typed_value_make (result_type, TV_MUTABLE));
    }

    void visit (ast_select_expr_t& node)
    {
      ast_t::iterator left = node.base_iter ();
      ast_t *right = node.identifier ();
      string_t identifier = ast_get_identifier (right);
      type_check_lvalue (left);
      typed_value_t tv = ast_get_typed_value (*left);

      const type_t* t = tv.type;

      if (type_dereference (t))
        {
          // Selecting from a pointer.  Insert a dereference.
          ast_dereference_expr_t* deref = new ast_dereference_expr_t (node.line, *left);
          deref->set_type (typed_value_dereference (tv));
          *left = deref;
          tv = ast_get_typed_value (*left);
        }

      node.field = type_select_field (tv.type, identifier);
      if (node.field)
        {
          node.set_type (typed_value_make (field_type (node.field), tv.kind));
          return;
        }

      method_t* m = type_select_method (tv.type, identifier);
      if (m)
        {
          node.set_type (typed_value_make_method (m));
          return;
        }

      reaction_t* r = type_select_reaction (tv.type, identifier);
      if (r)
        {
          node.set_type (typed_value_make_reaction (r));
          return;
        }

      error_at_line (-1, 0, node.file, node.line,
                     "cannot select %s from expression of type %s",
                     get (identifier), tv.type->to_string ().c_str ());
    }
  };
  check_lvalue_visitor_t check_lvalue_visitor (ptr);
  (*ptr)->accept (check_lvalue_visitor);
}

static void
check_assignment_target (ast_t::iterator left)
{
  type_check_lvalue (left);
  typed_value_t tv = ast_get_typed_value (*left);
  if (tv.kind != TV_MUTABLE)
    {
      error_at_line (-1, 0, (*left)->file, (*left)->line,
                     "cannot assign to read-only location of type %s", tv.type->to_string ().c_str ());
    }

  if (type_contains_pointer_to_foreign (tv.type))
    {
      error_at_line (-1, 0, (*left)->file, (*left)->line,
                     "assignment leaks pointer to foreign");
    }
}

static void arithmetic_assign (ast_binary_t* node, const char* symbol)
{
  ast_t::iterator left = node->left_iter ();
  check_assignment_target (left);
  ast_t::iterator right = node->right_iter ();
  type_check_rvalue (right);
  typed_value_t left_tv = ast_get_typed_value (*left);
  typed_value_t right_tv = ast_get_typed_value (*right);
  if (!type_is_convertible (left_tv.type, right_tv.type))
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
}

static void check_rvalue_list (ast_t * node);

static bool
in_mutable_section (const ast_t* node)
{
  return get_current_action (node) != NULL &&
    get_current_trigger (node) != NULL;
}

static bool logic_or (bool x, bool y) { return x || y; }
static bool logic_and (bool x, bool y) { return x && y; }

void
type_check_rvalue (ast_t::iterator ptr)
{
  struct check_rvalue_visitor_t : public ast_visitor_t
  {
    ast_t::iterator ptr;

    check_rvalue_visitor_t (ast_t::iterator p) : ptr (p) { }

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
      binary_arithmetic (node, "+");
    }

    void check_address_of (ast_address_of_expr_t& node)
    {
      ast_t::iterator expr = node.get_child_ptr (UNARY_CHILD);
      typed_value_t in = ast_get_typed_value (*expr);
      typed_value_t out = typed_value_address_of (in);
      if (out.type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "incompatible types: (%s)&", in.type->to_string ().c_str ());
        }
      node.set_type (out);
    }

    void visit (ast_address_of_expr_t& node)
    {
      ast_t::iterator expr = node.get_child_ptr (UNARY_CHILD);
      type_check_lvalue (expr);
      check_address_of (node);
    }

    void check_call (ast_expr_t& node, const signature_type_t* signature, const type_t* return_type, ast_t* args)
    {
      size_t argument_count = args->size ();
      size_t parameter_count = signature->arity ();
      if (argument_count != parameter_count)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "method call expects %zd arguments but given %zd",
                         parameter_count, argument_count);
        }

      size_t idx = 0;
      for (signature_type_t::const_iterator pos = signature->begin (),
             limit = signature->end ();
           pos != limit;
           ++pos, ++idx)
        {
          ast_t *arg = args->at (idx);
          typed_value_t argument_tv  = ast_get_typed_value (arg);
          typed_value_t parameter_tv = typed_value_make (parameter_type (*pos), TV_MUTABLE);
          if (!type_is_convertible (parameter_tv.type, argument_tv.type))
            {
              error_at_line (-1, 0, arg->file,
                             arg->line,
                             "cannot convert %s to %s for argument %zd", argument_tv.type->to_string ().c_str (), parameter_tv.type->to_string ().c_str (), idx);
            }
        }

      // Set the return type.
      node.set_type (typed_value_make (return_type, TV_MUTABLE));
    }

    void visit (ast_call_expr_t& node)
    {
      ast_t::iterator expr = node.expr_iter ();
      ast_t *args = node.args ();

      // Analyze the args.
      check_rvalue_list (args);

      // Analyze the callee.
      type_check_rvalue (expr);

      typed_value_t expr_tv = ast_get_typed_value (*expr);

      // Fixup method calls.
      struct visitor : public const_type_visitor_t
      {
        check_rvalue_visitor_t& rvalue_visitor;
        ast_call_expr_t& node;

        visitor (check_rvalue_visitor_t& rv,
                 ast_call_expr_t& n) : rvalue_visitor (rv), node (n) { }

        void default_action (const type_t& type)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot call %s", type.to_string ().c_str ());

        }

        void visit (const function_type_t& type)
        {
          rvalue_visitor.check_call (node, type.signature (), type.return_type (), node.args ());
        }

        void visit (const method_type_t& type)
        {
          rvalue_visitor.check_call (node, type.signature, type.return_type, node.args ());

          // Transfer the computation for the receiver to the argument list.
          // The receiver is either a copy or a pointer.
          bool receiver_is_pointer = type_dereference (type.receiver_type);
          ast_t* expr = node.expr ();
          ast_t* args = node.args ();

          ast_t* receiver_select_expr = dynamic_cast<ast_select_expr_t*> (expr)->base ();
          if (receiver_is_pointer)
            {
              ast_address_of_expr_t* e = new ast_address_of_expr_t (node.line, receiver_select_expr);
              rvalue_visitor.check_address_of (*e);
              args->prepend (e);
            }
          else
            {
              args->prepend (receiver_select_expr);
            }

          if (in_mutable_section (&node))
            {
              const named_type_t* t = type.named_type;
              if (type_cast<component_type_t> (type_strip (t)))
                {
                  // Invoking a method on a component in a mutable section.
                  // Ensure the receiver is this.
                  std::cout << node.line << '\n';
                  unimplemented;
                }
            }
        }
      };

      visitor v (*this, node);
      expr_tv.type->accept (v);
      assert (expr_tv.has_value);
    }

    void visit (ast_dereference_expr_t& node)
    {
      ast_t::iterator child = node.get_child_ptr (UNARY_CHILD);
      type_check_rvalue (child);
      typed_value_t in = ast_get_typed_value (*child);
      typed_value_t out = typed_value_dereference (in);
      if (out.type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "incompatible types: (%s)@", out.type->to_string ().c_str ());
        }
      node.set_type (out);
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
      type_check_lvalue (ptr);
    }

    void visit (ast_logic_and_expr_t& node)
    {
      binary_boolean (node, typed_value_make_bool (bool_type_t::instance (), true), "&&", logic_and);
    }

    void visit (ast_logic_not_expr_t& node)
    {
      ast_t::iterator child = node.get_child_ptr (UNARY_CHILD);
      type_check_rvalue (child);
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

        void visit (const bool_type_t& type)
        {
          if (tv.has_value)
            {
              tv.bool_value = !tv.bool_value;
            }
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
      binary_boolean (node, typed_value_make_bool (bool_type_t::instance (), false), "||", logic_or);
    }

    void visit (ast_merge_expr_t& node)
    {
      ast_t::iterator child = node.get_child_ptr (UNARY_CHILD);
      type_check_rvalue (child);
      typed_value_t tv = ast_get_typed_value (*child);
      const type_t* type = type_merge (tv.type);
      if (type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot merge expression of type %s", tv.type->to_string ().c_str ());
        }
      tv = typed_value_make (type, TV_MUTABLE);
      node.set_type (tv);
    }

    void visit (ast_move_expr_t& node)
    {
      ast_t::iterator child = node.get_child_ptr (UNARY_CHILD);
      type_check_rvalue (child);
      typed_value_t tv = ast_get_typed_value (*child);
      const type_t* type = type_move (tv.type);
      if (type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot move expression of type %s", tv.type->to_string ().c_str ());
        }
      tv = typed_value_make (type, TV_MUTABLE);
      node.set_type (tv);
    }

    void visit (ast_new_expr_t& node)
    {
      ast_t* type_spec = node.at (UNARY_CHILD);
      const type_t* t = process_type_spec (type_spec, false);
      const type_t* type = pointer_type_t::make (t);
      node.set_type (typed_value_make (type, TV_MUTABLE));
    }

    void visit (ast_not_equal_expr_t& node)
    {
      check_comparison (node, "!=");
    }

    void visit (ast_port_call_expr_t& node)
    {
      ast_t *expr = node.identifier ();
      ast_t *args = node.args ();
      string_t port_identifier = ast_get_identifier (expr);
      const type_t *this_type = get_current_receiver_type (&node);
      const port_type_t *port_type = type_cast<port_type_t> (type_select (this_type, port_identifier));
      if (port_type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "no port named %s", get (port_identifier));
        }
      check_rvalue_list (args);
      check_call (node, port_type->signature (), void_type_t::instance (), args);
      node.field = type_select_field (this_type, port_identifier);
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

      const array_type_t* array_type = type_cast<array_type_t> (type);
      if (!array_type)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "%s is not an array of ports", get (port_identifier));
        }

      const port_type_t* port_type = type_cast<port_type_t> (array_type->base_type ());

      if (!port_type)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "%s is not an array of ports", get (port_identifier));
        }

      ast_t::iterator index = node.index_iter ();
      type_check_rvalue (index);
      check_index (array_type, ast_get_typed_value (*index), **index);

      ast_t *args = node.args ();
      check_rvalue_list (args);
      check_call (node, port_type->signature (), void_type_t::instance (), args);

      node.field = type_select_field (this_type, port_identifier);
      node.array_type = array_type;
    }

    void visit (ast_select_expr_t& node)
    {
      type_check_lvalue (ptr);
    }

    void
    check_comparison (ast_binary_expr_t& node, const char* operation)
    {
      ast_t::iterator left = node.left_iter ();
      ast_t::iterator right = node.right_iter ();
      type_check_rvalue (left);
      type_check_rvalue (right);
      typed_value_t left_tv = ast_get_typed_value (*left);
      typed_value_t right_tv = ast_get_typed_value (*right);
      if (!type_is_equivalent (left_tv.type, right_tv.type))
        {
          error_at_line (-1, 0, node.file, node.line,
                         "incompatible types (%s) %s (%s)", left_tv.type->to_string ().c_str (), operation, right_tv.type->to_string ().c_str ());

        }

      node.set_type (typed_value_make (bool_type_t::instance (), TV_MUTABLE));
    }

    void
    binary_boolean (ast_expr_t& node, const typed_value_t identity_value, const char* operator_str, bool (*func) (bool, bool))
    {
      typed_value_t tv = identity_value;
      for (ast_t::iterator child = node.begin (), limit = node.end ();
           child != limit;
           ++child)
        {
          type_check_rvalue (child);
          typed_value_t v = ast_get_typed_value (*child);

          if (!type_is_equivalent (tv.type, v.type))
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
                  tv.bool_value = func (tv.bool_value, v.bool_value);
                }
              else
                {
                  tv.has_value = false;
                }
            }

            void default_action (const type_t& type)
            {
              error_at_line (-1, 0, node.file, node.line,
                             "incompatible types (%s) %s (%s)", tv.type->to_string ().c_str (), operator_str, v.type->to_string ().c_str ());
            }
          };
          visitor vis (node, operator_str, func, tv, v);
          tv.type->accept (vis);
        }

      node.set_type (tv);
    }

    void
    binary_arithmetic (ast_binary_expr_t& node, const char* operator_str)
    {
      type_check_rvalue (node.left_iter ());
      type_check_rvalue (node.right_iter ());
      typed_value_t left_tv = ast_get_typed_value (node.left ());
      typed_value_t right_tv = ast_get_typed_value (node.right ());
      typed_value_t tv = typed_value_binary_arithmetic (left_tv, right_tv);
      if (!tv.type)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "incompatible types (%s) %s (%s)", left_tv.type->to_string ().c_str (), operator_str, right_tv.type->to_string ().c_str ());
        }
      node.set_type (tv);
    }

  };
  check_rvalue_visitor_t check_rvalue_visitor (ptr);
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
          type_check_rvalue (child);
        }
    }
  };

  check_rvalue_list_visitor_t check_rvalue_list_visitor;
  node->accept (check_rvalue_list_visitor);
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

    typed_value_t bind (ast_t& node, ast_t::iterator port_node, ast_t::iterator reaction_node)
    {
      type_check_lvalue (port_node);
      type_check_rvalue (reaction_node);

      typed_value_t port_tv = ast_get_typed_value (*port_node);
      typed_value_t reaction_tv = ast_get_typed_value (*reaction_node);

      const port_type_t *port_type = type_cast<port_type_t> (port_tv.type);

      if (port_type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "source of bind is not a port");
        }

      const reaction_type_t *reaction_type = type_cast<reaction_type_t> (reaction_tv.type);
      if (reaction_type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "target of bind is not a reaction");
        }

      if (!type_is_equal (port_type->signature (), reaction_type->signature ()))
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot bind %s to %s", port_type->to_string ().c_str (), reaction_type->to_string ().c_str ());
        }

      return reaction_tv;
    }

    void visit (ast_bind_statement_t& node)
    {
      bind (node, node.left_iter (), node.right_iter ());
    }

    void visit (ast_bind_param_statement_t& node)
    {
      typed_value_t reaction_tv = bind (node, node.left_iter (), node.right_iter ());
      ast_t::iterator param_node = node.param_iter ();
      type_check_rvalue (param_node);
      typed_value_t param_tv = ast_get_typed_value (*param_node);
      assert (reaction_tv.has_value);
      reaction_t* reaction = reaction_tv.reaction_value;
      if (!reaction->has_dimension ())
        {
          error_at_line (-1, 0, node.file, node.line,
                         "parameter specified for non-parameterized reaction");
        }
      check_index (new array_type_t (reaction->dimension (), reaction->reaction_type ()), param_tv, **param_node);
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
      ast_t::iterator left = node.left_iter ();
      check_assignment_target (left);
      ast_t::iterator right = node.right_iter ();
      type_check_rvalue (right);
      typed_value_t left_tv = ast_get_typed_value (*left);
      typed_value_t right_tv = ast_get_typed_value (*right);

      if (!type_is_convertible (left_tv.type, right_tv.type))
        {
          error_at_line (-1, 0, node.file, node.line,
                         "incompatible types (%s) = (%s)", left_tv.type->to_string ().c_str (), right_tv.type->to_string ().c_str ());
        }

      switch (right_tv.kind)
        {
        case TV_MUTABLE:
          // No check.
          break;
        case TV_IMMUTABLE:
          if (!type_is_immutable_safe (right_tv.type))
            {
              error_at_line (-1, 0, node.file, node.line,
                             "assignment leaks mutable pointers");
            }
          break;
        case TV_FOREIGN:
          if (!type_is_foreign_safe (right_tv.type))
            {
              error_at_line (-1, 0, node.file, node.line,
                             "assignment leaks mutable pointers");
            }
          break;
        case TV_CONSTANT:
          // No check.
          break;
        }
    }

    void visit (ast_change_statement_t& node)
    {
      ast_t::iterator expr = node.expr_iter ();
      string_t identifier = ast_get_identifier (node.identifier ());
      ast_t* type = node.type ();
      ast_t* body = node.body ();

      // Process the expression.
      type_check_rvalue (expr);
      typed_value_t tv = ast_get_typed_value (*expr);

      const type_t* root_type = type_change (tv.type);
      if (root_type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot change expression of type %s", tv.type->to_string ().c_str ());
        }

      // Process the root variable.
      const type_t* proposed_root_type = process_type_spec (type, false);

      if (!type_is_convertible (proposed_root_type, root_type))
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
      type_check_rvalue (child);
    }

    void visit (ast_if_statement_t& node)
    {
      ast_t::iterator condition_node = node.get_child_ptr (IF_CONDITION);
      ast_t::iterator true_branch = node.get_child_ptr (IF_TRUE_BRANCH);

      // Check the condition.
      type_check_rvalue (condition_node);

      typed_value_t tv = ast_get_typed_value (*condition_node);

      if (type_cast<bool_type_t> (type_strip (tv.type)) == NULL)
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
      type_check_rvalue (expr);
      typed_value_t expr_tv = ast_get_typed_value (*expr);

      // Check that it matches with the return type.
      const type_t* return_type = get_current_return_type (&node);
      assert (return_type != NULL);
      typed_value_t return_tv = typed_value_make (return_type, TV_MUTABLE);
      if (!type_is_convertible (return_tv.type, expr_tv.type))
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot convert %s to %s in return", expr_tv.type->to_string ().c_str (), return_tv.type->to_string ().c_str ());
        }
    }

    void visit (ast_increment_statement_t& node)
    {
      ast_t::iterator expr = node.child_iter ();
      check_assignment_target (expr);
      struct visitor : public const_type_visitor_t
      {
        ast_t& node;

        visitor (ast_t& n) : node (n) { }

        void default_action (const type_t& type)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot increment location of type %s", type.to_string ().c_str ());
        }

        void visit (const named_type_t& type)
        {
          type.subtype ()->accept (*this);
        }

        void visit (const uint_type_t& type)
        {
          // Okay.
        }
      };
      visitor v (node);
      static_cast<ast_expr_t*> (*expr)->get_type ().type->accept (v);
    }

    void visit (ast_decrement_statement_t& node)
    {
      unimplemented;
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
      if (type_cast<pointer_type_t> (tv.type))
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

    void visit (ast_increment_statement_t& node)
    {
      derived_visitor v;
      node.child ()->accept (v);
      mutates_receiver = mutates_receiver || v.derived_from_receiver;

      node.child ()->accept (*this);
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
    void default_action (ast_t& node)
    {
      not_reached;
    }

    void visit (ast_type_definition_t& node)
    { }

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
      type_check_rvalue (precondition_node);

      /* Must be typed since it will be evaluated. */
      typed_value_t tv = ast_get_typed_value (*precondition_node);

      if (type_cast<bool_type_t> (type_strip (tv.type)) == NULL)
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

      /* Insert "this" into the symbol table. */
      ast_t *this_node = receiver_node->at (RECEIVER_THIS_IDENTIFIER);
      string_t this_identifier = ast_get_identifier (this_node);
      enter_symbol (node.symtab,
                    symbol_make_receiver (this_identifier,
                                          pointer_to_immutable_type_t::make (get_current_receiver_type
                                                                             (&node)), this_node), node.this_symbol);

      /* Insert "iota" into the symbol table. */
      enter_symbol (node.symtab,
                    symbol_make_parameter (enter ("IOTA"), new iota_type_t (node.action->dimension ()), dimension_node),
                    node.iota_symbol);

      /* Check the precondition. */
      type_check_rvalue (precondition_node);

      /* Must be typed since it will be evaluated. */
      typed_value_t tv = ast_get_typed_value (*precondition_node);

      if (type_cast<bool_type_t> (type_strip (tv.type)) == NULL)
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
      enter_symbol (node.symtab, symbol_make_return_parameter (enter ("0return"), function->func_type->return_type (), return_type_node), node.return_symbol);

      /* Enter the signature into the symbol table. */
      enter_signature (signature_node, function->func_type->signature ());

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
      enter_symbol (node.symtab, symbol_make_return_parameter (enter ("0return"), method->method_type->return_type, return_type_node), node.return_symbol);

      /* Enter the signature into the symbol table. */
      enter_signature (signature_node, method->method_type->function_type->signature ());

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
      if (method->method_type->signature->arity () != 0)
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
      enter_signature (signature_node, reaction->reaction_type ()->signature ());

      type_check_statement (body_node);
      control_check_statement (body_node);
      mutates_check_statement (body_node);
    }

    void visit (ast_dimensioned_reaction_t& node)
    {
      ast_t *dimension_node = node.dimension ();
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


      /* Insert "iota" into the symbol table. */
      enter_symbol (node.symtab,
                    symbol_make_parameter (enter ("IOTA"), new iota_type_t (node.reaction->dimension ()), dimension_node),
                    node.iota_symbol);

      /* Enter the signature into the symbol table. */
      enter_signature (signature_node, reaction->reaction_type ()->signature ());

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
