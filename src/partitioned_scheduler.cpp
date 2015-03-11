#include "partitioned_scheduler.hpp"
#include "debug.hpp"
#include "instance.hpp"
#include "heap.hpp"
#include "runtime.hpp"
#include <list>

void
partitioned_scheduler_t::run (instance_table_t& instance_table,
                              size_t stack_size,
                              size_t thread_count)
{
  // Set up data structures.
  for (instance_table_t::InstancesType::const_iterator pos = instance_table.instances.begin (),
         limit = instance_table.instances.end ();
       pos != limit;
       ++pos)
    {
      instance_t* instance = pos->second;
      new info_t (instance);
    }

  {
    // Initialize.
    executor_t exec (*this, 0, stack_size, &stdout_mutex_);
    for (instance_table_t::InstancesType::const_iterator pos = instance_table.instances.begin (),
           limit = instance_table.instances.end ();
         pos != limit;
         ++pos)
      {
        instance_t* instance = pos->second;
        runtime::initialize (exec, instance);
      }
  }

  for (processor_id_t i = 0; i != thread_count; ++i)
    {
      executors_.push_back (new executor_t (*this, i, stack_size, &stdout_mutex_));
    }

  {
    // Create tasks.
    for (instance_table_t::InstancesType::const_iterator instance_pos = instance_table.instances.begin (),
           instance_limit = instance_table.instances.end ();
         instance_pos != instance_limit;
         ++instance_pos)
      {
        instance_t* instance = instance_pos->second;

        for (instance_t::InstanceSetsType::const_iterator action_pos = instance_pos->second->instance_sets.begin (),
               action_limit = instance_pos->second->instance_sets.end ();
             action_pos != action_limit;
             ++action_pos)
          {
            executors_[rand () % thread_count]->add (instance, *action_pos);
          }

        executors_[rand () % thread_count]->add (instance);
      }
  }

  dirty_count_ = thread_count;

  // Compute the masks.
  for (processor_id_t i = 0; i != thread_count; ++i)
    {
      executors_[i]->compute_masks ();
    }

  for (processor_id_t i = 0; i != thread_count; ++i)
    {
      executors_[i]->spawn ();
    }

  for (processor_id_t i = 0; i != thread_count; ++i)
    {
      executors_[i]->join ();
    }

  for (processor_id_t i = 0; i != thread_count; ++i)
    {
      delete executors_[i];
    }

  executors_.clear ();
}

void
partitioned_scheduler_t::executor_t::run_i ()
{
  mode_ = DIRTY;

  for (;;)
    {
      // Reset the dirty flag for this cycle.
      dirty_flag_ = false;

      // TODO:  Pick tasks at random?
      for (ListType::const_iterator pos = tasks_.begin (), limit = tasks_.end ();
           pos != limit;
           ++pos)
        {
          task_t* task = *pos;
          if (task->processor_list.size () != 1)
            {
              // Lock.
              for (task_t::ProcessorListType::const_iterator pos = task->processor_list.begin (),
                     limit = task->processor_list.end ();
                   pos != limit;
                   ++pos)
                {
                  processor_id_t pid = *pos;
                  uint64_t mask;
                  if (pid != id_)
                    {
                      mask = (1 << pid);
                    }
                  else
                    {
                      mask = task->mask;
                    }

                  for (;;)
                    {
                      uint64_t oldval = scheduler_.executors_[pid]->mask_;
                      if ((oldval & mask) == 0)
                        {
                          uint64_t newval = oldval ^ mask;
                          if (__sync_bool_compare_and_swap (&scheduler_.executors_[pid]->mask_, oldval, newval))
                            {
                              break;
                            }
                        }
                    }
                }
            }

          task->execute (*this);

          // Unlock.
          if (task->processor_list.size () != 1)
            {
              // Lock.
              for (task_t::ProcessorListType::const_iterator pos = task->processor_list.begin (),
                     limit = task->processor_list.end ();
                   pos != limit;
                   ++pos)
                {
                  processor_id_t pid = *pos;
                  uint64_t mask;
                  if (pid != id_)
                    {
                      mask = (1 << pid);
                    }
                  else
                    {
                      mask = task->mask;
                    }

                  __sync_xor_and_fetch (&scheduler_.executors_[pid]->mask_, mask);
                }
            }
        }

      switch (mode_)
        {
        case DIRTY:
          if (dirty_flag_)
            {
              // Do nothing.
            }
          else
            {
              // Last run was clean.
              mode_ = CLEAN;
              __sync_sub_and_fetch (&scheduler_.dirty_count_, 1);
            }
          break;
        case CLEAN:
          if (dirty_flag_)
            {
              // Last run was dirty.
              mode_ = DIRTY;
              __sync_add_and_fetch (&scheduler_.dirty_count_, 1);
            }
          else
            {
              if (__sync_add_and_fetch (&scheduler_.dirty_count_, 0) == 0)
                {
                  return;
                }
            }
          break;
        }
    }
}
