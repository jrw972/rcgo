#include "ast.hpp"
#include "semantic.hpp"
#include <error.h>
#include "Symbol.hpp"
#include "action.hpp"
#include "bind.hpp"
#include "parameter.hpp"
#include "Callable.hpp"

static named_type_t*
processReceiver (ast_t* n, ast_t* identifierNode, parameter_t*& this_parameter, ParameterSymbol*& thisSymbol,
                 bool requireComponent, bool requireImmutableDereferenceMutability)
{
  ast_receiver_t* node = ast_cast<ast_receiver_t> (n);
  assert (node != NULL);

  ast_t *type_identifier_node = node->type_identifier ();
  const std::string& type_identifier = ast_get_identifier (type_identifier_node);
  TypeSymbol* symbol = processAndLookup<TypeSymbol> (type_identifier_node, type_identifier);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, type_identifier_node->location.File.c_str (), type_identifier_node->location.Line,
                     "E53: %s does not refer to a type",
                     type_identifier.c_str ());
    }

  named_type_t *type = symbol->type;

  if (requireComponent && type_strip_cast<component_type_t> (type) == NULL)
    {
      error_at_line (-1, 0, type_identifier_node->location.File.c_str (), type_identifier_node->location.Line,
                     "E54: %s does not refer to a component",
                     type_identifier.c_str ());
    }

  if (requireComponent && !node->isPointer)
    {
      // Components must have pointer receivers.
      error_at_line (-1, 0, node->location.File.c_str (),
                     node->location.Line,
                     "component receivers must be pointers");
    }

  if (requireImmutableDereferenceMutability && node->dereferenceMutability < IMMUTABLE)
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "E55: receiver must be declared +const or +foreign");

    }

  {
    const std::string& identifier = ast_get_identifier (identifierNode);
    const type_t *t = type_select (type, identifier);
    if (t != NULL)
      {
        error_at_line (-1, 0, identifierNode->location.File.c_str (),
                       identifierNode->location.Line,
                       "type already contains a member named %s",
                       identifier.c_str ());
      }
  }

  ast_t *this_identifier_node = node->this_identifier ();
  const std::string& this_identifier = ast_get_identifier (this_identifier_node);
  typed_value_t this_value =
    typed_value_t::make_value (pointer_type_t::make (type),
                               typed_value_t::STACK,
                               node->mutability,
                               node->dereferenceMutability);

  this_parameter = new parameter_t (this_identifier_node,
                                    this_identifier,
                                    this_value,
                                    true);

  thisSymbol = ParameterSymbol::makeReceiver (this_parameter);
  node->this_symbol = thisSymbol;

  return type;
}

static typed_value_t
processIota (ast_t& node, ast_t*& dimensionNode, ParameterSymbol*& parameterSymbol)
{
  typed_value_t dimension = process_array_dimension (dimensionNode);
  /* Insert "iota" into the symbol table. */
  typed_value_t zero = dimension;
  zero.zero ();

  typed_value_t iota_value = typed_value_t::make_range (zero, dimension, typed_value_t::STACK, IMMUTABLE, IMMUTABLE);

  parameter_t* iota_parameter = new parameter_t (dimensionNode,
      "IOTA",
      iota_value,
      false);
  parameterSymbol = enter_symbol (node,
                                  ParameterSymbol::make (iota_parameter));

  return dimension;
}

static void
processSignatureReturn (ast_t* signatureNode, ast_t* returnTypeNode, Mutability dereferenceMutability, bool requireForeignSafe,
                        const signature_type_t*& signature, parameter_t*& returnParameter, ParameterSymbol*& returnSymbol)
{
  /* Process the signature. */
  signature = type_cast<signature_type_t> (process_type_spec (signatureNode, true));

  /* Process the return type. */
  const type_t* return_type = process_type_spec (returnTypeNode, true);
  typed_value_t return_value = typed_value_t::make_value (return_type,
                               typed_value_t::STACK,
                               MUTABLE,
                               dereferenceMutability);

  returnParameter = new parameter_t (returnTypeNode,
                                     "0return",
                                     return_value,
                                     false);

  returnSymbol = ParameterSymbol::makeReturn (returnParameter);

  if (requireForeignSafe)
    {
      CheckForForeignSafe (signature, returnParameter);
    }
}

