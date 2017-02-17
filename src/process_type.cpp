#include "process_type.hpp"

#include <error.h>

#include "type.hpp"
#include "node.hpp"
#include "node_visitor.hpp"
#include "node_cast.hpp"
#include "symbol.hpp"
#include "parameter_list.hpp"
#include "error_reporter.hpp"
#include "scope.hpp"
#include "symbol_cast.hpp"
#include "symbol_table.hpp"
#include "identifier.hpp"

namespace semantic
{
using namespace util;
using namespace type;
using namespace ast;
using namespace decl;
using namespace source;

long
process_array_dimension (ErrorReporter& er, decl::SymbolTable& symbol_table, ast::Node* node)
{
  if (!check_constant_expression (node, er, symbol_table))
    {
      return -1;
    }

  // Convert to an int.
  if (!node->eval.representable (&named_int))
    {
      er.non_integer_array_dimension (node->location, node->eval.type);
      return -1;
    }

  node->eval.convert (&named_int);
  node->eval.type = &named_int;
  long dim = node->eval.value.int_value;
  if (dim < 0)
    {
      er.negative_array_dimension (node->location, dim);
      return -1;
    }
  return dim;
}

void
check_for_foreign_safe (ErrorReporter& er,
                        const decl::ParameterList* parameter_list)
{
  for (decl::ParameterList::const_iterator pos = parameter_list->begin (), limit = parameter_list->end ();
       pos != limit;
       ++pos)
    {
      if (!(*pos)->is_foreign_safe ())
        {
          er.parameter_is_not_foreign_safe ((*pos)->identifier.location ());
        }
    }
}

Parameter*
process_receiver (ErrorReporter& er,
                  decl::SymbolTable& symbol_table,
                  const type::NamedType* named_type,
                  ast::Receiver* node,
                  bool require_component,
                  bool require_immutable_dereference_mutability)
{
  if (require_component && named_type->underlying_type ()->kind () != Component_Kind)
    {
      er.expected_a_component (node->location);
    }

  if (require_component && !node->is_pointer)
    {
      er.expected_a_pointer (node->location);
    }

  if (require_immutable_dereference_mutability && node->indirection_mutability < Immutable)
    {
      er.expected_immutable_indirection_mutability (node->location);
    }

  const type::Type* receiver_type;
  if (node->is_pointer)
    {
      receiver_type = named_type->get_pointer ();
    }
  else
    {
      receiver_type = named_type;
    }

  return Parameter::make_receiver (node->identifier,
                                   node->mutability,
                                   node->indirection_mutability,
                                   receiver_type);
}

const decl::ParameterList*
process_parameter_list (ErrorReporter& er,
                        decl::SymbolTable& symbol_table,
                        ast::ParameterList* node,
                        bool is_return)
{
  decl::ParameterList* parameter_list = new decl::ParameterList (node->location);
  for (List::ConstIterator pos1 = node->begin (), limit1 = node->end ();
       pos1 != limit1;
       ++pos1)
    {
      VariableList* child = static_cast<VariableList*> (*pos1);
      IdentifierList* identifier_list = child->identifiers;
      Node* type_spec = child->type;
      process_type (type_spec, er, symbol_table);
      if (type_spec->eval.kind == ExpressionValue::Error)
        {
          continue;
        }
      const type::Type* type = type_spec->eval.type;
      for (IdentifierList::const_iterator pos2 = identifier_list->begin (), limit2 = identifier_list->end ();
           pos2 != limit2;
           ++pos2)
        {
          const Identifier& identifier = *pos2;
          if (is_return)
            {
              parameter_list->append (Parameter::make_return (identifier, child->indirection_mutability, type));
            }
          else
            {
              parameter_list->append (Parameter::make (identifier, child->mutability, child->indirection_mutability, type));
            }
        }
    }

  return parameter_list;
}

void
process_signature (ErrorReporter& er,
                   decl::SymbolTable& symbol_table,
                   ast::ParameterList* parameter_list_node,
                   ast::ParameterList* return_parameter_list_node,
                   bool require_foreign_safe,
                   const decl::ParameterList*& parameter_list,
                   const decl::ParameterList*& return_parameter_list)
{
  /* Process the signature. */
  parameter_list = process_parameter_list (er, symbol_table, parameter_list_node, false);

  /* Process the return type. */
  return_parameter_list = process_parameter_list (er, symbol_table, return_parameter_list_node, true);

  if (require_foreign_safe)
    {
      check_for_foreign_safe (er, parameter_list);
      check_for_foreign_safe (er, return_parameter_list);
    }
}

void
check_unique_parameters (util::ErrorReporter& er,
                         const type::Function* type)
{
  Scope s;

  for (decl::ParameterList::const_iterator pos = type->parameter_list->begin (),
       limit = type->parameter_list->end ();
       pos != limit;
       ++pos)
    {
      Parameter* param = *pos;
      const Symbol* sym = s.find_local_symbol (param->identifier.identifier ());
      if (sym)
        {
          er.already_declared (param->identifier.location (), param->identifier.identifier (), sym->identifier.location ());
        }
      s.enter_symbol (param);
    }

  for (decl::ParameterList::const_iterator pos = type->return_parameter_list->begin (),
       limit = type->return_parameter_list->end ();
       pos != limit;
       ++pos)
    {
      Parameter* param = *pos;
      const Symbol* sym = s.find_local_symbol (param->identifier.identifier ());
      if (sym)
        {
          er.already_declared (param->identifier.location (), param->identifier.identifier (), sym->identifier.location ());
        }
      s.enter_symbol (param);
    }
}

void
process_type (Node* node, ErrorReporter& er, decl::SymbolTable& symbol_table, bool require_named_types_to_be_defined)
{
  if (node->eval.kind != ExpressionValue::Unknown) return;

  struct Visitor : public ast::DefaultNodeVisitor
  {
    ErrorReporter& er;
    decl::SymbolTable& symbol_table;
    ExpressionValue& out;
    bool const require_named_types_to_be_defined;

    Visitor (ErrorReporter& a_er,
             decl::SymbolTable& a_symbol_table,
             ExpressionValue& a_out,
             bool a_require_named_types_to_be_defined)
      : er (a_er)
      , symbol_table (a_symbol_table)
      , out (a_out)
      , require_named_types_to_be_defined (a_require_named_types_to_be_defined)
    { }

    void default_action (Node& node)
    {
      NODE_NOT_REACHED (node);
    }

    void visit (IdentifierType& node)
    {
      const Identifier& identifier = node.identifier;
      Symbol* s = symbol_table.retrieve_symbol (identifier);
      if (s == NULL)
        {
          er.not_declared (identifier);
          out.kind = ExpressionValue::Error;
          return;
        }

      NamedType* t = symbol_cast<NamedType> (s);
      if (t == NULL)
        {
          er.expected_a_type (node.location);
          out.kind = ExpressionValue::Error;
          return;
        }

      if (require_named_types_to_be_defined && !t->process_declaration (er, symbol_table))
        {
          er.not_defined (identifier.location (), identifier.identifier ());
          out.kind = ExpressionValue::Error;
          return;
        }

      out.kind = ExpressionValue::Type;
      out.type = t;
    }

    void visit (ast::Array& node)
    {
      long dimension = process_array_dimension (er, symbol_table, node.dimension);
      if (dimension == -1)
        {
          out.kind = ExpressionValue::Error;
          return;
        }

      process_type (node.base_type, er, symbol_table);
      if (node.base_type->eval.kind == ExpressionValue::Error)
        {
          out.kind = ExpressionValue::Error;
          return;
        }

      out.kind = ExpressionValue::Type;
      out.type = node.base_type->eval.type->get_array (dimension);
    }

    void visit (ast::Slice& node)
    {
      process_type (node.child, er, symbol_table, false);
      if (node.child->eval.kind == ExpressionValue::Error)
        {
          out.kind = ExpressionValue::Error;
          return;
        }

      out.kind = ExpressionValue::Type;
      out.type = node.child->eval.type->get_slice ();
    }

    void visit (ast::Pointer& node)
    {
      process_type (node.child, er, symbol_table, false);
      if (node.child->eval.kind == ExpressionValue::Error)
        {
          out.kind = ExpressionValue::Error;
          return;
        }

      out.kind = ExpressionValue::Type;
      out.type = node.child->eval.type->get_pointer ();
    }

    void visit (ast::Heap& node)
    {
      process_type (node.child, er, symbol_table, false);
      if (node.child->eval.kind == ExpressionValue::Error)
        {
          out.kind = ExpressionValue::Error;
          return;
        }

      out.kind = ExpressionValue::Type;
      out.type = node.child->eval.type->get_heap ();
    }

    void visit (EmptyType& node)
    {
      out.kind = ExpressionValue::EmptyType;
    }

    void visit (FieldList& node)
    {
      Struct* field_list;
      if (node.is_component)
        {
          field_list = new Component (symbol_table.package (), node.location);
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
          IdentifierList* identifier_list = c->identifiers;
          Node* type_spec = c->type;
          process_type (type_spec, er, symbol_table);
          if (type_spec->eval.kind == ExpressionValue::Error)
            {
              continue;
            }
          const type::Type* type = type_spec->eval.type;
          for (IdentifierList::const_iterator pos2 = identifier_list->begin (),
               limit2 = identifier_list->end ();
               pos2 != limit2;
               ++pos2)
            {
              const Identifier& identifier = *pos2;
              if (!field_list->has_member (identifier.identifier ()))
                {
                  field_list->append_field (symbol_table.package (), false, identifier, type, TagSet ());
                }
              else
                {
                  Field* f = field_list->find_field (identifier.identifier ());
                  er.already_declared (identifier.location (), identifier.identifier (), f->identifier.location ());
                }
            }
        }

      out.kind = ExpressionValue::Type;
      out.type = field_list;
    }

    void visit (ast::PushPort& node)
    {
      const decl::ParameterList* parameter_list = process_parameter_list (er, symbol_table, node.parameters, false);
      const decl::ParameterList* return_parameter_list = new decl::ParameterList (node.location);
      check_for_foreign_safe (er, parameter_list);
      check_for_foreign_safe (er, return_parameter_list);
      out.kind = ExpressionValue::Type;
      out.type = new type::PushPort (parameter_list, return_parameter_list);
    }

    void visit (ast::PullPort& node)
    {
      const decl::ParameterList* parameter_list = process_parameter_list (er, symbol_table, node.parameters, false);
      const decl::ParameterList* return_parameter_list = process_parameter_list (er, symbol_table, node.return_parameters, true);
      check_for_foreign_safe (er, parameter_list);
      check_for_foreign_safe (er, return_parameter_list);
      out.kind = ExpressionValue::Type;
      out.type = new type::PullPort (parameter_list, return_parameter_list);
    }
  };

  Visitor type_spec_visitor (er, symbol_table, node->eval, require_named_types_to_be_defined);
  node->accept (type_spec_visitor);
}

}
