#include "process_top_level_declarations.hpp"
#include "node_visitor.hpp"
#include "node.hpp"
#include "type.hpp"
#include "semantic.hpp"
#include "callable.hpp"
#include "node_cast.hpp"
#include "action.hpp"
#include "bind.hpp"
#include "process_type.hpp"

namespace semantic
{
namespace
{

using namespace ast;
using namespace util;
using namespace decl;
using namespace type;

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
    if (!node.symbol) return;
    node.symbol->process_declaration (er, file_scope);
  }

  void visit (MethodDecl& node)
  {
    Parameter* thisSymbol;
    NamedType* type = process_receiver (er, file_scope, node.receiver, thisSymbol, false, false);

    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature (er, file_scope, node.parameters, node.return_parameters, false,
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
    NamedType* type = process_receiver (er, file_scope, node.receiver, thisSymbol, true, false);

    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature (er, file_scope, node.parameters, node.return_parameters, true,
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
    NamedType* type = process_receiver (er, file_scope, node.receiver, thisSymbol, true, true);

    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature (er, file_scope, node.parameters, node.return_parameters, true,
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
    NamedType* type = process_receiver (er, file_scope, node.receiver, receiver_parameter, true, true);
    decl::Action *action = new decl::Action (receiver_parameter, node.precondition, node.body, node.identifier->identifier);
    type->insert_action (action);
    node.action = action;
    node.type = type;
  }

  void visit (DimensionedActionDecl& node)
  {
    Parameter* receiver_parameter;
    NamedType* type = process_receiver (er, file_scope, node.receiver, receiver_parameter, true, true);
    Parameter* iota_parameter = Parameter::make (node.dimension->location, "IOTA", type::Int::instance (), Immutable, Immutable);
    // TODO:  Check for error.
    long dimension = process_array_dimension (er, file_scope, node.dimension);
    decl::Action *action = new decl::Action (receiver_parameter, node.precondition, node.body, node.identifier->identifier, iota_parameter, dimension);
    type->insert_action (action);
    node.action = action;
    node.type = type;
  }

  void visit (ReactionDecl& node)
  {
    Parameter* thisSymbol;
    NamedType* type = process_receiver (er, file_scope, node.receiver, thisSymbol, true, true);

    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature (er, file_scope, node.parameters, node.return_parameters, true,
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
    NamedType* type = process_receiver (er, file_scope, node.receiver, thisSymbol, true, true);

    const decl::ParameterList* parameter_list;
    const decl::ParameterList* return_parameter_list;
    process_signature (er, file_scope, node.parameters, node.return_parameters, true,
                       parameter_list, return_parameter_list);

    Parameter* iotaSymbol = Parameter::make (node.dimension->location, "IOTA", type::Int::instance (), Immutable, Immutable);
    // TODO:  Check for error.
    long dimension = process_array_dimension (er, file_scope, node.dimension);

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
    NamedType* type = process_receiver (er, file_scope, node.receiver, thisSymbol, true, false);
    decl::Bind* bind = new decl::Bind (&node, node.identifier->identifier, thisSymbol);
    type->insert_bind (bind);
    node.bind = bind;
  }

  void visit (InstanceDecl& node)
  {
    if (!node.symbol) return;
    node.symbol->process_declaration (er, file_scope);
  }
};

}

void process_top_level_declarations (ast::Node* root, util::ErrorReporter& er, decl::Scope* file_scope)
{
  visitor v (er, file_scope);
  root->accept (v);
}
}
