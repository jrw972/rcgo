#include "process_top_level_declarations.hpp"
#include "node_visitor.hpp"
#include "node.hpp"
#include "type.hpp"
#include "semantic.hpp"
#include "callable.hpp"
#include "node_cast.hpp"
#include "action.hpp"
#include "bind.hpp"

namespace semantic
{
namespace
{

using namespace ast;
using namespace util;
using namespace decl;
using namespace type;

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

struct visitor : public DefaultNodeVisitor
{

  ErrorReporter& er;
  Scope* file_scope;

  visitor (ErrorReporter& a_er,
           Scope* a_file_scope)
    : er (a_er)
    , file_scope (a_file_scope)
  { }

  void visit (SourceFile& node)
  {
    node.top_level_decl_list->accept (*this);
  }

  void visit (TopLevelDeclList& node)
  {
    node.visit_children (*this);
  }

  void visit (ConstDecl& node)
  {
    for (ConstDecl::SymbolsType::const_iterator pos = node.symbols.begin (),
         limit = node.symbols.end ();
         pos != limit;
         ++pos)
      {
        (*pos)->process_declaration (er, file_scope);
      }
  }

  void visit (TypeDecl& node)
  {
    if (!node.symbol) return;
    node.symbol->process_declaration (er, file_scope);
  }

  void visit (FunctionDecl& node)
  {
    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature_return (er, file_scope, node.parameters, node.return_parameters, false,
                              parameter_list, return_parameter_list);
    node.symbol->type = new type::Function (parameter_list, return_parameter_list);
  }

  void visit (MethodDecl& node)
  {
    Parameter* thisSymbol;
    NamedType* type = processReceiver (file_scope, node.receiver, node.identifier, thisSymbol, false, false);

    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature_return (er, file_scope, node.parameters, node.return_parameters, false,
                              parameter_list, return_parameter_list);

    type::Method* method_type = new type::Method (type,
        thisSymbol,
        parameter_list,
        return_parameter_list);
    decl::Method* method = new decl::Method (node.identifier->identifier, method_type);

    type->insert_method (method);
    node.method = method;
  }

  void visit (InitDecl& node)
  {
    Parameter* thisSymbol;
    NamedType* type = processReceiver (file_scope, node.receiver, node.identifier, thisSymbol, true, false);

    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature_return (er, file_scope, node.parameters, node.return_parameters, true,
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

  void visit (GetterDecl& node)
  {
    Parameter* thisSymbol;
    NamedType* type = processReceiver (file_scope, node.receiver, node.identifier, thisSymbol, true, true);

    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature_return (er, file_scope, node.parameters, node.return_parameters, true,
                              parameter_list, return_parameter_list);

    type::Getter* getter_type = new type::Getter (type,
        thisSymbol,
        parameter_list,
        return_parameter_list);

    decl::Getter* getter = new decl::Getter (&node, node.identifier->identifier, getter_type);

    type->insert_getter (getter);
    node.getter = getter;
  }

  void visit (ActionDecl& node)
  {
    Parameter* receiver_parameter;
    NamedType* type = processReceiver (file_scope, node.receiver, node.identifier, receiver_parameter, true, true);
    decl::Action *action = new decl::Action (receiver_parameter, node.precondition, node.body, node.identifier->identifier);
    type->insert_action (action);
    node.action = action;
    node.type = type;
  }

  void visit (DimensionedActionDecl& node)
  {
    Parameter* receiver_parameter;
    NamedType* type = processReceiver (file_scope, node.receiver, node.identifier, receiver_parameter, true, true);
    Parameter* iota_parameter = Parameter::make (node.dimension->location, "IOTA", type::Int::instance (), Immutable, Immutable);
    long dimension = process_array_dimension (node.dimension, er, file_scope);
    decl::Action *action = new decl::Action (receiver_parameter, node.precondition, node.body, node.identifier->identifier, iota_parameter, dimension);
    type->insert_action (action);
    node.action = action;
    node.type = type;
  }

  void visit (ReactionDecl& node)
  {
    Parameter* thisSymbol;
    NamedType* type = processReceiver (file_scope, node.receiver, node.identifier, thisSymbol, true, true);

    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature_return (er, file_scope, node.parameters, node.return_parameters, true,
                              parameter_list, return_parameter_list);

    type::Reaction* reaction_type = new type::Reaction (type,
        thisSymbol,
        parameter_list,
        return_parameter_list);

    decl::Reaction* reaction = new decl::Reaction (node.body, node.identifier->identifier, reaction_type);

    type->insert_reaction (reaction);
    node.reaction = reaction;
  }

  void visit (DimensionedReactionDecl& node)
  {
    Parameter* thisSymbol;
    NamedType* type = processReceiver (file_scope, node.receiver, node.identifier, thisSymbol, true, true);

    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature_return (er, file_scope, node.parameters, node.return_parameters, true,
                              parameter_list, return_parameter_list);

    Parameter* iotaSymbol = Parameter::make (node.dimension->location, "IOTA", type::Int::instance (), Immutable, Immutable);
    long dimension = process_array_dimension (node.dimension, er, file_scope);

    type::Reaction* reaction_type = new type::Reaction (type,
        thisSymbol,
        parameter_list,
        return_parameter_list);

    decl::Reaction* reaction = new decl::Reaction (node.body, node.identifier->identifier, reaction_type, iotaSymbol, dimension);

    type->insert_reaction (reaction);
    node.reaction = reaction;
  }

  void visit (BindDecl& node)
  {
    Parameter* thisSymbol;
    NamedType* type = processReceiver (file_scope, node.receiver, node.identifier, thisSymbol, true, false);
    decl::Bind* bind = new decl::Bind (&node, node.identifier->identifier, thisSymbol);
    type->insert_bind (bind);
    node.bind = bind;
  }

  void visit (InstanceDecl& node)
  {
    const std::string& initializer_identifier = node.initializer->identifier;

    // TODO:  Check for failure.
    const type::NamedType* type = process_type (node.type, er, file_scope)->to_named_type ();

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
    node.symbol->type = type;
    node.symbol->initializer = initializer;
  }
};

}

void process_top_level_declarations (ast::Node* root, util::ErrorReporter& er, decl::Scope* file_scope)
{
  visitor v (er, file_scope);
  root->accept (v);
}
}
