#include "partitioned_scheduler.hpp"
#include "debug.hpp"
#include "instance.hpp"
#include "heap.hpp"
#include "runtime.hpp"
#include <list>
#include <errno.h>

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
          instance_t::ConcreteAction action = *action_pos;
          switch (action.action->precondition_kind)
            {
            case action_t::DYNAMIC:
              initialize_task (new action_task_t (instance, action), thread_count);
              break;
            case action_t::STATIC_TRUE:
              initialize_task (new always_task_t (instance, action), thread_count);
              break;
            case action_t::STATIC_FALSE:
              // Do nothing.
              break;
            }
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
    POLL,
  };

  State state = NORMAL;
  size_t generation = 0;
  size_t points = 0;
  for (;;)
    {
      // Get a task from the ready list and/or a message.
      task_t* task = NULL;
      Message message;
      bool flag = get_ready_task_and_message (task, message);

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
                      disableFileDescriptorTracking ();
                      ++generation;
                      points = 0;
                      send (Message::make_reset (id_));
                      break;
                    }
                  break;
                case WAIT1:
                case WAIT2:
                  // Straggler.
                  task->resume (generation);
                  break;
                case POLL:
                  not_reached;
                  break;
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
                      send (message);
                    }
                  break;
                case Message::START_WAITING1:
                  if (state == WAIT1)
                    {
                      if (message.id != id_)
                        {
                          send (message);
                        }
                      else
                        {
                          state = DOUBLE_CHECK;
                          enableFileDescriptorTracking ();
                          ++generation;
                          points = 0;
                          send (Message::make_start_double_check (id_));
                        }
                    }
                  break;
                case Message::START_DOUBLE_CHECK:
                  if (message.id != id_ && state == WAIT1)
                    {
                      state = DOUBLE_CHECK;
                      enableFileDescriptorTracking ();
                      ++generation;
                      points = 0;
                      send (message);
                    }
                  break;
                case Message::START_WAITING2:
                  if (state == WAIT2)
                    {
                      if (message.id != id_)
                        {
                          send (message);
                        }
                      else
                        {
                          send (Message::make_terminate ());
                          return;
                        }
                    }
                  break;
                case Message::TERMINATE:
                  assert (state == WAIT2);
                  send (message);
                  return;
                case Message::RESET:
                  if (message.id != id_)
                    {
                      state = NORMAL;
                      disableFileDescriptorTracking ();
                      ++generation;
                      points = 0;
                      send (message);
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
              send (Message::make_start_shoot_down (id_));
              break;
            case SHOOT_DOWN:
              state = WAIT1;
              send (Message::make_start_waiting1 (id_));
              break;
            case WAIT1:
              sleep ();
              break;
            case DOUBLE_CHECK:
              if (fileDescriptorMap_.empty ()) {
                state = WAIT2;
                send (Message::make_start_waiting2 (id_));
              } else {
                state = POLL;
                pthread_mutex_lock (&mutex_);
                using_eventfd_ = true;
                pthread_mutex_unlock (&mutex_);
              }
              break;
            case WAIT2:
              sleep ();
              break;
            case POLL:
              if (poll ()) {
                state = NORMAL;
                pthread_mutex_lock (&mutex_);
                using_eventfd_ = false;
                pthread_mutex_unlock (&mutex_);
                disableFileDescriptorTracking ();
                ++generation;
                points = 0;
                // Wakeup everyone.
                // This is overkill and could be improved.
                send (Message::make_reset (id_));
              }
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
                  disableFileDescriptorTracking ();
                  ++generation;
                  points = 0;
                  send (Message::make_reset (id_));
                  break;
                }
              break;
            case WAIT1:
            case WAIT2:
            case POLL:
              not_reached;
              break;
            }
          continue;
        }

      // Sleep until something is put on the queues.
      // TODO:  Steal work from another executor.
      sleep ();
    }
}

bool
partitioned_scheduler_t::executor_t::poll ()
{
  // Generate a list of fds to poll.
  struct pollfd pfd;
  std::vector<struct pollfd> pfds;
  // Add the event fd so other executors can wake this executor.
  pfd.fd = eventfd_;
  pfd.events = POLLIN;
  pfds.push_back (pfd);

  for (FileDescriptorMapType::const_iterator pos = fileDescriptorMap_.begin (),
         limit = fileDescriptorMap_.end ();
       pos != limit;
       ++pos) {
    pfd.fd = pos->first->fd ();
    pfd.events = pos->second;
    pfds.push_back (pfd);
  }

  int r = ::poll (&pfds[0], pfds.size (), -1);

  if (r < 1) {
    error (EXIT_FAILURE, errno, "poll");
  } else if (r > 1) {
    // At least one fd besides the eventfd is ready.
    return true;
  } else {
    // r == 1
    if (pfds[0].revents == 0) {
      // No events on eventfd so must be some other fd.
      return true;
    }
  }

  return false;
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
        case ACTIVATION_READ:
          if (info->read_lock (this))
            {
              ++pos_;
              return NONE;
            }
          break;
        case ACTIVATION_WRITE:
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
        case ACTIVATION_READ:
          info->read_unlock ();
          break;
        case ACTIVATION_WRITE:
          info->write_unlock ();
          break;
        }
    }

  // Put back on the ready list.
  to_idle_list ();

  return er;
}
