#include "ast.hpp"

void
construct_symbol_table (ast_t * node)
{
  struct visitor : public ast_visitor_t
  {
    symtab_t* symtab;

    visitor () : symtab (symtab_make (NULL))
    { }

    void default_action (ast_t& node)
    {
      ast_set_symtab (&node, symtab);
      AST_FOREACH (child, &node)
        {
          symtab_t* s = symtab;
          child->accept (*this);
          symtab = s;
        }
    }

    void visit (ast_if_statement_t& node)
    {
      make (node);
    }

    void visit (ast_while_statement_t& node)
    {
      make (node);
    }

    void visit (ast_for_iota_statement_t& node)
    {
      make (node);
    }

    void visit (ast_change_statement_t& node)
    {
      make (node);
    }

    void visit (ast_trigger_statement_t& node)
    {
      make (node);
    }

    void visit (ast_list_statement_t& node)
    {
      make (node);
    }

    void visit (ast_action_t& node)
    {
      make (node);
    }

    void visit (ast_dimensioned_action_t& node)
    {
      make (node);
    }

    void visit (ast_bind_t& node)
    {
      make (node);
    }

    void visit (ast_function_t& node)
    {
      make (node);
    }

    void visit (ast_method_t& node)
    {
      make (node);
    }

    void visit (ast_reaction_t& node)
    {
      make (node);
    }

    void visit (ast_dimensioned_reaction_t& node)
    {
      make (node);
    }

    void visit (ast_top_level_list_t& node)
    {
      make (node);
    }

    void make (ast_t& node)
    {
      /* Create a new symbol table. */
      symtab = symtab_make (symtab);
      default_action (node);
    }
  };

  visitor v;
  node->accept (v);
}
