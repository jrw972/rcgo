#include "ast.hpp"
#include <stdlib.h>
#include <string.h>
#include "debug.hpp"
#include "util.hpp"
#include "type.hpp"
#include <vector>
#include "method.hpp"
#include "function.hpp"

void
ast_print (const ast_t& node)
{
  struct visitor : public ast_const_visitor_t
  {
    size_t indent;

    visitor () : indent (0) { }

    void default_action (const ast_t& node)
    {
      not_reached;
    }

    void visit (const ast_identifier_t& node)
    {
      print_indent (node);
      std::cout << "identifier " << node.identifier << '\n';
      print_children (node);
    }

    void visit (const ast_for_iota_statement_t& node)
    {
      print (node, "for_iota_statement");
    }

    void visit (const ast_address_of_expr_t& node)
    {
      print_expr (node, "address_of_expr");
    }

    void visit (const ast_binary_arithmetic_expr_t& node)
    {
      print_expr (node, "binary_arithmetic_expr");
    }

    void visit (const ast_logic_not_expr_t& node)
    {
      print_expr (node, "logic_not_expr");
    }

    void visit (const ast_new_expr_t& node)
    {
      print_expr (node, "new_expr");
    }

    void visit (const ast_dereference_expr_t& node)
    {
      print_expr (node, "dereference_expr");
    }

    void visit (const ast_implicit_dereference_expr_t& node)
    {
      print_expr (node, "implicit_dereference_expr");
    }

    void visit (const ast_call_expr_t& node)
    {
      print_expr (node, "call_expr");
    }

    void visit (const ast_identifier_expr_t& node)
    {
      print_expr (node, "identifier_expr");
    }

    void visit (const ast_index_expr_t& node)
    {
      print_expr (node, "index_expr");
    }

    void visit (const ast_indexed_port_call_expr_t& node)
    {
      print_expr (node, "indexed_port_call_expr");
    }

    void visit (const ast_list_expr_t& node)
    {
      print_expr (node, "list_expr");
    }

    void visit (const ast_literal_expr_t& node)
    {
      print_expr (node, "literal_expr");
    }

    void visit (const ast_select_expr_t& node)
    {
      print_expr (node, "select_expr");
    }

    void visit (const ast_port_call_expr_t& node)
    {
      print_expr (node, "port_call_expr");
    }

    void visit (const ast_top_level_list_t& node)
    {
      print (node, "top_level_list");
    }

    void visit (const ast_type_definition_t& node)
    {
      print (node, "type_definition");
    }

    void visit (const ast_struct_type_spec_t& node)
    {
      print (node, "struct_type_spec");
    }

    void visit (const ast_array_type_spec_t& node)
    {
      print (node, "array_type_spec");
    }

    void visit (const ast_port_type_spec_t& node)
    {
      print (node, "port_type_spec");
    }

    void visit (const ast_component_type_spec_t& node)
    {
      print (node, "component_type_spec");
    }

    void visit (const ast_empty_type_spec_t& node)
    {
      print (node, "empty_type_spec");
    }

    void visit (const ast_signature_type_spec_t& node)
    {
      print (node, "signature_type_spec");
    }

    void visit (const ast_field_list_type_spec_t& node)
    {
      print (node, "field_list_type_spec");
    }

    void visit (const ast_identifier_list_type_spec_t& node)
    {
      print (node, "identifier_list_type_spec");
    }

    void visit (const ast_identifier_type_spec_t& node)
    {
      print (node, "identifier_type_spec");
    }

    void visit (const ast_pointer_type_spec_t& node)
    {
      print (node, "pointer_type_spec");
    }

    void visit (const ast_identifier_list_t& node)
    {
      print (node, "identifier_list");
    }

    void visit (const ast_method_t& node)
    {
      print (node, "method");
    }

    void visit (const ast_reaction_t& node)
    {
      print (node, "reaction");
    }

    void visit (const ast_dimensioned_reaction_t& node)
    {
      print (node, "dimensioned_reaction");
    }

    void visit (const ast_action_t& node)
    {
      print (node, "action");
    }

    void visit (const ast_dimensioned_action_t& node)
    {
      print (node, "dimensioned_action");
    }

    void visit (const ast_instance_t& node)
    {
      print (node, "instance");
    }

    void visit (const ast_bind_t& node)
    {
      print (node, "bind");
    }

    void visit (const ast_bind_port_statement_t& node)
    {
      print (node, "bind_port_statement");
    }

    void visit (const ast_bind_port_param_statement_t& node)
    {
      print (node, "bind_param_statement");
    }

    void visit (const ast_bind_pfunc_statement_t& node)
    {
      print (node, "bind_pfunc_statement");
    }

    void visit (const ast_trigger_statement_t& node)
    {
      print (node, "trigger_statement");
    }

    void visit (const ast_println_statement_t& node)
    {
      print (node, "println_statement");
    }

    void visit (const ast_expression_statement_t& node)
    {
      print (node, "expression_statement");
    }

    void visit (const ast_return_statement_t& node)
    {
      print (node, "return_statement");
    }

    void visit (const ast_var_statement_t& node)
    {
      print (node, "var_statement");
    }

    void visit (const ast_if_statement_t& node)
    {
      print (node, "if_statement");
    }

    void visit (const ast_list_statement_t& node)
    {
      print (node, "list_statement");
    }

    void visit (const ast_assign_statement_t& node)
    {
      print (node, "assign_statement");
    }

    void print_indent (const ast_t& node)
    {
      for (size_t idx = 0; idx != indent; ++idx)
        {
          std::cout << ' ';
        }
      std::cout << node.line << ' ';
    }

    void print (const ast_t& node, const char* s)
    {
      print_indent (node);
      std::cout << s << '\n';
      print_children (node);
    }

    void print_expr (const ast_expr_t& node, const char* s)
    {
      print_indent (node);
      std::cout << s << ' ' << node.get_type () << '\n';
      print_children (node);
    }

    void print_children (const ast_t& node)
    {
      size_t old_indent = indent;
      indent += 2;
      for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          (*pos)->accept (*this);
        }
      indent = old_indent;
    }
  };

  visitor v;
  node.accept (v);
}

