#include "process_functions_and_methods.hpp"

#include "AstVisitor.hpp"
#include "semantic.hpp"
#include "Symbol.hpp"
#include "action.hpp"
#include "reaction.hpp"
#include "semantic.hpp"
#include "bind.hpp"

namespace semantic
{
  using namespace ast;
  using namespace Type;
  using namespace decl;

  namespace
  {

// template <typename T>
// static T*
// enter_undefined_symbol (T* s,
//                         Node& a)
// {
//   ast::Node* symtab = a.GetParent ();
//   const std::string& identifier = s->identifier;
//   Symbol *symbol = symtab->FindLocalSymbol (identifier);
//   if (symbol == NULL)
//     {
//       symtab->EnterSymbol (s);
//     }
//   else
//     {
//       error_at_line (-1, 0, s->definingNode->location.File.c_str (),
//                      s->definingNode->location.Line,
//                      "%s is already defined in this scope (E73)", identifier.c_str ());
//     }
//   return s;
// }

    static NamedType*
    processReceiver (ast::Node* n, ast::Node* identifierNode, ParameterSymbol*& receiver_symbol,
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

      receiver_symbol = ParameterSymbol::makeReceiver (this_identifier_node,
                        this_identifier,
                        this_value.type,
                        node->mutability,
                        node->dereferenceMutability);

      node->this_symbol = receiver_symbol;

      return type;
    }

    static void
    processSignatureReturn (ast::Node* signatureNode, ast::Node* returnType, Mutability dereferenceMutability, bool requireForeignSafe,
                            const Signature*& signature, ParameterSymbol*& returnSymbol)
    {
      /* Process the signature. */
      signature = type_cast<Signature> (process_type (signatureNode, true));

      /* Process the return type. */
      const Type::Type* return_type = process_type (returnType, true);

      returnSymbol = ParameterSymbol::makeReturn (returnType,
                     ReturnSymbol,
                     return_type,
                     dereferenceMutability);

      if (requireForeignSafe)
        {
          CheckForForeignSafe (signature, returnSymbol);
        }
    }

    static void
    process_signature_return (ast::Node* signatureNode, ast::Node* returnType, Mutability dereferenceMutability, bool requireForeignSafe,
                              const Signature*& signature, ParameterSymbol*& returnSymbol)
    {
      /* Process the signature. */
      signature = type_cast<Signature> (process_type (signatureNode, true));

      /* Process the return type. */
      const Type::Type* return_type = process_type (returnType, true);

      returnSymbol = ParameterSymbol::makeReturn (returnType,
                     ReturnSymbol,
                     return_type,
                     dereferenceMutability);

      if (requireForeignSafe)
        {
          CheckForForeignSafe (signature, returnSymbol);
        }
    }

    struct Visitor : public ast::DefaultVisitor
    {
      void default_action (Node& node)
      {
        ast_not_reached (node);
      }

      void visit (SourceFile& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast::Type& node)
      {
        // Do nothing.
      }

      void visit (ast_function_t& node)
      {
        const Signature* signature;
        ParameterSymbol* return_symbol;
        process_signature_return (node.signature (), node.return_type (), node.dereferenceMutability, false,
                                  signature, return_symbol);
        const Type::Function* function_type = new Type::Function (Type::Function::FUNCTION, signature, return_symbol);
        node.function = new ::Function (node, function_type);

        // Enter the return first as it is deeper on the stack.
        enter_symbol (node, return_symbol);
        enter_signature (node, signature);

        enter_symbol (*node.GetParent (), node.function);
      }

      void visit (ast_method_t& node)
      {
        ParameterSymbol* thisSymbol;
        NamedType* type = processReceiver (node.receiver (), node.identifier (), thisSymbol, false, false);

        const Signature* signature;
        ParameterSymbol* return_symbol;
        processSignatureReturn (node.signature (), node.return_type (), node.return_dereference_mutability, false,
                                signature, return_symbol);

        enter_symbol (node, return_symbol);
        enter_symbol (node, thisSymbol);
        enter_signature (node, signature);

        Type::Method* method_type = new Type::Method (Type::Method::METHOD, type,
            thisSymbol,
            signature,
            return_symbol);
        ::Method* method = new ::Method (&node, ast_get_identifier (node.identifier ()), method_type);

        type->Add (method);
        node.method = method;
      }

      void visit (ast_initializer_t& node)
      {
        ParameterSymbol* thisSymbol;
        NamedType* type = processReceiver (node.receiver (), node.identifier (), thisSymbol, true, false);

        const Signature* signature;
        ParameterSymbol* return_symbol;
        processSignatureReturn (node.signature (), node.return_type (), node.return_dereference_mutability, true,
                                signature, return_symbol);

        enter_symbol (node, return_symbol);
        enter_symbol (node, thisSymbol);
        enter_signature (node, signature);

        Type::Method* initializer_type =
          new Type::Method (Type::Method::INITIALIZER, type,
                            thisSymbol,
                            signature,
                            return_symbol);

        Initializer* initializer = new Initializer (&node, ast_get_identifier (node.identifier ()), initializer_type);

        type->Add (initializer);
        node.initializer = initializer;
      }

