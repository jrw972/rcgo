#include "ast.hpp"
#include "semantic.hpp"
#include <error.h>
#include "Symbol.hpp"
#include "action.hpp"
#include "bind.hpp"
#include "parameter.hpp"
#include "Callable.hpp"

void
process_declarations (ast_t * node)
{
  struct visitor : public ast_visitor_t
  {
    void default_action (ast_t& node)
    {
      ast_not_reached (node);
    }

    void visit (ast_const_t& node)
    {
      typed_value_t right_tv = type_check_expr (node.expr ());

      if (!right_tv.value.present)
        {
          error_at_line (-1, 0, node.location.file, node.location.line,
                         "expression is not constant");
        }

      const type_t* type = process_type_spec (node.type_spec (), true);
      typed_value_t left_tv = typed_value_t::make_ref (type, typed_value_t::STACK, MUTABLE, IMMUTABLE);

      check_assignment (left_tv, right_tv, node,
                        "E11: incompatible types (%s) = (%s)",
                        "argument leaks mutable pointers",
                        "argument may store foreign pointer");

      enter_symbol (node.symtab,
                    new TypedConstantSymbol (ast_get_identifier (node.identifier ()), &node, right_tv),
                    node.symbol);
    }

    void visit (ast_action_t& node)
    {
      ast_t *type_identifier_node = node.type_identifier ();
      const std::string& type_identifier = ast_get_identifier (type_identifier_node);
      TypeSymbol* symbol = processAndLookup<TypeSymbol> (type_identifier_node, type_identifier);
      if (symbol == NULL)
        {
          error_at_line (-1, 0, type_identifier_node->location.file, type_identifier_node->location.line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = symbol->type;
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_identifier_node->location.file, type_identifier_node->location.line,
                         "%s does not refer to a component",
                         type_identifier.c_str ());
        }
      action_t *action = new action_t (type, &node, node.precondition (), node.body ());
      type->add_action (action);
      node.action = action;
      symtab_set_current_action (node.symtab, action);
      symtab_set_current_receiver_type (node.symtab, type);
    }

    void visit (ast_dimensioned_action_t& node)
    {
      typed_value_t dimension = process_array_dimension (node.dimension ());
      ast_t *type_identifier_node = node.type_identifier ();
      const std::string& type_identifier = ast_get_identifier (type_identifier_node);
      TypeSymbol* symbol = processAndLookup<TypeSymbol> (type_identifier_node, type_identifier);
      if (symbol == NULL)
        {
          error_at_line (-1, 0, type_identifier_node->location.file, type_identifier_node->location.line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = symbol->type;
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_identifier_node->location.file, type_identifier_node->location.line,
                         "%s does not refer to a component",
                         type_identifier.c_str ());
        }
      action_t *action = new action_t (type, &node, node.precondition (), node.body (), dimension);
      type->add_action (action);
      node.action = action;
      symtab_set_current_action (node.symtab, action);
      symtab_set_current_receiver_type (node.symtab, type);
    }

    void visit (ast_bind_t& node)
    {
      ast_t *type_node = node.type_identifier ();
      const std::string& type_identifier = ast_get_identifier (type_node);
      TypeSymbol* symbol = processAndLookup<TypeSymbol> (type_node, type_identifier);
      if (symbol == NULL)
        {
          error_at_line (-1, 0, type_node->location.file, type_node->location.line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = symbol->type;
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_node->location.file, type_node->location.line,
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
      ast_t *signature_node = node.signature ();
      ast_t *return_type_node = node.return_type ();
      Symbol* symbol = node.function_symbol.symbol ();

      /* Process the signature. */
      const signature_type_t *signature = type_cast<signature_type_t> (process_type_spec (signature_node, true));

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

      Symbol* return_symbol = ParameterSymbol::makeReturn (return_parameter);

      Function* function = new Function (&node, symbol->identifier, new function_type_t (signature, return_parameter), return_symbol);

      // Enter the return first as it is deeper on the stack.
      enter_symbol (node.symtab, return_symbol, node.return_symbol);
      enter_signature (signature_node, signature);

      node.function = function;
      SymbolCast<FunctionSymbol> (symbol)->function (function);
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
      TypeSymbol* symbol = processAndLookup<TypeSymbol> (type_node, type_identifier);
      if (symbol == NULL)
        {
          error_at_line (-1, 0, type_node->location.file, type_node->location.line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = symbol->type;
      const type_t *t = type_select (type, identifier);
      if (t != NULL)
        {
          error_at_line (-1, 0, identifier_node->location.file,
                         identifier_node->location.line,
                         "component already contains a member named %s",
                         identifier.c_str ());
        }

      /* Determine the type of this. */
      const type_t* this_type = pointer_type_t::make (type);

      if (type_cast<component_type_t> (type_strip (type)) != NULL &&
          type_dereference (this_type) == NULL)
        {
          // Components must have pointer receivers.
          error_at_line (-1, 0, node.location.file,
                         node.location.line,
                         "component methods must have pointer receiver");
        }

      /* Process the signature. */
      const signature_type_t *signature = type_cast<signature_type_t> (process_type_spec (signature_node, true));

      /* Process the return type. */
      const type_t *return_type = process_type_spec (return_type_node, true);
      typed_value_t return_value = typed_value_t::make_value (return_type,
                                                              typed_value_t::STACK,
                                                              MUTABLE,
                                                              node.return_dereference_mutability);

      parameter_t* return_parameter = new parameter_t (return_type_node,
                                                       "0return",
                                                       return_value,
                                                       false);

      Symbol* return_symbol = ParameterSymbol::makeReturn (return_parameter);

      method_type_t* method_type = new method_type_t (type,
                                                      ast_get_identifier (this_node),
                                                      this_type,
                                                      node.dereference_mutability,
                                                      signature,
                                                      return_parameter);

      Method* method = new Method (&node, identifier, method_type, return_symbol);

      // Enter the return first as it is deeper on the stack.
      enter_symbol (node.symtab, return_symbol, node.return_symbol);
      enter_signature (signature_node, method->methodType->function_type->signature ());

      type->add_method (method);
      node.method = method;
      symtab_set_current_method (node.symtab, method);
      symtab_set_current_receiver_type (node.symtab, type);
    }

    void visit (ast_initializer_t& node)
    {
      ast_t* this_node = node.this_identifier ();
      ast_t *type_node = node.type_identifier ();
      ast_t *signature_node = node.signature ();
      ast_t *identifier_node = node.identifier ();
      const std::string& identifier = ast_get_identifier (identifier_node);
      const std::string& type_identifier = ast_get_identifier (type_node);
      TypeSymbol* symbol = processAndLookup<TypeSymbol> (type_node, type_identifier);
      if (symbol == NULL)
        {
          error_at_line (-1, 0, type_node->location.file, type_node->location.line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = symbol->type;

      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, node.location.file,
                         node.location.line,
                         "E22: initializers must belong to a component");
        }

      const type_t *t = type_select (type, identifier);
      if (t != NULL)
        {
          error_at_line (-1, 0, identifier_node->location.file,
                         identifier_node->location.line,
                         "component already contains a member named %s",
                         identifier.c_str ());
        }

      /* Determine the type of this. */
      const type_t* this_type = pointer_type_t::make (type);

      /* Process the signature. */
      const signature_type_t *signature = type_cast<signature_type_t> (process_type_spec (signature_node, true));

      const type_t *return_type = void_type_t::instance ();
      typed_value_t return_value = typed_value_t::make_value (return_type,
                                                              typed_value_t::STACK,
                                                              MUTABLE,
                                                              IMMUTABLE);

      parameter_t* return_parameter = new parameter_t (&node,
                                                       "0return",
                                                       return_value,
                                                       false);

      initializer_type_t* initializer_type =
        new initializer_type_t (type,
                                ast_get_identifier (this_node),
                                this_type,
                                MUTABLE,
                                signature,
                                return_parameter);

      Initializer* initializer = new Initializer (&node, identifier, initializer_type);

      enter_signature (signature_node, initializer->initializerType->function_type->signature ());

      type->add_initializer (initializer);
      node.initializer = initializer;
      symtab_set_current_initializer (node.symtab, initializer);
      symtab_set_current_receiver_type (node.symtab, type);
    }

    void visit (ast_getter_t& node)
    {
      ast_t* this_node = node.this_identifier ();
      ast_t *type_node = node.type_identifier ();
      ast_t *signature_node = node.signature ();
      ast_t *return_type_node = node.return_type ();
      ast_t *identifier_node = node.identifier ();
      const std::string& identifier = ast_get_identifier (identifier_node);
      const std::string& type_identifier = ast_get_identifier (type_node);
      TypeSymbol *symbol = processAndLookup<TypeSymbol> (type_node, type_identifier);
      if (symbol == NULL)
        {
          error_at_line (-1, 0, type_node->location.file, type_node->location.line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = SymbolCast<TypeSymbol> (symbol)->type;

      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, node.location.file,
                         node.location.line,
                         "E22: getters must belong to a component");
        }

      const type_t *t = type_select (type, identifier);
      if (t != NULL)
        {
          error_at_line (-1, 0, identifier_node->location.file,
                         identifier_node->location.line,
                         "component already contains a member named %s",
                         identifier.c_str ());
        }

      /* Determine the type of this. */
      const type_t* this_type = pointer_type_t::make (type);

      /* Process the signature. */
      const signature_type_t *signature = type_cast<signature_type_t> (process_type_spec (signature_node, true));

      /* Process the return type. */
      const type_t *return_type = process_type_spec (return_type_node, true);
      typed_value_t return_value = typed_value_t::make_value (return_type,
                                                              typed_value_t::STACK,
                                                              MUTABLE,
                                                              IMMUTABLE);

      parameter_t* return_parameter = new parameter_t (return_type_node,
                                                       "0return",
                                                       return_value,
                                                       false);

      Symbol* return_symbol = ParameterSymbol::makeReturn (return_parameter);

      getter_type_t* getter_type = new getter_type_t (type,
                                                      ast_get_identifier (this_node),
                                                      this_type,
                                                      signature,
                                                      return_parameter);

      Getter* getter = new Getter (&node, identifier, getter_type, return_symbol);

      // Enter the return first as it is deeper on the stack.
      enter_symbol (node.symtab, return_symbol, node.return_symbol);
      enter_signature (signature_node, getter->getterType->function_type->signature ());

      type->add_getter (getter);
      node.getter = getter;
      symtab_set_current_getter (node.symtab, getter);
      symtab_set_current_receiver_type (node.symtab, type);
    }

    void visit (ast_instance_t& node)
    {
      ast_t *type_identifier_node = node.type_identifier ();
      const std::string& type_identifier = ast_get_identifier (type_identifier_node);
      TypeSymbol *symbol = processAndLookup<TypeSymbol> (type_identifier_node, ast_get_identifier (type_identifier_node));
      if (symbol == NULL)
        {
          error_at_line (-1, 0, type_identifier_node->location.file,
                         type_identifier_node->location.line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = symbol->type;
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_identifier_node->location.file,
                         type_identifier_node->location.line,
                         "%s does not refer to a component",
                         type_identifier.c_str ());
        }
      SymbolCast<InstanceSymbol> (node.symbol.symbol ())->type = type;
    }

    void visit (ast_reaction_t& node)
    {
      ast_t *type_node = node.type_identifier ();
      ast_t *signature_node = node.signature ();
      ast_t *identifier_node = node.identifier ();
      const std::string& identifier = ast_get_identifier (identifier_node);
      const std::string& type_identifier = ast_get_identifier (type_node);
      TypeSymbol* symbol = processAndLookup<TypeSymbol> (type_node, type_identifier);
      if (symbol == NULL)
        {
          error_at_line (-1, 0, type_node->location.file, type_node->location.line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = SymbolCast<TypeSymbol> (symbol)->type;
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_node->location.file, type_node->location.line,
                         "%s does not refer to a component",
                         type_identifier.c_str ());
        }
      const type_t *t = type_select (type, identifier);
      if (t != NULL)
        {
          error_at_line (-1, 0, identifier_node->location.file,
                         identifier_node->location.line,
                         "component already contains a member named %s",
                         identifier.c_str ());
        }

      /* Process the signature. */
      const signature_type_t *signature = type_cast<signature_type_t> (process_type_spec (signature_node, true));
      check_port_reaction_signature (signature);

      reaction_t* reaction = new reaction_t (type, &node, node.body (), identifier, signature);
      type->add_reaction (reaction);
      node.reaction = reaction;
      symtab_set_current_action (node.symtab, reaction);
      symtab_set_current_receiver_type (node.symtab, type);
    }

    void visit (ast_dimensioned_reaction_t& node)
    {
      typed_value_t dimension = process_array_dimension (node.dimension ());
      ast_t *type_node = node.type_identifier ();
      ast_t *signature_node = node.signature ();
      ast_t *identifier_node = node.identifier ();
      const std::string& identifier = ast_get_identifier (identifier_node);
      const std::string& type_identifier = ast_get_identifier (type_node);
      TypeSymbol* symbol = processAndLookup<TypeSymbol> (type_node, type_identifier);
      if (symbol == NULL)
        {
          error_at_line (-1, 0, type_node->location.file, type_node->location.line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = SymbolCast<TypeSymbol> (symbol)->type;
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_node->location.file, type_node->location.line,
                         "%s does not refer to a component",
                         type_identifier.c_str ());
        }
      const type_t *t = type_select (type, identifier);
      if (t != NULL)
        {
          error_at_line (-1, 0, identifier_node->location.file,
                         identifier_node->location.line,
                         "component already contains a member named %s",
                         identifier.c_str ());
        }

      /* Process the signature. */
      const signature_type_t *signature = type_cast<signature_type_t> (process_type_spec (signature_node, true));

      reaction_t* reaction = new reaction_t (type, &node, node.body (), identifier, signature, dimension);
      type->add_reaction (reaction);
      node.reaction = reaction;
      symtab_set_current_action (node.symtab, reaction);
      symtab_set_current_receiver_type (node.symtab, type);
    }

    void visit (ast_top_level_list_t& node)
    {
      for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          process_declarations (*pos);
        }
    }

    void visit (ast_type_definition_t& node)
    {
      Symbol *symbol = node.symbol.symbol ();
      const std::string& identifier = symbol->identifier;
      named_type_t *type = SymbolCast<TypeSymbol> (symbol)->type;

      if (symbol->defined ())
        return;

      if (symbol->in_progress)
        {
          error_at_line (-1, 0, node.location.file, node.location.line,
                         "%s is defined recursively", identifier.c_str ());
        }

      symbol->in_progress = true;
      ast_t *type_spec_node = node.type_spec ();
      const type_t *new_type = process_type_spec (type_spec_node, true, false, type);
      type->subtype (new_type);
      symbol->in_progress = false;
    }
  };

  visitor v;
  node->accept (v);
}

Symbol *
lookup_no_force (ast_t * node, const std::string& identifier)
{
  Symbol *symbol = node->symtab->find (identifier);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, node->location.file, node->location.line,
                     "%s was not declared in this scope", identifier.c_str ());
    }
  return symbol;
}