void
ast_set_symtab (ast_t * node, symtab_t * symtab)
{
  assert (node->symtab == NULL);
  node->symtab = symtab;
}

typed_value_t ast_get_typed_value (const ast_t* node)
{
  const ast_expr_t* expr = dynamic_cast<const ast_expr_t*> (node);
  assert (expr != NULL);
  return expr->get_type ();
}

void
ast_set_type (ast_t * node, typed_value_t typed_value)
{
  ast_expr_t* expr = dynamic_cast<ast_expr_t*> (node);
  assert (expr != NULL);
  expr->set_type (typed_value);
}

#define ACCEPT(type) void \
type::accept (ast_visitor_t& visitor) \
{ \
  visitor.visit (*this); \
} \
void \
type::accept (ast_const_visitor_t& visitor) const \
{ \
  visitor.visit (*this); \
}

ACCEPT (ast_identifier_t)
ACCEPT (ast_identifier_list_t)
ACCEPT (ast_array_type_spec_t)
ACCEPT (ast_component_type_spec_t)
ACCEPT (ast_empty_type_spec_t)
ACCEPT (ast_enum_type_spec_t)
ACCEPT (ast_field_list_type_spec_t)
ACCEPT (ast_heap_type_spec_t)
ACCEPT (ast_identifier_list_type_spec_t)
ACCEPT (ast_identifier_type_spec_t)
ACCEPT (ast_pointer_type_spec_t)
ACCEPT (ast_port_type_spec_t)
ACCEPT (ast_pfunc_type_spec_t)
ACCEPT (ast_signature_type_spec_t)
ACCEPT (ast_struct_type_spec_t)
ACCEPT (ast_binary_arithmetic_expr_t)
ACCEPT (ast_address_of_expr_t)
ACCEPT (ast_call_expr_t)
ACCEPT (ast_dereference_expr_t)
ACCEPT (ast_implicit_dereference_expr_t)
ACCEPT (ast_identifier_expr_t)
ACCEPT (ast_index_expr_t)
ACCEPT (ast_indexed_port_call_expr_t)
ACCEPT (ast_list_expr_t)
ACCEPT (ast_literal_expr_t)
ACCEPT (ast_logic_not_expr_t)
ACCEPT (ast_merge_expr_t)
ACCEPT (ast_move_expr_t)
ACCEPT (ast_new_expr_t)
ACCEPT (ast_port_call_expr_t)
ACCEPT (ast_select_expr_t)

ACCEPT (ast_empty_statement_t)
ACCEPT (ast_add_assign_statement_t)
ACCEPT (ast_change_statement_t)
ACCEPT (ast_assign_statement_t)
ACCEPT (ast_expression_statement_t)
ACCEPT (ast_if_statement_t)
ACCEPT (ast_while_statement_t)
ACCEPT (ast_println_statement_t)
ACCEPT (ast_list_statement_t)
ACCEPT (ast_return_statement_t)
ACCEPT (ast_increment_statement_t)
ACCEPT (ast_decrement_statement_t)
ACCEPT (ast_subtract_assign_statement_t)
ACCEPT (ast_trigger_statement_t)
ACCEPT (ast_var_statement_t)
ACCEPT (ast_bind_port_statement_t)
ACCEPT (ast_bind_port_param_statement_t)
ACCEPT (ast_bind_pfunc_statement_t)
ACCEPT (ast_for_iota_statement_t)

ACCEPT (ast_action_t)
ACCEPT (ast_dimensioned_action_t)
ACCEPT (ast_bind_t)
ACCEPT (ast_function_t)
ACCEPT (ast_instance_t)
ACCEPT (ast_method_t)
ACCEPT (ast_reaction_t)
ACCEPT (ast_dimensioned_reaction_t)
ACCEPT (ast_type_definition_t)
ACCEPT (ast_top_level_list_t)

const std::string& ast_get_identifier (const ast_t* ast)
{
  const ast_identifier_t* id = dynamic_cast<const ast_identifier_t*> (ast);
  assert (id != NULL);
  return id->identifier;
}

named_type_t *
get_current_receiver_type (const ast_t * node)
{
  return symtab_get_current_receiver_type (node->symtab);
}

trigger_t *
get_current_trigger (const ast_t * node)
{
  return symtab_get_current_trigger (node->symtab);
}

action_reaction_base_t *
get_current_action (const ast_t * node)
{
  return symtab_get_current_action (node->symtab);
}

method_t *
get_current_method (const ast_t * node)
{
  return symtab_get_current_method (node->symtab);
}

function_t *
get_current_function (const ast_t * node)
{
  return symtab_get_current_function (node->symtab);
}

const symbol_t*
get_current_return_symbol (const ast_t * node)
{
  {
    method_t* g = get_current_method (node);
    if (g != NULL)
      {
        return g->return_symbol;
      }
  }
  {
    function_t* f = get_current_function (node);
    if (f != NULL)
      {
        return f->return_symbol;
      }
  }

  return NULL;
}
