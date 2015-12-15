#ifndef rc_src_ast_visitor_hpp
#define rc_src_ast_visitor_hpp

#include "ast.hpp"

namespace ast
{

struct Visitor
{
  virtual void visit (ast_identifier_t& node) = 0;
  virtual void visit (ast_identifier_list_t& node) = 0;
  virtual void visit (ast_receiver_t& node) = 0;
  virtual void visit (ast_array_type_spec_t& node) = 0;
  virtual void visit (ast_slice_type_spec_t& node) = 0;
  virtual void visit (ast_empty_type_spec_t& node) = 0;
  virtual void visit (ast_field_list_type_spec_t& node) = 0;
  virtual void visit (ast_heap_type_spec_t& node) = 0;
  virtual void visit (ast_identifier_list_type_spec_t& node) = 0;
  virtual void visit (ast_identifier_type_spec_t& node) = 0;
  virtual void visit (ast_pointer_type_spec_t& node) = 0;
  virtual void visit (ast_push_port_type_spec_t& node) = 0;
  virtual void visit (ast_pull_port_type_spec_t& node) = 0;
  virtual void visit (ast_signature_type_spec_t& node) = 0;
  virtual void visit (ast_implicit_conversion_expr_t& node) = 0;
  virtual void visit (TypeExpression& node) = 0;
  virtual void visit (ast_binary_arithmetic_expr_t& node) = 0;
  virtual void visit (ast_address_of_expr_t& node) = 0;
  virtual void visit (ast_call_expr_t& node) = 0;
  virtual void visit (ast_conversion_expr_t& node) = 0;
  virtual void visit (ast_dereference_expr_t& node) = 0;
  virtual void visit (ast_implicit_dereference_expr_t& node) = 0;
  virtual void visit (ast_identifier_expr_t& node) = 0;
  virtual void visit (ast_index_expr_t& node) = 0;
  virtual void visit (ast_slice_expr_t& node) = 0;
  virtual void visit (ast_auto_expr_t& node) = 0;
  virtual void visit (ast_indexed_port_call_expr_t& node) = 0;
  virtual void visit (ast_list_expr_t& node) = 0;
  virtual void visit (ast_literal_expr_t& node) = 0;
  virtual void visit (ast_unary_arithmetic_expr_t& node) = 0;
  virtual void visit (ast_push_port_call_expr_t& node) = 0;
  virtual void visit (ast_select_expr_t& node) = 0;
  virtual void visit (ast_empty_statement_t& node) = 0;
  virtual void visit (ast_add_assign_statement_t& node) = 0;
  virtual void visit (ast_change_statement_t& node) = 0;
  virtual void visit (ast_assign_statement_t& node) = 0;
  virtual void visit (ast_expression_statement_t& node) = 0;
  virtual void visit (ast_if_statement_t& node) = 0;
  virtual void visit (ast_while_statement_t& node) = 0;
  virtual void visit (ast_list_statement_t& node) = 0;
  virtual void visit (ast_return_statement_t& node) = 0;
  virtual void visit (ast_increment_statement_t& node) = 0;
  virtual void visit (ast_decrement_statement_t& node) = 0;
  virtual void visit (ast_subtract_assign_statement_t& node) = 0;
  virtual void visit (ast_activate_statement_t& node) = 0;
  virtual void visit (ast_var_statement_t& node) = 0;
  virtual void visit (ast_bind_push_port_statement_t& node) = 0;
  virtual void visit (ast_bind_push_port_param_statement_t& node) = 0;
  virtual void visit (ast_bind_pull_port_statement_t& node) = 0;
  virtual void visit (ast_for_iota_statement_t& node) = 0;
  virtual void visit (ast_action_t& node) = 0;
  virtual void visit (ast_const_t& node) = 0;
  virtual void visit (ast_dimensioned_action_t& node) = 0;
  virtual void visit (ast_bind_t& node) = 0;
  virtual void visit (ast_function_t& node) = 0;
  virtual void visit (ast_getter_t& node) = 0;
  virtual void visit (ast_initializer_t& node) = 0;
  virtual void visit (ast_instance_t& node) = 0;
  virtual void visit (ast_method_t& node) = 0;
  virtual void visit (ast_reaction_t& node) = 0;
  virtual void visit (ast_dimensioned_reaction_t& node) = 0;
  virtual void visit (Type& node) = 0;
  virtual void visit (SourceFile& node) = 0;
  virtual void visit (ast_element_list_t& node) = 0;
  virtual void visit (ast_composite_literal_t& node) = 0;
};

struct ConstVisitor
{
  virtual void visit (const ast_identifier_t& node) = 0;
  virtual void visit (const ast_identifier_list_t& node) = 0;
  virtual void visit (const ast_receiver_t& node) = 0;
  virtual void visit (const ast_array_type_spec_t& node) = 0;
  virtual void visit (const ast_slice_type_spec_t& node) = 0;
  virtual void visit (const ast_empty_type_spec_t& node) = 0;
  virtual void visit (const ast_field_list_type_spec_t& node) = 0;
  virtual void visit (const ast_heap_type_spec_t& node) = 0;
  virtual void visit (const ast_identifier_list_type_spec_t& node) = 0;
  virtual void visit (const ast_identifier_type_spec_t& node) = 0;
  virtual void visit (const ast_pointer_type_spec_t& node) = 0;
  virtual void visit (const ast_push_port_type_spec_t& node) = 0;
  virtual void visit (const ast_pull_port_type_spec_t& node) = 0;
  virtual void visit (const ast_signature_type_spec_t& node) = 0;
  virtual void visit (const ast_implicit_conversion_expr_t& node) = 0;
  virtual void visit (const TypeExpression& node) = 0;
  virtual void visit (const ast_binary_arithmetic_expr_t& node) = 0;
  virtual void visit (const ast_address_of_expr_t& node) = 0;
  virtual void visit (const ast_call_expr_t& node) = 0;
  virtual void visit (const ast_conversion_expr_t& node) = 0;
  virtual void visit (const ast_dereference_expr_t& node) = 0;
  virtual void visit (const ast_implicit_dereference_expr_t& node) = 0;
  virtual void visit (const ast_identifier_expr_t& node) = 0;
  virtual void visit (const ast_index_expr_t& node) = 0;
  virtual void visit (const ast_slice_expr_t& node) = 0;
  virtual void visit (const ast_auto_expr_t& node) = 0;
  virtual void visit (const ast_indexed_port_call_expr_t& node) = 0;
  virtual void visit (const ast_list_expr_t& node) = 0;
  virtual void visit (const ast_literal_expr_t& node) = 0;
  virtual void visit (const ast_unary_arithmetic_expr_t& node) = 0;
  virtual void visit (const ast_push_port_call_expr_t& node) = 0;
  virtual void visit (const ast_select_expr_t& node) = 0;
  virtual void visit (const ast_empty_statement_t& node) = 0;
  virtual void visit (const ast_add_assign_statement_t& node) = 0;
  virtual void visit (const ast_change_statement_t& node) = 0;
  virtual void visit (const ast_assign_statement_t& node) = 0;
  virtual void visit (const ast_expression_statement_t& node) = 0;
  virtual void visit (const ast_if_statement_t& node) = 0;
  virtual void visit (const ast_while_statement_t& node) = 0;
  virtual void visit (const ast_list_statement_t& node) = 0;
  virtual void visit (const ast_return_statement_t& node) = 0;
  virtual void visit (const ast_increment_statement_t& node) = 0;
  virtual void visit (const ast_decrement_statement_t& node) = 0;
  virtual void visit (const ast_subtract_assign_statement_t& node) = 0;
  virtual void visit (const ast_activate_statement_t& node) = 0;
  virtual void visit (const ast_var_statement_t& node) = 0;
  virtual void visit (const ast_bind_push_port_statement_t& node) = 0;
  virtual void visit (const ast_bind_push_port_param_statement_t& node) = 0;
  virtual void visit (const ast_bind_pull_port_statement_t& node) = 0;
  virtual void visit (const ast_for_iota_statement_t& node) = 0;
  virtual void visit (const ast_action_t& node) = 0;
  virtual void visit (const ast_const_t& node) = 0;
  virtual void visit (const ast_dimensioned_action_t& node) = 0;
  virtual void visit (const ast_bind_t& node) = 0;
  virtual void visit (const ast_function_t& node) = 0;
  virtual void visit (const ast_getter_t& node) = 0;
  virtual void visit (const ast_initializer_t& node) = 0;
  virtual void visit (const ast_instance_t& node) = 0;
  virtual void visit (const ast_method_t& node) = 0;
  virtual void visit (const ast_reaction_t& node) = 0;
  virtual void visit (const ast_dimensioned_reaction_t& node) = 0;
  virtual void visit (const Type& node) = 0;
  virtual void visit (const SourceFile& node) = 0;
  virtual void visit (const ast_element_list_t& node) = 0;
  virtual void visit (const ast_composite_literal_t& node) = 0;
};

struct DefaultVisitor : public Visitor
{
  virtual void visit (ast_identifier_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_identifier_list_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_receiver_t& node)
  {
    default_action (node);
  }

