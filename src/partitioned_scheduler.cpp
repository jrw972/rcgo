#include "partitioned_scheduler.hpp"
#include "debug.hpp"
#include "instance.hpp"
#include "heap.hpp"
#include "runtime.hpp"
#include <list>

void
partitioned_scheduler_t::initialize_task (task_t* t, size_t thread_count)
{
  size_t pid = rand () % thread_count;
  executor_t* e = executors_[pid];
  t->executor = e;
  t->to_idle_list ();
  e->add_task ();
}

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
    executor_t exec (*this, 0, 0, stack_size, &stdout_mutex_);
    for (instance_table_t::InstancesType::const_iterator pos = instance_table.instances.begin (),
           limit = instance_table.instances.end ();
         pos != limit;
         ++pos)
      {
        instance_t* instance = pos->second;
        runtime::initialize (exec, instance);
      }
  }

  for (size_t i = 0; i != thread_count; ++i)
    {
      executors_.push_back (new executor_t (*this, i, (i + 1) % thread_count, stack_size, &stdout_mutex_));
    }

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
          initialize_task (new action_task_t (instance, *action_pos), thread_count);
        }

      initialize_task (new gc_task_t (instance), thread_count);
    }

  //dirty_count_ = thread_count;

  for (size_t i = 0; i != thread_count; ++i)
    {
      executors_[i]->spawn ();
    }

  for (size_t i = 0; i != thread_count; ++i)
    {
      executors_[i]->join ();
    }

  for (size_t i = 0; i != thread_count; ++i)
    {
      delete executors_[i];
    }

  executors_.clear ();
}

