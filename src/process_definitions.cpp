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

symbol_t*
enter_symbol (symtab_t* symtab, symbol_t * symbol, symbol_holder& holder)
{
  // Check if the symbol is defined locally.
  const std::string& identifier = symbol_identifier (symbol);
  symbol_t *s = symtab->find_current (identifier);
  if (s == NULL)
    {
      symtab->enter (symbol);
      holder.symbol (symbol);
    }
  else
    {
      const ast_t* node = symbol_defining_node (symbol);
      error_at_line (-1, 0, node->file, node->line,
		     "%s is already defined in this scope", identifier.c_str ());
    }
  return symbol;
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
check_assignment (typed_value_t left_tv, typed_value_t right_tv, const ast_t& node, const char* conversion_message, const char* leak_message, const char* store_foreign_message)
{
  assert (left_tv.type != NULL);
  assert (left_tv.kind == typed_value_t::REFERENCE);
  assert (right_tv.type != NULL);
  assert (right_tv.kind == typed_value_t::VALUE);

  if (!type_is_convertible (left_tv.type, right_tv.type))
    {
      error_at_line (-1, 0, node.file, node.line,
                     conversion_message, left_tv.type->to_string ().c_str (), right_tv.type->to_string ().c_str ());
    }

  if (type_contains_pointer (left_tv.type))
    {
      if (left_tv.dereference_mutability < right_tv.intrinsic_mutability ||
          left_tv.dereference_mutability < right_tv.dereference_mutability)
        {
          error_at_line (-1, 0, node.file, node.line,
                         leak_message);
        }

      if (right_tv.intrinsic_mutability == FOREIGN &&
          left_tv.region != typed_value_t::STACK)
        {
          error_at_line (-1, 0, node.file, node.line,
                         store_foreign_message);
        }
    }
}

static void check_rvalue_list (ast_t * node);

static bool
in_mutable_section (const ast_t* node)
{
  return get_current_action (node) != NULL &&
    get_current_trigger (node) != NULL;
}



typed_value_t
type_check_expr (ast_t* ptr)
{
  struct check_visitor : public ast_visitor_t
  {
    ast_t* ptr;

    check_visitor (ast_t* p) : ptr (p) { }

    void default_action (ast_t& node)
    {
      not_reached;
    }

    void visit (ast_indexed_port_call_expr_t& node)
    {
      const std::string& port_identifier = ast_get_identifier (node.identifier ());
      const type_t *this_type = get_current_receiver_type (&node);
      const type_t *type = type_select (this_type, port_identifier);

      if (type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "no port named %s", port_identifier.c_str ());
        }

      const array_type_t* array_type = type_cast<array_type_t> (type);
      if (!array_type)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "%s is not an array of ports", port_identifier.c_str ());
        }

      const port_type_t* port_type = type_cast<port_type_t> (array_type->base_type ());

      if (!port_type)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "%s is not an array of ports", port_identifier.c_str ());
        }

      ast_t* index = node.index ();
      type_check_expr (index);
      check_index (array_type, ast_get_typed_value (index), *index);

      ast_t *args = node.args ();
      check_rvalue_list (args);
      typed_value_t tv = typed_value_t::make_value (void_type_t::instance (),
                                                    typed_value_t::STACK,
                                                    IMMUTABLE,
                                                    IMMUTABLE);
      check_call (node, port_type->signature (), tv, args);
      node.field = type_select_field (this_type, port_identifier);
      node.array_type = array_type;
    }

    void visit (ast_merge_expr_t& node)
    {
      ast_t* child = node.child ();
      typed_value_t in = type_check_expr (child);
      typed_value_t out = typed_value_t::merge (in);
      if (out.type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot merge expression of type %s", in.type->to_string ().c_str ());
        }
      node.set_type (out);
    }

    void visit (ast_move_expr_t& node)
    {
      ast_t* child = node.child ();
      typed_value_t in = type_check_expr (child);
      typed_value_t out = typed_value_t::move (in);
      if (out.type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot move expression of type %s", in.type->to_string ().c_str ());
        }
      node.set_type (out);
    }

    void visit (ast_new_expr_t& node)
    {
      ast_t* type_spec = node.child ();
      const type_t* t = process_type_spec (type_spec, false);
      const type_t* type = pointer_type_t::make (t);
      node.set_type (typed_value_t::make_value (type, typed_value_t::HEAP, MUTABLE, MUTABLE));
    }

    void visit (ast_identifier_expr_t& node)
    {
      ast_t *identifier_node = node.child ();
      const std::string& identifier = ast_get_identifier (identifier_node);
      symbol_t *symbol = node.symtab->find (identifier);
      if (symbol == NULL)
        {
          error_at_line (-1, 0, identifier_node->file,
                         identifier_node->line, "%s is not defined",
                         identifier.c_str ());
        }

      switch (symbol_kind (symbol))
        {
        case SymbolFunction:
          node.set_type (typed_value_t::make_ref (typed_value_t (symbol_get_function_function (symbol))));
          break;

        case SymbolInstance:
          unimplemented;

        case SymbolParameter:
          node.set_type (symbol_parameter_value (symbol));
          break;

        case SymbolType:
          unimplemented;

        case SymbolTypedConstant:
          node.set_type (symbol_typed_constant_value (symbol));
          break;

        case SymbolVariable:
          node.set_type (symbol_variable_value (symbol));
          break;

        case SymbolHidden:
          error_at_line (-1, 0, identifier_node->file,
                         identifier_node->line, "%s is not accessible in this scope",
                         identifier.c_str ());
          break;
        }

      node.symbol.symbol (symbol);
    }

    void visit (ast_implicit_dereference_expr_t& node)
    {
      typed_value_t tv = type_check_expr (node.child ());
      tv = typed_value_t::implicit_dereference (tv);
      node.set_type (tv);
    }

    void visit (ast_select_expr_t& node)
    {
      ast_t::iterator left = node.base_iter ();
      ast_t *right = node.identifier ();
      const std::string& identifier = ast_get_identifier (right);
      typed_value_t in = type_check_expr (*left);

      assert (in.type != NULL);
      assert (in.kind == typed_value_t::REFERENCE);

      if (type_dereference (in.type))
        {
          // Selecting from a pointer.
          // Insert an implicit dereference.
          ast_implicit_dereference_expr_t* id = new ast_implicit_dereference_expr_t (node.line, *left);
          in = typed_value_t::implicit_dereference (in);
          id->set_type (in);

          // Insert a dereference.
          ast_dereference_expr_t* deref = new ast_dereference_expr_t (node.line, id);
          in = typed_value_t::dereference (in);
          deref->set_type (in);
          *left = deref;
        }

      typed_value_t out = typed_value_t::select (in, identifier);

      if (out.type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot select %s from expression of type %s",
                         identifier.c_str (), in.type->to_string ().c_str ());
        }

      node.set_type (out);
    }

    void visit (ast_dereference_expr_t& node)
    {
      ast_t* child = node.child ();
      typed_value_t in = type_check_expr (child);
      typed_value_t out = typed_value_t::dereference (in);
      if (out.type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "incompatible types: (%s)@", in.type->to_string ().c_str ());
        }
      node.set_type (out);
    }

    void visit (ast_literal_expr_t& node)
    {
      // Do nothing.
    }

    void check_address_of (ast_address_of_expr_t& node)
    {
      ast_t* expr = node.child ();
      typed_value_t in = ast_get_typed_value (expr);
      typed_value_t out = typed_value_t::address_of (in);
      if (out.type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "incompatible types: (%s)&", in.type->to_string ().c_str ());
        }
      node.set_type (out);
    }

    void visit (ast_address_of_expr_t& node)
    {
      ast_t* expr = node.child ();
      type_check_expr (expr);
      check_address_of (node);
    }

    void visit (ast_logic_not_expr_t& node)
    {
      ast_t* child = node.child ();
      typed_value_t in = type_check_expr (child);
      typed_value_t out = typed_value_t::logic_not (in);
      if (out.type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "incompatible types (%s) !", in.type->to_string ().c_str ());
        }
      node.set_type (out);
    }

    void visit (ast_binary_arithmetic_expr_t& node)
    {
      typed_value_t left = type_check_expr (node.left ());
      typed_value_t right = type_check_expr (node.right ());
      typed_value_t result;
      const char* operator_str = "";

      switch (node.arithmetic)
        {
        case ast_binary_arithmetic_expr_t::EQUAL:
          result = typed_value_t::equal (left, right);
          operator_str = "==";
          break;
        case ast_binary_arithmetic_expr_t::NOT_EQUAL:
          result = typed_value_t::not_equal (left, right);
          operator_str = "!=";
          break;
        case ast_binary_arithmetic_expr_t::LOGIC_OR:
          result = typed_value_t::logic_or (left, right);
          operator_str = "||";
          break;
        case ast_binary_arithmetic_expr_t::LOGIC_AND:
          result = typed_value_t::logic_and (left, right);
          operator_str = "&&";
          break;
        case ast_binary_arithmetic_expr_t::ADD:
          result = typed_value_t::add (left, right);
          operator_str = "+";
          break;
        case ast_binary_arithmetic_expr_t::SUBTRACT:
          unimplemented;
          //binary_arithmetic (node, "-");
          break;
        }

      if (result.type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "incompatible types (%s) %s (%s)", left.type->to_string ().c_str (), operator_str, right.type->to_string ().c_str ());
        }

      node.set_type (result);
    }

    void check_call (ast_expr_t& node, const signature_type_t* signature, typed_value_t return_value, ast_t* args)
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
          typed_value_t parameter_tv = typed_value_t::make_ref ((*pos)->value);
          check_assignment (parameter_tv, argument_tv, *arg,
                            "incompatible types (%s) = (%s)",
                            "argument leaks mutable pointers",
                            "argument may store foreign pointer");
        }

      // Set the return type.
      node.set_type (return_value);
    }

    void visit (ast_call_expr_t& node)
    {
      ast_t* expr = node.expr ();
      ast_t* args = node.args ();

      // Analyze the args.
      check_rvalue_list (args);

      // Analyze the callee.
      typed_value_t expr_tv = type_check_expr (expr);

      struct visitor : public const_type_visitor_t
      {
        check_visitor& rvalue_visitor;
        ast_call_expr_t& node;

        visitor (check_visitor& rv,
                 ast_call_expr_t& n) : rvalue_visitor (rv), node (n) { }

        void default_action (const type_t& type)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "cannot call %s", type.to_string ().c_str ());
        }

        void visit (const function_type_t& type)
        {
          node.kind = ast_call_expr_t::FUNCTION;
          rvalue_visitor.check_call (node, type.signature (), type.return_parameter ()->value, node.args ());
        }

        void visit (const pfunc_type_t& type)
        {
          node.kind = ast_call_expr_t::PFUNC;
          rvalue_visitor.check_call (node, type.signature (), type.return_parameter ()->value, node.args ());
          if (in_mutable_section (&node))
            {
              error_at_line (-1, 0, node.file, node.line,
                             "cannot call pfunc in mutable section");

            }
        }

        void visit (const method_type_t& type)
        {
          node.kind = ast_call_expr_t::METHOD;
          rvalue_visitor.check_call (node, type.signature (), type.return_parameter ()->value, node.args ());

          if (type_dereference (type.receiver_type) != NULL)
            {
              // Method expects a pointer.  Insert address of.
              // Strip off implicit deref and select.
              ast_t* receiver_select_expr = node.expr ()->children[0]->children[0];
              ast_address_of_expr_t* e = new ast_address_of_expr_t (node.line, receiver_select_expr);
              rvalue_visitor.check_address_of (*e);
              node.expr ()->children[0]->children[0] = e;
            }

          if (type_cast<component_type_t> (type_strip (type.named_type)))
            {
              // Invoking a method on a component.
              if (in_mutable_section (&node))
                {
                  // Invoking a method on a component in a mutable section.
                  // Ensure the receiver is this.
                  // TODO
                  std::cout << node.line << '\n';
                  unimplemented;
                }
            }

          typed_value_t argument_tv = ast_get_typed_value (node.expr ()->children[0]->children[0]);
          typed_value_t parameter_tv = typed_value_t::make_ref (type.this_parameter->value);
          check_assignment (parameter_tv, argument_tv, node,
                            "call expects %s but given %s",
                            "argument leaks mutable pointers",
                            "argument may store foreign pointer");
        }
      };

      visitor v (*this, node);
      expr_tv.type->accept (v);
      assert (expr_tv.type != NULL);
    }

    void visit (ast_port_call_expr_t& node)
    {
      ast_t *expr = node.identifier ();
      ast_t *args = node.args ();
      const std::string& port_identifier = ast_get_identifier (expr);
      const type_t *this_type = get_current_receiver_type (&node);
      const port_type_t *port_type = type_cast<port_type_t> (type_select (this_type, port_identifier));
      if (port_type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "no port named %s", port_identifier.c_str ());
        }
      check_rvalue_list (args);
      typed_value_t tv = typed_value_t::make_value (void_type_t::instance (),
                                                    typed_value_t::STACK,
                                                    IMMUTABLE,
                                                    IMMUTABLE);
      check_call (node, port_type->signature (), tv, args);
      node.field = type_select_field (this_type, port_identifier);
    }

    void visit (ast_index_expr_t& node)
    {
      ast_t* expr = node.base ();
      ast_t* idx = node.index ();
      typed_value_t expr_tv = type_check_expr (expr);
      typed_value_t idx_tv = type_check_expr (idx);
      typed_value_t result = typed_value_t::index (expr_tv, idx_tv);
      if (result.type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "incompatible types (%s)[%s]", expr_tv.type->to_string ().c_str (), idx_tv.type->to_string ().c_str ());
        }
      node.set_type (result);
    }

  };
  check_visitor check_lvalue_visitor (ptr);
  ptr->accept (check_lvalue_visitor);
  return ast_get_typed_value (ptr);
}

