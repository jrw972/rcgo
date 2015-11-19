#include "semantic.hpp"
#include "Type.hpp"
#include "Ast.hpp"
#include <error.h>
#include "Symbol.hpp"
#include "parameter.hpp"
#include "AstVisitor.hpp"

using namespace Type;
using namespace Ast;

// Look up a symbol.  Error if it is not defined.
static Symbol *
lookup_no_force (Node * node, const std::string& identifier)
{
  Symbol *symbol = node->FindGlobalSymbol (identifier);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "%s was not declared in this scope (E102)", identifier.c_str ());
    }
  return symbol;
}

typed_value_t
process_array_dimension (Ast::Node*& ptr)
{
  typed_value_t tv = CheckAndImplicitlyDereferenceAndConvertToDefault (ptr);
  tv.ArrayDimension (ptr->location);
  return tv;
}

void
CheckForForeignSafe (const Signature* signature, const parameter_t* return_parameter)
{
  for (Signature::const_iterator pos = signature->Begin (), limit = signature->End ();
       pos != limit;
       ++pos)
    {
      const parameter_t* parameter = *pos;
      if (!parameter->value.is_foreign_safe ())
        {
          error_at_line (-1, 0, parameter->defining_node->location.File.c_str (), parameter->defining_node->location.Line,
                         "parameter is not foreign safe (E106)");
        }
    }
  if (return_parameter != NULL && !return_parameter->value.is_foreign_safe ())
    {
      error_at_line (-1, 0, return_parameter->defining_node->location.File.c_str (), return_parameter->defining_node->location.Line,
                     "return parameter is not foreign safe (E107)");
    }
}

const Type::Type *
process_type_spec (Node * node, bool force_identifiers, bool is_component, NamedType* named_type)
{
  struct type_spec_visitor_t : public Ast::DefaultVisitor
  {
    const Type::Type* type;
    bool force_identifiers;
    bool is_component;
    NamedType* named_type;

    type_spec_visitor_t (bool fi,
                         bool ic,
                         NamedType* nt)
      : type (NULL)
      , force_identifiers (fi)
      , is_component (ic)
      , named_type (nt)
    { }

    void default_action (Node& node)
    {
      ast_not_reached (node);
    }

    void visit (ast_array_type_spec_t& node)
    {
      typed_value_t dimension = process_array_dimension (node.dimension_ref ());
      const Type::Type* base_type = process_type_spec (node.base_type (), true);
      type = base_type->GetArray (dimension.integral_value ());
    }

    void visit (ast_slice_type_spec_t& node)
    {
      const Type::Type* base_type = process_type_spec (node.child (), true);
      type = base_type->GetSlice ();
    }

    void visit (ast_empty_type_spec_t& node)
    {
      type = Void::Instance ();
    }

    void visit (ast_enum_type_spec_t& node)
    {
      type = Enum::Instance ();

      Ast::Node* value = node.values ();
      size_t e = 0;
      for (Node::ConstIterator pos = value->Begin (), limit = value->End ();
           pos != limit;
           ++pos, ++e)
        {
          std::string id = ast_get_identifier (*pos);
          if (node.GetParent ()->GetParent ()->FindLocalSymbol (id) != NULL)
            {
              error_at_line (-1, 0, (*pos)->location.File.c_str (), (*pos)->location.Line,
                             "%s is already defined in this scope (E108)", id.c_str ());
            }

          node.GetParent ()->GetParent ()->EnterSymbol (new TypedConstantSymbol (id,
              *pos,
              typed_value_t (named_type, e)));
        }
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
          Ast::Node* child = *pos;
          ast_identifier_list_type_spec_t* c = static_cast<ast_identifier_list_type_spec_t*> (child);
          Node *identifier_list = c->identifier_list ();
          Node *type_spec = c->type_spec ();
          const Type::Type *type = process_type_spec (type_spec, true);
          for (Node::ConstIterator pos2 = identifier_list->Begin (),
               limit2 = identifier_list->End ();
               pos2 != limit2;
               ++pos2)
            {
              Ast::Node* id = *pos2;
              const std::string& identifier = ast_get_identifier (id);
              const Type::Type *field = type_select (field_list, identifier);
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
      type = process_type_spec (node.child (), false)->GetHeap ();
    }

    void visit (ast_identifier_type_spec_t& node)
    {
      Node *child = node.child ();
      const std::string& identifier = ast_get_identifier (child);
      TypeSymbol* symbol;
      if (force_identifiers)
        {
          symbol = processAndLookup<TypeSymbol> (child, identifier);
        }
      else
        {
          symbol = SymbolCast<TypeSymbol> (lookup_no_force (child, identifier));
        }

      if (symbol == NULL)
        {
          error_at_line (-1, 0, child->location.File.c_str (), child->location.Line,
                         "%s does not refer to a type (E110)", identifier.c_str ());
        }
      type = SymbolCast<TypeSymbol> (symbol)->type;
    }

    void visit (ast_pointer_type_spec_t& node)
    {
      const Type::Type* base_type = process_type_spec (node.child (), false);
      type = base_type->GetPointer ();
    }

    void visit (ast_push_port_type_spec_t& node)
    {
      const Signature* signature = type_cast<Signature> (process_type_spec (node.signature (), true));
      const Type::Type* return_type = Type::Void::Instance ();
      typed_value_t return_value = typed_value_t::make_value (return_type, IMMUTABLE, IMMUTABLE, false);
      parameter_t* return_parameter = new parameter_t (&node,
          ReturnSymbol,
          return_value,
          false);
      CheckForForeignSafe (signature, return_parameter);
      type = new Type::Function (Type::Function::PUSH_PORT, signature, return_parameter);
    }

    void visit (ast_pull_port_type_spec_t& node)
    {
      const Signature* signature = type_cast<Signature> (process_type_spec (node.signature (), true));
      const Type::Type* return_type = process_type_spec (node.return_type (), true);
      typed_value_t return_value = typed_value_t::make_value (return_type, MUTABLE, node.dereferenceMutability, false);
      parameter_t* return_parameter = new parameter_t (node.return_type (),
          ReturnSymbol,
          return_value,
          false);
      CheckForForeignSafe (signature, return_parameter);
      type = new Type::Function (Type::Function::PULL_PORT, signature, return_parameter);
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
          const Type::Type* type = process_type_spec (type_spec, true);
          for (Node::Iterator pos2 = identifier_list->Begin (), limit2 = identifier_list->End ();
               pos2 != limit2;
               ++pos2)
            {
              Ast::Node* id = *pos2;
              const std::string& identifier = ast_get_identifier (id);
              const parameter_t *parameter = signature->Find (identifier);
              if (parameter == NULL)
                {
                  typed_value_t tv = typed_value_t::make_value (type,
                                     child->mutability,
                                     child->dereferenceMutability,
                                     false);
                  signature->Append (new parameter_t (id, identifier, tv, false));
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
  type_spec_visitor_t type_spec_visitor (force_identifiers, is_component, named_type);
  node->Accept (type_spec_visitor);
  return type_spec_visitor.type;
}
