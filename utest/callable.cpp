#include "tap.hpp"
#include "callable.hpp"
#include "node.hpp"
#include "mock_execution.hpp"
#include "visitor_helper.hpp"
#include "symbol_visitor.hpp"

using namespace decl;

int
main (int argc, char** argv)
{
  Tap tap;

  util::Location loc;

  {
    type::Function functype (new ParameterList (loc), new ParameterList (loc));
    Function func ("name", loc, &functype);
    tap.tassert ("decl::Function::Function",
                 func.name == "name"
                 && func.location == loc
                 && func.type == &functype
                 && func.operation == NULL);
    tap.tassert ("decl::Function::parameter_list ()",
                 func.parameter_list () == functype.parameter_list);
    tap.tassert ("decl::Function::return_parameter_list ()",
                 func.return_parameter_list () == functype.return_parameter_list);
    tap.tassert ("decl::Function::callable_type ()",
                 func.callable_type () == &functype);
    // Just to exercise the code.
    func.check (new ast::ListExpr (1));
    ReceiverAccess ra;
    bool flag;
    func.compute_receiver_access (semantic::ExpressionValueList (), ra, flag);

    MockExecutor exec;
    MockOperation op;
    func.operation = &op;
    func.call (exec);
    tap.tassert ("decl::Function::call ()", op.was_executed);

    TestVisitor<SymbolVisitor, FunctionBase> v;
    func.accept (v);
    tap.tassert ("Function::accept(SymbolVisitor)", v.item == &func);
    TestConstVisitor<ConstSymbolVisitor, FunctionBase> cv;
    func.accept (cv);
    tap.tassert ("Function::accept(ConstSymbolVisitor)", cv.item == &func);

  }

  {
    type::NamedType x ("mytype", loc, &type::named_int);
    type::Method methodtype (&x, Parameter::make_receiver (loc, "this", &type::named_int, Immutable, Immutable), new ParameterList (loc), new ParameterList (loc));
    Method meth ("name", &methodtype);
    tap.tassert ("decl::Method::Function",
                 meth.name == "name"
                 && meth.type == &methodtype
                 && meth.operation == NULL);
    tap.tassert ("decl::Method::parameter_list ()",
                 meth.parameter_list () == methodtype.function_type->parameter_list);
    tap.tassert ("decl::Method::return_parameter_list ()",
                 meth.return_parameter_list () == methodtype.return_parameter_list);
    tap.tassert ("decl::Method::callable_type ()",
                 meth.callable_type () == &methodtype);
    // Just to exercise the code.
    meth.check (new ast::ListExpr (1));
    ReceiverAccess ra;
    bool flag;
    meth.compute_receiver_access (semantic::ExpressionValueList (), ra, flag);

    MockExecutor exec;
    MockOperation op;
    meth.operation = &op;
    meth.call (exec);
    tap.tassert ("decl::Method::call ()", op.was_executed);
  }

  {
    type::NamedType x ("mytype", loc, &type::named_int);
    type::Initializer initializertype (&x, Parameter::make_receiver (loc, "this", &type::named_int, Immutable, Immutable), new ParameterList (loc), new ParameterList (loc));
    Initializer initializer ("name", &initializertype);
    tap.tassert ("decl::Initializer::Function",
                 initializer.name == "name"
                 && initializer.type == &initializertype
                 && initializer.operation == NULL);
    tap.tassert ("decl::Initializer::parameter_list ()",
                 initializer.parameter_list () == initializertype.function_type->parameter_list);
    tap.tassert ("decl::Initializer::return_parameter_list ()",
                 initializer.return_parameter_list () == initializertype.return_parameter_list);
    tap.tassert ("decl::Initializer::callable_type ()",
                 initializer.callable_type () == &initializertype);
    // Just to exercise the code.
    initializer.check (new ast::ListExpr (1));
    ReceiverAccess ra;
    bool flag;
    initializer.compute_receiver_access (semantic::ExpressionValueList (), ra, flag);

    MockExecutor exec;
    MockOperation op;
    initializer.operation = &op;
    initializer.call (exec);
    tap.tassert ("decl::Initializer::call ()", op.was_executed);
  }

  {
    ast::Getter node (1, NULL, NULL, NULL, NULL, NULL);
    type::NamedType x ("mytype", loc, &type::named_int);
    type::Getter gettertype (&x, Parameter::make_receiver (loc, "this", &type::named_int, Immutable, Immutable), new ParameterList (loc), new ParameterList (loc));
    Getter getter (&node, "name", &gettertype);
    tap.tassert ("decl::Getter::Function",
                 getter.name == "name"
                 && getter.type == &gettertype
                 && getter.node == &node
                 && getter.operation == NULL);
    tap.tassert ("decl::Getter::parameter_list ()",
                 getter.parameter_list () == gettertype.function_type->parameter_list);
    tap.tassert ("decl::Getter::return_parameter_list ()",
                 getter.return_parameter_list () == gettertype.return_parameter_list);
    tap.tassert ("decl::Getter::callable_type ()",
                 getter.callable_type () == &gettertype);
    // Just to exercise the code.
    getter.check (new ast::ListExpr (1));
    ReceiverAccess ra;
    bool flag;
    getter.compute_receiver_access (semantic::ExpressionValueList (), ra, flag);

    MockExecutor exec;
    MockOperation op;
    getter.operation = &op;
    getter.call (exec);
    tap.tassert ("decl::Getter::call ()", op.was_executed);
  }

  {
    util::Location loc;
    Parameter* receiver = Parameter::make (loc, "there", &type::named_int, Immutable, Immutable);
    ast::Node* body = new ast::ListStatement (1);
    const std::string name ("myreaction");
    ParameterList* parameter_list = new ParameterList (loc);
    const type::Reaction* reaction_type = new type::Reaction (NULL, receiver, parameter_list, new ParameterList (loc));

    decl::Reaction r (body, name, reaction_type);
    tap.tassert ("Reaction::Reaction ()",
                 r.operation == NULL
                 && r.body == body
                 && r.name == name
                 && r.iota == NULL
                 && r.type
                 && r.has_dimension () == false
                 && r.callable_type () == reaction_type
                 && r.parameter_list () == reaction_type->function_type->parameter_list
                 && r.return_parameter_list () == reaction_type->function_type->return_parameter_list
                );
  }

  {
    util::Location loc;
    Parameter* receiver = Parameter::make (loc, "there", &type::named_int, Immutable, Immutable);
    Parameter* iota = Parameter::make (loc, "there", &type::named_int, Immutable, Immutable);
    ast::Node* body = new ast::ListStatement (1);
    const std::string name ("myreaction");
    const type::Reaction* reaction_type = new type::Reaction (NULL, receiver, new ParameterList (loc), new ParameterList (loc));

    decl::Reaction r (body, name, reaction_type, iota, 5);
    tap.tassert ("Reaction::Reaction (dimensioned)",
                 r.operation == NULL &&
                 r.body == body &&
                 r.name == name &&
                 r.iota == iota &&
                 r.dimension == 5 &&
                 r.type &&
                 r.has_dimension () == true);
  }

  {
    MockOperation op;
    MockExecutor exec;
    decl::Reaction r (NULL, "r", NULL);
    r.operation = &op;
    r.call (exec);
    tap.tassert ("Reaction::call", op.was_executed);
  }

  tap.print_plan ();

  return 0;
}
