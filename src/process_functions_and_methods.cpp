#include "process_functions_and_methods.hpp"

#include "node.hpp"
#include "node_visitor.hpp"
#include "node_cast.hpp"
#include "semantic.hpp"
#include "symbol.hpp"
#include "action.hpp"
#include "reaction.hpp"
#include "semantic.hpp"
#include "bind.hpp"
#include "parameter_list.hpp"

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
processReceiver (decl::SymbolTable& symtab, ast::Node* n, ast::Identifier* identifierNode, ParameterSymbol*& receiver_symbol,
                 bool requireComponent, bool requireImmutableDereferenceMutability)
{
  Receiver* node = node_cast<Receiver> (n);
  assert (node != NULL);

  Identifier* type_identifier_node = node->type_identifier;
  const std::string& type_identifier = type_identifier_node->identifier;
  TypeSymbol* symbol = processAndLookup<TypeSymbol> (symtab, type_identifier, type_identifier_node->location);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, type_identifier_node->location.file.c_str (), type_identifier_node->location.line,
                     "%s does not refer to a type (E57)",
                     type_identifier.c_str ());
    }

  NamedType *type = symbol->type;

  if (requireComponent && type_strip_cast<Component> (type) == NULL)
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

  Identifier *this_identifier_node = node->this_identifier;
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
    ParameterSymbol::makeReceiver (this_identifier_node->location,
                                   this_identifier,
                                   receiver_type,
                                   node->mutability,
                                   node->indirection_mutability);

  return type;
}

static void
processSignatureReturn (ErrorReporter& er,
                        decl::SymbolTable& symtab,
                        ast::Node* signatureNode,
                        ast::Node* returnType,
                        Mutability dereferenceMutability,
                        bool requireForeignSafe,
                        const ParameterList*& signature,
                        ParameterSymbol*& returnSymbol)
{
  /* Process the signature. */
  signature = process_signature (signatureNode, er, symtab);

  /* Process the return type. */
  const type::Type* return_type = process_type (returnType, er, symtab, true);

  returnSymbol = ParameterSymbol::makeReturn (returnType->location,
                 ReturnSymbol,
                 return_type,
                 dereferenceMutability);

  if (requireForeignSafe)
    {
      CheckForForeignSafe (er, signature, returnSymbol);
    }
}

static void
process_signature_return (ErrorReporter& er,
                          decl::SymbolTable& symtab,
                          ast::Node* signatureNode,
                          ast::Node* returnType,
                          Mutability dereferenceMutability,
                          bool requireForeignSafe,
                          const ParameterList*& signature,
                          ParameterSymbol*& returnSymbol)
{
  /* Process the signature. */
  signature = process_signature (signatureNode, er, symtab);

  /* Process the return type. */
  const type::Type* return_type = process_type (returnType, er, symtab, true);

  returnSymbol = ParameterSymbol::makeReturn (returnType->location,
                 ReturnSymbol,
                 return_type,
                 dereferenceMutability);

  if (requireForeignSafe)
    {
      CheckForForeignSafe (er, signature, returnSymbol);
    }
}

struct Visitor : public ast::DefaultNodeVisitor
{
  ErrorReporter& er;
  decl::SymbolTable& symtab;

  Visitor (ErrorReporter& a_er,
           decl::SymbolTable& st)
    : er (a_er)
    , symtab (st)
  { }

  void default_action (Node& node)
  {
    AST_NOT_REACHED (node);
  }

  void visit (SourceFile& node)
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
    const ParameterList* signature;
    ParameterSymbol* return_symbol;
    process_signature_return (er, symtab, node.signature, node.return_type, node.indirection_mutability, false,
                              signature, return_symbol);
    const type::Function* function_type = new type::Function (type::Function::FUNCTION, signature, (new ParameterList (node.location))->append (return_symbol));
    node.function = new decl::Function (&node, function_type);

