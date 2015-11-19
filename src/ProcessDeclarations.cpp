#include "Ast.hpp"
#include "semantic.hpp"
#include <error.h>
#include "Symbol.hpp"
#include "action.hpp"
#include "bind.hpp"
#include "parameter.hpp"
#include "Callable.hpp"
#include "AstVisitor.hpp"

using namespace Type;
using namespace Ast;

static NamedType*
processReceiver (Ast::Node* n, Ast::Node* identifierNode, parameter_t*& this_parameter, ParameterSymbol*& thisSymbol,
                 bool requireComponent, bool requireImmutableDereferenceMutability)
{
  ast_receiver_t* node = ast_cast<ast_receiver_t> (n);
  assert (node != NULL);

  Node *type_identifier_node = node->type_identifier ();
  const std::string& type_identifier = ast_get_identifier (type_identifier_node);
  TypeSymbol* symbol = processAndLookup<TypeSymbol> (type_identifier_node, type_identifier);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, type_identifier_node->location.File.c_str (), type_identifier_node->location.Line,
                     "%s does not refer to a type (E57)",
                     type_identifier.c_str ());
    }

  NamedType *type = symbol->type;

  if (requireComponent && type_strip_cast<Component> (type) == NULL)
    {
      error_at_line (-1, 0, type_identifier_node->location.File.c_str (), type_identifier_node->location.Line,
                     "%s does not refer to a component (E58)",
                     type_identifier.c_str ());
    }

  if (requireComponent && !node->isPointer)
    {
      // Components must have pointer receivers.
      error_at_line (-1, 0, node->location.File.c_str (),
                     node->location.Line,
                     "component receivers must be pointers (E59)");
    }

  if (requireImmutableDereferenceMutability && node->dereferenceMutability < IMMUTABLE)
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "receiver must be declared +const or +foreign (E60)");

    }

  {
    const std::string& identifier = ast_get_identifier (identifierNode);
    const Type::Type *t = type_select (type, identifier);
    if (t != NULL)
      {
        error_at_line (-1, 0, identifierNode->location.File.c_str (),
                       identifierNode->location.Line,
                       "type already contains a member named %s (E61)",
                       identifier.c_str ());
      }
  }

  Node *this_identifier_node = node->this_identifier ();
  const std::string& this_identifier = ast_get_identifier (this_identifier_node);

  const Type::Type* receiver_type;
  if (node->isPointer)
    {
      receiver_type = type->GetPointer ();
    }
  else
    {
      receiver_type = type;
    }

  typed_value_t this_value =
    typed_value_t::make_value (receiver_type,
                               node->mutability,
                               node->dereferenceMutability, true);

  this_parameter = new parameter_t (this_identifier_node,
                                    this_identifier,
                                    this_value,
                                    true);

  thisSymbol = ParameterSymbol::makeReceiver (this_parameter);
  node->this_symbol = thisSymbol;

  return type;
}

static Type::Uint::ValueType
processIota (Node& node, Ast::Node*& dimensionNode, ParameterSymbol*& parameterSymbol)
{
  typed_value_t dimension = process_array_dimension (dimensionNode);

  /* Insert "iota" into the symbol table. */
  typed_value_t zero = dimension;
  zero.zero ();

  typed_value_t iota_value = typed_value_t::make_range (zero, dimension, IMMUTABLE, IMMUTABLE);

  parameter_t* iota_parameter = new parameter_t (dimensionNode,
      "IOTA",
      iota_value,
      false);
  parameterSymbol = ParameterSymbol::make (iota_parameter);

  return dimension.Convert (dimensionNode->location, Type::Uint::Instance (), node).value.ref (*Type::Uint::Instance ());
}

