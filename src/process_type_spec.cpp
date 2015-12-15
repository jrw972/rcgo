#include "semantic.hpp"

#include <error.h>

#include "type.hpp"
#include "ast.hpp"
#include "symbol.hpp"
#include "ast_visitor.hpp"

namespace semantic
{

using namespace type;
using namespace ast;
using namespace decl;

static void
process_constant_expression (ast::Node* node)
{
  struct visitor : public ast::DefaultVisitor
  {
    void default_action (Node& node)
    {
      AST_NOT_REACHED (node);
    }

    void visit (ast_literal_expr_t& node)
    {
      // Do nothing.
    }
  };
  visitor v;
  node->Accept (v);
}

type::Int::ValueType
process_array_dimension (ast::Node* node)
{
  process_constant_expression (node);
  // Convert to an int.
  if (!node->value.representable (node->type, &NamedInt))
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "array dimension is not an integer (E108)");
    }

  node->value.convert (node->type, &NamedInt);
  node->type = &NamedInt;
  type::Int::ValueType dim = node->value.ref (*type::Int::Instance ());
  if (dim < 0)
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "array dimension is negative (E174)");
    }
  return dim;
}

void
CheckForForeignSafe (const Signature* signature, const ParameterSymbol* return_parameter)
{
  // TODO:  Move this up to the function/method type.
  signature->check_foreign_safe ();
  if (return_parameter != NULL)
    {
      return_parameter->check_foreign_safe ();
    }
}

const type::Type *
process_type (Node* node, bool force)
{
  struct Visitor : public ast::DefaultVisitor
  {
    const type::Type* type;

    Visitor ()
      : type (NULL)
    { }

    void default_action (Node& node)
    {
      AST_NOT_REACHED (node);
    }

    void visit (ast_array_type_spec_t& node)
    {
      type::Int::ValueType dimension = process_array_dimension (node.dimension ());
      const type::Type* base_type = process_type (node.base_type (), true);
      type = base_type->GetArray (dimension);
    }

    void visit (ast_slice_type_spec_t& node)
    {
      const type::Type* base_type = process_type (node.child (), false);
      type = base_type->GetSlice ();
    }

    void visit (ast_empty_type_spec_t& node)
    {
      type = Void::Instance ();
    }

    void visit (ast_field_list_type_spec_t& node)
    {
      Struct* field_list;
      if (node.IsComponent)
        {
          field_list = new Component ();
        }
      else
        {
          field_list = new Struct ();
        }

      for (Node::ConstIterator pos = node.Begin (), limit = node.End ();
           pos != limit;
           ++pos)
        {
          ast::Node* child = *pos;
          ast_identifier_list_type_spec_t* c = static_cast<ast_identifier_list_type_spec_t*> (child);
          Node *identifier_list = c->identifier_list ();
          Node *type_spec = c->type_spec ();
          const type::Type *type = process_type (type_spec, true);
          for (Node::ConstIterator pos2 = identifier_list->Begin (),
               limit2 = identifier_list->End ();
               pos2 != limit2;
               ++pos2)
            {
              ast::Node* id = *pos2;
              const std::string& identifier = ast_get_identifier (id);
              const type::Type *field = type_select (field_list, identifier);
              if (field == NULL)
                {
                  field_list->Append (identifier, type);
                }
              else
                {
                  error_at_line (-1, 0, id->location.File.c_str (), id->location.Line,
                                 "duplicate field name %s (E109)", identifier.c_str ());
                }
            }
        }
      type = field_list;
    }

    void visit (ast_heap_type_spec_t& node)
    {
      type = process_type (node.child (), false)->GetHeap ();
    }

    void visit (ast_identifier_type_spec_t& node)
    {
      Node *child = node.child ();
      const std::string& identifier = ast_get_identifier (child);
      Symbol* s = node.FindGlobalSymbol (identifier);
      if (s == NULL)
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "%s was not declared in this scope (E102)", identifier.c_str ());
        }
      TypeSymbol* symbol = SymbolCast<TypeSymbol> (s);
      if (symbol == NULL)
        {
          error_at_line (-1, 0, child->location.File.c_str (), child->location.Line,
                         "%s does not refer to a type (E110)", identifier.c_str ());
        }
      type = symbol->type;
    }

    void visit (ast_pointer_type_spec_t& node)
    {
      const type::Type* base_type = process_type (node.child (), false);
      type = base_type->GetPointer ();
    }

    void visit (ast_push_port_type_spec_t& node)
    {
      const Signature* signature = type_cast<Signature> (process_type (node.signature (), true));
      ParameterSymbol* return_parameter = ParameterSymbol::makeReturn (&node,
                                          ReturnSymbol,
                                          type::Void::Instance (),
                                          IMMUTABLE);

      CheckForForeignSafe (signature, return_parameter);
      type = new type::Function (type::Function::PUSH_PORT, signature, return_parameter);
    }

    void visit (ast_pull_port_type_spec_t& node)
    {
      const Signature* signature = type_cast<Signature> (process_type (node.signature (), true));
      const type::Type* return_type = process_type (node.return_type (), true);
      ParameterSymbol* return_parameter = ParameterSymbol::makeReturn (&node,
                                          ReturnSymbol,
                                          return_type,
                                          node.dereferenceMutability);
      CheckForForeignSafe (signature, return_parameter);
      type = new type::Function (type::Function::PULL_PORT, signature, return_parameter);
    }

    void visit (ast_signature_type_spec_t& node)
    {
      Signature *signature = new Signature ();
      for (Node::Iterator pos1 = node.Begin (), limit1 = node.End ();
           pos1 != limit1;
           ++pos1)
        {
          ast_identifier_list_type_spec_t* child = static_cast<ast_identifier_list_type_spec_t*> (*pos1);
          Node *identifier_list = child->identifier_list ();
          Node *type_spec = child->type_spec ();
          const type::Type* type = process_type (type_spec, true);
          for (Node::Iterator pos2 = identifier_list->Begin (), limit2 = identifier_list->End ();
               pos2 != limit2;
               ++pos2)
            {
              ast::Node* id = *pos2;
              const std::string& identifier = ast_get_identifier (id);
              const ParameterSymbol* parameter = signature->Find (identifier);
              if (parameter == NULL)
                {
                  signature->Append (ParameterSymbol::make (id, identifier, type, child->mutability, child->dereferenceMutability));
                }
              else
                {
                  error_at_line (-1, 0, id->location.File.c_str (), id->location.Line,
                                 "duplicate parameter name %s (E111)",
                                 identifier.c_str ());
                }
            }
        }
      type = signature;
    }

  };

  Visitor type_spec_visitor;
  node->Accept (type_spec_visitor);

  if (force && type_spec_visitor.type->UnderlyingType () == NULL)
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "type is defined recursively (E173)");
    }

  return type_spec_visitor.type;
}

}
