#include "ast.hpp"
#include <stdlib.h>
#include <string.h>
#include "debug.hpp"
#include "util.hpp"
#include "type.hpp"
#include <vector>
#include "Callable.hpp"

std::ostream&
operator<< (std::ostream& out, const ast_t& node)
{
  struct visitor : public ast_const_visitor_t
  {
    std::ostream& out;
    size_t indent;

    visitor (std::ostream& o) : out (o), indent (0) { }

    void default_action (const ast_t& node)
    {
      print_indent (node);
      node.print (out);
      out << ' ' << node.typed_value << '\n';
      print_children (node);
    }

    void print_indent (const ast_t& node)
    {
      for (size_t idx = 0; idx != indent; ++idx)
        {
          out << ' ';
        }
      out << node.location.Line << ' ';
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

  visitor v (out);
  node.accept (v);
  return out;
}

void
ast_set_symtab (ast_t * node, symtab_t * symtab)
{
  assert (node->symtab == NULL);
  node->symtab = symtab;
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
ACCEPT (ast_push_port_type_spec_t)
ACCEPT (ast_pull_port_type_spec_t)
ACCEPT (ast_signature_type_spec_t)
ACCEPT (ast_struct_type_spec_t)

ACCEPT (ast_cast_expr_t)
ACCEPT (ast_binary_arithmetic_expr_t)
ACCEPT (ast_address_of_expr_t)
ACCEPT (ast_call_expr_t)
ACCEPT (ast_dereference_expr_t)
ACCEPT (ast_implicit_dereference_expr_t)
ACCEPT (ast_identifier_expr_t)
ACCEPT (ast_index_expr_t)
ACCEPT (ast_slice_expr_t)
ACCEPT (ast_indexed_port_call_expr_t)
ACCEPT (ast_list_expr_t)
ACCEPT (ast_literal_expr_t)
ACCEPT (ast_logic_not_expr_t)
ACCEPT (ast_merge_expr_t)
ACCEPT (ast_move_expr_t)
ACCEPT (ast_new_expr_t)
ACCEPT (ast_push_port_call_expr_t)
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
ACCEPT (ast_var_type_init_statement_t)
ACCEPT (ast_bind_push_port_statement_t)
ACCEPT (ast_bind_push_port_param_statement_t)
ACCEPT (ast_bind_pull_port_statement_t)
ACCEPT (ast_for_iota_statement_t)

ACCEPT (ast_action_t)
ACCEPT (ast_const_t)
ACCEPT (ast_dimensioned_action_t)
ACCEPT (ast_bind_t)
ACCEPT (ast_function_t)
ACCEPT (ast_getter_t)
ACCEPT (ast_initializer_t)
ACCEPT (ast_instance_t)
ACCEPT (ast_method_t)
ACCEPT (ast_reaction_t)
ACCEPT (ast_dimensioned_reaction_t)
ACCEPT (ast_type_definition_t)
ACCEPT (ast_top_level_list_t)

std::string ast_get_identifier (const ast_t* ast)
{
  struct visitor : public ast_const_visitor_t
  {
    std::string retval;

    void default_action (const ast_t* node)
    {
      not_reached;
    }

    void visit (const ast_identifier_t& ast)
    {
      retval = ast.identifier;
    }
  };
  visitor v;
  ast->accept (v);
  return v.retval;
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

Method*
get_current_method (const ast_t * node)
{
  return symtab_get_current_method (node->symtab);
}

Getter*
get_current_getter (const ast_t * node)
{
  return symtab_get_current_getter (node->symtab);
}

Initializer*
get_current_initializer (const ast_t * node)
{
  return symtab_get_current_initializer (node->symtab);
}

Function*
get_current_function (const ast_t * node)
{
  return symtab_get_current_function (node->symtab);
}

const Symbol*
get_current_return_symbol (const ast_t * node)
{
  {
    Getter* g = get_current_getter (node);
    if (g != NULL)
      {
        return g->returnSymbol;
      }
  }
  {
    Method* g = get_current_method (node);
    if (g != NULL)
      {
        return g->returnSymbol;
      }
  }
  {
    Function* f = get_current_function (node);
    if (f != NULL)
      {
        return f->returnSymbol ();
      }
  }

  return NULL;
}
