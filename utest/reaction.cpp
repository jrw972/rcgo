#include "type.hpp"

#include "tap.hpp"
#include "reaction.hpp"
#include "runtime.hpp"
#include "node.hpp"
#include "parameter_list.hpp"

using namespace decl;
using namespace type;

struct MockOperation : public runtime::Operation
{
  MockOperation ()
    : was_executed (false)
  { }

  virtual runtime::OpReturn execute (runtime::ExecutorBase& exec) const
  {
    const_cast<MockOperation*> (this)->was_executed = true;
    return runtime::kContinue;
  }

  virtual void dump () const { }

  bool was_executed;
};

struct MockExecutor : public runtime::ExecutorBase
{
  MockExecutor ()
    : ExecutorBase (0, NULL, 0)
  { }
  virtual runtime::Heap* heap () const
  {
    return NULL;
  }
  virtual void heap (runtime::Heap* heap) { }
  virtual void push () { }
};

int
main (int argc, char** argv)
{
  Tap tap;

  {
    util::Location loc;
    ParameterSymbol* receiver = ParameterSymbol::make (loc, "there", &named_int, Immutable, Immutable);
    ast::Node* body = new ast::ListStatement (1);
    const std::string name ("myreaction");
    ParameterList* parameter_list = new ParameterList (loc);
    const type::Method* reaction_type = new type::Method (type::Method::METHOD, NULL, receiver, parameter_list, new ParameterList (loc));

    Reaction r (body, name, reaction_type);
    tap.tassert ("Reaction::Reaction ()",
                 r.operation == NULL
                 && r.body == body
                 && r.name == name
                 && r.iota == NULL
                 && r.reaction_type
                 && r.has_dimension () == false
                 && r.callable_type () == reaction_type
                 && r.return_size_on_stack () == 0
                 && r.receiver_size_on_stack () == static_cast<size_t> (util::align_up (receiver->type->size (), arch::stack_alignment ()))
                 && r.parameters_size_on_stack () == 0
                 && r.parameter_list () == parameter_list
                );
  }

  {
    util::Location loc;
    ParameterSymbol* receiver = ParameterSymbol::make (loc, "there", &named_int, Immutable, Immutable);
    ParameterSymbol* iota = ParameterSymbol::make (loc, "there", &named_int, Immutable, Immutable);
    ast::Node* body = new ast::ListStatement (1);
    const std::string name ("myreaction");
    const type::Method* reaction_type = new type::Method (type::Method::METHOD, NULL, receiver, new ParameterList (loc), new ParameterList (loc));

    Reaction r (body, name, reaction_type, iota, 5);
    tap.tassert ("Reaction::Reaction (dimensioned)",
                 r.operation == NULL &&
                 r.body == body &&
                 r.name == name &&
                 r.iota == iota &&
                 r.dimension == 5 &&
                 r.reaction_type &&
                 r.has_dimension () == true);
  }

  {
    MockOperation op;
    MockExecutor exec;
    Reaction r (NULL, "r", NULL);
    r.operation = &op;
    r.call (exec);
    tap.tassert ("Reaction::call", op.was_executed);
  }

  tap.print_plan ();

  return 0;
}
