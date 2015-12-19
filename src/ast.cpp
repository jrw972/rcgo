#include "ast.hpp"

#include <cstdlib>
#include <cstring>
#include <vector>

#include "debug.hpp"
#include "util.hpp"
#include "type.hpp"
#include "callable.hpp"
#include "symbol_visitor.hpp"
#include "action.hpp"
#include "reaction.hpp"
#include "ast_visitor.hpp"

namespace ast
{
using namespace decl;
using namespace semantic;

std::ostream&
operator<< (std::ostream& out, Node& node)
{
  struct visitor : public Visitor
  {
    std::ostream& out;
    size_t indent;

    visitor (std::ostream& o) : out (o), indent (0) { }

    void print_indent (const Node& node)
    {
      for (size_t idx = 0; idx != indent; ++idx)
        {
          out << ' ';
        }
      out << node.location.Line << ' ';
    }

    void print_children (const Node& node)
    {
      size_t old_indent = indent;
      indent += 2;
      for (Node::ConstIterator pos = node.Begin (), limit = node.End ();
           pos != limit;
           ++pos)
        {
          (*pos)->Accept (*this);
        }
      indent = old_indent;
    }

    // void default_action (const Node& node)
    // {
    //   print_indent (node);
    //   node.print (out);
    //   out << ' ' << node.typed_value << '\n';
    //   print_children (node);
    // }

    void print_common (const Node& node)
    {
      if (node.type != NULL)
        {
          out << ' ' << *node.type;
          if (node.value.present)
            {
              out << ' ' << ValuePrinter (node.type, node.value);
            }
          out << ' ';
          switch (node.expression_kind)
            {
            case kValue:
              out << "value";
              break;
            case kVariable:
              out << "variable";
              break;
            case kType:
              out << "type";
              break;
            }
          out << ' ';
          switch (node.intrinsic_mutability)
            {
            case Mutable:
              break;
            case Immutable:
              out << "const";
              break;
            case Foreign:
              out << "foreign";
              break;
            }
          out << ' ';
          switch (node.dereference_mutability)
            {
            case Mutable:
              break;
            case Immutable:
              out << "+const";
              break;
            case Foreign:
              out << "+foreign";
              break;
            }
        }
      out << '\n';
    }

    void visit (ast_identifier_t& node)
    {
      print_indent (node);
      out << "identifier " << node.identifier;
      print_common (node);
      print_children (node);
    }
    void visit (ast_identifier_list_t& node)
    {
      print_indent (node);
      out << "identifier_list";
      print_common (node);
      print_children (node);
    }
    void visit (ast_receiver_t& node)
    {
      print_indent (node);
      out << "receiver";
      print_common (node);
      print_children (node);
    }
    void visit (ast_array_type_spec_t& node)
    {
      print_indent (node);
      out << "array_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (ast_slice_type_spec_t& node)
    {
      print_indent (node);
      out << "slice_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (ast_map_type_spec_t& node)
    {
      print_indent (node);
      out << "map_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (ast_empty_type_spec_t& node)
    {
      print_indent (node);
      out << "empty_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (ast_field_list_type_spec_t& node)
    {
      print_indent (node);
      out << "field_list_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (ast_heap_type_spec_t& node)
    {
      print_indent (node);
      out << "heap_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (ast_identifier_list_type_spec_t& node)
    {
      print_indent (node);
      out << "identifier_list_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (ast_identifier_type_spec_t& node)
    {
      print_indent (node);
      out << "identifier_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (ast_pointer_type_spec_t& node)
    {
      print_indent (node);
      out << "pointer_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (ast_push_port_type_spec_t& node)
    {
      print_indent (node);
      out << "push_port_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (ast_pull_port_type_spec_t& node)
    {
      print_indent (node);
      out << "pull_port_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (ast_signature_type_spec_t& node)
    {
      print_indent (node);
      out << "signature_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (ast_implicit_conversion_expr_t& node)
    {
      print_indent (node);
      out << "implicit_conversion_expr";
      print_common (node);
      print_children (node);
    }
    void visit (TypeExpression& node)
    {
      print_indent (node);
      out << "TypeExpression";
      print_common (node);
      print_children (node);
    }
    void visit (ast_binary_arithmetic_expr_t& node)
    {
      print_indent (node);
      out << "binary_arithmetic_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ast_address_of_expr_t& node)
    {
      print_indent (node);
      out << "address_of_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ast_call_expr_t& node)
    {
      print_indent (node);
      out << "call_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ast_conversion_expr_t& node)
    {
      print_indent (node);
      out << "conversion_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ast_dereference_expr_t& node)
    {
      print_indent (node);
      out << "dereference_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ast_implicit_dereference_expr_t& node)
    {
      print_indent (node);
      out << "implicit_dereference_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ast_identifier_expr_t& node)
    {
      print_indent (node);
      out << "identifier_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ast_index_expr_t& node)
    {
      print_indent (node);
      out << "index_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ast_slice_expr_t& node)
    {
      print_indent (node);
      out << "slice_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ast_auto_expr_t& node)
    {
      print_indent (node);
      out << "auto_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ast_indexed_port_call_expr_t& node)
    {
      print_indent (node);
      out << "indexed_port_call_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ast_list_expr_t& node)
    {
      print_indent (node);
      out << "list_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ast_literal_expr_t& node)
    {
      print_indent (node);
      out << "literal_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ast_unary_arithmetic_expr_t& node)
    {
      print_indent (node);
      out << "unary_arithmetic_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ast_push_port_call_expr_t& node)
    {
      print_indent (node);
      out << "push_port_call_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ast_select_expr_t& node)
    {
      print_indent (node);
      out << "select_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ast_empty_statement_t& node)
    {
      print_indent (node);
      out << "empty_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_add_assign_statement_t& node)
    {
      print_indent (node);
      out << "add_assign_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_change_statement_t& node)
    {
      print_indent (node);
      out << "change_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_assign_statement_t& node)
    {
      print_indent (node);
      out << "assign_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_expression_statement_t& node)
    {
      print_indent (node);
      out << "expression_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_if_statement_t& node)
    {
      print_indent (node);
      out << "if_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_while_statement_t& node)
    {
      print_indent (node);
      out << "while_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_list_statement_t& node)
    {
      print_indent (node);
      out << "list_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_return_statement_t& node)
    {
      print_indent (node);
      out << "return_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_increment_statement_t& node)
    {
      print_indent (node);
      out << "increment_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_decrement_statement_t& node)
    {
      print_indent (node);
      out << "decrement_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_subtract_assign_statement_t& node)
    {
      print_indent (node);
      out << "subtract_assign_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_activate_statement_t& node)
    {
      print_indent (node);
      out << "activate_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_var_statement_t& node)
    {
      print_indent (node);
      out << "var_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_bind_push_port_statement_t& node)
    {
      print_indent (node);
      out << "bind_push_port_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_bind_push_port_param_statement_t& node)
    {
      print_indent (node);
      out << "bind_push_port_param_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_bind_pull_port_statement_t& node)
    {
      print_indent (node);
      out << "bind_pull_port_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_for_iota_statement_t& node)
    {
      print_indent (node);
      out << "for_iota_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ast_action_t& node)
    {
      print_indent (node);
      out << "action";
      print_common (node);
      print_children (node);
    }
    void visit (ast_const_t& node)
    {
      print_indent (node);
      out << "const";
      print_common (node);
      print_children (node);
    }
    void visit (ast_dimensioned_action_t& node)
    {
      print_indent (node);
      out << "dimensioned_action";
      print_common (node);
      print_children (node);
    }
    void visit (ast_bind_t& node)
    {
      print_indent (node);
      out << "bind";
      print_common (node);
      print_children (node);
    }
    void visit (ast_function_t& node)
    {
      print_indent (node);
      out << "function";
      print_common (node);
      print_children (node);
    }
    void visit (ast_getter_t& node)
    {
      print_indent (node);
      out << "getter";
      print_common (node);
      print_children (node);
    }
    void visit (ast_initializer_t& node)
    {
      print_indent (node);
      out << "initializer";
      print_common (node);
      print_children (node);
    }
    void visit (ast_instance_t& node)
    {
      print_indent (node);
      out << "instance";
      print_common (node);
      print_children (node);
    }
    void visit (ast_method_t& node)
    {
      print_indent (node);
      out << "method";
      print_common (node);
      print_children (node);
    }
    void visit (ast_reaction_t& node)
    {
      print_indent (node);
      out << "reaction";
      print_common (node);
      print_children (node);
    }
    void visit (ast_dimensioned_reaction_t& node)
    {
      print_indent (node);
      out << "dimensioned_reaction";
      print_common (node);
      print_children (node);
    }
    void visit (Type& node)
    {
      print_indent (node);
      out << "Type";
      print_common (node);
      print_children (node);
    }
    void visit (SourceFile& node)
    {
      print_indent (node);
      out << "SourceFile";
      print_common (node);
      print_children (node);
    }
    void visit (ast_element_list_t& node)
    {
      print_indent (node);
      out << "element_list";
      print_common (node);
      print_children (node);
    }
    void visit (ast_element_t& node)
    {
      print_indent (node);
      out << "element";
      print_common (node);
      print_children (node);
    }
    void visit (ast_composite_literal_t& node)
    {
      print_indent (node);
      out << "composite_literal";
      print_common (node);
      print_children (node);
    }
  };

  visitor v (out);
  node.Accept (v);
  return out;
}

#define ACCEPT(type) void                               \
  type::Accept (Visitor& visitor)                 \
  {                                                     \
    visitor.visit (*this);                              \
  }

ACCEPT (ast_identifier_t)
ACCEPT (ast_identifier_list_t)
ACCEPT (ast_receiver_t)
ACCEPT (ast_array_type_spec_t)
ACCEPT (ast_slice_type_spec_t)
ACCEPT (ast_map_type_spec_t)
ACCEPT (ast_empty_type_spec_t)
ACCEPT (ast_field_list_type_spec_t)
ACCEPT (ast_heap_type_spec_t)
ACCEPT (ast_identifier_list_type_spec_t)
ACCEPT (ast_identifier_type_spec_t)
ACCEPT (ast_pointer_type_spec_t)
ACCEPT (ast_push_port_type_spec_t)
ACCEPT (ast_pull_port_type_spec_t)
ACCEPT (ast_signature_type_spec_t)

ACCEPT (ast_implicit_conversion_expr_t)
ACCEPT (TypeExpression)
ACCEPT (ast_binary_arithmetic_expr_t)
ACCEPT (ast_address_of_expr_t)
ACCEPT (ast_call_expr_t)
ACCEPT (ast_conversion_expr_t)
ACCEPT (ast_dereference_expr_t)
ACCEPT (ast_implicit_dereference_expr_t)
ACCEPT (ast_identifier_expr_t)
ACCEPT (ast_index_expr_t)
ACCEPT (ast_slice_expr_t)
ACCEPT (ast_auto_expr_t)
ACCEPT (ast_indexed_port_call_expr_t)
ACCEPT (ast_list_expr_t)
ACCEPT (ast_literal_expr_t)
ACCEPT (ast_unary_arithmetic_expr_t)
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
ACCEPT (Type)
ACCEPT (SourceFile)
ACCEPT (ast_element_list_t)
ACCEPT (ast_element_t)
ACCEPT (ast_composite_literal_t)

std::string ast_get_identifier (Node* ast)
{
  struct visitor : public DefaultVisitor
  {
    std::string retval;

    void default_action (const Node* node)
    {
      NOT_REACHED;
    }

    void visit (ast_identifier_t& ast)
    {
      retval = ast.identifier;
    }
  };
  visitor v;
  ast->Accept (v);
  return v.retval;
}

void
Node::Activate ()
{
  // Remove all parameters containing pointers to avoid a leak.
  Node* s;
  for (s = m_parent; s != NULL; s = s->m_parent)
    {
      for (SymbolsType::const_iterator ptr = s->m_symbols.begin (),
           limit = s->m_symbols.end ();
           ptr != limit;
           ++ptr)
        {
          {
            ParameterSymbol* symbol = decl::SymbolCast<ParameterSymbol> (*ptr);
            if (symbol != NULL)
              {
                if (symbol->kind == ParameterSymbol::Receiver)
                  {
                    EnterSymbol (symbol->duplicate (Mutable));
                  }
                else
                  {
                    if (type_contains_pointer (symbol->type) && symbol->dereference_mutability == Foreign)
                      {
                        // Hide this parameter.
                        EnterSymbol (new HiddenSymbol (symbol, this));
                      }
                  }
              }
          }

          {
            const VariableSymbol* symbol = SymbolCast<VariableSymbol> (*ptr);
            if (symbol != NULL)
              {
                if (type_contains_pointer (symbol->type) && symbol->dereference_mutability == Foreign)
                  {
                    // Hide this variable.
                    EnterSymbol (new HiddenSymbol (symbol, this));
                  }
              }
          }

        }
    }
}

void
Node::Change ()
{
  Node* s;
  for (s = m_parent; s != NULL; s = s->m_parent)
    {
      for (SymbolsType::const_iterator ptr = s->m_symbols.begin (),
           limit = s->m_symbols.end ();
           ptr != limit;
           ++ptr)
        {
          {
            ParameterSymbol* symbol = SymbolCast<ParameterSymbol> (*ptr);
            if (symbol != NULL)
              {
                if (type_contains_pointer (symbol->type))
                  {
                    // Enter as a duplicate.
                    Symbol* dup = symbol->duplicate (Foreign);
                    EnterSymbol (dup);
                  }
              }
          }

          {
            VariableSymbol* symbol = SymbolCast<VariableSymbol> (*ptr);
            if (symbol != NULL)
              {
                if (type_contains_pointer (symbol->type))
                  {
                    // Enter as a duplicate.
                    Symbol* dup = symbol->duplicate ();
                    EnterSymbol (dup);
                  }
              }
          }
        }
    }
}

Symbol *
Node::FindGlobalSymbol (const std::string& identifier) const
{
  for (SymbolsType::const_iterator pos = m_symbols.begin (),
       limit = m_symbols.end ();
       pos != limit;
       ++pos)
    {
      Symbol *s = *pos;
      if (identifier == s->identifier)
        {
          return s;
        }
    }

  /* Not found in this scope.  Try the parent. */
  if (m_parent)
    {
      return m_parent->FindGlobalSymbol (identifier);
    }
  else
    {
      return NULL;
    }
}

Symbol *
Node::FindLocalSymbol (const std::string& identifier) const
{
  for (std::vector<Symbol*>::const_iterator pos = m_symbols.begin (),
       limit = m_symbols.end ();
       pos != limit;
       ++pos)
    {
      Symbol *s = *pos;
      if (identifier == s->identifier)
        {
          return s;
        }
    }

  return NULL;
}

void
Node::VisitChildren (Visitor& visitor)
{
  for (ConstIterator pos = Begin (), limit = End (); pos != limit; ++pos)
    {
      (*pos)->Accept (visitor);
    }
}

Node*
Node::Prepend (Node * child)
{
  child->m_parent = this;
  m_children.insert (m_children.begin (), child);
  return this;
}

Node*
Node::Append (Node * child)
{
  child->m_parent = this;
  m_children.push_back (child);
  return this;
}

void
Node::set (size_t idx, Node* child)
{
  child->m_parent = this;
  At (idx) = child;
}

Node::Node (unsigned int line_, size_t children_count)
  : location (line_)
  , type (NULL)
  , field (NULL)
  , callable (NULL)
  , temp (NULL)
  , receiver_state (true)
  , receiver_access (AccessWrite)
  , operation (NULL)
  , m_parent (NULL)
{
  assert (location.Line != 0);
  m_children.resize (children_count);
}

ast_literal_expr_t::ast_literal_expr_t (unsigned int line, const ::type::Type* t, const Value& v)
  : Node (line, 0)
{
  type = t;
  value = v;
}

}