// void
// type_check_rvalue (ast_t::iterator ptr)
// {
//   struct check_rvalue_visitor_t : public ast_visitor_t
//   {

//     void
//     binary_arithmetic (ast_binary_expr_t& node, const char* operator_str)
//     {
//       type_check_rvalue (node.left_iter ());
//       type_check_rvalue (node.right_iter ());
//       typed_value_t left_tv = ast_get_typed_value (node.left ());
//       typed_value_t right_tv = ast_get_typed_value (node.right ());
//       typed_value_t tv = typed_value_binary_arithmetic (left_tv, right_tv);
//       if (!tv.type)
//         {
//           error_at_line (-1, 0, node.file, node.line,
//                          "incompatible types (%s) %s (%s)", left_tv.type->to_string ().c_str (), operator_str, right_tv.type->to_string ().c_str ());
//         }
//       node.set_type (tv);
//     }

//   };
//   check_rvalue_visitor_t check_rvalue_visitor (ptr);
//   (*ptr)->accept (check_rvalue_visitor);
// }

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
          type_check_expr (*child);
        }
    }
  };

  check_rvalue_list_visitor_t check_rvalue_list_visitor;
  node->accept (check_rvalue_list_visitor);
}

static void
check_condition (ast_t* condition_node)
{
  type_check_expr (condition_node);
  typed_value_t tv = ast_get_typed_value (condition_node);
  if (!type_is_boolean (tv.type))
    {
      error_at_line (-1, 0, condition_node->file,
                     condition_node->line,
                     "cannot convert (%s) to boolean expression in condition", tv.type->to_string ().c_str ());
    }
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

    void visit (ast_empty_statement_t& node)
    { }

    typed_value_t bind (ast_t& node, ast_t* port_node, ast_t* reaction_node)
    {
      type_check_expr (port_node);
      type_check_expr (reaction_node);

      typed_value_t port_tv = ast_get_typed_value (port_node);
      typed_value_t reaction_tv = ast_get_typed_value (reaction_node);

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

    void visit (ast_bind_port_statement_t& node)
    {
      bind (node, node.left (), node.right ());
    }

    void visit (ast_bind_port_param_statement_t& node)
    {
      typed_value_t reaction_tv = bind (node, node.left (), node.right ());
      ast_t* param_node = node.param ();
      type_check_expr (param_node);
      typed_value_t param_tv = ast_get_typed_value (param_node);
      assert (reaction_tv.has_value);
      reaction_t* reaction = reaction_tv.reaction_value;
      if (!reaction->has_dimension ())
        {
          error_at_line (-1, 0, node.file, node.line,
                         "parameter specified for non-parameterized reaction");
        }
      check_index (new array_type_t (reaction->dimension (), reaction->reaction_type ()), param_tv, *param_node);
    }

    void visit (ast_bind_pfunc_statement_t& node)
    {
      ast_t* pfunc_node = node.left ();
      ast_t* func_node = node.right ();
      type_check_expr (pfunc_node);
      type_check_expr (func_node);
      typed_value_t pfunc_tv = ast_get_typed_value (pfunc_node);
      typed_value_t func_tv = ast_get_typed_value (func_node);

      const pfunc_type_t* pfunc_type = type_cast<pfunc_type_t> (pfunc_tv.type);

      if (pfunc_type == NULL)
        {
          error_at_line (-1, 0, node.file, node.line,
                         "target of bind is not a pfunc");
        }

      const method_type_t* method_type = type_cast<method_type_t> (func_tv.type);

      if (method_type != NULL)
        {
          if (type_dereference (method_type->receiver_type) == NULL)
            {
              error_at_line (-1, 0, node.file, node.line,
                             "method must take pointer receiver");
            }

          if (!type_is_equal (pfunc_type->bind_type (), method_type->bind_type ()))
            {
              error_at_line (-1, 0, node.file, node.line,
                             "cannot bind %s to %s", pfunc_type->to_string ().c_str (), method_type->to_string ().c_str ());
            }
          return;
        }

      const function_type_t* function_type = type_cast<function_type_t> (func_tv.type);

      if (function_type != NULL)
        {
          unimplemented;
          return;
        }

      error_at_line (-1, 0, node.file, node.line,
                     "source of bind is not a method or function");

      // if (!type_is_equal (port_type->signature (), reaction_type->signature ()))
      //   {
      //     error_at_line (-1, 0, node.file, node.line,
      //                    "cannot bind %s to %s", port_type->to_string ().c_str (), reaction_type->to_string ().c_str ());
      //   }
    }

    void visit (ast_for_iota_statement_t& node)
    {
      const std::string& identifier = ast_get_identifier (node.identifier ());
      size_t limit = process_array_dimension (node.limit_node ());
      symbol_t* symbol = symbol_make_variable (identifier, new iota_type_t (limit), node.identifier ());
      enter_symbol (node.symtab, symbol, node.symbol);
      type_check_statement (node.body ());
      node.limit = limit;
    }

    static typed_value_t
    check_assignment_target (ast_t* left)
    {
      typed_value_t tv = type_check_expr (left);
      assert (tv.kind == typed_value_t::REFERENCE);
      if (tv.intrinsic_mutability != MUTABLE)
        {
          error_at_line (-1, 0, left->file, left->line,
                         "cannot assign to read-only location of type %s", tv.type->to_string ().c_str ());
        }

      return tv;
    }

    static void arithmetic_assign (ast_binary_t* node, const char* symbol)
    {
      ast_t* left = node->left ();
      check_assignment_target (left);
      ast_t* right = node->right ();
      type_check_expr (right);
      typed_value_t left_tv = ast_get_typed_value (left);
      typed_value_t right_tv = ast_get_typed_value (right);
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

    void visit (ast_assign_statement_t& node)
    {
      typed_value_t left_tv = check_assignment_target (node.left ());
      typed_value_t right_tv = type_check_expr (node.right ());
      check_assignment (left_tv, right_tv, node,
                        "incompatible types (%s) = (%s)",
                        "assignment leaks mutable pointers",
                        "assignment may store foreign pointer");
    }

    void visit (ast_change_statement_t& node)
    {
      ast_t* expr = node.expr ();
      const std::string& identifier = ast_get_identifier (node.identifier ());
      ast_t* type = node.type ();
      ast_t* body = node.body ();

      // Process the expression.
      type_check_expr (expr);
      typed_value_t tv = ast_get_typed_value (expr);

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
      node.symtab->change ();

      // Check the body.
      type_check_statement (body);
    }

    void visit (ast_expression_statement_t& node)
    {
      ast_t* child = node.child ();
      type_check_expr (child);
    }

    void visit (ast_if_statement_t& node)
    {
      check_condition (node.condition ());
      type_check_statement (node.true_branch ());
      type_check_statement (node.false_branch ());
    }

    void visit (ast_while_statement_t& node)
    {
      ast_t* condition = node.condition ();
      ast_t* body = node.body ();
      check_condition (condition);
      type_check_statement (body);
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
      for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          type_check_statement (*pos);
        }
    }

    void visit (ast_return_statement_t& node)
    {
      // Check the expression.
      typed_value_t expr_tv = type_check_expr (node.child ());

      // Check that it matches with the return type.
      node.return_symbol = get_current_return_symbol (&node);
      assert (node.return_symbol != NULL);

      check_assignment (symbol_parameter_value (node.return_symbol), expr_tv, node,
                        "cannot convert to (%s) from (%s) in return",
                        "return leaks mutable pointers",
                        "return may store foreign pointer");
    }

    void visit (ast_increment_statement_t& node)
    {
      ast_t* expr = node.child ();
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
      static_cast<ast_expr_t*> (expr)->get_type ().type->accept (v);
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
      node.symtab->trigger (node.this_symbol, &node);

      /* Check the body. */
      type_check_statement (body_node);
    }

    void visit (ast_var_statement_t& node)
    {
      ast_t* identifier_list = node.identifier_list ();
      ast_t* type_spec = node.type_spec ();

      // Process the type spec.
      const type_t* type = process_type_spec (type_spec, true);

      // Enter each symbol.
      for (ast_t::const_iterator pos = identifier_list->begin (),
             limit = identifier_list->end ();
           pos != limit;
           ++pos)
        {
          const std::string& name = ast_get_identifier (*pos);
          symbol_t* symbol = symbol_make_variable (name, type, *pos);
          node.symbols.push_back (symbol_holder ());
          enter_symbol (node.symtab, symbol, node.symbols.back ());
        }
    }

    void visit (ast_println_statement_t& node)
    {
      check_rvalue_list (node.child ());
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
      node.true_branch ()->accept (*this);
      node.false_branch ()->accept (*this);
    }

    void visit (ast_while_statement_t& node)
    {
      node.body ()->accept (*this);
    }

    void visit (ast_list_statement_t& node)
    {
      for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          (*pos)->accept (*this);
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

    void visit (ast_implicit_dereference_expr_t& node)
    {
      node.visit_children (*this);
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
        case SymbolHidden:
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

      if (node.kind == ast_call_expr_t::METHOD)
        {
          check_for_pointer_copy (node.expr ()->children[0]->children[0]);
        }

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

    void visit (ast_implicit_dereference_expr_t& node)
    {
      node.visit_children (*this);
    }

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

    void visit (ast_logic_not_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_binary_arithmetic_expr_t& node)
    {
      node.visit_children (*this);
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
      node.child ()->accept (*this);
    }

    void visit (ast_if_statement_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_while_statement_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_list_statement_t& node)
    {
      for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          (*pos)->accept (*this);
          if (mutates_receiver)
            {
              break;
            }
        }
    }

    void visit (ast_println_statement_t& node)
    {
      node.child ()->accept (*this);
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
      node.true_branch ()->accept (*this);
      node.false_branch ()->accept (*this);
    }

    void visit (ast_while_statement_t& node)
    {
      node.body ()->accept (*this);
    }

    void visit (ast_list_statement_t& node)
    {
      node.visit_children (*this);
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

// TODO: Replace node with its symbol table.
void
enter_signature (ast_t * node, const signature_type_t * type)
{
  for (signature_type_t::ParametersType::const_iterator pos = type->begin (), limit = type->end ();
       pos != limit; ++pos)
    {
      const parameter_t* parameter = *pos;
      // Check if the symbol is defined locally.
      const std::string& identifier = parameter->name;
      symbol_t *s = node->symtab->find_current (identifier);
      if (s == NULL)
        {
          if (parameter->is_receiver)
            {
              s = symbol_make_receiver (parameter);
            }
          else
            {
              s = symbol_make_parameter (parameter);
            }
          node->symtab->enter (s);
        }
      else
        {
          error_at_line (-1, 0, parameter->defining_node->file, parameter->defining_node->line,
        		 "%s is already defined in this scope",
        		 identifier.c_str ());
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
      ast_t* precondition_node = node.precondition ();
      ast_t *body_node = node.body ();

      /* Insert "this" into the symbol table. */
      // TODO:  Unify with others as entering signature and return value.
      ast_t *this_identifier_node = node.this_identifier ();
      const std::string& this_identifier = ast_get_identifier (this_identifier_node);
      typed_value_t this_value = typed_value_t::make_value (pointer_type_t::make (get_current_receiver_type
                                                                                  (&node)),
                                                            typed_value_t::STACK,
                                                            MUTABLE,
                                                            IMMUTABLE);
      parameter_t* this_parameter = new parameter_t (this_identifier_node,
                                                     this_identifier,
                                                     this_value,
                                                     true);

      enter_symbol (node.symtab,
                    symbol_make_receiver (this_parameter),
                    node.this_symbol);

      check_condition (precondition_node);
      type_check_statement (body_node);
      control_check_statement (body_node);
      mutates_check_statement (body_node);

      typed_value_t tv = ast_get_typed_value (precondition_node);
      if (tv.has_value)
        {
          if (tv.bool_value)
            {
              node.action->precondition_kind = action_t::STATIC_TRUE;
            }
          else
            {
              node.action->precondition_kind = action_t::STATIC_FALSE;
            }
        }
    }

    void visit (ast_dimensioned_action_t& node)
    {
      ast_t *dimension_node = node.dimension ();
      ast_t* precondition_node = node.precondition ();
      ast_t *body_node = node.body ();

      /* Insert "this" into the symbol table. */
      ast_t *this_identifier_node = node.this_identifier ();
      const std::string& this_identifier = ast_get_identifier (this_identifier_node);
      typed_value_t this_value = typed_value_t::make_value (pointer_type_t::make (get_current_receiver_type
                                                                                  (&node)),
                                                            typed_value_t::STACK,
                                                            MUTABLE,
                                                            IMMUTABLE);
      parameter_t* this_parameter = new parameter_t (this_identifier_node,
                                                     this_identifier,
                                                     this_value,
                                                     true);

      enter_symbol (node.symtab,
                    symbol_make_receiver (this_parameter),
                    node.this_symbol);

      /* Insert "iota" into the symbol table. */
      typed_value_t iota_value = typed_value_t::make_value (new iota_type_t (node.action->dimension ()),
                                                            typed_value_t::STACK,
                                                            IMMUTABLE,
                                                            IMMUTABLE);
      parameter_t* iota_parameter = new parameter_t (dimension_node,
                                                     "IOTA",
                                                     iota_value,
                                                     false);

      enter_symbol (node.symtab,
                    symbol_make_parameter (iota_parameter),
                    node.iota_symbol);

      check_condition (precondition_node);
      type_check_statement (body_node);
      control_check_statement (body_node);
      mutates_check_statement (body_node);
    }

    void visit (ast_bind_t& node)
    {
      ast_t *body_node = node.body ();
      /* Insert "this" into the symbol table. */
      ast_t *this_node = node.this_identifier ();
      const std::string& this_identifier = ast_get_identifier (this_node);
      typed_value_t this_value = typed_value_t::make_value (pointer_type_t::make
                                                            (get_current_receiver_type
                                                             (&node)),
                                                            typed_value_t::STACK,
                                                            MUTABLE,
                                                            MUTABLE);
      parameter_t* this_parameter = new parameter_t (this_node,
                                                     this_identifier,
                                                     this_value,
                                                     true);

      enter_symbol (node.symtab,
                    symbol_make_receiver (this_parameter),
                    node.this_symbol);

      /* Check the body. */
      type_check_statement (body_node);
      control_check_statement (body_node);
    }

    void visit (ast_function_t& node)
    {
      ast_t *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
    }

    void visit (ast_method_t& node)
    {
      ast_t *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
    }

    void visit (ast_instance_t& node)
    {
      // Check the initialization function.
      symbol_t* symbol = node.symbol.symbol ();
      const named_type_t* type = symbol_get_instance_type (symbol);
      ast_t* initializer = node.initializer ();
      method_t* method = type->get_method (ast_get_identifier (initializer));
      if (method == NULL)
        {
          error_at_line (-1, 0, initializer->file,
                         initializer->line,
                         "no method named %s",
                         ast_get_identifier (initializer).c_str ());
        }
      if (method->method_type->signature ()->arity () != 0)
        {
          error_at_line (-1, 0, initializer->file,
                         initializer->line,
                         "named method is not null-ary");
        }
      symbol_set_instance_method (symbol, method);
    }

    void visit (ast_reaction_t& node)
    {
      ast_t *signature_node = node.signature ();
      ast_t *body_node = node.body ();
      reaction_t *reaction = node.reaction;
      /* Insert "this" into the symbol table. */
      ast_t *this_identifier_node = node.this_identifier ();
      const std::string& this_identifier = ast_get_identifier (this_identifier_node);
      typed_value_t this_value = typed_value_t::make_value (pointer_type_t::make (reaction->component_type ()),
                                                            typed_value_t::STACK,
                                                            MUTABLE,
                                                            IMMUTABLE);
      parameter_t* this_parameter = new parameter_t (this_identifier_node,
                                                     this_identifier,
                                                     this_value,
                                                     true);
      enter_symbol (node.symtab,
                    symbol_make_receiver (this_parameter),
                    node.this_symbol);
      /* Enter the signature into the symbol table. */
      enter_signature (signature_node, reaction->reaction_type ()->signature ());

      type_check_statement (body_node);
      control_check_statement (body_node);
      mutates_check_statement (body_node);
    }

    void visit (ast_dimensioned_reaction_t& node)
    {
      ast_t *dimension_node = node.dimension ();
      ast_t *signature_node = node.signature ();
      ast_t *body_node = node.body ();
      reaction_t *reaction = dynamic_cast<reaction_t*> (get_current_action (&node));

      /* Insert "this" into the symbol table. */
      ast_t *this_identifier_node = node.this_identifier ();
      const std::string& this_identifier = ast_get_identifier (this_identifier_node);
      typed_value_t this_value = typed_value_t::make_value (pointer_type_t::make (reaction->component_type ()),
                                                            typed_value_t::STACK,
                                                            MUTABLE,
                                                            IMMUTABLE);
      parameter_t* this_parameter = new parameter_t (this_identifier_node,
                                                     this_identifier,
                                                     this_value,
                                                     true);
      enter_symbol (node.symtab,
                    symbol_make_receiver (this_parameter),
                    node.this_symbol);

      /* Insert "iota" into the symbol table. */
      typed_value_t iota_value = typed_value_t::make_value (new iota_type_t (node.reaction->dimension ()),
                                                            typed_value_t::STACK,
                                                            IMMUTABLE,
                                                            IMMUTABLE);
      parameter_t* iota_parameter = new parameter_t (dimension_node,
                                                     "IOTA",
                                                     iota_value,
                                                     false);

      enter_symbol (node.symtab,
                    symbol_make_parameter (iota_parameter),
                    node.iota_symbol);

      /* Enter the signature into the symbol table. */
      enter_signature (signature_node, reaction->reaction_type ()->signature ());

      type_check_statement (body_node);
      control_check_statement (body_node);
      mutates_check_statement (body_node);
    }

    void visit (ast_top_level_list_t& node)
    {
      node.visit_children (*this);
    }
  };

  visitor v;
  node->accept (v);
}
