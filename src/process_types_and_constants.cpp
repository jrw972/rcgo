#include "process_types_and_constants.hpp"

#include "ast.hpp"
#include "ast_visitor.hpp"
#include "ast_cast.hpp"
#include "symbol.hpp"
#include "semantic.hpp"
#include "check_types.hpp"
#include "symbol_table.hpp"

namespace semantic
{
using namespace util;
using namespace ast;
using namespace type;
using namespace decl;

namespace
{
struct Visitor : public ast::DefaultVisitor
{
  ErrorReporter& er;
  SymbolTable& symtab;

  Visitor (ErrorReporter& a_er,
           SymbolTable& st)
    : er (a_er)
    , symtab (st)
  { }

  void default_action (Node& node)
  {
    AST_NOT_REACHED (node);
  }

  void visit (SourceFile& node)
  {
    symtab.open_scope ();
    node.visit_children (*this);
  }

  void visit (ast::Type& node)
  {
    const std::string& name = node.identifier->identifier;
    NamedType* type = new NamedType (name);
    symtab.enter_symbol (new TypeSymbol (name, node.identifier->location, type));
    type->UnderlyingType (process_type (node.type_spec, er, symtab, true));
  }

  void visit (Const& node)
  {
    ast::List* identifier_list = node.identifier_list;
    ast::Node* type_spec = node.type_spec;
    ast::List* expression_list = node.expression_list;

    if (expression_list->size () != 0 &&
        identifier_list->size () != expression_list->size ())
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "wrong number of initializers (E88)");
      }

    // Process the type spec.
    const type::Type* type = process_type (type_spec, er, symtab, true);

    if (type_cast<Void> (type) == NULL)
      {
        // Type, expressions.

        // Enter each symbol.
        for (List::ConstIterator id_pos = identifier_list->begin (),
             id_limit = identifier_list->end (),
             init_pos = expression_list->begin ();
             id_pos != id_limit;
             ++id_pos, ++init_pos)
          {
            Node* n = *init_pos;
            check_types (n, er, symtab);
            if (!n->value.present)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "expression is not constant (E130)");
              }
            if (!assignable (n->type, n->value, type))
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "cannot assign %s to %s in initialization (E131)", n->type->to_string ().c_str (), type->to_string ().c_str ());
              }

            n->value.convert (n->type, type);
            n->type = type;

            const std::string& name = ast_cast<Identifier> (*id_pos)->identifier;
            Symbol* symbol = new ConstantSymbol (name, (*id_pos)->location, type, n->value);
            symtab.enter_symbol (symbol);
          }

        node.done = true;
        return;
      }

    // No type, expressions.

    // Enter each symbol.
    for (List::ConstIterator id_pos = identifier_list->begin (),
         id_limit = identifier_list->end (),
         init_pos = expression_list->begin ();
         id_pos != id_limit;
         ++id_pos, ++init_pos)
      {
        Node* n = *init_pos;
        check_types (n, er, symtab);
        if (!n->value.present)
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "expression is not constant (E89)");
          }

        const std::string& name = ast_cast<Identifier> (*id_pos)->identifier;
        Symbol* symbol = new ConstantSymbol (name, (*id_pos)->location, n->type, n->value);
        symtab.enter_symbol (symbol);
      }
    node.done = true;
  }

  void visit (ast::Function& node)
  {
    // Do nothing.
  }

  void visit (ast::Method& node)
  {
    // Do nothing.
  }

  void visit (ast::Initializer& node)
  {
    // Do nothing.
  }

  void visit (ast::Action& node)
  {
    // Do nothing.
  }

  void visit (DimensionedAction& node)
  {
    // Do nothing.
  }

  void visit (ast::Reaction& node)
  {
    // Do nothing.
  }

  void visit (DimensionedReaction& node)
  {
    // Do nothing.
  }

  void visit (ast::Getter& node)
  {
    // Do nothing.
  }

  void visit (Bind& node)
  {
    // Do nothing.
  }

  void visit (Instance& node)
  {
    // Do nothing.
  }

};
}

void process_types_and_constants (ast::Node* root, ErrorReporter& er, SymbolTable& symtab)
{
  Visitor v (er, symtab);
  root->accept (v);
}
}