void
ProcessDeclarations (ast_t * node)
{
  struct visitor : public ast_visitor_t
  {
    void default_action (ast_t& node)
    {
      ast_not_reached (node);
    }

    void visit (ast_const_t& node)
    {
      typed_value_t right_tv = checkAndImplicitlyDereference (node.expr_ref ());
      if (!right_tv.value.present)
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "expression is not constant");
        }
      const type_t* type = process_type_spec (node.type_spec (), true);
      typed_value_t left_tv = typed_value_t::make_ref (type, typed_value_t::STACK, MUTABLE, IMMUTABLE);
      check_assignment (left_tv, right_tv, node,
                        "E11: incompatible types (%s) = (%s)",
                        "argument leaks mutable pointers");
      node.symbol = enter_symbol (*node.parent (),
                                  new TypedConstantSymbol (ast_get_identifier (node.identifier ()), &node, right_tv));
    }

    void visit (ast_action_t& node)
    {
      parameter_t* this_parameter;
      ParameterSymbol* thisSymbol;
      named_type_t* type = processReceiver (node.receiver (), node.identifier (), this_parameter, thisSymbol, true, true);
      enter_symbol (node, thisSymbol);
      action_t *action = new action_t (type, &node, node.body ());
      type->add_action (action);
      node.action = action;
    }

    void visit (ast_dimensioned_action_t& node)
    {
      parameter_t* this_parameter;
      ParameterSymbol* thisSymbol;
      named_type_t* type = processReceiver (node.receiver (), node.identifier (), this_parameter, thisSymbol, true, true);
      enter_symbol (node, thisSymbol);
      typed_value_t dimension = processIota (node, node.dimension_ref (), node.iota_symbol);
      action_t *action = new action_t (type, &node, node.body (), dimension);
      type->add_action (action);
      node.action = action;
    }

    void visit (ast_bind_t& node)
    {
      parameter_t* this_parameter;
      ParameterSymbol* thisSymbol;
      named_type_t* type = processReceiver (node.receiver (), node.identifier (), this_parameter, thisSymbol, true, false);
      enter_symbol (node, thisSymbol);
      bind_t* bind = new bind_t (&node);
      type->add_bind (bind);
      node.bind = bind;
    }

    void visit (ast_function_t& node)
    {
      const signature_type_t* signature;
      parameter_t* return_parameter;
      ParameterSymbol* return_symbol;
      processSignatureReturn (node.signature (), node.return_type (), node.dereferenceMutability, false,
                              signature, return_parameter, return_symbol);
      node.function->set (new function_type_t (signature, return_parameter), return_symbol);
      // Enter the return first as it is deeper on the stack.
      node.return_symbol = enter_symbol (node, return_symbol);
      enter_signature (node, signature);
    }

    void visit (ast_method_t& node)
    {
      parameter_t* this_parameter;
      ParameterSymbol* thisSymbol;
      named_type_t* type = processReceiver (node.receiver (), node.identifier (), this_parameter, thisSymbol, false, false);

      const signature_type_t* signature;
      parameter_t* return_parameter;
      ParameterSymbol* return_symbol;
      processSignatureReturn (node.signature (), node.return_type (), node.return_dereference_mutability, false,
                              signature, return_parameter, return_symbol);

      node.return_symbol = enter_symbol (node, return_symbol);
      enter_symbol (node, thisSymbol);
      enter_signature (node, signature);

      method_type_t* method_type = new method_type_t (type,
          this_parameter,
          signature,
          return_parameter);
      Method* method = new Method (&node, ast_get_identifier (node.identifier ()), method_type, return_symbol);

      type->add_method (method);
      node.method = method;
    }

    void visit (ast_initializer_t& node)
    {
      parameter_t* this_parameter;
      ParameterSymbol* thisSymbol;
      named_type_t* type = processReceiver (node.receiver (), node.identifier (), this_parameter, thisSymbol, true, false);

      const signature_type_t* signature;
      parameter_t* return_parameter;
      ParameterSymbol* return_symbol;
      processSignatureReturn (node.signature (), node.return_type (), node.return_dereference_mutability, true,
                              signature, return_parameter, return_symbol);

      node.return_symbol = enter_symbol (node, return_symbol);
      enter_symbol (node, thisSymbol);
      enter_signature (node, signature);

      initializer_type_t* initializer_type =
        new initializer_type_t (type,
                                this_parameter,
                                signature,
                                return_parameter);

      Initializer* initializer = new Initializer (&node, ast_get_identifier (node.identifier ()), initializer_type);

      type->add_initializer (initializer);
      node.initializer = initializer;
    }

    void visit (ast_getter_t& node)
    {
      parameter_t* this_parameter;
      ParameterSymbol* thisSymbol;
      named_type_t* type = processReceiver (node.receiver (), node.identifier (), this_parameter, thisSymbol, true, true);

      const signature_type_t* signature;
      parameter_t* return_parameter;
      ParameterSymbol* return_symbol;
      processSignatureReturn (node.signature (), node.return_type (), node.dereferenceMutability, true,
                              signature, return_parameter, return_symbol);

      node.return_symbol = enter_symbol (node, return_symbol);
      enter_symbol (node, thisSymbol);
      enter_signature (node, signature);

      getter_type_t* getter_type = new getter_type_t (type,
          this_parameter,
          signature,
          return_parameter);

      Getter* getter = new Getter (&node, ast_get_identifier (node.identifier ()), getter_type, return_symbol);

      type->add_getter (getter);
      node.getter = getter;
    }

    void visit (ast_instance_t& node)
    {
      ast_t *type_identifier_node = node.type_identifier ();
      const std::string& type_identifier = ast_get_identifier (type_identifier_node);
      TypeSymbol *symbol = processAndLookup<TypeSymbol> (type_identifier_node, ast_get_identifier (type_identifier_node));
      if (symbol == NULL)
        {
          error_at_line (-1, 0, type_identifier_node->location.File.c_str (),
                         type_identifier_node->location.Line,
                         "%s does not refer to a type",
                         type_identifier.c_str ());
        }
      named_type_t *type = symbol->type;
      if (type_cast<component_type_t> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_identifier_node->location.File.c_str (),
                         type_identifier_node->location.Line,
                         "%s does not refer to a component",
                         type_identifier.c_str ());
        }
      node.symbol->type = type;
    }

    void visit (ast_reaction_t& node)
    {
      parameter_t* this_parameter;
      ParameterSymbol* thisSymbol;
      named_type_t* type = processReceiver (node.receiver (), node.identifier (), this_parameter, thisSymbol, true, true);

      const signature_type_t* signature;
      parameter_t* return_parameter;
      ParameterSymbol* return_symbol;
      processSignatureReturn (node.signature (), node.return_type (), FOREIGN, true,
                              signature, return_parameter, return_symbol);

      // No return type.
      enter_symbol (node, thisSymbol);
      enter_signature (node, signature);

      reaction_t* reaction = new reaction_t (type, &node, node.body (), ast_get_identifier (node.identifier ()), signature);
      type->add_reaction (reaction);
      node.reaction = reaction;
    }

    void visit (ast_dimensioned_reaction_t& node)
    {
      parameter_t* this_parameter;
      ParameterSymbol* thisSymbol;
      named_type_t* type = processReceiver (node.receiver (), node.identifier (), this_parameter, thisSymbol, true, true);

      const signature_type_t* signature;
      parameter_t* return_parameter;
      ParameterSymbol* return_symbol;
      processSignatureReturn (node.signature (), node.return_type (), FOREIGN, true,
                              signature, return_parameter, return_symbol);

      typed_value_t dimension = processIota (node, node.dimension_ref (), node.iota_symbol);

      // No return type.
      enter_symbol (node, thisSymbol);
      enter_signature (node, signature);

      reaction_t* reaction = new reaction_t (type, &node, node.body (), ast_get_identifier (node.identifier ()), signature, dimension);
      type->add_reaction (reaction);
      node.reaction = reaction;
    }

    void visit (ast_top_level_list_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_type_definition_t& node)
    {
      TypeSymbol* symbol = node.symbol;
      if (symbol->defined ())
        return;
      if (symbol->inProgress)
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "%s is defined recursively", symbol->identifier.c_str ());
        }
      symbol->inProgress = true;
      named_type_t* type = SymbolCast<TypeSymbol> (symbol)->type;
      type->subtype (process_type_spec (node.type_spec (), true, false, type));
      symbol->inProgress = false;
    }
  };

  visitor v;
  node->accept (v);
}
