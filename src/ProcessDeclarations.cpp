#include <error.h>

#include "ast.hpp"
#include "semantic.hpp"
#include "Symbol.hpp"
#include "action.hpp"
#include "reaction.hpp"
#include "bind.hpp"
#include "Callable.hpp"
#include "AstVisitor.hpp"

using namespace Type;
using namespace ast;
using namespace decl;

void
ProcessDeclarations (Node * node)
{
  struct visitor : public ast::DefaultVisitor
  {
    void default_action (Node& node)
    {
      ast_not_reached (node);
    }

    void visit (ast_dimensioned_action_t& node)
    {
      unimplemented;
      // ParameterSymbol* thisSymbol;
      // NamedType* type = processReceiver (node.receiver (), node.identifier (), thisSymbol, true, true);
      // enter_symbol (node, thisSymbol);
      // ParameterSymbol* iotaSymbol;
      // Type::Uint::ValueType dimension = processIota (node, node.dimension_ref (), iotaSymbol);
      // enter_symbol (node, iotaSymbol);
      // Action *action = new Action (node.body (), ast_get_identifier (node.identifier ()), dimension);
      // type->Add (action);
      // node.action = action;
      // node.type = type;
    }

    void visit (ast_method_t& node)
    {
      unimplemented;
      // ParameterSymbol* thisSymbol;
      // NamedType* type = processReceiver (node.receiver (), node.identifier (), thisSymbol, false, false);

      // const Signature* signature;
      // ParameterSymbol* return_symbol;
      // processSignatureReturn (node.signature (), node.return_type (), node.return_dereference_mutability, false,
      //                         signature, return_symbol);

      // enter_symbol (node, return_symbol);
      // enter_symbol (node, thisSymbol);
      // enter_signature (node, signature);

      // Type::Method* method_type = new Type::Method (Type::Method::METHOD, type,
      //     thisSymbol,
      //     signature,
      //     return_symbol);
      // ::Method* method = new ::Method (&node, ast_get_identifier (node.identifier ()), method_type);

      // type->Add (method);
      // node.method = method;
    }

    void visit (ast_dimensioned_reaction_t& node)
    {
      unimplemented;
      // ParameterSymbol* thisSymbol;
      // NamedType* type = processReceiver (node.receiver (), node.identifier (), thisSymbol, true, true);

      // const Signature* signature;
      // ParameterSymbol* return_symbol;
      // processSignatureReturn (node.signature (), node.return_type (), FOREIGN, true,
      //                         signature, return_symbol);

      // ParameterSymbol* iotaSymbol;
      // Type::Uint::ValueType dimension = processIota (node, node.dimension_ref (), iotaSymbol);

      // // No return type.
      // enter_symbol (node, thisSymbol);
      // enter_symbol (node, iotaSymbol);
      // enter_signature (node, signature);

      // Type::Method* reaction_type = new Type::Method (Type::Method::REACTION, type,
      //     thisSymbol,
      //     signature,
      //     return_symbol);

      // reaction_t* reaction = new reaction_t (type, &node, thisSymbol, node.body (), ast_get_identifier (node.identifier ()), reaction_type, iotaSymbol, dimension);

      // type->Add (reaction);
      // node.reaction = reaction;
    }
  };

  visitor v;
  node->Accept (v);
}
