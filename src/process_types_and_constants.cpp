#include "process_types_and_constants.hpp"

#include "AstVisitor.hpp"
#include "Symbol.hpp"
#include "semantic.hpp"

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

      void visit (ast_function_t& node)
      {
        // Do nothing.
      }

      void visit (ast_initializer_t& node)
      {
        // Do nothing.
      }

      void visit (ast_reaction_t& node)
      {
        // Do nothing.
      }

      void visit (ast_getter_t& node)
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
