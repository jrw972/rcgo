#include "ast.hpp"
#include <stdlib.h>
#include <string.h>
#include "debug.hpp"
#include "util.hpp"
#include "type.hpp"
#include <vector>
#include "Callable.hpp"
#include "SymbolVisitor.hpp"

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
ACCEPT (ast_receiver_t)
ACCEPT (ast_array_type_spec_t)
ACCEPT (ast_slice_type_spec_t)
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
ACCEPT (ast_type_expr_t)
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
ACCEPT (ast_push_port_call_expr_t)
ACCEPT (ast_select_expr_t)

ACCEPT (ast_empty_statement_t)
ACCEPT (ast_add_assign_statement_t)
ACCEPT (ast_change_statement_t)
ACCEPT (ast_assign_statement_t)
ACCEPT (ast_expression_statement_t)
ACCEPT (ast_if_statement_t)
ACCEPT (ast_while_statement_t)
ACCEPT (ast_list_statement_t)
ACCEPT (ast_return_statement_t)
ACCEPT (ast_increment_statement_t)
ACCEPT (ast_decrement_statement_t)
ACCEPT (ast_subtract_assign_statement_t)
ACCEPT (ast_activate_statement_t)
ACCEPT (ast_var_statement_t)
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

Method*
get_current_method (const ast_t * node)
{
  unimplemented;
  //return symtab_get_current_method (node->symtab);
}

Function*
get_current_function (const ast_t * node)
{
  unimplemented;
  //return symtab_get_current_function (node->symtab);
}

Symbol*
ast_t::Activate ()
{
  Symbol *this_symbol = GetReceiverSymbol ();
  Symbol *new_this_symbol = SymbolCast<ParameterSymbol> (this_symbol)->duplicate (MUTABLE);
  EnterSymbol (new_this_symbol);

  // Remove all parameters containing pointers to avoid a leak.
  ast_t* s;
  for (s = parent_; s != NULL; s = s->parent_)
    {
      for (SymbolsType::const_iterator ptr = s->symbols.begin (),
           limit = s->symbols.end ();
           ptr != limit;
           ++ptr)
        {
          {
            const ParameterSymbol* symbol = SymbolCast<ParameterSymbol> (*ptr);
            if (symbol != NULL && symbol != this_symbol)
              {
                typed_value_t tv = symbol->value;
                if (type_contains_pointer (tv.type) && tv.dereference_mutability == FOREIGN)
                  {
                    // Hide this parameter.
                    EnterSymbol (new HiddenSymbol (symbol, this));
                  }
              }
          }

          {
            const VariableSymbol* symbol = SymbolCast<VariableSymbol> (*ptr);
            if (symbol != NULL)
              {
                typed_value_t tv = symbol->value;
                if (type_contains_pointer (tv.type) && tv.dereference_mutability == FOREIGN)
                  {
                    // Hide this variable.
                    EnterSymbol (new HiddenSymbol (symbol, this));
                  }
              }
          }

        }
    }

  return new_this_symbol;
}

void
ast_t::Change ()
{
  ast_t* s;
  for (s = parent_; s != NULL; s = s->parent_)
    {
      for (SymbolsType::const_iterator ptr = s->symbols.begin (),
           limit = s->symbols.end ();
           ptr != limit;
           ++ptr)
        {
          {
            ParameterSymbol* symbol = SymbolCast<ParameterSymbol> (*ptr);
            if (symbol != NULL)
              {
                typed_value_t tv = SymbolCast<ParameterSymbol> (symbol)->value;
                if (type_contains_pointer (tv.type))
                  {
                    // Enter as a duplicate.
                    Symbol* dup = SymbolCast<ParameterSymbol> (symbol)->duplicate (FOREIGN);
                    EnterSymbol (dup);
                  }
              }
          }

          {
            VariableSymbol* symbol = SymbolCast<VariableSymbol> (*ptr);
            if (symbol != NULL)
              {
                typed_value_t tv = SymbolCast<VariableSymbol> (symbol)->value;
                if (type_contains_pointer (tv.type))
                  {
                    // Enter as a duplicate.
                    Symbol* dup = SymbolCast<VariableSymbol> (symbol)->duplicate ();
                    EnterSymbol (dup);
                  }
              }
          }
        }
    }
}

Symbol *
ast_action_t::GetReceiverSymbol () const
{
  return ast_cast<ast_receiver_t> (receiver ())->this_symbol;
}

Symbol *
ast_dimensioned_action_t::GetReceiverSymbol () const
{
  return ast_cast<ast_receiver_t> (receiver ())->this_symbol;
}

Symbol *
ast_reaction_t::GetReceiverSymbol () const
{
  return ast_cast<ast_receiver_t> (receiver ())->this_symbol;
}

Symbol *
ast_dimensioned_reaction_t::GetReceiverSymbol () const
{
  return ast_cast<ast_receiver_t> (receiver ())->this_symbol;
}
