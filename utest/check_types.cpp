#include "check_types.hpp"

#include "tap.hpp"
#include "node.hpp"
#include "error_reporter.hpp"
#include "symbol_table.hpp"
#include "semantic.hpp"
#include "builtin_function.hpp"

using namespace semantic;
using namespace ast;
using namespace util;
using namespace decl;
using namespace type;

static IdentifierExpr* make_identifier_expr (const std::string& id)
{
  return new IdentifierExpr (1, new Identifier (1, id));
}

static LiteralExpr* make_untyped_one ()
{
  semantic::Value v;
  v.present = true;
  v.untyped_integer_value = 1;
  return new LiteralExpr (1, UntypedInteger::instance (), v);
}

static LiteralExpr* make_untyped_false ()
{
  semantic::Value v;
  v.present = true;
  v.untyped_boolean_value = false;
  return new LiteralExpr (1, UntypedBoolean::instance (), v);
}

// static LiteralExpr* make_typed_false ()
// {
//   semantic::Value v;
//   v.present = true;
//   v.bool_value = false;
//   return new LiteralExpr (1, &named_bool, v);
// }

// static ast::Function* make_function_node ()
// {
//   return new ast::Function (1, new Identifier (1, "myfunc"), NULL, NULL, NULL);
// }

static type::Function* make_function_type ()
{
  return new type::Function (NULL, NULL);
}

static bool no_error (const ErrorReporter& er)
{
  return er.list ().empty ();
}

static bool last_error (const ErrorReporter& er, ErrorCode code)
{
  return !er.list ().empty () && er.list ().back () == code;
}

