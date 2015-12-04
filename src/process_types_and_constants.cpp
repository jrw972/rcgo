#include "process_types_and_constants.hpp"

#include "AstVisitor.hpp"
#include "Symbol.hpp"
#include "semantic.hpp"
#include "check_types.hpp"

namespace semantic
{
  using namespace ast;
  using namespace Type;

  namespace
  {
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
        const std::string& name = ast_get_identifier (node.identifier ());
        NamedType* type = new NamedType (name);
        enter_symbol (*node.GetParent (), new TypeSymbol (name, node.identifier (), type));
        type->UnderlyingType (process_type (node.type_spec (), true));
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
                Node* n = *init_pos;
                check_types (n);
                if (!n->value.present)
                  {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "expression is not constant (E130)");
                  }
                if (!assignable (n->type, n->value, type))
                  {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "cannot assign %s to %s in initialization (E131)", n->type->ToString ().c_str (), type->ToString ().c_str ());
                  }

                n->value.convert (n->type, type);
                n->type = type;

                const std::string& name = ast_get_identifier (*id_pos);
                Symbol* symbol = new ConstantSymbol (name, *id_pos, type, n->value);
                enter_symbol (*node.GetParent (), symbol);
              }

            node.done = true;
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
            Node* n = *init_pos;
            check_types (n);
            if (!n->value.present)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "expression is not constant (E89)");
              }

            const std::string& name = ast_get_identifier (*id_pos);
            Symbol* symbol = new ConstantSymbol (name, *id_pos, n->type, n->value);
            enter_symbol (*node.GetParent (), symbol);
          }
        node.done = true;
      }

      void visit (ast_function_t& node)
      {
        // Do nothing.
      }

      void visit (ast_method_t& node)
      {
        // Do nothing.
      }

      void visit (ast_initializer_t& node)
      {
        // Do nothing.
      }

      void visit (ast_action_t& node)
      {
        // Do nothing.
      }

      void visit (ast_dimensioned_action_t& node)
      {
        // Do nothing.
      }

      void visit (ast_reaction_t& node)
      {
        // Do nothing.
      }

      void visit (ast_dimensioned_reaction_t& node)
      {
        // Do nothing.
      }

      void visit (ast_getter_t& node)
      {
        // Do nothing.
      }

      void visit (ast_bind_t& node)
      {
        // Do nothing.
      }

      void visit (ast_instance_t& node)
      {
        // Do nothing.
      }

    };
  }

  void process_types_and_constants (ast::Node* root)
  {
    Visitor v;
    root->Accept (v);
  }
}
