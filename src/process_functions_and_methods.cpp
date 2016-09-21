#include "process_functions_and_methods.hpp"

#include "node.hpp"
#include "node_visitor.hpp"
#include "node_cast.hpp"
#include "semantic.hpp"
#include "symbol.hpp"
#include "action.hpp"
#include "semantic.hpp"
#include "bind.hpp"
#include "parameter_list.hpp"
#include "callable.hpp"

namespace semantic
{
using namespace util;
using namespace ast;
using namespace type;
using namespace decl;

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

static NamedType*
processReceiver (decl::Scope* scope, ast::Node* n, ast::Identifier* identifierNode, Parameter*& receiver_symbol,
                 bool requireComponent, bool requireImmutableDereferenceMutability)
{
  Receiver* node = node_cast<Receiver> (n);
  assert (node != NULL);

  Identifier* type_identifier_node = node->type;
  const std::string& type_identifier = type_identifier_node->identifier;
  NamedType* type = processAndLookup<NamedType> (scope, type_identifier, type_identifier_node->location);
  if (type == NULL)
    {
      error_at_line (-1, 0, type_identifier_node->location.file.c_str (), type_identifier_node->location.line,
                     "%s does not refer to a type (E57)",
                     type_identifier.c_str ());
    }

  if (requireComponent && type->underlying_type ()->kind () != Component_Kind)
    {
      error_at_line (-1, 0, type_identifier_node->location.file.c_str (), type_identifier_node->location.line,
                     "%s does not refer to a component (E58)",
                     type_identifier.c_str ());
    }

  if (requireComponent && !node->is_pointer)
    {
      // Components must have pointer receivers.
      error_at_line (-1, 0, node->location.file.c_str (),
                     node->location.line,
                     "component receivers must be pointers (E59)");
    }

  if (requireImmutableDereferenceMutability && node->indirection_mutability < Immutable)
    {
      error_at_line (-1, 0, node->location.file.c_str (), node->location.line,
                     "receiver must be declared +const or +foreign (E60)");

    }

  {
    const std::string& identifier = identifierNode->identifier;
    const type::Type *t = type->find (identifier);
    if (t != NULL)
      {
        error_at_line (-1, 0, identifierNode->location.file.c_str (),
                       identifierNode->location.line,
                       "type already contains a member named %s (E61)",
                       identifier.c_str ());
      }
  }

  Identifier *this_identifier_node = node->identifier;
  const std::string& this_identifier = this_identifier_node->identifier;

  const type::Type* receiver_type;
  if (node->is_pointer)
    {
      receiver_type = type->get_pointer ();
    }
  else
    {
      receiver_type = type;
    }

  receiver_symbol =
    Parameter::make_receiver (this_identifier_node->location,
                              this_identifier,
                              receiver_type,
                              node->mutability,
                              node->indirection_mutability);

  return type;
}

static void
process_signature_return (ErrorReporter& er,
                          decl::Scope* scope,
                          ast::List* parameter_list_node,
                          ast::List* return_parameter_list_node,
                          bool requireForeignSafe,
                          const decl::ParameterList*& parameter_list,
                          const decl::ParameterList*& return_parameter_list)
{
  /* Process the signature. */
  parameter_list = process_parameter_list (parameter_list_node, er, scope, false);

  /* Process the return type. */
  return_parameter_list = process_parameter_list (return_parameter_list_node, er, scope, true);

  if (requireForeignSafe)
    {
      CheckForForeignSafe (er, parameter_list, return_parameter_list);
    }
}

struct Visitor : public ast::DefaultNodeVisitor
{
  ErrorReporter& er;
  decl::Scope* scope;

  Visitor (ErrorReporter& a_er,
           decl::Scope* a_scope)
    : er (a_er)
    , scope (a_scope)
  { }

  void default_action (Node& node)
  {
    AST_NOT_REACHED (node);
  }

  void visit (SourceFile& node)
  {
    node.top_level_decl_list->accept (*this);
  }

  void visit (TopLevelDeclList& node)
  {
    node.visit_children (*this);
  }

  void visit (ast::Type& node)
  {
    // Do nothing.
  }

  void visit (Const& node)
  {
    // Do nothing.
  }

  void visit (ast::Function& node)
  {
    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature_return (er, scope, node.parameters, node.return_parameters, false,
                              parameter_list, return_parameter_list);
    const type::Function* function_type = new type::Function (parameter_list, return_parameter_list);
    node.function = new decl::Function (node.identifier->identifier, node.identifier->location, function_type);

    scope->enter_symbol (node.function);
  }

  void visit (ast::Method& node)
  {
    Parameter* thisSymbol;
    NamedType* type = processReceiver (scope, node.receiver, node.identifier, thisSymbol, false, false);

    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature_return (er, scope, node.parameters, node.return_parameters, false,
                              parameter_list, return_parameter_list);

    type::Method* method_type = new type::Method (type,
        thisSymbol,
        parameter_list,
        return_parameter_list);
    decl::Method* method = new decl::Method (node.identifier->identifier, method_type);

    type->insert_method (method);
    node.method = method;
  }

