#include "Ast.hpp"
#include <cstdlib>
#include <cstring>
#include "debug.hpp"
#include "util.hpp"
#include "Type.hpp"
#include <vector>
#include "Callable.hpp"
#include "SymbolVisitor.hpp"
#include "action.hpp"
#include "AstVisitor.hpp"

namespace Ast
{

  std::ostream&
  operator<< (std::ostream& out, const Node& node)
  {
    struct visitor : public ConstVisitor
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

      void visit (const ast_identifier_t& node)
      {
        print_indent (node);
        out << "identifier " << node.identifier << '\n';
        print_children (node);
      }
      void visit (const ast_identifier_list_t& node)
      {
        unimplemented;
      }
      void visit (const ast_receiver_t& node)
      {
        unimplemented;
      }
      void visit (const ast_array_type_spec_t& node)
      {
        unimplemented;
      }
      void visit (const ast_slice_type_spec_t& node)
      {
        unimplemented;
      }
      void visit (const ast_empty_type_spec_t& node)
      {
        unimplemented;
      }
      void visit (const ast_enum_type_spec_t& node)
      {
        unimplemented;
      }
      void visit (const ast_field_list_type_spec_t& node)
      {
        unimplemented;
      }
      void visit (const ast_heap_type_spec_t& node)
      {
        unimplemented;
      }
      void visit (const ast_identifier_list_type_spec_t& node)
      {
        unimplemented;
      }
      void visit (const ast_identifier_type_spec_t& node)
      {
        unimplemented;
      }
      void visit (const ast_pointer_type_spec_t& node)
      {
        unimplemented;
      }
      void visit (const ast_push_port_type_spec_t& node)
      {
        unimplemented;
      }
      void visit (const ast_pull_port_type_spec_t& node)
      {
        unimplemented;
      }
      void visit (const ast_signature_type_spec_t& node)
      {
        unimplemented;
      }
      void visit (const ast_cast_expr_t& node)
      {
        unimplemented;
      }
      void visit (const ast_implicit_conversion_expr_t& node)
      {
        unimplemented;
      }
      void visit (const TypeExpression& node)
      {
        print_indent (node);
        out << "TypeExpression " << node.typed_value << '\n';
        print_children (node);
      }
      void visit (const ast_binary_arithmetic_expr_t& node)
      {
        unimplemented;
      }
      void visit (const ast_address_of_expr_t& node)
      {
        unimplemented;
      }
      void visit (const ast_call_expr_t& node)
      {
        unimplemented;
      }
      void visit (const ast_dereference_expr_t& node)
      {
        unimplemented;
      }
      void visit (const ast_implicit_dereference_expr_t& node)
      {
        unimplemented;
      }
      void visit (const ast_identifier_expr_t& node)
      {
        unimplemented;
      }
      void visit (const ast_index_expr_t& node)
      {
        unimplemented;
      }
      void visit (const ast_slice_expr_t& node)
      {
        unimplemented;
      }
      void visit (const ast_indexed_port_call_expr_t& node)
      {
        unimplemented;
      }
      void visit (const ast_list_expr_t& node)
      {
        print_indent (node);
        out << "list_expr\n";
        print_children (node);
      }
      void visit (const ast_literal_expr_t& node)
      {
        unimplemented;
      }
      void visit (const ast_unary_arithmetic_expr_t& node)
      {
        unimplemented;
      }
      void visit (const ast_push_port_call_expr_t& node)
      {
        print_indent (node);
        out << "push_port_call_expr " << node.typed_value << '\n';
        print_children (node);
      }
      void visit (const ast_select_expr_t& node)
      {
        unimplemented;
      }
      void visit (const ast_empty_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_add_assign_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_change_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_assign_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_expression_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_if_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_while_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_list_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_return_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_increment_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_decrement_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_subtract_assign_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_activate_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_var_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_bind_push_port_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_bind_push_port_param_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_bind_pull_port_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_for_iota_statement_t& node)
      {
        unimplemented;
      }
      void visit (const ast_action_t& node)
      {
        unimplemented;
      }
      void visit (const ast_const_t& node)
      {
        unimplemented;
      }
      void visit (const ast_dimensioned_action_t& node)
      {
        unimplemented;
      }
      void visit (const ast_bind_t& node)
      {
        unimplemented;
      }
      void visit (const ast_function_t& node)
      {
        unimplemented;
      }
      void visit (const ast_getter_t& node)
      {
        unimplemented;
      }
      void visit (const ast_initializer_t& node)
      {
        unimplemented;
      }
      void visit (const ast_instance_t& node)
      {
        unimplemented;
      }
      void visit (const ast_method_t& node)
      {
        unimplemented;
      }
      void visit (const ast_reaction_t& node)
      {
        unimplemented;
      }
      void visit (const ast_dimensioned_reaction_t& node)
      {
        unimplemented;
      }
      void visit (const Type& node)
      {
        unimplemented;
      }
      void visit (const SourceFile& node)
      {
        unimplemented;
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
  }                                                     \
  void                                                  \
  type::Accept (ConstVisitor& visitor) const     \
  {                                                     \
    visitor.visit (*this);                              \
  }

  ACCEPT (ast_identifier_t)
  ACCEPT (ast_identifier_list_t)
  ACCEPT (ast_receiver_t)
  ACCEPT (ast_array_type_spec_t)
  ACCEPT (ast_slice_type_spec_t)
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

  ACCEPT (ast_cast_expr_t)
  ACCEPT (ast_implicit_conversion_expr_t)
  ACCEPT (TypeExpression)
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

  std::string ast_get_identifier (const Node* ast)
  {
    struct visitor : public DefaultConstVisitor
    {
      std::string retval;

      void default_action (const Node* node)
      {
        not_reached;
      }

      void visit (const ast_identifier_t& ast)
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
              ParameterSymbol* symbol = SymbolCast<ParameterSymbol> (*ptr);
              if (symbol != NULL)
                {
                  if (symbol->kind == ParameterSymbol::Receiver)
                    {
                      EnterSymbol (symbol->duplicate (MUTABLE));
                    }
                  else
                    {
                      typed_value_t tv = symbol->value;
                      if (type_contains_pointer (tv.type) && tv.dereference_mutability == FOREIGN)
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

  const ::Type::Type*
  ast_action_t::GetReceiverType () const
  {
    return action->type ();
  }

  const ::Type::Type*
  ast_dimensioned_action_t::GetReceiverType () const
  {
    return action->type ();
  }

  const ::Type::Type*
  ast_reaction_t::GetReceiverType () const
  {
    return reaction->type ();
  }

  const ::Type::Type*
  ast_dimensioned_reaction_t::GetReceiverType () const
  {
    unimplemented;
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

  void
  Node::VisitChildren (ConstVisitor& visitor) const
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

  bool
  Node::GetInMutableSection () const
  {
    if (inMutableSection)
      {
        return true;
      }

    if (m_parent == NULL)
      {
        return false;
      }
    return m_parent->GetInMutableSection ();
  }

  Node::Context
  Node::GetContext () const
  {
    if (o_context != Parent || m_parent == NULL)
      {
        return o_context;
      }
    return m_parent->GetContext ();
  }

  Node::Node (unsigned int line_, size_t children_count)
    : inMutableSection (false)
    , location (line_)
    , operation (NULL)
    , m_parent (NULL)
    , o_context (Parent)
  {
    assert (location.Line != 0);
    m_children.resize (children_count);
  }

  const ::Type::Type*
  Node::GetReceiverType () const
  {
    if (m_parent == NULL)
      {
        return NULL;
      }
    return m_parent->GetReceiverType ();
  }
}