void
partitioned_scheduler_t::executor_t::run_i ()
{
  enum State
  {
    NORMAL,
    SHOOT_DOWN,
    WAIT1,
    DOUBLE_CHECK,
    WAIT2,
  };

  State state = NORMAL;
  size_t generation = 0;
  size_t points = 0;
  for (;;)
    {
      // Get a task from the ready list and/or a message.
      pthread_mutex_lock (&mutex_);
      task_t* task = ready_head_;
      if (task != NULL)
        {
          ready_head_ = task->next;
          if (ready_head_ == NULL)
            {
              ready_tail_ = &ready_head_;
            }
        }
      bool flag = false;
      Message message;
      if (!message_queue_.empty ())
        {
          flag = true;
          message = message_queue_.front ();
          message_queue_.pop ();
        }
      pthread_mutex_unlock (&mutex_);

      if (task != NULL || flag)
        {
          if (task != NULL)
            {
              task->next = NULL;
              switch (state)
                {
                case NORMAL:
                  switch (task->resume (generation))
                    {
                    case NONE:
                    case SKIP:
                    case HIT:
                      break;
                    case FIRST_SKIP:
                      ++points;
                      break;
                    case FIRST_HIT:
                      --points;
                      break;
                    }
                  break;
                case SHOOT_DOWN:
                case DOUBLE_CHECK:
                  switch (task->resume (generation))
                    {
                    case NONE:
                      break;
                    case SKIP:
                      break;
                    case FIRST_SKIP:
                      ++points;
                      break;
                    case HIT:
                    case FIRST_HIT:
                      state = NORMAL;
                      ++generation;
                      points = 0;
                      scheduler_.executors_[neighbor_id_]->send (Message::make_reset (id_));
                      break;
                    }
                  break;
                case WAIT1:
                case WAIT2:
                  bug ("illegal state");
                }
            }

          if (flag)
            {
              switch (message.kind)
                {
                case Message::START_SHOOT_DOWN:
                  if (message.id != id_ && state == NORMAL)
                    {
                      state = SHOOT_DOWN;
                      ++generation;
                      points = 0;
                      scheduler_.executors_[neighbor_id_]->send (message);
                    }
                  break;
                case Message::START_WAITING1:
                  if (state == WAIT1)
                    {
                      if (message.id != id_)
                        {
                          scheduler_.executors_[neighbor_id_]->send (message);
                        }
                      else
                        {
                          state = DOUBLE_CHECK;
                          ++generation;
                          points = 0;
                          scheduler_.executors_[neighbor_id_]->send (Message::make_start_double_check (id_));
                        }
                    }
                  break;
                case Message::START_DOUBLE_CHECK:
                  if (message.id != id_ && state == WAIT1)
                    {
                      state = DOUBLE_CHECK;
                      ++generation;
                      points = 0;
                      scheduler_.executors_[neighbor_id_]->send (message);
                    }
                  break;
                case Message::START_WAITING2:
                  if (state == WAIT2)
                    {
                      if (message.id != id_)
                        {
                          scheduler_.executors_[neighbor_id_]->send (message);
                        }
                      else
                        {
                          scheduler_.executors_[neighbor_id_]->send (Message::make_terminate ());
                          return;
                        }
                    }
                  break;
                case Message::TERMINATE:
                  assert (state == WAIT2);
                  scheduler_.executors_[neighbor_id_]->send (message);
                  return;
                case Message::RESET:
                  if (message.id != id_)
                    {
                      state = NORMAL;
                      ++generation;
                      points = 0;
                      scheduler_.executors_[neighbor_id_]->send (message);
                    }
                  break;
                }
            }
          continue;
        }

      if (points == task_count_)
        {
          switch (state)
            {
            case NORMAL:
              state = SHOOT_DOWN;
              ++generation;
              points = 0;
              scheduler_.executors_[neighbor_id_]->send (Message::make_start_shoot_down (id_));
              break;
            case SHOOT_DOWN:
              state = WAIT1;
              scheduler_.executors_[neighbor_id_]->send (Message::make_start_waiting1 (id_));
              break;
            case WAIT1:
              sleep ();
              break;
            case DOUBLE_CHECK:
              state = WAIT2;
              scheduler_.executors_[neighbor_id_]->send (Message::make_start_waiting2 (id_));
              break;
            case WAIT2:
              sleep ();
              break;
            }

          continue;
        }

      // Execute a task on the idle list.
      task = idle_head_;

      if (task != NULL)
        {
          idle_head_ = task->next;
          task->next = NULL;
          if (idle_head_ == NULL)
            {
              idle_tail_ = &idle_head_;
            }
          switch (state)
            {
            case NORMAL:
              switch (task->execute (generation))
                {
                case NONE:
                case SKIP:
                case HIT:
                  break;
                case FIRST_SKIP:
                  ++points;
                  break;
                case FIRST_HIT:
                  --points;
                  break;
                }
              break;
            case SHOOT_DOWN:
            case DOUBLE_CHECK:
              switch (task->execute (generation))
                {
                case NONE:
                  break;
                case SKIP:
                  break;
                case FIRST_SKIP:
                  ++points;
                  break;
                case HIT:
                case FIRST_HIT:
                  state = NORMAL;
                  ++generation;
                  points = 0;
                  scheduler_.executors_[neighbor_id_]->send (Message::make_reset (id_));
                  break;
                }
              break;
            case WAIT1:
            case WAIT2:
              bug ("illegal state");
            }
          continue;
        }

      // Sleep until something is put on the queue.
      sleep ();
    }

  // mode_ = DIRTY;

  // size_t local_opportunities = 0;
  // size_t previous_neighbor_opportunities = 0;

  // for (;;)
  //   {
  //     // Grab a migration task.
  //     task_t* task = __sync_lock_test_and_set (&task_, NULL);
  //     if (task != NULL)
  //       {
  //         unimplemented;
  //         //tasks_.push_back (task);
  //       }

  //     // Reset the dirty flag for this cycle.
  //     dirty_flag_ = false;

    //   // TODO:  Pick tasks at random?
    //   for (ListType::const_iterator pos = tasks_.begin (), limit = tasks_.end ();
    //        pos != limit;
    //        ++pos)
    //     {
    //       task_t* task = *pos;

    //       // Lock.
    //       for (instance_set_t::const_iterator pos = task->set ().begin (),
    //              limit = task->set ().end ();
    //            pos != limit;
    //            ++pos)
    //         {
    //           instance_t* instance = pos->first;
    //           info_t* info = *reinterpret_cast<info_t**> (instance->ptr ());
    //           assert (info->instance == instance);
    //           switch (pos->second)
    //             {
    //             case TRIGGER_READ:
    //               std::cout << "Lock " << info << " READ" << std::endl;
    //               break;
    //             case TRIGGER_WRITE:
    //               std::cout << "Lock " << info << " WRITE" << std::endl;
    //               break;
    //             }
    //         }


    //       unimplemented;

    //       // Lock.
    //       if (task->processor_list ().size () != 1)
    //         {
    //           for (task_t::ProcessorListType::const_iterator pos = task->processor_list ().begin (),
    //                  limit = task->processor_list ().end ();
    //                pos != limit;
    //                ++pos)
    //             {
    //               processor_id_t pid = *pos;
    //               uint64_t mask;
    //               if (pid != id_)
    //                 {
    //                   mask = (1 << pid);
    //                 }
    //               else
    //                 {
    //                   mask = task->mask ();
    //                 }


    //               for (;;)
    //                 {
    //                   uint64_t oldval;
    //                   // Spin until all our bits are clear.
    //                   while ((oldval = scheduler_.executors_[pid]->mask_) & mask) ;
    //                   // Set the bits.
    //                   uint64_t newval = oldval | mask;
    //                   if (__sync_bool_compare_and_swap (&scheduler_.executors_[pid]->mask_, oldval, newval))
    //                     {
    //                       break;
    //                     }
    //                 }
    //             }
    //         }

    //       if (task->execute (*this))
    //         {
    //           add_execution ();
    //         }

    //       // Unlock.
    //       if (task->processor_list ().size () != 1)
    //         {
    //           for (task_t::ProcessorListType::const_iterator pos = task->processor_list ().begin (),
    //                  limit = task->processor_list ().end ();
    //                pos != limit;
    //                ++pos)
    //             {
    //               processor_id_t pid = *pos;
    //               uint64_t mask;
    //               if (pid != id_)
    //                 {
    //                   mask = (1 << pid);
    //                 }
    //               else
    //                 {
    //                   mask = task->mask ();
    //                 }

    //               uint64_t v = __sync_and_and_fetch (&scheduler_.executors_[pid]->mask_, ~mask);
    //               assert ((v & mask) == 0);
    //             }
    //         }

    //       // Update the number of opportunities.
    //       add_opportunity ();
    //       ++local_opportunities;

    //       // TODO: Magic numbers.
    //       const size_t sample_size = 1000;
    //       const size_t migration_threshold = sample_size; // * 1618 / 1000;

    //       if (local_opportunities == sample_size)
    //         {
    //           std::cout << "ID " << (int)id_ << ' ' << opportunities_ << ' ' << executions_ << '\n';

    //           size_t current_neighbor_opportunities = scheduler_.executors_[neighbor_id_]->get_opportunities ();

    //           if (previous_neighbor_opportunities < current_neighbor_opportunities)
    //             {
    //               size_t diff = current_neighbor_opportunities - previous_neighbor_opportunities;
    //               if (diff > migration_threshold)
    //                 {
    //                   task_t* task = get_task_to_migrate ();
    //                   if (task != NULL)
    //                     {
    //                       if (!__sync_bool_compare_and_swap (&scheduler_.executors_[neighbor_id_]->task_, NULL, task))
    //                         {
    //                           // Neighbor was not ready.
    //                           tasks_.push_back (task);
    //                         }
    //                     }
    //                 }
    //             }

    //           local_opportunities = 0;
    //           previous_neighbor_opportunities = current_neighbor_opportunities;
    //         }
    //     }

    //   switch (mode_)
    //     {
    //     case DIRTY:
    //       if (dirty_flag_)
    //         {
    //           // Do nothing.
    //         }
    //       else
    //         {
    //           // Last run was clean.
    //           mode_ = CLEAN;
    //           __sync_sub_and_fetch (&scheduler_.dirty_count_, 1);
    //         }
    //       break;
    //     case CLEAN:
    //       if (dirty_flag_)
    //         {
    //           // Last run was dirty.
    //           mode_ = DIRTY;
    //           __sync_add_and_fetch (&scheduler_.dirty_count_, 1);
    //         }
    //       else
    //         {
    //           if (__sync_add_and_fetch (&scheduler_.dirty_count_, 0) == 0)
    //             {
    //               return;
    //             }
    //         }
    //       break;
    //     }
    // }
}

