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

    void visit (ast_const_t& node)
    {
      ast::Node* identifier_list = node.identifier_list ();
      ast::Node* type_spec = node.type_spec ();
      ast::Node* expression_list = node.expression_list ();

      if (expression_list->Size () != 0 &&
          identifier_list->Size () != expression_list->Size ())
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "wrong number of initializers (E88)");
        }

      // Process the type spec.
      const Type::Type* type = process_type (type_spec, true);

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
              unimplemented;
              // Symbol* symbol = new ConstantSymbol (name, *id_pos, right_tv);
              // enter_symbol (*node.GetParent (), symbol);
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
          unimplemented;
          // typed_value_t right_tv = CheckAndImplicitlyDereference (*init_pos);
          // if (!right_tv.value.present)
          //   {
          //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
          //                    "expression is not constant (E89)");
          //   }
          // typed_value_t left_tv = typed_value_t::make_ref (right_tv);
          // left_tv.intrinsic_mutability = MUTABLE;
          // left_tv.dereference_mutability = IMMUTABLE;
          // check_assignment (left_tv, right_tv, node,
          //                   "incompatible types (%s) = (%s) (E130)",
          //                   "assignment leaks mutable pointers (E131)");
          // // Convert to specified mutability.
          // right_tv.intrinsic_mutability = IMMUTABLE;
          // const std::string& name = ast_get_identifier (*id_pos);
          // Symbol* symbol = new TypedConstantSymbol (name, *id_pos, right_tv);
          // enter_symbol (*node.GetParent (), symbol);
          // //node.symbols.push_back (enter_symbol (*node.GetParent (), symbol));
        }
    }

    void visit (ast_action_t& node)
    {
      unimplemented;
      // ParameterSymbol* thisSymbol;
      // NamedType* type = processReceiver (node.receiver (), node.identifier (), thisSymbol, true, true);
      // enter_symbol (node, thisSymbol);
      // Action *action = new Action (node.body (), ast_get_identifier (node.identifier ()));
      // type->Add (action);
      // node.action = action;
      // node.type = type;
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

    void visit (ast_bind_t& node)
    {
      unimplemented;
      // ParameterSymbol* thisSymbol;
      // NamedType* type = processReceiver (node.receiver (), node.identifier (), thisSymbol, true, false);
      // enter_symbol (node, thisSymbol);
      // bind_t* bind = new bind_t (&node, ast_get_identifier (node.identifier ()));
      // type->Add (bind);
      // node.bind = bind;
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
