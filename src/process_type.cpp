#include "process_type.hpp"

#include <error.h>

#include "type.hpp"
#include "node.hpp"
#include "node_visitor.hpp"
#include "node_cast.hpp"
#include "symbol.hpp"
#include "check_types.hpp"
#include "parameter_list.hpp"
#include "error_reporter.hpp"
#include "scope.hpp"
#include "symbol_cast.hpp"

namespace semantic
{
using namespace util;
using namespace type;
using namespace ast;
using namespace decl;

long
process_array_dimension (ErrorReporter& er, decl::Scope* scope, ast::Node* node)
{
  if (!check_constant_expression (node, er, scope))
    {
      return -1;
    }

  // Convert to an int.
  if (!node->eval.value.representable (node->eval.type, &named_int))
    {
      er.non_integer_array_dimension (node->location, node->eval.type);
      return -1;
    }

  node->eval.value.convert (node->eval.type, &named_int);
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
          er.parameter_is_not_foreign_safe ((*pos)->location);
        }
    }
}

NamedType*
process_receiver (ErrorReporter& er,
                  decl::Scope* scope,
                  ast::Receiver* node,
                  Parameter*& receiver_symbol,
                  bool require_component,
                  bool require_immutable_dereference_mutability)
{
  receiver_symbol = NULL;

  Identifier* type_identifier_node = node->type;
  const std::string& type_identifier = type_identifier_node->identifier;

  decl::Symbol* symbol = scope->find_global_symbol (type_identifier);
  if (symbol == NULL)
    {
      er.not_declared (type_identifier_node->location, type_identifier);
      return NULL;
    }

  NamedType* type = decl::symbol_cast<NamedType> (symbol);

  if (type == NULL)
    {
      er.expected_a_type (type_identifier_node->location);
      return NULL;
    }

  assert (type->state () == Symbol::Defined);

  if (require_component && type->underlying_type ()->kind () != Component_Kind)
    {
      er.expected_a_component (type_identifier_node->location);
      return NULL;
    }

  if (require_component && !node->is_pointer)
    {
      er.expected_a_pointer (type_identifier_node->location);
      return NULL;
    }

  if (require_immutable_dereference_mutability && node->indirection_mutability < Immutable)
    {
      er.expected_immutable_indirection_mutability (node->location);
      return NULL;
    }

  const type::Type* receiver_type;
  if (node->is_pointer)
    {
      receiver_type = type->get_pointer ();
    }
  else
    {
      receiver_type = type;
    }

  Identifier* this_identifier_node = node->identifier;

  receiver_symbol =
    Parameter::make_receiver (this_identifier_node->location,
                              this_identifier_node->identifier,
                              receiver_type,
                              node->mutability,
                              node->indirection_mutability);

  return type;
}

const decl::ParameterList*
process_parameter_list (ErrorReporter& er,
                        decl::Scope* scope,
                        ast::ParameterList* node,
                        bool is_return)
{
  decl::ParameterList* parameter_list = new decl::ParameterList (node->location);
  for (List::ConstIterator pos1 = node->begin (), limit1 = node->end ();
       pos1 != limit1;
       ++pos1)
    {
      VariableList* child = static_cast<VariableList*> (*pos1);
      List* identifier_list = child->identifiers;
      Node* type_spec = child->type;
      process_type (type_spec, er, scope);
      if (type_spec->eval.expression_kind == ErrorExpressionKind)
        {
          continue;
        }
      const type::Type* type = type_spec->eval.type;
      for (List::ConstIterator pos2 = identifier_list->begin (), limit2 = identifier_list->end ();
           pos2 != limit2;
           ++pos2)
        {
          ast::Node* id = *pos2;
          const std::string& identifier = node_cast<Identifier> (id)->identifier;
          const Parameter* parameter = parameter_list->find (identifier);
          if (parameter == NULL)
            {
              if (is_return)
                {
                  parameter_list->append (Parameter::make_return (id->location, identifier, type, child->indirection_mutability));
                }
              else
                {
                  parameter_list->append (Parameter::make (id->location, identifier, type, child->mutability, child->indirection_mutability));
                }
            }
          else
            {
              er.already_declared (id->location, identifier, parameter->location);
            }
        }
    }

  return parameter_list;
}

void
process_signature (ErrorReporter& er,
                   decl::Scope* scope,
                   ast::ParameterList* parameter_list_node,
                   ast::ParameterList* return_parameter_list_node,
                   bool require_foreign_safe,
                   const decl::ParameterList*& parameter_list,
                   const decl::ParameterList*& return_parameter_list)
{
  /* Process the signature. */
  parameter_list = process_parameter_list (er, scope, parameter_list_node, false);

  /* Process the return type. */
  return_parameter_list = process_parameter_list (er, scope, return_parameter_list_node, true);

  if (require_foreign_safe)
    {
      check_for_foreign_safe (er, parameter_list);
      check_for_foreign_safe (er, return_parameter_list);
    }
}

