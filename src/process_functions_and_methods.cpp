#include "process_functions_and_methods.hpp"

#include "AstVisitor.hpp"
#include "semantic.hpp"
#include "Symbol.hpp"

namespace semantic
{
  using namespace ast;
  using namespace Type;

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
    };
  }

  void process_functions_and_methods (ast::Node* root)
  {
    Visitor visitor;
    root->Accept (visitor);
  }
}