  virtual void visit (ast_array_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_slice_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_empty_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_field_list_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_heap_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_identifier_list_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_identifier_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_pointer_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_push_port_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_pull_port_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_signature_type_spec_t& node)
  {
    default_action (node);
  }

  virtual void visit (ast_implicit_conversion_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (TypeExpression& node)
  {
    default_action (node);
  }
  virtual void visit (ast_binary_arithmetic_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_address_of_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_call_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_conversion_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_dereference_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_implicit_dereference_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_identifier_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_index_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_slice_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_auto_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_indexed_port_call_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_list_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_literal_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_unary_arithmetic_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_push_port_call_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_select_expr_t& node)
  {
    default_action (node);
  }

  virtual void visit (ast_empty_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_add_assign_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_change_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_assign_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_expression_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_if_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_while_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_list_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_return_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_increment_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_decrement_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_subtract_assign_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_activate_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_var_statement_t& node)
  {
    default_action (node);
  }

  virtual void visit (ast_bind_push_port_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_bind_push_port_param_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_bind_pull_port_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_for_iota_statement_t& node)
  {
    default_action (node);
  }

  virtual void visit (ast_action_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_const_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_dimensioned_action_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_bind_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_function_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_getter_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_initializer_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_instance_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_method_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_reaction_t& node)
  {
    default_action (node);
  }
  virtual void visit (ast_dimensioned_reaction_t& node)
  {
    default_action (node);
  }
  virtual void visit (Type& node)
  {
    default_action (node);
  }

  virtual void visit (SourceFile& node)
  {
    default_action (node);
  }

  virtual void visit (ast_element_list_t& node)
  {
    default_action (node);
  }

  virtual void visit (ast_composite_literal_t& node)
  {
    default_action (node);
  }

  virtual void default_action (Node& node) { }
};

struct DefaultConstVisitor : public ConstVisitor
{
  virtual void visit (const ast_identifier_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_identifier_list_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_receiver_t& node)
  {
    default_action (node);
  }

