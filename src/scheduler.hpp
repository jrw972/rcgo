#ifndef RC_SRC_SCHEDULER_HPP
#define RC_SRC_SCHEDULER_HPP

namespace runtime
{
struct Scheduler
{
  virtual void init (composition::Composer& instance_table,
                     size_t stack_size,
                     size_t thread_count,
                     size_t profile) = 0;

  virtual void run () = 0;

  virtual void fini (FILE* profile_out) = 0;
};
}

#endif // RC_SRC_SCHEDULER_HPP