partitioned_scheduler_t::task_t*
partitioned_scheduler_t::executor_t::get_task_to_migrate ()
{
  // TODO
  unimplemented;
  // // TODO:  Make a better decision.
  // partitioned_scheduler_t::task_t* task = NULL;
  // size_t task_index = 0;
  // size_t set_size = 1;

  // for (size_t idx = 0, limit = tasks_.size ();
  //      idx != limit;
  //      ++idx)
  //   {
  //     size_t s = tasks_[idx]->set ().size ();
  //     if (s > set_size)
  //       {
  //         task = tasks_[idx];
  //         task_index = idx;
  //         set_size = s;
  //       }
  //   }

  // if (task != NULL)
  //   {
  //     std::swap (tasks_[task_index], tasks_.back ());
  //     tasks_.pop_back ();
  //     scheduler_.lock_sets ();
  //     task->change_pid (neighbor_id_);
  //     scheduler_.unlock_sets ();
  //     return task;
  //   }
  // else
  //   {
  //     return NULL;
  //   }
}

partitioned_scheduler_t::ExecutionResult
partitioned_scheduler_t::task_t::execute (size_t generation)
{
  // Lock.
  pos_ = set ().begin ();
  limit_ = set ().end ();
  return resume (generation);
}

partitioned_scheduler_t::ExecutionResult
partitioned_scheduler_t::task_t::resume (size_t generation)
{
  for (; pos_ != limit_; ++pos_)
    {
      info_t* info = *reinterpret_cast<info_t**> (pos_->first->ptr ());
      switch (pos_->second)
        {
        case TRIGGER_READ:
          if (info->read_lock (this))
            {
              ++pos_;
              return NONE;
            }
          break;
        case TRIGGER_WRITE:
          if (info->write_lock (this))
            {
              ++pos_;
              return NONE;
            }
          break;
        }
    }

  if (generation != generation_)
    {
      generation_ = generation;
      last_execution_kind_ = HIT;
    }

  // Got all the locks.  Execute.
  ExecutionResult er;
  bool hit = execute_i ();
  switch (last_execution_kind_)
    {
    case HIT:
      if (hit)
        {
          last_execution_kind_ = HIT;
          er = partitioned_scheduler_t::HIT;
        }
      else
        {
          last_execution_kind_ = SKIP;
          er = partitioned_scheduler_t::FIRST_SKIP;
        }
      break;
    case SKIP:
      if (hit)
        {
          last_execution_kind_ = HIT;
          er = partitioned_scheduler_t::FIRST_HIT;
        }
      else
        {
          last_execution_kind_ = SKIP;
          er = partitioned_scheduler_t::SKIP;
        }
      break;
    }

  // Unlock.
  pos_ = set ().begin ();
  limit_ = set ().end ();
  for (; pos_ != limit_; ++pos_)
    {
      info_t* info = *reinterpret_cast<info_t**> (pos_->first->ptr ());
      switch (pos_->second)
        {
        case TRIGGER_READ:
          info->read_unlock ();
          break;
        case TRIGGER_WRITE:
          info->write_unlock ();
          break;
        }
    }

  // Put back on the ready list.
  to_idle_list ();

  return er;
}
