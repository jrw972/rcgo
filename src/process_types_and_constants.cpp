#include "process_types_and_constants.hpp"

#include "node.hpp"
#include "node_visitor.hpp"
#include "node_cast.hpp"
#include "symbol.hpp"
#include "semantic.hpp"
#include "check_types.hpp"
#include "scope.hpp"
#include "error_reporter.hpp"

namespace semantic
{
using namespace util;
using namespace ast;
using namespace type;
using namespace decl;

namespace
{
struct Visitor : public ast::DefaultNodeVisitor
{
  ErrorReporter& er;
  Scope* scope;

  Visitor (ErrorReporter& a_er,
           Scope* a_scope)
    : er (a_er)
    , scope (a_scope)
  { }

  void visit (SourceFile& node)
  {
    node.visit_children (*this);
  }

  void visit (ast::Type& node)
  {
    NamedType* type = new NamedType (node.identifier->identifier, node.identifier->location);
    // Enter the type before processing it to support recursive types.
    scope->enter_symbol (type);
    type->underlying_type (process_type (node.type, er, scope, true));
  }

  void visit (Const& node)
  {
    ast::List* identifier_list = node.identifiers;
    ast::Node* type_spec = node.type;
    ast::List* expression_list = node.expressions;

    if (identifier_list->size () != expression_list->size ())
      {
        er.expected_n_expressions (node.location, identifier_list->size (), expression_list->size ());
      }

    const size_t limit = std::min (identifier_list->size (), expression_list->size ());

    // Process the type spec.
    const type::Type* type = process_type (type_spec, er, scope, true);

    if (type->kind () != Void_Kind)
      {
        // Type, expressions.

        // Enter each symbol.
        size_t idx = 0;
        for (List::ConstIterator id_pos = identifier_list->begin (),
             init_pos = expression_list->begin ();
             idx != limit;
             ++id_pos, ++init_pos, ++idx)
          {
            Node* n = *init_pos;
            check_types (n, er, scope);
            if (!n->eval.value.present)
              {
                er.expression_is_not_constant (n->location);
                continue;
              }
            if (!are_assignable (n->eval.type, n->eval.value, type))
              {
                er.cannot_convert (n->location, n->eval.type, type);
                continue;
              }

            n->eval.value.convert (n->eval.type, type);
            n->eval.type = type;

            const std::string& name = node_cast<Identifier> (*id_pos)->identifier;
            Symbol* symbol = new Constant (name, (*id_pos)->location, type, n->eval.value);
            scope->enter_symbol (symbol);
          }

        node.done = true;
        return;
      }

    // No type, just expressions.

    // Enter each symbol.
    size_t idx = 0;
    for (List::ConstIterator id_pos = identifier_list->begin (),
         init_pos = expression_list->begin ();
         idx != limit;
         ++id_pos, ++init_pos, ++idx)
      {
        Node* n = *init_pos;
        check_types (n, er, scope);
        if (!n->eval.value.present)
          {
            er.expression_is_not_constant (n->location);
            continue;
          }

        const std::string& name = node_cast<Identifier> (*id_pos)->identifier;
        Symbol* symbol = new Constant (name, (*id_pos)->location, n->eval.type, n->eval.value);
        scope->enter_symbol (symbol);
      }
    node.done = true;
  }
};
}

void process_types_and_constants (ast::Node* root, ErrorReporter& er, Scope* scope)
{
  Visitor v (er, scope);
  root->accept (v);
}
}