int
main (int argc, char** argv)
{
  Tap tap;

  {
    Node* root = make_untyped_one ();
    ErrorReporter er;
    SymbolTable symtab;

    check_types (root, er, symtab);

    tap.tassert ("semantic::check_types LiteralExpr",
                 root->eval.expression_kind == ValueExpressionKind &&
                 root->eval.intrinsic_mutability == Immutable &&
                 root->eval.indirection_mutability == Immutable &&
                 no_error (er));
    delete root;
  }

  {
    Node* child1 = make_untyped_one ();
    Node* child2 = make_untyped_one ();
    Node* root = (new ListExpr (1))->append (child1)->append (child2);
    ErrorReporter er;
    SymbolTable symtab;

    check_types (root, er, symtab);

    tap.tassert ("semantic::check_types ListExpr",
                 child2->eval.expression_kind == ValueExpressionKind &&
                 child2->eval.intrinsic_mutability == Immutable &&
                 child2->eval.indirection_mutability == Immutable &&
                 no_error (er));
    delete root;
  }

  {
    Node* root = make_identifier_expr ("int64");
    ErrorReporter er;
    SymbolTable symtab;
    symtab.open_scope ();

    check_types (root, er, symtab);

    tap.tassert ("semantic::check_types IdentifierExpr undefined",
                 root->eval.expression_kind == ErrorExpressionKind &&
                 last_error (er, Undefined));

    delete root;
  }

  {
    Node* root = make_identifier_expr ("readable");
    ErrorReporter er;
    runtime::BuiltinFunction* symbol = new runtime::Readable (Location (1));
    SymbolTable symtab;
    symtab.open_scope ();
    symtab.enter_symbol (symbol);

    check_types (root, er, symtab);

    tap.tassert ("semantic::check_types IdentifierExpr BuiltinFunction",
                 root->eval.expression_kind == ValueExpressionKind &&
                 root->eval.type == symbol->type &&
                 root->callable == symbol &&
                 root->eval.intrinsic_mutability == Immutable &&
                 root->eval.indirection_mutability == Immutable &&
                 no_error (er));

    delete root;
  }

  {
    Node* root = make_identifier_expr ("new");
    ErrorReporter er;
    decl::PolymorphicFunction* symbol = new semantic::New (Location (1));
    SymbolTable symtab;
    symtab.open_scope ();
    symtab.enter_symbol (symbol);

    check_types (root, er, symtab);

    tap.tassert ("semantic::check_types IdentifierExpr Template",
                 root->eval.expression_kind == ValueExpressionKind &&
                 root->eval.type == type::PolymorphicFunction::instance () &&
                 root->temp == symbol &&
                 root->eval.intrinsic_mutability == Immutable &&
                 root->eval.indirection_mutability == Immutable &&
                 no_error (er));

    delete root;
  }

  {
    util::Location loc;
    Node* root = make_identifier_expr ("myfunc");
    ErrorReporter er;
    decl::Function* symbol = new decl::Function ("myfunc", loc, make_function_type ());
    SymbolTable symtab;
    symtab.open_scope ();
    symtab.enter_symbol (symbol);

    check_types (root, er, symtab);

    tap.tassert ("semantic::check_types IdentifierExpr Function",
                 root->eval.expression_kind == ValueExpressionKind &&
                 root->eval.type == symbol->type &&
                 root->callable == symbol &&
                 root->eval.intrinsic_mutability == Immutable &&
                 root->eval.indirection_mutability == Immutable &&
                 no_error (er));

    delete root;
  }

  {
    Node* root = make_identifier_expr ("myparam");
    ErrorReporter er;
    decl::Parameter* symbol = Parameter::make (Location (1), "myparam", &named_int, Mutable, Mutable);
    SymbolTable symtab;
    symtab.open_scope ();
    symtab.enter_symbol (symbol);

    check_types (root, er, symtab);

    tap.tassert ("semantic::check_types IdentifierExpr Parameter",
                 root->eval.expression_kind == VariableExpressionKind &&
                 root->eval.type == symbol->type &&
                 root->eval.intrinsic_mutability == Mutable &&
                 root->eval.indirection_mutability == Mutable &&
                 no_error (er));

    delete root;
  }

  {
    Node* root = make_identifier_expr ("bool");
    ErrorReporter er;
    SymbolTable symtab;
    symtab.open_scope ();
    symtab.enter_symbol (&named_bool);

    check_types (root, er, symtab);

    tap.tassert ("semantic::check_types IdentifierExpr Type",
                 root->eval.expression_kind == TypeExpressionKind &&
                 root->eval.type == &named_bool &&
                 no_error (er));

    delete root;
  }

  {
    Node* root = make_identifier_expr ("nil");
    ErrorReporter er;
    Value v;
    v.present = true;
    decl::Constant* symbol = new Constant ("nil", Location (1), type::UntypedNil::instance (), v);
    SymbolTable symtab;
    symtab.open_scope ();
    symtab.enter_symbol (symbol);

    check_types (root, er, symtab);

    tap.tassert ("semantic::check_types IdentifierExpr Constant",
                 root->eval.expression_kind == ValueExpressionKind &&
                 root->eval.type == symbol->type &&
                 root->eval.intrinsic_mutability == Immutable &&
                 root->eval.indirection_mutability == Mutable &&
                 no_error (er));

    delete root;
  }

  {
    Node* root = make_identifier_expr ("myvar");
    ErrorReporter er;
    decl::Variable* symbol = new Variable ("myvar", Location (1), &named_int, Mutable, Mutable);
    SymbolTable symtab;
    symtab.open_scope ();
    symtab.enter_symbol (symbol);

    check_types (root, er, symtab);

    tap.tassert ("semantic::check_types IdentifierExpr Variable",
                 root->eval.expression_kind == VariableExpressionKind &&
                 root->eval.type == symbol->type &&
                 root->eval.intrinsic_mutability == Mutable &&
                 root->eval.indirection_mutability == Mutable &&
                 no_error (er));

    delete root;
  }

  {
    Node* root = make_identifier_expr ("myvar");
    ErrorReporter er;
    decl::Hidden* symbol = new decl::Hidden (new Variable ("myvar", Location (1), &named_int, Mutable, Mutable), Location (1));
    SymbolTable symtab;
    symtab.open_scope ();
    symtab.enter_symbol (symbol);

    check_types (root, er, symtab);

    tap.tassert ("semantic::check_types IdentifierExpr Hidden",
                 root->eval.expression_kind == ErrorExpressionKind &&
                 last_error (er, util::Hidden_Symbol));

    delete root;
  }

  {
    Node* root = new UnaryArithmeticExpr (1, &logic_not_temp, new UnaryArithmeticExpr (1, &logic_not_temp, make_untyped_one ()));
    ErrorReporter er;
    SymbolTable symtab;
    symtab.open_scope ();
    enter_symbols (symtab);

    check_types (root, er, symtab);

    tap.tassert ("semantic::check_types UnaryArithmeticExpr subexpression error",
                 root->eval.expression_kind == ErrorExpressionKind &&
                 last_error (er, Cannot_Be_Applied));

    delete root;
  }

  {
    Node* root = new UnaryArithmeticExpr (1, &logic_not_temp, make_untyped_false ());
    ErrorReporter er;
    SymbolTable symtab;
    symtab.open_scope ();
    enter_symbols (symtab);

    check_types (root, er, symtab);

    tap.tassert ("semantic::check_types UnaryArithmeticExpr",
                 root->eval.expression_kind == ValueExpressionKind &&
                 root->eval.type == UntypedBoolean::instance () &&
                 root->eval.value.present &&
                 root->eval.value.bool_value == true &&
                 no_error (er));

    delete root;
  }

  {
    Node* root = new BinaryArithmeticExpr (1, &multiply_temp, new UnaryArithmeticExpr (1, &logic_not_temp, make_untyped_one ()), make_untyped_one ());
    ErrorReporter er;
    SymbolTable symtab;
    symtab.open_scope ();
    enter_symbols (symtab);

    check_types (root, er, symtab);

    tap.tassert ("semantic::check_types BinaryArithmeticExpr left error",
                 root->eval.expression_kind == ErrorExpressionKind &&
                 last_error (er, Cannot_Be_Applied));

    delete root;
  }

  {
    Node* root = new BinaryArithmeticExpr (1, &multiply_temp, make_untyped_one (), new UnaryArithmeticExpr (1, &logic_not_temp, make_untyped_one ()));
    ErrorReporter er;
    SymbolTable symtab;
    symtab.open_scope ();
    enter_symbols (symtab);

    check_types (root, er, symtab);

    tap.tassert ("semantic::check_types BinaryArithmeticExpr right error",
                 root->eval.expression_kind == ErrorExpressionKind &&
                 last_error (er, Cannot_Be_Applied));

    delete root;
  }

  {
    Node* root = new BinaryArithmeticExpr (1, &multiply_temp, make_untyped_one (), make_untyped_one ());
    ErrorReporter er;
    SymbolTable symtab;
    symtab.open_scope ();
    enter_symbols (symtab);

    check_types (root, er, symtab);

    tap.tassert ("semantic::check_types BinaryArithmeticExpr",
                 root->eval.expression_kind == ValueExpressionKind &&
                 root->eval.type == UntypedInteger::instance () &&
                 root->eval.value.present &&
                 root->eval.value.untyped_integer_value == 1 &&
                 no_error (er));

    delete root;
  }


  tap.print_plan ();

  return 0;
}