static void
processSignatureReturn (Ast::Node* signatureNode, Ast::Node* returnType, Mutability dereferenceMutability, bool requireForeignSafe,
                        const Signature*& signature, parameter_t*& returnParameter, ParameterSymbol*& returnSymbol)
{
  /* Process the signature. */
  signature = type_cast<Signature> (process_type_spec (signatureNode, true));

  /* Process the return type. */
  const Type::Type* return_type = process_type_spec (returnType, true);
  typed_value_t return_value = typed_value_t::make_value (return_type,
                               MUTABLE,
                               dereferenceMutability,
                               false);

  returnParameter = new parameter_t (returnType,
                                     ReturnSymbol,
                                     return_value,
                                     false);

  returnSymbol = ParameterSymbol::makeReturn (returnParameter);

  if (requireForeignSafe)
    {
      CheckForForeignSafe (signature, returnParameter);
    }
}

void
ProcessDeclarations (Node * node)
{
  struct visitor : public Ast::DefaultVisitor
  {
    void default_action (Node& node)
    {
      ast_not_reached (node);
    }

    void visit (ast_const_t& node)
    {
      Ast::Node* identifier_list = node.identifier_list ();
      Ast::Node* type_spec = node.type_spec ();
      Ast::Node* expression_list = node.expression_list ();

      if (expression_list->Size () != 0 &&
          identifier_list->Size () != expression_list->Size ())
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "wrong number of initializers (E88)");
        }

      // Process the type spec.
      const Type::Type* type = process_type_spec (type_spec, true);

      if (type_cast<Void> (type) == NULL)
        {
          // Type, expressions.

          // Enter each symbol.
          for (Node::Iterator id_pos = identifier_list->Begin (),
               id_limit = identifier_list->End (),
               init_pos = expression_list->Begin ();
               id_pos != id_limit;
               ++id_pos, ++init_pos)
            {
              // Assume left is mutable.
              typed_value_t left_tv = typed_value_t::make_ref (type, MUTABLE, MUTABLE, false);
              typed_value_t right_tv = CheckAndImplicitlyDereferenceAndConvert (*init_pos, left_tv.type);
              if (!right_tv.value.present)
                {
                  error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                 "expression is not constant (E62)");
                }
              check_assignment (left_tv, right_tv, node,
                                "incompatible types (%s) = (%s) (E90)",
                                "assignment leaks mutable pointers (E92)");
              // Convert to specified mutability.
              right_tv.intrinsic_mutability = IMMUTABLE;
              const std::string& name = ast_get_identifier (*id_pos);
              Symbol* symbol = new TypedConstantSymbol (name, *id_pos, right_tv);
              enter_symbol (*node.GetParent (), symbol);
              //node.symbols.push_back (enter_symbol (*node.GetParent (), symbol));
            }

          return;
        }

      // No type, expressions.

      // Enter each symbol.
      for (Node::Iterator id_pos = identifier_list->Begin (),
           id_limit = identifier_list->End (),
           init_pos = expression_list->Begin ();
           id_pos != id_limit;
           ++id_pos, ++init_pos)
        {
          // Process the initializer.
          typed_value_t right_tv = CheckAndImplicitlyDereference (*init_pos);
          if (!right_tv.value.present)
            {
              error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                             "expression is not constant (E89)");
            }
          typed_value_t left_tv = typed_value_t::make_ref (right_tv);
          left_tv.intrinsic_mutability = MUTABLE;
          left_tv.dereference_mutability = IMMUTABLE;
          check_assignment (left_tv, right_tv, node,
                            "incompatible types (%s) = (%s) (E130)",
                            "assignment leaks mutable pointers (E131)");
          // Convert to specified mutability.
          right_tv.intrinsic_mutability = IMMUTABLE;
          const std::string& name = ast_get_identifier (*id_pos);
          Symbol* symbol = new TypedConstantSymbol (name, *id_pos, right_tv);
          enter_symbol (*node.GetParent (), symbol);
          //node.symbols.push_back (enter_symbol (*node.GetParent (), symbol));
        }
    }

    void visit (ast_action_t& node)
    {
      parameter_t* this_parameter;
      ParameterSymbol* thisSymbol;
      NamedType* type = processReceiver (node.receiver (), node.identifier (), this_parameter, thisSymbol, true, true);
      enter_symbol (node, thisSymbol);
      action_t *action = new action_t (type, &node, thisSymbol, node.body (), ast_get_identifier (node.identifier ()));
      type->Add (action);
      node.action = action;
    }

    void visit (ast_dimensioned_action_t& node)
    {
      parameter_t* this_parameter;
      ParameterSymbol* thisSymbol;
      NamedType* type = processReceiver (node.receiver (), node.identifier (), this_parameter, thisSymbol, true, true);
      enter_symbol (node, thisSymbol);
      ParameterSymbol* iotaSymbol;
      Type::Uint::ValueType dimension = processIota (node, node.dimension_ref (), iotaSymbol);
      enter_symbol (node, iotaSymbol);
      action_t *action = new action_t (type, &node, thisSymbol, node.body (), ast_get_identifier (node.identifier ()), iotaSymbol, dimension);
      type->Add (action);
      node.action = action;
    }

    void visit (ast_bind_t& node)
    {
      parameter_t* this_parameter;
      ParameterSymbol* thisSymbol;
      NamedType* type = processReceiver (node.receiver (), node.identifier (), this_parameter, thisSymbol, true, false);
      enter_symbol (node, thisSymbol);
      bind_t* bind = new bind_t (&node, ast_get_identifier (node.identifier ()));
      type->Add (bind);
      node.bind = bind;
    }

    void visit (ast_function_t& node)
    {
      const Signature* signature;
      parameter_t* return_parameter;
      ParameterSymbol* return_symbol;
      processSignatureReturn (node.signature (), node.return_type (), node.dereferenceMutability, false,
                              signature, return_parameter, return_symbol);
      node.function->set (new Type::Function (Type::Function::FUNCTION, signature, return_parameter), return_symbol);
      // Enter the return first as it is deeper on the stack.
      enter_symbol (node, return_symbol);
      enter_signature (node, signature);
    }

    void visit (ast_method_t& node)
    {
      parameter_t* this_parameter;
      ParameterSymbol* thisSymbol;
      NamedType* type = processReceiver (node.receiver (), node.identifier (), this_parameter, thisSymbol, false, false);

      const Signature* signature;
      parameter_t* return_parameter;
      ParameterSymbol* return_symbol;
      processSignatureReturn (node.signature (), node.return_type (), node.return_dereference_mutability, false,
                              signature, return_parameter, return_symbol);

      enter_symbol (node, return_symbol);
      enter_symbol (node, thisSymbol);
      enter_signature (node, signature);

      Type::Method* method_type = new Type::Method (Type::Method::METHOD, type,
          this_parameter,
          signature,
          return_parameter);
      ::Method* method = new ::Method (&node, ast_get_identifier (node.identifier ()), method_type, return_symbol);

      type->Add (method);
      node.method = method;
    }

    void visit (ast_initializer_t& node)
    {
      parameter_t* this_parameter;
      ParameterSymbol* thisSymbol;
      NamedType* type = processReceiver (node.receiver (), node.identifier (), this_parameter, thisSymbol, true, false);

      const Signature* signature;
      parameter_t* return_parameter;
      ParameterSymbol* return_symbol;
      processSignatureReturn (node.signature (), node.return_type (), node.return_dereference_mutability, true,
                              signature, return_parameter, return_symbol);

      enter_symbol (node, return_symbol);
      enter_symbol (node, thisSymbol);
      enter_signature (node, signature);

      Type::Method* initializer_type =
        new Type::Method (Type::Method::INITIALIZER, type,
                          this_parameter,
                          signature,
                          return_parameter);

      Initializer* initializer = new Initializer (&node, ast_get_identifier (node.identifier ()), initializer_type, return_symbol);

      type->Add (initializer);
      node.initializer = initializer;
    }

    void visit (ast_getter_t& node)
    {
      parameter_t* this_parameter;
      ParameterSymbol* thisSymbol;
      NamedType* type = processReceiver (node.receiver (), node.identifier (), this_parameter, thisSymbol, true, true);

      const Signature* signature;
      parameter_t* return_parameter;
      ParameterSymbol* return_symbol;
      processSignatureReturn (node.signature (), node.return_type (), node.dereferenceMutability, true,
                              signature, return_parameter, return_symbol);

      enter_symbol (node, return_symbol);
      enter_symbol (node, thisSymbol);
      enter_signature (node, signature);

      Type::Method* getter_type = new Type::Method (Type::Method::GETTER, type,
          this_parameter,
          signature,
          return_parameter);

      Getter* getter = new Getter (&node, ast_get_identifier (node.identifier ()), getter_type, return_symbol);

      type->Add (getter);
      node.getter = getter;
    }

    void visit (ast_instance_t& node)
    {
      Node *type_identifier_node = node.type_identifier ();
      const std::string& type_identifier = ast_get_identifier (type_identifier_node);
      TypeSymbol *symbol = processAndLookup<TypeSymbol> (type_identifier_node, ast_get_identifier (type_identifier_node));
      if (symbol == NULL)
        {
          error_at_line (-1, 0, type_identifier_node->location.File.c_str (),
                         type_identifier_node->location.Line,
                         "%s does not refer to a type (E63)",
                         type_identifier.c_str ());
        }
      NamedType *type = symbol->type;
      if (type_cast<Component> (type_strip (type)) == NULL)
        {
          error_at_line (-1, 0, type_identifier_node->location.File.c_str (),
                         type_identifier_node->location.Line,
                         "%s does not refer to a component (E64)",
                         type_identifier.c_str ());
        }
      node.symbol->type = type;
    }

    void visit (ast_reaction_t& node)
    {
      parameter_t* this_parameter;
      ParameterSymbol* thisSymbol;
      NamedType* type = processReceiver (node.receiver (), node.identifier (), this_parameter, thisSymbol, true, true);

      const Signature* signature;
      parameter_t* return_parameter;
      ParameterSymbol* return_symbol;
      processSignatureReturn (node.signature (), node.return_type (), FOREIGN, true,
                              signature, return_parameter, return_symbol);

      // No return type.
      enter_symbol (node, thisSymbol);
      enter_signature (node, signature);

      Type::Method* reaction_type = new Type::Method (Type::Method::REACTION, type,
          this_parameter,
          signature,
          return_parameter);

      reaction_t* reaction = new reaction_t (type, &node, thisSymbol, node.body (), ast_get_identifier (node.identifier ()), reaction_type);

      type->Add (reaction);
      node.reaction = reaction;
    }

    void visit (ast_dimensioned_reaction_t& node)
    {
      parameter_t* this_parameter;
      ParameterSymbol* thisSymbol;
      NamedType* type = processReceiver (node.receiver (), node.identifier (), this_parameter, thisSymbol, true, true);

      const Signature* signature;
      parameter_t* return_parameter;
      ParameterSymbol* return_symbol;
      processSignatureReturn (node.signature (), node.return_type (), FOREIGN, true,
                              signature, return_parameter, return_symbol);

      ParameterSymbol* iotaSymbol;
      Type::Uint::ValueType dimension = processIota (node, node.dimension_ref (), iotaSymbol);

      // No return type.
      enter_symbol (node, thisSymbol);
      enter_symbol (node, iotaSymbol);
      enter_signature (node, signature);

      Type::Method* reaction_type = new Type::Method (Type::Method::REACTION, type,
          this_parameter,
          signature,
          return_parameter);

      reaction_t* reaction = new reaction_t (type, &node, thisSymbol, node.body (), ast_get_identifier (node.identifier ()), reaction_type, iotaSymbol, dimension);

      type->Add (reaction);
      node.reaction = reaction;
    }

    void visit (SourceFile& node)
    {
      node.VisitChildren (*this);
    }

    void visit (Ast::Type& node)
    {
      TypeSymbol* symbol = node.symbol;
      if (symbol->defined ())
        return;
      if (symbol->inProgress)
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "%s is defined recursively (E65)", symbol->identifier.c_str ());
        }
      symbol->inProgress = true;
      NamedType* type = SymbolCast<TypeSymbol> (symbol)->type;
      type->UnderlyingType (process_type_spec (node.type_spec (), true, false, type));
      symbol->inProgress = false;
    }
  };

  visitor v;
  node->Accept (v);
}
