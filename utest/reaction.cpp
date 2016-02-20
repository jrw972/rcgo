#include "type.hpp"

#include "tap.hpp"
#include "reaction.hpp"
#include "runtime.hpp"

using namespace decl;

struct MockOperation : public runtime::Operation
{
  MockOperation ()
    : was_executed (false)
  { }

  virtual runtime::OpReturn execute (runtime::ExecutorBase& exec) const
  {
    const_cast<MockOperation*> (this)->was_executed = true;
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
    MockOperation op;
    MockExecutor exec;
    Reaction r (NULL, NULL, "r", NULL);
    r.operation = &op;
    r.call (exec);
    tap.tassert ("Reaction::call", op.was_executed);
  }

  tap.print_plan ();

  return 0;
}