void
process_type (Node* node, ErrorReporter& er, decl::Scope* scope, bool require_named_types_to_be_defined)
{
  if (node->eval.expression_kind != UnknownExpressionKind) return;

  struct Visitor : public ast::DefaultNodeVisitor
  {
    ErrorReporter& er;
    decl::Scope* scope;
    ExpressionValue& out;
    bool const require_named_types_to_be_defined;

    Visitor (ErrorReporter& a_er,
             decl::Scope* a_scope,
             ExpressionValue& a_out,
             bool a_require_named_types_to_be_defined)
      : er (a_er)
      , scope (a_scope)
      , out (a_out)
      , require_named_types_to_be_defined (a_require_named_types_to_be_defined)
    { }

    void default_action (Node& node)
    {
      AST_NOT_REACHED (node);
    }

    void visit (IdentifierType& node)
    {
      Identifier* child = node.child;
      const std::string& identifier = child->identifier;
      Symbol* s = scope->find_global_symbol (identifier);
      if (s == NULL)
        {
          er.not_declared (node.location, identifier);
          out.expression_kind = ErrorExpressionKind;
          return;
        }

      NamedType* t = symbol_cast<NamedType> (s);
      if (t == NULL)
        {
          er.expected_a_type (node.location);
          out.expression_kind = ErrorExpressionKind;
          return;
        }

      if (require_named_types_to_be_defined && !t->process_declaration (er, scope))
        {
          er.not_defined (node.location, identifier);
          out.expression_kind = ErrorExpressionKind;
          return;
        }

      out.expression_kind = TypeExpressionKind;
      out.type = t;
    }

    void visit (ast::Array& node)
    {
      long dimension = process_array_dimension (er, scope, node.dimension);
      if (dimension == -1)
        {
          out.expression_kind = ErrorExpressionKind;
          return;
        }

      process_type (node.base_type, er, scope);
      if (node.base_type->eval.expression_kind == ErrorExpressionKind)
        {
          out.expression_kind = ErrorExpressionKind;
          return;
        }

      out.expression_kind = TypeExpressionKind;
      out.type = node.base_type->eval.type->get_array (dimension);
    }

    void visit (ast::Slice& node)
    {
      process_type (node.child, er, scope, false);
      if (node.child->eval.expression_kind == ErrorExpressionKind)
        {
          out.expression_kind = ErrorExpressionKind;
          return;
        }

      out.expression_kind = TypeExpressionKind;
      out.type = node.child->eval.type->get_slice ();
    }

    void visit (ast::Pointer& node)
    {
      process_type (node.child, er, scope, false);
      if (node.child->eval.expression_kind == ErrorExpressionKind)
        {
          out.expression_kind = ErrorExpressionKind;
          return;
        }

      out.expression_kind = TypeExpressionKind;
      out.type = node.child->eval.type->get_pointer ();
    }

    void visit (ast::Heap& node)
    {
      process_type (node.child, er, scope, false);
      if (node.child->eval.expression_kind == ErrorExpressionKind)
        {
          out.expression_kind = ErrorExpressionKind;
          return;
        }

      out.expression_kind = TypeExpressionKind;
      out.type = node.child->eval.type->get_heap ();
    }

    void visit (EmptyType& node)
    {
      out.expression_kind = EmptyTypeExpressionKind;
    }

    void visit (FieldList& node)
    {
      Struct* field_list;
      if (node.is_component)
        {
          field_list = new Component (scope->package (), node.location);
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
          List* identifier_list = c->identifiers;
          Node* type_spec = c->type;
          process_type (type_spec, er, scope);
          if (type_spec->eval.expression_kind == ErrorExpressionKind)
            {
              continue;
            }
          const type::Type* type = type_spec->eval.type;
          for (List::ConstIterator pos2 = identifier_list->begin (),
               limit2 = identifier_list->end ();
               pos2 != limit2;
               ++pos2)
            {
              ast::Node* id = *pos2;
              const std::string& identifier = node_cast<Identifier> (id)->identifier;
              if (!field_list->has_member (identifier))
                {
                  field_list->append_field (scope->package (), false, identifier, id->location, type, TagSet ());
                }
              else
                {
                  Field* f = field_list->find_field (identifier);
                  er.already_declared (id->location, identifier, f->location);
                }
            }
        }

      out.expression_kind = TypeExpressionKind;
      out.type = field_list;
    }

    void visit (ast::PushPort& node)
    {
      const decl::ParameterList* parameter_list = process_parameter_list (er, scope, node.parameters, false);
      const decl::ParameterList* return_parameter_list = new decl::ParameterList (node.location);
      check_for_foreign_safe (er, parameter_list);
      check_for_foreign_safe (er, return_parameter_list);
      out.expression_kind = TypeExpressionKind;
      out.type = new type::PushPort (parameter_list, return_parameter_list);
    }

    void visit (ast::PullPort& node)
    {
      const decl::ParameterList* parameter_list = process_parameter_list (er, scope, node.parameters, false);
      const decl::ParameterList* return_parameter_list = process_parameter_list (er, scope, node.return_parameters, true);
      check_for_foreign_safe (er, parameter_list);
      check_for_foreign_safe (er, return_parameter_list);
      out.expression_kind = TypeExpressionKind;
      out.type = new type::PullPort (parameter_list, return_parameter_list);
    }
  };

  Visitor type_spec_visitor (er, scope, node->eval, require_named_types_to_be_defined);
  node->accept (type_spec_visitor);
}

}
