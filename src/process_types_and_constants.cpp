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

      // void visit (ast::Type& node)
      // {
      //   TypeSymbol* symbol = node.symbol;
      //   if (symbol->defined ())
      //     return;
      //   if (symbol->inProgress)
      //     {
      //       error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      //                      "%s is defined recursively (E65)", symbol->identifier.c_str ());
      //     }
      //   symbol->inProgress = true;
      //   NamedType* type = SymbolCast<TypeSymbol> (symbol)->type;
      //   type->UnderlyingType (process_type_spec (node.type_spec (), true, false, type));
      //   symbol->inProgress = false;
      // }

      void visit (ast_function_t& node)
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
