#include "check_types.hpp"

#include "tap.hpp"
#include "node.hpp"
#include "error_reporter.hpp"
#include "scope.hpp"
#include "semantic.hpp"
#include "builtin_function.hpp"
#include "enter_predeclared_identifiers.hpp"

using namespace semantic;
using namespace ast;
using namespace util;
using namespace decl;
using namespace type;

static IdentifierExpression* make_identifier_expr (const std::string& id)
{
  return new IdentifierExpression (1, new Identifier (1, id));
}

static Literal* make_untyped_one ()
{
  semantic::Value v;
  v.present = true;
  v.untyped_integer_value = 1;
  return new Literal (1, UntypedInteger::instance (), v);
}

static Literal* make_untyped_false ()
{
  semantic::Value v;
  v.present = true;
  v.untyped_boolean_value = false;
  return new Literal (1, UntypedBoolean::instance (), v);
}

// static Literal* make_typed_false ()
// {
//   semantic::Value v;
//   v.present = true;
//   v.bool_value = false;
//   return new Literal (1, &named_bool, v);
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
    Scope scope;

    check_types (root, er, &scope);

    tap.tassert ("semantic::check_types Literal",
                 root->eval.expression_kind == ValueExpressionKind &&
                 root->eval.intrinsic_mutability == Immutable &&
                 root->eval.indirection_mutability == Immutable &&
                 no_error (er));
    delete root;
  }

  {
    Node* child1 = make_untyped_one ();
    Node* child2 = make_untyped_one ();
    Node* root = (new ExpressionList (1))->append (child1)->append (child2);
    ErrorReporter er;
    Scope scope;

    check_types (root, er, &scope);

    tap.tassert ("semantic::check_types ExpressionList",
                 child2->eval.expression_kind == ValueExpressionKind &&
                 child2->eval.intrinsic_mutability == Immutable &&
                 child2->eval.indirection_mutability == Immutable &&
                 no_error (er));
    delete root;
  }

  {
    Node* root = make_identifier_expr ("int64");
    ErrorReporter er;
    Scope scope;

    check_types (root, er, &scope);

    tap.tassert ("semantic::check_types IdentifierExpression undefined",
                 root->eval.expression_kind == ErrorExpressionKind &&
                 last_error (er, Undefined));

    delete root;
  }

  {
    Node* root = make_identifier_expr ("readable");
    ErrorReporter er;
    runtime::BuiltinFunction* symbol = new runtime::Readable (Location (1));
    Scope scope;
    scope.enter_symbol (symbol);

    check_types (root, er, &scope);

    tap.tassert ("semantic::check_types IdentifierExpression BuiltinFunction",
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
    Scope scope;
    scope.enter_symbol (symbol);

    check_types (root, er, &scope);

    tap.tassert ("semantic::check_types IdentifierExpression Template",
                 root->eval.expression_kind == ValueExpressionKind &&
                 root->eval.type == type::PolymorphicFunction::instance () &&
                 root->polymorphic_function == symbol &&
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
    Scope scope;
    scope.enter_symbol (symbol);

    check_types (root, er, &scope);

    tap.tassert ("semantic::check_types IdentifierExpression Function",
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
    Scope scope;
    scope.enter_symbol (symbol);

    check_types (root, er, &scope);

    tap.tassert ("semantic::check_types IdentifierExpression Parameter",
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
    Scope scope;
    scope.enter_symbol (&named_bool);

    check_types (root, er, &scope);

    tap.tassert ("semantic::check_types IdentifierExpression Type",
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
    Scope scope;
    scope.enter_symbol (symbol);

    check_types (root, er, &scope);

    tap.tassert ("semantic::check_types IdentifierExpression Constant",
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
    Scope scope;
    scope.enter_symbol (symbol);

    check_types (root, er, &scope);

    tap.tassert ("semantic::check_types IdentifierExpression Variable",
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
    Scope scope;
    scope.enter_symbol (symbol);

    check_types (root, er, &scope);

    tap.tassert ("semantic::check_types IdentifierExpression Hidden",
                 root->eval.expression_kind == ErrorExpressionKind &&
                 last_error (er, util::Hidden_Symbol));

    delete root;
  }

  {
    Node* root = new ast::UnaryArithmetic (1, &logic_not_temp, new ast::UnaryArithmetic (1, &logic_not_temp, make_untyped_one ()));
    ErrorReporter er;
    Scope scope;
    enter_predeclared_identifiers (&scope);

    check_types (root, er, &scope);

    tap.tassert ("semantic::check_types UnaryArithmetic subexpression error",
                 root->eval.expression_kind == ErrorExpressionKind &&
                 last_error (er, Cannot_Be_Applied));

    delete root;
  }

  {
    Node* root = new ast::UnaryArithmetic (1, &logic_not_temp, make_untyped_false ());
    ErrorReporter er;
    Scope scope;
    enter_predeclared_identifiers (&scope);

    check_types (root, er, &scope);

    tap.tassert ("semantic::check_types UnaryArithmetic",
                 root->eval.expression_kind == ValueExpressionKind &&
                 root->eval.type == UntypedBoolean::instance () &&
                 root->eval.value.present &&
                 root->eval.value.bool_value == true &&
                 no_error (er));

    delete root;
  }

  {
    Node* root = new ast::BinaryArithmetic (1, &multiply_temp, new ast::UnaryArithmetic (1, &logic_not_temp, make_untyped_one ()), make_untyped_one ());
    ErrorReporter er;
    Scope scope;
    enter_predeclared_identifiers (&scope);

    check_types (root, er, &scope);

    tap.tassert ("semantic::check_types BinaryArithmeticExpr left error",
                 root->eval.expression_kind == ErrorExpressionKind &&
                 last_error (er, Cannot_Be_Applied));

    delete root;
  }

  {
    Node* root = new ast::BinaryArithmetic (1, &multiply_temp, make_untyped_one (), new ast::UnaryArithmetic (1, &logic_not_temp, make_untyped_one ()));
    ErrorReporter er;
    Scope scope;
    enter_predeclared_identifiers (&scope);

    check_types (root, er, &scope);

    tap.tassert ("semantic::check_types BinaryArithmeticExpr right error",
                 root->eval.expression_kind == ErrorExpressionKind &&
                 last_error (er, Cannot_Be_Applied));

    delete root;
  }

  {
    Node* root = new ast::BinaryArithmetic (1, &multiply_temp, make_untyped_one (), make_untyped_one ());
    ErrorReporter er;
    Scope scope;
    enter_predeclared_identifiers (&scope);

    check_types (root, er, &scope);

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
