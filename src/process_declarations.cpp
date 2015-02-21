#include "ast.hpp"
#include "semantic.hpp"
#include <error.h>
#include "symbol.hpp"
#include "function.hpp"
#include "action.hpp"
#include "bind.hpp"

void
process_declarations (ast_t * node)
{
  struct visitor : public ast_visitor_t
  {

    void visit (ast_action_t& node)
    {
      ast_t *receiver = node.receiver ();
      ast_t *type_node = receiver->at (RECEIVER_TYPE_IDENTIFIER);
      string_t type_identifier = ast_get_identifier (type_node);
      symbol_t *symbol = lookup_force (type_node, type_identifier);
      if (symbol_kind (symbol) != SymbolType)
        {
          error_at_line (-1, 0, type_node->file, (type_node)->line,
                         "%s does not refer to a type",
                         get (type_identifier));
        }
      named_type_t *type = symbol_get_type_type (symbol);
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_node->file, (type_node)->line,
                         "%s does not refer to a component",
                         get (type_identifier));
        }
      action_t *action = new action_t (type, &node);
      type->add_action (action);
      symtab_set_current_action (node.symtab, action);
      symtab_set_current_receiver_type (node.symtab, type);
    }

    void visit (ast_dimensioned_action_t& node)
    {
      size_t dimension = process_array_dimension (node.dimension_iter ());
      ast_t *receiver = node.receiver ();
      ast_t *type_node = receiver->at (RECEIVER_TYPE_IDENTIFIER);
      string_t type_identifier = ast_get_identifier (type_node);
      symbol_t *symbol = lookup_force (type_node, type_identifier);
      if (symbol_kind (symbol) != SymbolType)
        {
          error_at_line (-1, 0, type_node->file, (type_node)->line,
                         "%s does not refer to a type",
                         get (type_identifier));
        }
      named_type_t *type = symbol_get_type_type (symbol);
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_node->file, (type_node)->line,
                         "%s does not refer to a component",
                         get (type_identifier));
        }
      action_t *action = new action_t (type, &node, dimension);
      type->add_action (action);
      symtab_set_current_action (node.symtab, action);
      symtab_set_current_receiver_type (node.symtab, type);
      node.action = action;
    }

    void visit (ast_bind_t& node)
    {
      ast_t *receiver = node.receiver ();
      ast_t *type_node = receiver->at (RECEIVER_TYPE_IDENTIFIER);
      string_t type_identifier = ast_get_identifier (type_node);
      symbol_t *symbol = lookup_force (type_node, type_identifier);
      if (symbol_kind (symbol) != SymbolType)
        {
          error_at_line (-1, 0, type_node->file, type_node->line,
                         "%s does not refer to a type",
                         get (type_identifier));
        }
      named_type_t *type = symbol_get_type_type (symbol);
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_node->file, type_node->line,
                         "%s does not refer to a component",
                         get (type_identifier));
        }
      bind_t* bind = new bind_t (&node);
      type->add_bind (bind);
      symtab_set_current_receiver_type (node.symtab, type);
    }

    void visit (ast_function_t& node)
    {
      ast_t *signature_node = node.at (FUNCTION_SIGNATURE);
      ast_t *return_type_node = node.at (FUNCTION_RETURN_TYPE);
      symbol_t* symbol = node.function_symbol.symbol ();

      /* Process the signature. */
      const signature_type_t *signature = dynamic_cast<const signature_type_t*> (process_type_spec (signature_node, true));

      /* Process the return type. */
      const type_t *return_type = process_type_spec (return_type_node, true);

      function_t* function = function_make (&node, symbol_identifier (symbol), signature, return_type);
      symbol_set_function_function (symbol, function);
      symtab_set_current_function (node.symtab, function);

    }

    void visit (ast_method_t& node)
    {
      ast_receiver_definition_t* receiver = dynamic_cast<ast_receiver_definition_t*> (node.at (METHOD_RECEIVER));
      ast_t *type_node = receiver->at (RECEIVER_TYPE_IDENTIFIER);
      ast_t *signature_node = node.at (METHOD_SIGNATURE);
      ast_t *return_type_node = node.at (METHOD_RETURN_TYPE);
      ast_t *identifier_node = node.at (METHOD_IDENTIFIER);
      string_t identifier = ast_get_identifier (identifier_node);
      string_t type_identifier = ast_get_identifier (type_node);
      symbol_t *symbol = lookup_force (type_node, type_identifier);
      if (symbol_kind (symbol) != SymbolType)
        {
          error_at_line (-1, 0, type_node->file, type_node->line,
                         "%s does not refer to a type",
                         get (type_identifier));
        }
      named_type_t *type = symbol_get_type_type (symbol);
      const type_t *t = type_select (type, identifier);
      if (t != NULL)
        {
          error_at_line (-1, 0, identifier_node->file,
                         identifier_node->line,
                         "component already contains a member named %s",
                         get (identifier));
        }

      /* Determine the type of this. */
      const type_t* this_type;
      switch (receiver->kind ())
        {
        case ast_receiver_definition_t::AstPointerReceiver:
          this_type = pointer_type_t::make (type);
          break;
        case ast_receiver_definition_t::AstPointerToImmutableReceiver:
          this_type = pointer_to_immutable_type_t::make (type);
          break;
        }

      /* Process the signature. */
      signature_type_t *signature = const_cast<signature_type_t*> (dynamic_cast<const signature_type_t*> (process_type_spec (signature_node, true)));
      /* Prepend the signature with the receiver. */
      signature->prepend (ast_get_identifier (receiver->at (RECEIVER_THIS_IDENTIFIER)),
                          this_type, true);

      /* Process the return type. */
      const type_t *return_type = process_type_spec (return_type_node, true);

      method_t* method = type->add_method (&node, identifier, signature, return_type);
      symtab_set_current_method (node.symtab, method);
      symtab_set_current_receiver_type (node.symtab, type);
    }

    void visit (ast_instance_t& node)
    {
      ast_t *type_identifier_node =
        node.at (INSTANCE_TYPE_IDENTIFIER);
      string_t type_identifier = ast_get_identifier (type_identifier_node);
      symbol_t *symbol =
        lookup_force (type_identifier_node,
                      ast_get_identifier (type_identifier_node));
      if (symbol_kind (symbol) != SymbolType)
        {
          error_at_line (-1, 0, type_identifier_node->file,
                         type_identifier_node->line,
                         "%s does not refer to a type",
                         get (type_identifier));
        }
      named_type_t *type = symbol_get_type_type (symbol);
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_identifier_node->file,
                         type_identifier_node->line,
                         "%s does not refer to a component",
                         get (type_identifier));
        }
      symbol = node.symbol.symbol ();
      symbol_set_instance_type (symbol, type);
    }

    void visit (ast_reaction_t& node)
    {
      ast_t *receiver = node.receiver ();
      ast_t *type_node = receiver->at (RECEIVER_TYPE_IDENTIFIER);
      ast_t *signature_node = node.signature ();
      ast_t *identifier_node = node.identifier ();
      string_t identifier = ast_get_identifier (identifier_node);
      string_t type_identifier = ast_get_identifier (type_node);
      symbol_t *symbol = lookup_force (type_node, type_identifier);
      if (symbol_kind (symbol) != SymbolType)
        {
          error_at_line (-1, 0, type_node->file, type_node->line,
                         "%s does not refer to a type",
                         get (type_identifier));
        }
      named_type_t *type = symbol_get_type_type (symbol);
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_node->file, type_node->line,
                         "%s does not refer to a component",
                         get (type_identifier));
        }
      const type_t *t = type_select (type, identifier);
      if (t != NULL)
        {
          error_at_line (-1, 0, identifier_node->file,
                         identifier_node->line,
                         "component already contains a member named %s",
                         get (identifier));
        }

      /* Process the signature. */
      const signature_type_t *signature = dynamic_cast<const signature_type_t*> (process_type_spec (signature_node, true));

      reaction_t* reaction = new reaction_t (type, &node, identifier, signature);
      type->add_reaction (reaction);
      symtab_set_current_action (node.symtab, reaction);
      symtab_set_current_receiver_type (node.symtab, type);
    }

    void visit (ast_dimensioned_reaction_t& node)
    {
      size_t dimension = process_array_dimension (node.dimension_iter ());
      ast_t *receiver = node.receiver ();
      ast_t *type_node = receiver->at (RECEIVER_TYPE_IDENTIFIER);
      ast_t *signature_node = node.signature ();
      ast_t *identifier_node = node.identifier ();
      string_t identifier = ast_get_identifier (identifier_node);
      string_t type_identifier = ast_get_identifier (type_node);
      symbol_t *symbol = lookup_force (type_node, type_identifier);
      if (symbol_kind (symbol) != SymbolType)
        {
          error_at_line (-1, 0, type_node->file, type_node->line,
                         "%s does not refer to a type",
                         get (type_identifier));
        }
      named_type_t *type = symbol_get_type_type (symbol);
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_node->file, type_node->line,
                         "%s does not refer to a component",
                         get (type_identifier));
        }
      const type_t *t = type_select (type, identifier);
      if (t != NULL)
        {
          error_at_line (-1, 0, identifier_node->file,
                         identifier_node->line,
                         "component already contains a member named %s",
                         get (identifier));
        }

      /* Process the signature. */
      const signature_type_t *signature = dynamic_cast<const signature_type_t*> (process_type_spec (signature_node, true));

      reaction_t* reaction = new reaction_t (type, &node, identifier, signature, dimension);
      type->add_reaction (reaction);
      symtab_set_current_action (node.symtab, reaction);
      symtab_set_current_receiver_type (node.symtab, type);
      node.reaction = reaction;
    }

    void visit (ast_top_level_list_t& node)
    {
      AST_FOREACH (child, &node)
        {
          process_declarations (child);
        }
    }

    void visit (ast_type_definition_t& node)
    {
      symbol_t *symbol = node.symbol.symbol ();
      string_t identifier = symbol_identifier (symbol);
      named_type_t *type = symbol_get_type_type (symbol);

      if (symbol_defined (symbol))
        return;

      if (symbol_get_in_progress (symbol))
        {
          error_at_line (-1, 0, node.file, node.line,
                         "%s is defined recursively", get (identifier));
        }

      symbol_set_in_progress (symbol, true);
      ast_t *type_spec_node = node.at (TYPE_TYPE_SPEC);
      const type_t *new_type = process_type_spec (type_spec_node, true);
      type->subtype (new_type);
      symbol_set_in_progress (symbol, false);
    }
  };

  visitor v;
  node->accept (v);
}

// Look up a symbol.  If it is not defined, process its definition.
symbol_t *
lookup_force (ast_t * node, string_t identifier)
{
  symbol_t *symbol = symtab_find (node->symtab, identifier);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, node->file, node->line,
                     "%s was not declared in this scope", get (identifier));
    }
  if (symbol_get_in_progress (symbol))
    {
      error_at_line (-1, 0, node->file, node->line,
                     "%s is defined recursively", get (identifier));
    }
  if (!symbol_defined (symbol))
    {
      // Process the definition.
      unimplemented;
    }

  return symbol;
}

symbol_t *
lookup_no_force (ast_t * node, string_t identifier)
{
  symbol_t *symbol = symtab_find (node->symtab, identifier);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, node->file, node->line,
                     "%s was not declared in this scope", get (identifier));
    }
  return symbol;
}