  void visit (ast::Initializer& node)
  {
    Parameter* thisSymbol;
    NamedType* type = processReceiver (scope, node.receiver, node.identifier, thisSymbol, true, false);

    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature_return (er, scope, node.parameters, node.return_parameters, true,
                              parameter_list, return_parameter_list);

    type::Initializer* initializer_type =
      new type::Initializer (type,
                             thisSymbol,
                             parameter_list,
                             return_parameter_list);

    decl::Initializer* initializer = new decl::Initializer (node.identifier->identifier, initializer_type);

    type->insert_initializer (initializer);
    node.initializer = initializer;
  }

  void visit (ast::Action& node)
  {
    Parameter* receiver_parameter;
    NamedType* type = processReceiver (scope, node.receiver, node.identifier, receiver_parameter, true, true);
    decl::Action *action = new decl::Action (receiver_parameter, node.precondition, node.body, node.identifier->identifier);
    type->insert_action (action);
    node.action = action;
    node.type = type;
  }

  void visit (DimensionedAction& node)
  {
    Parameter* receiver_parameter;
    NamedType* type = processReceiver (scope, node.receiver, node.identifier, receiver_parameter, true, true);
    Parameter* iota_parameter = Parameter::make (node.dimension->location, "IOTA", type::Int::instance (), Immutable, Immutable);
    long dimension = process_array_dimension (node.dimension, er, scope);
    decl::Action *action = new decl::Action (receiver_parameter, node.precondition, node.body, node.identifier->identifier, iota_parameter, dimension);
    type->insert_action (action);
    node.action = action;
    node.type = type;
  }

  void visit (ast::Reaction& node)
  {
    Parameter* thisSymbol;
    NamedType* type = processReceiver (scope, node.receiver, node.identifier, thisSymbol, true, true);

    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature_return (er, scope, node.parameters, node.return_parameters, true,
                              parameter_list, return_parameter_list);

    type::Reaction* reaction_type = new type::Reaction (type,
        thisSymbol,
        parameter_list,
        return_parameter_list);

    decl::Reaction* reaction = new decl::Reaction (node.body, node.identifier->identifier, reaction_type);

    type->insert_reaction (reaction);
    node.reaction = reaction;
  }

  void visit (DimensionedReaction& node)
  {
    Parameter* thisSymbol;
    NamedType* type = processReceiver (scope, node.receiver, node.identifier, thisSymbol, true, true);

    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature_return (er, scope, node.parameters, node.return_parameters, true,
                              parameter_list, return_parameter_list);

    Parameter* iotaSymbol = Parameter::make (node.dimension->location, "IOTA", type::Int::instance (), Immutable, Immutable);
    long dimension = process_array_dimension (node.dimension, er, scope);

    type::Reaction* reaction_type = new type::Reaction (type,
        thisSymbol,
        parameter_list,
        return_parameter_list);

    decl::Reaction* reaction = new decl::Reaction (node.body, node.identifier->identifier, reaction_type, iotaSymbol, dimension);

    type->insert_reaction (reaction);
    node.reaction = reaction;
  }

  void visit (ast::Getter& node)
  {
    Parameter* thisSymbol;
    NamedType* type = processReceiver (scope, node.receiver, node.identifier, thisSymbol, true, true);

    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature_return (er, scope, node.parameters, node.return_parameters, true,
                              parameter_list, return_parameter_list);

    type::Getter* getter_type = new type::Getter (type,
        thisSymbol,
        parameter_list,
        return_parameter_list);

    decl::Getter* getter = new decl::Getter (&node, node.identifier->identifier, getter_type);

    type->insert_getter (getter);
    node.getter = getter;
  }

  void visit (ast::Bind& node)
  {
    Parameter* thisSymbol;
    NamedType* type = processReceiver (scope, node.receiver, node.identifier, thisSymbol, true, false);
    decl::Bind* bind = new decl::Bind (&node, node.identifier->identifier, thisSymbol);
    type->insert_bind (bind);
    node.bind = bind;
  }

  void visit (ast::Instance& node)
  {
    const std::string& identifier = node.identifier->identifier;
    const std::string& initializer_identifier = node.initializer->identifier;

    const type::NamedType* type = process_type (node.type, er, scope, true)->to_named_type ();

    if (type->underlying_type ()->kind () != Component_Kind)
      {
        error_at_line (-1, 0, node.type->location.file.c_str (),
                       node.type->location.line,
                       "type does not refer to a component (E64)");
      }

    decl::Initializer* initializer = type->find_initializer (initializer_identifier);
    if (initializer == NULL)
      {
        error_at_line (-1, 0, node.initializer->location.file.c_str (),
                       node.initializer->location.line,
                       "no initializer named %s (E56)",
                       initializer_identifier.c_str ());
      }

    node.symbol = new decl::Instance (identifier, node.identifier->location, type, initializer);
    scope->enter_symbol (node.symbol);
  }

};
}

void process_functions_and_methods (ast::Node* root, ErrorReporter& er, decl::Scope* scope)
{
  Visitor visitor (er, scope);
  root->accept (visitor);
}
}
