#include "ast.hpp"
#include <stdlib.h>
#include <string.h>
#include "strtab.hpp"
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
      print_indent ();
      std::cout << "identifier " << get (node.identifier ()) << '\n';
      print_children (node);
    }

    void visit (const ast_address_of_expr_t& node)
    {
      print_expr (node, "address_of_expr");
    }

    void visit (const ast_add_expr_t& node)
    {
      print_expr (node, "add_expr");
    }

    void visit (const ast_logic_not_expr_t& node)
    {
      print_expr (node, "logic_not_expr");
    }

    void visit (const ast_logic_and_expr_t& node)
    {
      print_expr (node, "logic_and_expr");
    }

    void visit (const ast_new_expr_t& node)
    {
      print_expr (node, "new_expr");
    }

    void visit (const ast_equal_expr_t& node)
    {
      print_expr (node, "equal_expr");
    }

    void visit (const ast_not_equal_expr_t& node)
    {
      print_expr (node, "not_equal_expr");
    }

    void visit (const ast_dereference_expr_t& node)
    {
      print_expr (node, "dereference_expr");
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

    void visit (const ast_pointer_to_foreign_type_spec_t& node)
    {
      print (node, "pointer_to_foreign_type_spec");
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

    void visit (const ast_bind_statement_list_t& node)
    {
      print (node, "bind_statement_list");
    }

    void visit (const ast_bind_statement_t& node)
    {
      print (node, "bind_statement");
    }

    void visit (const ast_bind_param_statement_t& node)
    {
      print (node, "bind_param_statement");
    }

    void visit (const ast_receiver_definition_t& node)
    {
      print (node, "receiver_definition");
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

    void print_indent ()
    {
      for (size_t idx = 0; idx != indent; ++idx)
        {
          std::cout << ' ';
        }
    }

    void print (const ast_t& node, const char* s)
    {
      print_indent ();
      std::cout << s << '\n';
      print_children (node);
    }

    void print_expr (const ast_expr_t& node, const char* s)
    {
      print_indent ();
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

ast_t *
ast_make_identifier (unsigned int line, char *id)
{
  return new ast_identifier_t (line, 0, id);
}

ast_t *
ast_make_identifier_list (unsigned int line)
{
  return new ast_identifier_list_t (line, 0);
}

ast_t *
ast_make_identifier_list_type_spec (unsigned int line,
                                    ast_t * identifier_list,
				    ast_t * type_spec)
{
  ast_t *node = new ast_identifier_list_type_spec_t (line, 2);
  node->set (IDENTIFIER_LIST, identifier_list);
  node->set (TYPE_SPEC, type_spec);
  return node;
}

ast_t *
ast_make_logic_and (unsigned int line, ast_t * left, ast_t * right)
{
  ast_t *retval = new ast_logic_and_expr_t (line, 0);
  retval->children.push_back (left);
  retval->children.push_back (right);
  return retval;
}

ast_t *
ast_make_logic_or (unsigned int line, ast_t * left, ast_t * right)
{
  ast_t *retval = new ast_logic_or_expr_t (line, 0);
  retval->children.push_back (left);
  retval->children.push_back (right);
  return retval;
}

ast_t *
ast_make_expr_stmt (unsigned int line, ast_t * expr)
{
  ast_t *retval = new ast_expression_statement_t (line, 1);
  retval->set (UNARY_CHILD, expr);
  return retval;
}

ast_t *
ast_make_var_stmt (unsigned int line, ast_t * identifier_list, ast_t * type_spec)
{
  ast_t *retval = new ast_var_statement_t (line, 2);
  retval->set (VAR_IDENTIFIER_LIST, identifier_list);
  retval->set (VAR_TYPE_SPEC, type_spec);
  return retval;
}

ast_t *
ast_make_stmt_list (unsigned int line)
{
  return new ast_list_statement_t (line, 0);
}

ast_t *ast_make_println_stmt (unsigned int line, ast_t * expr)
{
  ast_t *retval = new ast_println_statement_t (line, 1);
  retval->set (UNARY_CHILD, expr);
  return retval;
}

ast_t *
ast_make_bind_list_stmt (unsigned int line)
{
  return new ast_bind_statement_list_t (line);
}

ast_t *
ast_make_identifier_type_spec (unsigned int line, ast_t * identifier)
{
  ast_t *retval = new ast_identifier_type_spec_t (line, 1);
  retval->set (UNARY_CHILD, identifier);
  return retval;
}

ast_t *
ast_make_field_list (unsigned int line)
{
  return new ast_field_list_type_spec_t (line, 0);
}

ast_t *
ast_make_top_level_list (void)
{
  return new ast_top_level_list_t (1, 0);
}

ast_t *
ast_make_type_def (unsigned int line, ast_t * identifier, ast_t * type_spec)
{
  ast_t *retval = new ast_type_definition_t (line, 2);
  retval->set (TYPE_IDENTIFIER, identifier);
  retval->set (TYPE_TYPE_SPEC, type_spec);
  return retval;
}

ast_t *ast_make_method_def (unsigned int line, ast_t * receiver, ast_t * identifier,
                            ast_t * signature, ast_t * return_type,
                            ast_t* body)
{
  ast_t *retval = new ast_method_t (line, 5);
  retval->set (METHOD_RECEIVER, receiver);
  retval->set (METHOD_IDENTIFIER, identifier);
  retval->set (METHOD_SIGNATURE, signature);
  retval->set (METHOD_RETURN_TYPE, return_type);
  retval->set (METHOD_BODY, body);
  return retval;
}

ast_t *ast_make_function_def (unsigned int line, ast_t * identifier, ast_t * signature, ast_t* return_type, ast_t* body)
{
  ast_t *retval = new ast_function_t (line, 4);
  retval->set (FUNCTION_IDENTIFIER, identifier);
  retval->set (FUNCTION_SIGNATURE, signature);
  retval->set (FUNCTION_RETURN_TYPE, return_type);
  retval->set (FUNCTION_BODY, body);
  return retval;
}

void
ast_set_symtab (ast_t * node, symtab_t * symtab)
{
  assert (node->symtab == NULL);
  node->symtab = symtab;
}

ast_t *
ast_make_receiver (unsigned int line, ast_t * this_identifier, ast_t * type_identifier, ast_receiver_definition_t::Kind kind)
{
  ast_t *retval = new ast_receiver_definition_t (line, 2, kind);
  retval->set (RECEIVER_THIS_IDENTIFIER, this_identifier);
  retval->set (RECEIVER_TYPE_IDENTIFIER, type_identifier);
  return retval;
}

ast_t *
ast_make_instance_def (unsigned int line, ast_t * instance_id, ast_t * type_id, ast_t* initializer)
{
  ast_t *retval = new ast_instance_t (line, 3);
  retval->set (INSTANCE_IDENTIFIER, instance_id);
  retval->set (INSTANCE_TYPE_IDENTIFIER, type_id);
  retval->set (INSTANCE_INITIALIZER, initializer);
  return retval;
}

ast_t *
ast_make_return_stmt (unsigned int line, ast_t * expr)
{
  ast_t *retval = new ast_return_statement_t (line, 1);
  retval->set (UNARY_CHILD, expr);
  return retval;
}

ast_t *
ast_make_port (unsigned int line, ast_t * signature)
{
  ast_t *retval = new ast_port_type_spec_t (line, 1);
  retval->set (PORT_SIGNATURE, signature);
  return retval;
}

ast_t *
ast_make_component_type_spec (unsigned int line, ast_t * field_list)
{
  ast_t *retval = new ast_component_type_spec_t (line, 1);
  retval->set (COMPONENT_FIELD_LIST, field_list);
  return retval;
}

ast_t *
ast_make_struct_type_spec (unsigned int line, ast_t * field_list)
{
  ast_t *retval = new ast_struct_type_spec_t (line, 1);
  retval->set (STRUCT_FIELD_LIST, field_list);
  return retval;
}

ast_t *ast_make_empty_type_spec (unsigned int line)
{
  return new ast_empty_type_spec_t (line, 0);
}

ast_t *ast_make_heap_type_spec (unsigned int line, ast_t * type)
{
  ast_t * retval = new ast_heap_type_spec_t (line, 1);
  retval->set (HEAP_BASE_TYPE, type);
  return retval;
}

ast_t *ast_make_array_type_spec (unsigned int line, ast_t * dimension, ast_t * type_spec)
{
  ast_t * retval = new ast_array_type_spec_t (line, 2);
  retval->set (ARRAY_DIMENSION, dimension);
  retval->set (ARRAY_BASE_TYPE, type_spec);
  return retval;
}

ast_t *
ast_make_expression_list (unsigned int line)
{
  return new ast_list_expr_t (line, 0);
}

ast_t *
ast_make_typed_literal (unsigned int line, typed_value_t value)
{
  ast_literal_expr_t *node = new ast_literal_expr_t (line, 0);
  node->set_type (value);
  return node;
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

ast_t *
ast_make_signature (unsigned int line)
{
  return new ast_signature_type_spec_t (line, 0);
}

ast_t *ast_make_pointer_type_spec (unsigned int line, ast_t* type_spec)
{
  ast_t *retval = new ast_pointer_type_spec_t (line, 1);
  retval->set (POINTER_BASE_TYPE, type_spec);
  return retval;
}

ast_t *ast_make_pointer_to_immutable_type_spec (unsigned int line, ast_t* type_spec)
{
  ast_t *retval = new ast_pointer_to_immutable_type_spec_t (line, 1);
  retval->set (POINTER_BASE_TYPE, type_spec);
  return retval;
}

ast_t *ast_make_pointer_to_foreign_type_spec (unsigned int line, ast_t* type_spec)
{
  ast_t *retval = new ast_pointer_to_foreign_type_spec_t (line, 1);
  retval->set (POINTER_BASE_TYPE, type_spec);
  return retval;
}

ast_t *ast_make_foreign_type_spec (unsigned int line, ast_t* type_spec)
{
  ast_t *retval = new ast_foreign_type_spec_t (line, 1);
  retval->set (FOREIGN_BASE_TYPE, type_spec);
  return retval;
}

ast_t *ast_make_if_stmt (unsigned int line, ast_t* condition, ast_t* true_branch)
{
  ast_t *retval = new ast_if_statement_t (line, 2);
  retval->set (IF_CONDITION, condition);
  retval->set (IF_TRUE_BRANCH, true_branch);
  return retval;
}

ast_t *ast_make_new_expr (unsigned int line, ast_t* identifier)
{
  ast_t *retval = new ast_new_expr_t (line, 1);
  retval->set (UNARY_CHILD, identifier);
  return retval;
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
ACCEPT (ast_field_list_type_spec_t)
ACCEPT (ast_foreign_type_spec_t)
ACCEPT (ast_heap_type_spec_t)
ACCEPT (ast_identifier_list_type_spec_t)
ACCEPT (ast_identifier_type_spec_t)
ACCEPT (ast_pointer_to_foreign_type_spec_t)
ACCEPT (ast_pointer_to_immutable_type_spec_t)
ACCEPT (ast_pointer_type_spec_t)
ACCEPT (ast_port_type_spec_t)
ACCEPT (ast_signature_type_spec_t)
ACCEPT (ast_struct_type_spec_t)
ACCEPT (ast_add_expr_t)
ACCEPT (ast_address_of_expr_t)
ACCEPT (ast_call_expr_t)
ACCEPT (ast_dereference_expr_t)
ACCEPT (ast_equal_expr_t)
ACCEPT (ast_identifier_expr_t)
ACCEPT (ast_index_expr_t)
ACCEPT (ast_indexed_port_call_expr_t)
ACCEPT (ast_list_expr_t)
ACCEPT (ast_literal_expr_t)
ACCEPT (ast_logic_and_expr_t)
ACCEPT (ast_logic_not_expr_t)
ACCEPT (ast_logic_or_expr_t)
ACCEPT (ast_merge_expr_t)
ACCEPT (ast_move_expr_t)
ACCEPT (ast_new_expr_t)
ACCEPT (ast_not_equal_expr_t)
ACCEPT (ast_port_call_expr_t)
ACCEPT (ast_select_expr_t)
ACCEPT (ast_add_assign_statement_t)
ACCEPT (ast_change_statement_t)
ACCEPT (ast_assign_statement_t)
ACCEPT (ast_expression_statement_t)
ACCEPT (ast_if_statement_t)
ACCEPT (ast_println_statement_t)
ACCEPT (ast_list_statement_t)
ACCEPT (ast_return_statement_t)
ACCEPT (ast_increment_statement_t)
ACCEPT (ast_decrement_statement_t)
ACCEPT (ast_subtract_assign_statement_t)
ACCEPT (ast_trigger_statement_t)
ACCEPT (ast_var_statement_t)

ACCEPT (ast_bind_statement_t)
ACCEPT (ast_bind_param_statement_t)
ACCEPT (ast_bind_statement_list_t)
ACCEPT (ast_action_t)
ACCEPT (ast_dimensioned_action_t)
ACCEPT (ast_bind_t)
ACCEPT (ast_function_t)
ACCEPT (ast_instance_t)
ACCEPT (ast_method_t)
ACCEPT (ast_receiver_definition_t)
ACCEPT (ast_reaction_t)
ACCEPT (ast_dimensioned_reaction_t)
ACCEPT (ast_type_definition_t)
ACCEPT (ast_top_level_list_t)

string_t ast_get_identifier (const ast_t* ast)
{
  const ast_identifier_t* id = dynamic_cast<const ast_identifier_t*> (ast);
  assert (id != NULL);
  return id->identifier ();
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

const type_t *
get_current_return_type (const ast_t * node)
{
  {
    method_t* g = get_current_method (node);
    if (g != NULL)
      {
        return method_return_type (g);
      }
  }
  {
    function_t* f = get_current_function (node);
    if (f != NULL)
      {
        return function_return_type (f);
      }
  }

  return NULL;
}