  virtual void visit (const ast_array_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_slice_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_empty_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_field_list_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_heap_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_identifier_list_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_identifier_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_pointer_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_push_port_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_pull_port_type_spec_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_signature_type_spec_t& node)
  {
    default_action (node);
  }

  virtual void visit (const ast_implicit_conversion_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (const TypeExpression& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_binary_arithmetic_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_address_of_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_call_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_conversion_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_dereference_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_implicit_dereference_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_identifier_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_index_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_slice_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_auto_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_indexed_port_call_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_list_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_literal_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_unary_arithmetic_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_push_port_call_expr_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_select_expr_t& node)
  {
    default_action (node);
  }

  virtual void visit (const ast_empty_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_add_assign_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_change_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_assign_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_expression_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_if_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_while_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_list_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_return_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_increment_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_decrement_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_subtract_assign_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_activate_statement_t& node)
  {
    default_action (node);
  }

  virtual void visit (const ast_bind_push_port_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_bind_push_port_param_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_bind_pull_port_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_for_iota_statement_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_var_statement_t& node)
  {
    default_action (node);
  }

  virtual void visit (const ast_action_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_const_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_dimensioned_action_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_bind_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_function_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_getter_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_initializer_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_instance_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_method_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_reaction_t& node)
  {
    default_action (node);
  }
  virtual void visit (const ast_dimensioned_reaction_t& node)
  {
    default_action (node);
  }
  virtual void visit (const Type& node)
  {
    default_action (node);
  }

  virtual void visit (const SourceFile& node)
  {
    default_action (node);
  }

  virtual void visit (const ast_element_list_t& node)
  {
    default_action (node);
  }

  virtual void visit (const ast_composite_literal_t& node)
  {
    default_action (node);
  }

  virtual void default_action (const Node& node) { }
};

template<typename T>
T*
ast_cast (Node* node)
{
  if (node == NULL) return NULL;

  struct visitor : public DefaultVisitor
  {
    T* retval;

    visitor () : retval (NULL) { }

    void visit (T& type)
    {
      retval = &type;
    }
  };
  visitor v;
  node->Accept (v);
  return v.retval;
}

}

#endif // rc_src_ast_visitor_hpp
