#include "semantic.hpp"

#include <error.h>

#include "type.hpp"
#include "ast.hpp"
#include "ast_visitor.hpp"
#include "ast_cast.hpp"
#include "symbol.hpp"
#include "check_types.hpp"

namespace semantic
{
using namespace util;
using namespace type;
using namespace ast;
using namespace decl;

static void
process_constant_expression (ast::Node* node,
                             ErrorReporter& er,
                             decl::SymbolTable& symtab)
{
  check_types (node, er, symtab);
  if (!node->value.present)
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "expression must constant (E14)");

    }
}

type::Int::ValueType
process_array_dimension (ast::Node* node, ErrorReporter& er, decl::SymbolTable& symtab)
{
  process_constant_expression (node, er, symtab);
  // Convert to an int.
  if (!node->value.representable (node->type, &named_int))
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "array dimension is not an integer (E108)");
    }

  node->value.convert (node->type, &named_int);
  node->type = &named_int;
  type::Int::ValueType dim = node->value.int_value;
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
process_type (Node* node, ErrorReporter& er, decl::SymbolTable& symtab, bool force)
{
  struct Visitor : public ast::DefaultVisitor
  {
    ErrorReporter& er;
    decl::SymbolTable& symtab;
    const type::Type* type;

    Visitor (ErrorReporter& a_er,
             decl::SymbolTable& st)
      : er (a_er)
      , symtab (st)
      , type (NULL)
    { }

    void default_action (Node& node)
    {
      AST_NOT_REACHED (node);
    }

    void visit (ArrayTypeSpec& node)
    {
      type::Int::ValueType dimension = process_array_dimension (node.dimension, er, symtab);
      const type::Type* base_type = process_type (node.base_type, er, symtab, true);
      type = base_type->get_array (dimension);
    }

    void visit (SliceTypeSpec& node)
    {
      const type::Type* base_type = process_type (node.child, er, symtab, false);
      type = base_type->get_slice ();
    }

    void visit (EmptyTypeSpec& node)
    {
      type = Void::Instance ();
    }

    void visit (FieldListTypeSpec& node)
    {
      Struct* field_list;
      if (node.is_component)
        {
          field_list = new Component ();
        }
      else
        {
          field_list = new Struct ();
        }

      for (List::ConstIterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          ast::Node* child = *pos;
          IdentifierListTypeSpec* c = static_cast<IdentifierListTypeSpec*> (child);
          List *identifier_list = c->identifier_list;
          Node *type_spec = c->type_spec;
          const type::Type *type = process_type (type_spec, er, symtab, true);
          for (List::ConstIterator pos2 = identifier_list->begin (),
               limit2 = identifier_list->end ();
               pos2 != limit2;
               ++pos2)
            {
              ast::Node* id = *pos2;
              const std::string& identifier = ast_cast<Identifier> (id)->identifier;
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

    void visit (HeapTypeSpec& node)
    {
      type = process_type (node.child, er, symtab, false)->get_heap ();
    }

    void visit (IdentifierTypeSpec& node)
    {
      Identifier *child = node.child;
      const std::string& identifier = child->identifier;
      Symbol* s = symtab.find_global_symbol (identifier);
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

    void visit (PointerTypeSpec& node)
    {
      const type::Type* base_type = process_type (node.child, er, symtab, false);
      type = base_type->get_pointer ();
    }

    void visit (PushPortTypeSpec& node)
    {
      const Signature* signature = type_cast<Signature> (process_type (node.signature, er, symtab, true));
      ParameterSymbol* return_parameter = ParameterSymbol::makeReturn (node.location,
                                          ReturnSymbol,
                                          type::Void::Instance (),
                                          Immutable);

      CheckForForeignSafe (signature, return_parameter);
      type = new type::Function (type::Function::PUSH_PORT, signature, return_parameter);
    }

    void visit (PullPortTypeSpec& node)
    {
      const Signature* signature = type_cast<Signature> (process_type (node.signature, er, symtab, true));
      const type::Type* return_type = process_type (node.return_type, er, symtab, true);
      ParameterSymbol* return_parameter = ParameterSymbol::makeReturn (node.location,
                                          ReturnSymbol,
                                          return_type,
                                          node.indirection_mutability);
      CheckForForeignSafe (signature, return_parameter);
      type = new type::Function (type::Function::PULL_PORT, signature, return_parameter);
    }

    void visit (SignatureTypeSpec& node)
    {
      Signature *signature = new Signature ();
      for (List::ConstIterator pos1 = node.begin (), limit1 = node.end ();
           pos1 != limit1;
           ++pos1)
        {
          IdentifierListTypeSpec* child = static_cast<IdentifierListTypeSpec*> (*pos1);
          List *identifier_list = child->identifier_list;
          Node *type_spec = child->type_spec;
          const type::Type* type = process_type (type_spec, er, symtab, true);
          for (List::ConstIterator pos2 = identifier_list->begin (), limit2 = identifier_list->end ();
               pos2 != limit2;
               ++pos2)
            {
              ast::Node* id = *pos2;
              const std::string& identifier = ast_cast<Identifier> (id)->identifier;
              const ParameterSymbol* parameter = signature->Find (identifier);
              if (parameter == NULL)
                {
                  signature->Append (ParameterSymbol::make (id->location, identifier, type, child->mutability, child->indirection_mutability));
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

  Visitor type_spec_visitor (er, symtab);
  node->accept (type_spec_visitor);

  if (force && type_spec_visitor.type->UnderlyingType () == NULL)
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "type is defined recursively (E173)");
    }

  return type_spec_visitor.type;
}

}
