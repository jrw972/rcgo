#ifndef RC_SRC_SCHEDULER_HPP
#define RC_SRC_SCHEDULER_HPP

namespace runtime
{
struct Scheduler
{
  virtual void run (composition::Composer& instance_table,
                    size_t stack_size,
                    size_t thread_count) = 0;
};
}

#endif // RC_SRC_SCHEDULER_HPP
