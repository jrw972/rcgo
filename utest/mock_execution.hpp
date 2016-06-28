#ifndef RC_UTEST_MOCK_EXECUTION_HPP
#define RC_UTEST_MOCK_EXECUTION_HPP

#include "operation.hpp"
#include "executor_base.hpp"

struct MockOperation : public runtime::Operation
{
  MockOperation ()
    : was_executed (false)
  { }

  virtual runtime::Control execute (runtime::ExecutorBase& exec) const
  {
    const_cast<MockOperation*> (this)->was_executed = true;
    return runtime::Control_Continue;
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

#endif // RC_UTEST_MOCK_EXECUTION_HPP