    symtab.enter_symbol (node.function);
  }

  void visit (ast::Method& node)
  {
    ParameterSymbol* thisSymbol;
    NamedType* type = processReceiver (symtab, node.receiver, node.identifier, thisSymbol, false, false);

    const ParameterList* signature;
    ParameterSymbol* return_symbol;
    processSignatureReturn (er, symtab, node.signature, node.return_type, node.return_indirection_mutability, false,
                            signature, return_symbol);

    type::Method* method_type = new type::Method (type::Method::METHOD, type,
        thisSymbol,
        signature,
        (new ParameterList (node.location))->append (return_symbol));
    decl::Method* method = new decl::Method (&node, node.identifier->identifier, method_type);

    type->insert_method (method);
    node.method = method;
  }

  void visit (ast::Initializer& node)
  {
    ParameterSymbol* thisSymbol;
    NamedType* type = processReceiver (symtab, node.receiver, node.identifier, thisSymbol, true, false);

    const ParameterList* signature;
    ParameterSymbol* return_symbol;
    processSignatureReturn (er, symtab, node.signature, node.return_type, node.return_indirection_mutability, true,
                            signature, return_symbol);

    type::Method* initializer_type =
      new type::Method (type::Method::INITIALIZER, type,
                        thisSymbol,
                        signature,
                        (new ParameterList (node.location))->append (return_symbol));

    decl::Initializer* initializer = new decl::Initializer (&node, node.identifier->identifier, initializer_type);

    type->insert_initializer (initializer);
    node.initializer = initializer;
  }

  void visit (ast::Action& node)
  {
    ParameterSymbol* receiver_parameter;
    NamedType* type = processReceiver (symtab, node.receiver, node.identifier, receiver_parameter, true, true);
    decl::Action *action = new decl::Action (receiver_parameter, node.body, node.identifier->identifier);
    type->insert_action (action);
    node.action = action;
    node.type = type;
  }

  void visit (DimensionedAction& node)
  {
    ParameterSymbol* receiver_parameter;
    NamedType* type = processReceiver (symtab, node.receiver, node.identifier, receiver_parameter, true, true);
    ParameterSymbol* iota_parameter = ParameterSymbol::make (node.dimension->location, "IOTA", type::Int::instance (), Immutable, Immutable);
    type::Int::ValueType dimension = process_array_dimension (node.dimension, er, symtab);
    decl::Action *action = new decl::Action (receiver_parameter, node.body, node.identifier->identifier, iota_parameter, dimension);
    type->insert_action (action);
    node.action = action;
    node.type = type;
  }

  void visit (ast::Reaction& node)
  {
    ParameterSymbol* thisSymbol;
    NamedType* type = processReceiver (symtab, node.receiver, node.identifier, thisSymbol, true, true);

    const ParameterList* signature;
    ParameterSymbol* return_symbol;
    processSignatureReturn (er, symtab, node.signature, node.return_type, Foreign, true,
                            signature, return_symbol);

    type::Method* reaction_type = new type::Method (type::Method::REACTION, type,
        thisSymbol,
        signature,
        (new ParameterList (node.location))->append (return_symbol));

    decl::Reaction* reaction = new decl::Reaction (node.body, node.identifier->identifier, reaction_type);

    type->insert_reaction (reaction);
    node.reaction = reaction;
  }

  void visit (DimensionedReaction& node)
  {
    ParameterSymbol* thisSymbol;
    NamedType* type = processReceiver (symtab, node.receiver, node.identifier, thisSymbol, true, true);

    const ParameterList* signature;
    ParameterSymbol* return_symbol;
    processSignatureReturn (er, symtab, node.signature, node.return_type, Foreign, true,
                            signature, return_symbol);

    ParameterSymbol* iotaSymbol = ParameterSymbol::make (node.dimension->location, "IOTA", type::Int::instance (), Immutable, Immutable);
    type::Int::ValueType dimension = process_array_dimension (node.dimension, er, symtab);

    type::Method* reaction_type = new type::Method (type::Method::REACTION, type,
        thisSymbol,
        signature,
        (new ParameterList (node.location))->append (return_symbol));

    decl::Reaction* reaction = new decl::Reaction (node.body, node.identifier->identifier, reaction_type, iotaSymbol, dimension);

    type->insert_reaction (reaction);
    node.reaction = reaction;
  }

  void visit (ast::Getter& node)
  {
    ParameterSymbol* thisSymbol;
    NamedType* type = processReceiver (symtab, node.receiver, node.identifier, thisSymbol, true, true);

    const ParameterList* signature;
    ParameterSymbol* return_symbol;
    processSignatureReturn (er, symtab, node.signature, node.return_type, node.indirection_mutability, true,
                            signature, return_symbol);

    type::Method* getter_type = new type::Method (type::Method::GETTER, type,
        thisSymbol,
        signature,
        (new ParameterList (node.location))->append (return_symbol));

    decl::Getter* getter = new decl::Getter (&node, node.identifier->identifier, getter_type);

    type->insert_getter (getter);
    node.getter = getter;
  }

  void visit (ast::Bind& node)
  {
    ParameterSymbol* thisSymbol;
    NamedType* type = processReceiver (symtab, node.receiver, node.identifier, thisSymbol, true, false);
    decl::Bind* bind = new decl::Bind (&node, node.identifier->identifier, thisSymbol);
    type->insert_bind (bind);
    node.bind = bind;
  }

  void visit (Instance& node)
  {
    const std::string& identifier = node.identifier->identifier;
    const std::string& initializer_identifier = node.initializer->identifier;

    const type::NamedType* type = type_cast<NamedType> (process_type (node.type_name, er, symtab, true));

    if (type_cast<Component> (type_strip (type)) == NULL)
      {
        error_at_line (-1, 0, node.type_name->location.file.c_str (),
                       node.type_name->location.line,
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

    node.symbol = new InstanceSymbol (identifier, node.identifier->location, type, initializer);
    symtab.enter_symbol (node.symbol);
  }

};
}

void process_functions_and_methods (ast::Node* root, ErrorReporter& er, decl::SymbolTable& symtab)
{
  Visitor visitor (er, symtab);
  root->accept (visitor);
}
}