      void visit (ast_action_t& node)
      {
        ParameterSymbol* thisSymbol;
        NamedType* type = processReceiver (node.receiver (), node.identifier (), thisSymbol, true, true);
        enter_symbol (node, thisSymbol);
        Action *action = new Action (node.body (), ast_get_identifier (node.identifier ()));
        type->Add (action);
        node.receiver_symbol = thisSymbol;
        node.action = action;
        node.type = type;
      }

      void visit (ast_dimensioned_action_t& node)
      {
        ParameterSymbol* thisSymbol;
        NamedType* type = processReceiver (node.receiver (), node.identifier (), thisSymbol, true, true);
        enter_symbol (node, thisSymbol);
        ParameterSymbol* iotaSymbol = ParameterSymbol::make (node.dimension (), "IOTA", Type::Int::Instance (), IMMUTABLE, IMMUTABLE);
        enter_symbol (node, iotaSymbol);
        Type::Int::ValueType dimension = process_array_dimension (node.dimension ());
        Action *action = new Action (node.body (), ast_get_identifier (node.identifier ()), dimension);
        type->Add (action);
        node.receiver_symbol = thisSymbol;
        node.action = action;
        node.type = type;
      }

      void visit (ast_reaction_t& node)
      {
        ParameterSymbol* thisSymbol;
        NamedType* type = processReceiver (node.receiver (), node.identifier (), thisSymbol, true, true);

        const Signature* signature;
        ParameterSymbol* return_symbol;
        processSignatureReturn (node.signature (), node.return_type (), FOREIGN, true,
                                signature, return_symbol);

        // No return type.
        enter_symbol (node, thisSymbol);
        enter_signature (node, signature);

        Type::Method* reaction_type = new Type::Method (Type::Method::REACTION, type,
            thisSymbol,
            signature,
            return_symbol);

        reaction_t* reaction = new reaction_t (&node, thisSymbol, node.body (), ast_get_identifier (node.identifier ()), reaction_type);

        type->Add (reaction);
        node.reaction = reaction;
      }

      void visit (ast_dimensioned_reaction_t& node)
      {
        ParameterSymbol* thisSymbol;
        NamedType* type = processReceiver (node.receiver (), node.identifier (), thisSymbol, true, true);

        const Signature* signature;
        ParameterSymbol* return_symbol;
        processSignatureReturn (node.signature (), node.return_type (), FOREIGN, true,
                                signature, return_symbol);

        ParameterSymbol* iotaSymbol = ParameterSymbol::make (node.dimension (), "IOTA", Type::Int::Instance (), IMMUTABLE, IMMUTABLE);
        Type::Int::ValueType dimension = process_array_dimension (node.dimension ());

        // No return type.
        enter_symbol (node, thisSymbol);
        enter_symbol (node, iotaSymbol);
        enter_signature (node, signature);

        Type::Method* reaction_type = new Type::Method (Type::Method::REACTION, type,
            thisSymbol,
            signature,
            return_symbol);

        reaction_t* reaction = new reaction_t (&node, thisSymbol, node.body (), ast_get_identifier (node.identifier ()), reaction_type, iotaSymbol, dimension);

        type->Add (reaction);
        node.reaction = reaction;
      }

      void visit (ast_getter_t& node)
      {
        ParameterSymbol* thisSymbol;
        NamedType* type = processReceiver (node.receiver (), node.identifier (), thisSymbol, true, true);

        const Signature* signature;
        ParameterSymbol* return_symbol;
        processSignatureReturn (node.signature (), node.return_type (), node.dereferenceMutability, true,
                                signature, return_symbol);

        enter_symbol (node, return_symbol);
        enter_symbol (node, thisSymbol);
        enter_signature (node, signature);

        Type::Method* getter_type = new Type::Method (Type::Method::GETTER, type,
            thisSymbol,
            signature,
            return_symbol);

        Getter* getter = new Getter (&node, ast_get_identifier (node.identifier ()), getter_type);

        type->Add (getter);
        node.getter = getter;
      }

      void visit (ast_bind_t& node)
      {
        ParameterSymbol* thisSymbol;
        NamedType* type = processReceiver (node.receiver (), node.identifier (), thisSymbol, true, false);
        enter_symbol (node, thisSymbol);
        bind_t* bind = new bind_t (&node, ast_get_identifier (node.identifier ()), thisSymbol);
        type->Add (bind);
        node.bind = bind;
      }

      void visit (ast_instance_t& node)
      {
        const std::string& identifier = ast_get_identifier (node.identifier ());
        const std::string& initializer_identifier = ast_get_identifier (node.initializer ());

        const Type::NamedType* type = type_cast<NamedType> (process_type (node.type_name (), true));

        if (type_cast<Component> (type_strip (type)) == NULL)
          {
            error_at_line (-1, 0, node.type_name ()->location.File.c_str (),
                           node.type_name ()->location.Line,
                           "type does not refer to a component (E64)");
          }

        Initializer* initializer = type->GetInitializer (initializer_identifier);
        if (initializer == NULL)
          {
            error_at_line (-1, 0, node.initializer ()->location.File.c_str (),
                           node.initializer ()->location.Line,
                           "no initializer named %s (E56)",
                           initializer_identifier.c_str ());
          }

        node.symbol = new InstanceSymbol (identifier, node.identifier (), type, initializer);
        enter_symbol (*node.GetParent (), node.symbol);
      }

    };
  }

  void process_functions_and_methods (ast::Node* root)
  {
    Visitor visitor;
    root->Accept (visitor);
  }
}
