#include "semantic.hpp"

#include <error.h>

#include "type.hpp"
#include "node.hpp"
#include "node_visitor.hpp"
#include "node_cast.hpp"
#include "symbol.hpp"
#include "check_types.hpp"
#include "parameter_list.hpp"
#include "error_reporter.hpp"

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
  if (!node->eval.value.present)
    {
      error_at_line (-1, 0, node->location.file.c_str (), node->location.line,
                     "expression must constant (E14)");

    }
}

long
process_array_dimension (ast::Node* node, ErrorReporter& er, decl::SymbolTable& symtab)
{
  process_constant_expression (node, er, symtab);
  // Convert to an int.
  if (!node->eval.value.representable (node->eval.type, &named_int))
    {
      error_at_line (-1, 0, node->location.file.c_str (), node->location.line,
                     "array dimension is not an integer (E108)");
    }

  node->eval.value.convert (node->eval.type, &named_int);
  node->eval.type = &named_int;
  long dim = node->eval.value.int_value;
  if (dim < 0)
    {
      error_at_line (-1, 0, node->location.file.c_str (), node->location.line,
                     "array dimension is negative (E174)");
    }
  return dim;
}

void
CheckForForeignSafe (ErrorReporter& er,
                     const decl::ParameterList* parameter_list,
                     const decl::ParameterList* return_parameter_list)
{
  if (!parameter_list->is_foreign_safe ())
    {
      er.signature_is_not_foreign_safe (parameter_list->location);
    }
  if (!return_parameter_list->is_foreign_safe())
    {
      er.signature_is_not_foreign_safe (return_parameter_list->location);
    }
}

const decl::ParameterList*
process_parameter_list (Node* node, ErrorReporter& er, decl::SymbolTable& symtab, bool is_return)
{
  struct Visitor : public ast::DefaultNodeVisitor
  {
    ErrorReporter& er;
    decl::SymbolTable& symtab;
    bool is_return;
    const decl::ParameterList* parameter_list;

    Visitor (ErrorReporter& a_er,
             decl::SymbolTable& st,
             bool a_is_return)
      : er (a_er)
      , symtab (st)
      , is_return (a_is_return)
      , parameter_list (NULL)
    { }

    void default_action (Node& node)
    {
      AST_NOT_REACHED (node);
    }

    void visit (ast::ParameterList& node)
    {
      decl::ParameterList* sig = new decl::ParameterList (node.location);
      for (List::ConstIterator pos1 = node.begin (), limit1 = node.end ();
           pos1 != limit1;
           ++pos1)
        {
          VariableList* child = static_cast<VariableList*> (*pos1);
          List *identifier_list = child->identifiers;
          Node *type_spec = child->type;
          const type::Type* type = process_type (type_spec, er, symtab, true);
          for (List::ConstIterator pos2 = identifier_list->begin (), limit2 = identifier_list->end ();
               pos2 != limit2;
               ++pos2)
            {
              ast::Node* id = *pos2;
              const std::string& identifier = node_cast<Identifier> (id)->identifier;
              const Parameter* parameter = sig->find (identifier);
              if (parameter == NULL)
                {
                  if (is_return)
                    {
                      sig->append (Parameter::make_return (id->location, identifier, type, child->indirection_mutability));
                    }
                  else
                    {
                      sig->append (Parameter::make (id->location, identifier, type, child->mutability, child->indirection_mutability));
                    }
                }
              else
                {
                  error_at_line (-1, 0, id->location.file.c_str (), id->location.line,
                                 "duplicate parameter name %s (E111)",
                                 identifier.c_str ());
                }
            }
        }
      parameter_list = sig;
    }

  };

  Visitor v (er, symtab, is_return);
  node->accept (v);

  return v.parameter_list;
}

const type::Type *
process_type (Node* node, ErrorReporter& er, decl::SymbolTable& symtab, bool force)
{
  struct Visitor : public ast::DefaultNodeVisitor
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

    void visit (ast::Array& node)
    {
      long dimension = process_array_dimension (node.dimension, er, symtab);
      const type::Type* base_type = process_type (node.base_type, er, symtab, true);
      type = base_type->get_array (dimension);
    }

    void visit (ast::Slice& node)
    {
      const type::Type* base_type = process_type (node.child, er, symtab, false);
      type = base_type->get_slice ();
    }

    void visit (EmptyType& node)
    {
      type = Void::instance ();
    }

    void visit (FieldList& node)
    {
      Struct* field_list;
      if (node.is_component)
        {
          field_list = new Component (symtab.package ());
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
          VariableList* c = static_cast<VariableList*> (child);
          List *identifier_list = c->identifiers;
          Node *type_spec = c->type;
          const type::Type *type = process_type (type_spec, er, symtab, true);
          for (List::ConstIterator pos2 = identifier_list->begin (),
               limit2 = identifier_list->end ();
               pos2 != limit2;
               ++pos2)
            {
              ast::Node* id = *pos2;
              const std::string& identifier = node_cast<Identifier> (id)->identifier;
              const type::Type *field = field_list->find (identifier);
              if (field == NULL)
                {
                  field_list->append_field (symtab.package (), false, identifier, type, TagSet ());
                }
              else
                {
                  error_at_line (-1, 0, id->location.file.c_str (), id->location.line,
                                 "duplicate field name %s (E109)", identifier.c_str ());
                }
            }
        }
      type = field_list;
    }

    void visit (ast::Heap& node)
    {
      type = process_type (node.child, er, symtab, false)->get_heap ();
    }

    void visit (IdentifierType& node)
    {
      Identifier *child = node.child;
      const std::string& identifier = child->identifier;
      Symbol* s = symtab.find_global_symbol (identifier);
      if (s == NULL)
        {
          error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                         "%s was not declared in this scope (E102)", identifier.c_str ());
        }
      type = symbol_cast<NamedType> (s);
      if (type == NULL)
        {
          error_at_line (-1, 0, child->location.file.c_str (), child->location.line,
                         "%s does not refer to a type (E110)", identifier.c_str ());
        }
    }

    void visit (ast::Pointer& node)
    {
      const type::Type* base_type = process_type (node.child, er, symtab, false);
      type = base_type->get_pointer ();
    }

    void visit (ast::PushPort& node)
    {
      const decl::ParameterList* parameter_list = process_parameter_list (node.parameters, er, symtab, false);
      const decl::ParameterList* return_parameter_list = new decl::ParameterList (node.location);
      CheckForForeignSafe (er, parameter_list, return_parameter_list);
      type = new type::PushPort (parameter_list, return_parameter_list);
    }

    void visit (ast::PullPort& node)
    {
      const decl::ParameterList* parameter_list = process_parameter_list (node.parameters, er, symtab, false);
      const decl::ParameterList* return_parameter_list = process_parameter_list (node.return_parameters, er, symtab, true);
      CheckForForeignSafe (er, parameter_list, return_parameter_list);
      type = new type::PullPort (parameter_list, return_parameter_list);
    }
  };

  Visitor type_spec_visitor (er, symtab);
  node->accept (type_spec_visitor);

  if (force && type_spec_visitor.type->underlying_type () == NULL)
    {
      error_at_line (-1, 0, node->location.file.c_str (), node->location.line,
                     "type is defined recursively (E173)");
    }

  return type_spec_visitor.type;
}

}
