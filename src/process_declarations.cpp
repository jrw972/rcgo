#include "ast.hpp"
#include "semantic.hpp"
#include <error.h>
#include "symbol.hpp"
#include "function.hpp"
#include "action.hpp"
#include "bind.hpp"
#include "method.hpp"
#include "parameter.hpp"

void
process_declarations (ast_t * node)
{
  struct visitor : public ast_visitor_t
  {

    void visit (ast_action_t& node)
    {
      ast_t *type_identifier_node = node.type_identifier ();
      const std::string& type_identifier = ast_get_identifier (type_identifier_node);
      symbol_t *symbol = lookup_force (type_identifier_node, type_identifier);
      if (symbol_kind (symbol) != SymbolType)
        {
          error_at_line (-1, 0, type_identifier_node->file, (type_identifier_node)->line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = symbol_get_type_type (symbol);
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_identifier_node->file, (type_identifier_node)->line,
                         "%s does not refer to a component",
                         type_identifier.c_str ());
        }
      action_t *action = new action_t (type, &node);
      type->add_action (action);
      node.action = action;
      symtab_set_current_action (node.symtab, action);
      symtab_set_current_receiver_type (node.symtab, type);
    }

    void visit (ast_dimensioned_action_t& node)
    {
      size_t dimension = process_array_dimension (node.dimension ());
      ast_t *type_identifier_node = node.type_identifier ();
      const std::string& type_identifier = ast_get_identifier (type_identifier_node);
      symbol_t *symbol = lookup_force (type_identifier_node, type_identifier);
      if (symbol_kind (symbol) != SymbolType)
        {
          error_at_line (-1, 0, type_identifier_node->file, type_identifier_node->line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = symbol_get_type_type (symbol);
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_identifier_node->file, type_identifier_node->line,
                         "%s does not refer to a component",
                         type_identifier.c_str ());
        }
      action_t *action = new action_t (type, &node, dimension);
      type->add_action (action);
      node.action = action;
      symtab_set_current_action (node.symtab, action);
      symtab_set_current_receiver_type (node.symtab, type);
    }

    void visit (ast_bind_t& node)
    {
      ast_t *type_node = node.type_identifier ();
      const std::string& type_identifier = ast_get_identifier (type_node);
      symbol_t *symbol = lookup_force (type_node, type_identifier);
      if (symbol_kind (symbol) != SymbolType)
        {
          error_at_line (-1, 0, type_node->file, type_node->line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = symbol_get_type_type (symbol);
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_node->file, type_node->line,
                         "%s does not refer to a component",
                         type_identifier.c_str ());
        }
      bind_t* bind = new bind_t (&node);
      type->add_bind (bind);
      node.bind = bind;
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
      typed_value_t return_value = typed_value_t::make_value (return_type,
                                                              typed_value_t::STACK,
                                                              MUTABLE,
                                                              MUTABLE);

      parameter_t* return_parameter = new parameter_t (return_type_node,
                                                       "0return",
                                                       return_value,
                                                       false);

      symbol_t* return_symbol = symbol_make_return_parameter (return_parameter);

      function_t* function = new function_t (&node, symbol_identifier (symbol), new function_type_t (signature, return_parameter), return_symbol);

      // Enter the return first as it is deeper on the stack.
      enter_symbol (node.symtab, return_symbol, node.return_symbol);
      enter_signature (signature_node, signature);

      node.function = function;
      symbol_set_function_function (symbol, function);
      symtab_set_current_function (node.symtab, function);
    }

    void visit (ast_method_t& node)
    {
      ast_t* this_node = node.this_identifier ();
      ast_t *type_node = node.type_identifier ();
      ast_t *signature_node = node.signature ();
      ast_t *return_type_node = node.return_type ();
      ast_t *identifier_node = node.identifier ();
      const std::string& identifier = ast_get_identifier (identifier_node);
      const std::string& type_identifier = ast_get_identifier (type_node);
      symbol_t *symbol = lookup_force (type_node, type_identifier);
      if (symbol_kind (symbol) != SymbolType)
        {
          error_at_line (-1, 0, type_node->file, type_node->line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = symbol_get_type_type (symbol);
      const type_t *t = type_select (type, identifier);
      if (t != NULL)
        {
          error_at_line (-1, 0, identifier_node->file,
                         identifier_node->line,
                         "component already contains a member named %s",
                         identifier.c_str ());
        }

      /* Determine the type of this. */
      const type_t* this_type = pointer_type_t::make (type);

      if (type_cast<component_type_t> (type_strip (type)) != NULL &&
          type_dereference (this_type) == NULL)
        {
          // Components must have pointer receivers.
          error_at_line (-1, 0, node.file,
                         node.line,
                         "component methods must have pointer receiver");
        }


      /* Process the signature. */
      const signature_type_t *signature = type_cast<signature_type_t> (process_type_spec (signature_node, true));


      /* Process the return type. */
      const type_t *return_type = process_type_spec (return_type_node, true);
      typed_value_t return_value = typed_value_t::make_value (return_type,
                                                              typed_value_t::STACK,
                                                              IMMUTABLE,
                                                              node.return_dereference_mutability);

      parameter_t* return_parameter = new parameter_t (return_type_node,
                                                       "0return",
                                                       return_value,
                                                       false);

      symbol_t* return_symbol = symbol_make_return_parameter (return_parameter);

      method_type_t* method_type = new method_type_t (type,
                                                      ast_get_identifier (this_node),
                                                      this_type,
                                                      node.dereference_mutability,
                                                      signature,
                                                      return_parameter);

      method_t* method = new method_t (&node, identifier, method_type, return_symbol);

      // Enter the return first as it is deeper on the stack.
      enter_symbol (node.symtab, return_symbol, node.return_symbol);
      enter_signature (signature_node, method->method_type->function_type->signature ());

      type->add_method (method);
      node.method = method;
      symtab_set_current_method (node.symtab, method);
      symtab_set_current_receiver_type (node.symtab, type);
    }

    void visit (ast_instance_t& node)
    {
      ast_t *type_identifier_node =
        node.at (INSTANCE_TYPE_IDENTIFIER);
      const std::string& type_identifier = ast_get_identifier (type_identifier_node);
      symbol_t *symbol =
        lookup_force (type_identifier_node,
                      ast_get_identifier (type_identifier_node));
      if (symbol_kind (symbol) != SymbolType)
        {
          error_at_line (-1, 0, type_identifier_node->file,
                         type_identifier_node->line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = symbol_get_type_type (symbol);
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_identifier_node->file,
                         type_identifier_node->line,
                         "%s does not refer to a component",
                         type_identifier.c_str ());
        }
      symbol = node.symbol.symbol ();
      symbol_set_instance_type (symbol, type);
    }

    void visit (ast_reaction_t& node)
    {
      ast_t *type_node = node.type_identifier ();
      ast_t *signature_node = node.signature ();
      ast_t *identifier_node = node.identifier ();
      const std::string& identifier = ast_get_identifier (identifier_node);
      const std::string& type_identifier = ast_get_identifier (type_node);
      symbol_t *symbol = lookup_force (type_node, type_identifier);
      if (symbol_kind (symbol) != SymbolType)
        {
          error_at_line (-1, 0, type_node->file, type_node->line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = symbol_get_type_type (symbol);
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_node->file, type_node->line,
                         "%s does not refer to a component",
                         type_identifier.c_str ());
        }
      const type_t *t = type_select (type, identifier);
      if (t != NULL)
        {
          error_at_line (-1, 0, identifier_node->file,
                         identifier_node->line,
                         "component already contains a member named %s",
                         identifier.c_str ());
        }

      /* Process the signature. */
      const signature_type_t *signature = type_cast<signature_type_t> (process_type_spec (signature_node, true));
      check_port_reaction_signature (signature);

      reaction_t* reaction = new reaction_t (type, &node, identifier, signature);
      type->add_reaction (reaction);
      node.reaction = reaction;
      symtab_set_current_action (node.symtab, reaction);
      symtab_set_current_receiver_type (node.symtab, type);
    }

    void visit (ast_dimensioned_reaction_t& node)
    {
      size_t dimension = process_array_dimension (node.dimension ());
      ast_t *type_node = node.type_identifier ();
      ast_t *signature_node = node.signature ();
      ast_t *identifier_node = node.identifier ();
      const std::string& identifier = ast_get_identifier (identifier_node);
      const std::string& type_identifier = ast_get_identifier (type_node);
      symbol_t *symbol = lookup_force (type_node, type_identifier);
      if (symbol_kind (symbol) != SymbolType)
        {
          error_at_line (-1, 0, type_node->file, type_node->line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = symbol_get_type_type (symbol);
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_node->file, type_node->line,
                         "%s does not refer to a component",
                         type_identifier.c_str ());
        }
      const type_t *t = type_select (type, identifier);
      if (t != NULL)
        {
          error_at_line (-1, 0, identifier_node->file,
                         identifier_node->line,
                         "component already contains a member named %s",
                         identifier.c_str ());
        }

      /* Process the signature. */
      const signature_type_t *signature = dynamic_cast<const signature_type_t*> (process_type_spec (signature_node, true));

      reaction_t* reaction = new reaction_t (type, &node, identifier, signature, dimension);
      type->add_reaction (reaction);
      node.reaction = reaction;
      symtab_set_current_action (node.symtab, reaction);
      symtab_set_current_receiver_type (node.symtab, type);
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
      const std::string& identifier = symbol_identifier (symbol);
      named_type_t *type = symbol_get_type_type (symbol);

      if (symbol_defined (symbol))
        return;

      if (symbol_get_in_progress (symbol))
        {
          error_at_line (-1, 0, node.file, node.line,
                         "%s is defined recursively", identifier.c_str ());
        }

      symbol_set_in_progress (symbol, true);
      ast_t *type_spec_node = node.at (TYPE_TYPE_SPEC);
      const type_t *new_type = process_type_spec (type_spec_node, true, false, type);
      type->subtype (new_type);
      symbol_set_in_progress (symbol, false);
    }
  };

  visitor v;
  node->accept (v);
}

// Look up a symbol.  If it is not defined, process its definition.
symbol_t *
lookup_force (ast_t * node, const std::string& identifier)
{
  symbol_t *symbol = node->symtab->find (identifier);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, node->file, node->line,
                     "%s was not declared in this scope", identifier.c_str ());
    }
  if (symbol_get_in_progress (symbol))
    {
      error_at_line (-1, 0, node->file, node->line,
                     "%s is defined recursively", identifier.c_str ());
    }
  if (!symbol_defined (symbol))
    {
      // Process the definition.
      unimplemented;
    }

  return symbol;
}

symbol_t *
lookup_no_force (ast_t * node, const std::string& identifier)
{
  symbol_t *symbol = node->symtab->find (identifier);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, node->file, node->line,
                     "%s was not declared in this scope", identifier.c_str ());
    }
  return symbol;
}
