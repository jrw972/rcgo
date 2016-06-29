#ifndef RC_UTEST_MOCK_EXECUTION_HPP
#define RC_UTEST_MOCK_EXECUTION_HPP

#include "operation.hpp"
#include "executor_base.hpp"
#include "heap.hpp"
#include "composition.hpp"

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
    , instance (NULL, 0, &type::named_int, NULL, NULL, "instance_name")
  {
    instance.component = static_cast<runtime::component_t*> (malloc (8));
    cib = new runtime::ComponentInfoBase (&instance);
    cib->heap (new runtime::Heap (128));
    current_info (cib);
  }
  virtual void push () { }

  composition::Instance instance;
  runtime::ComponentInfoBase* cib;
};

#endif // RC_UTEST_MOCK_EXECUTION_HPP
