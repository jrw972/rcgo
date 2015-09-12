#include "instance_scheduler.hpp"
#include "runtime.hpp"
#include <stdlib.h>
#include <string.h>
#include "instance.hpp"
#include "heap.hpp"
#include "stack_frame.hpp"
#include "action.hpp"

void
instance_scheduler_t::lock (const instance_set_t& set)
{
  for (instance_set_t::const_iterator pos = set.begin (), limit = set.end ();
       pos != limit;
       ++pos)
    {
      instance_info_t* record = info_map_[pos->first];
      switch (pos->second)
        {
        case TRIGGER_READ:
          pthread_rwlock_rdlock (&record->lock);
          break;
        case TRIGGER_WRITE:
          pthread_rwlock_wrlock (&record->lock);
          break;
        }
    }
}

void
instance_scheduler_t::unlock (const instance_set_t& set)
{
  for (instance_set_t::const_iterator pos = set.begin (), limit = set.end ();
       pos != limit;
       ++pos)
    {
      instance_info_t* record = info_map_[pos->first];
      switch (pos->second)
        {
        case TRIGGER_READ:
        case TRIGGER_WRITE:
          pthread_rwlock_unlock (&record->lock);
          break;
        }
    }
}

// char*
// instance_scheduler_t::get_ptr (scheduler_info_t* info) const
// {
//   return static_cast<instance_info_t*> (info)->get_ptr ();
// }

void
instance_scheduler_t::push (instance_info_t* info)
{
  pthread_mutex_lock (&list_mutex_);
  if (info->next == 0)
    {
      // Not on the schedule.
      *tail_ = info;
      tail_ = &(info->next);
      info->next = (instance_info_t*)1;
    }
  pthread_cond_signal (&list_cond_);
  pthread_mutex_unlock (&list_mutex_);
}

void
instance_scheduler_t::dump_schedule () const
{
  if (head_ != NULL)
    {
      instance_info_t* record = head_;
      while (record != (instance_info_t*)1)
        {
          printf ("%p instance=%p type=%p\n", record, heap_instance (record->heap), record->instance->type ());
          record = record->next;
        }
    }
}

void
instance_scheduler_t::instance_executor_t::run_i ()
{
  for (;;)
    {
      pthread_mutex_lock (&scheduler_.list_mutex_);
      while (!(scheduler_.head_ != NULL ||
               (scheduler_.head_ == NULL && scheduler_.pending_ == 0)))
        {
          pthread_cond_wait (&scheduler_.list_cond_, &scheduler_.list_mutex_);
        }

      if (scheduler_.head_ == NULL)
        {
          // Implies that runtime->pending == 0.
          // Signal so other threads can exit.
          pthread_cond_signal (&scheduler_.list_cond_);
          pthread_mutex_unlock (&scheduler_.list_mutex_);
          pthread_exit (NULL);
        }

      /* printf ("BEGIN schedule before pop\n"); */
      /* dump_schedule (runtime); */
      /* printf ("END schedule before pop\n"); */

      // Get an instance to execute.
      instance_info_t* record = scheduler_.head_;
      scheduler_.head_= record->next;
      if (scheduler_.head_== (instance_info_t*)1)
        {
          scheduler_.head_= NULL;
          scheduler_.tail_= &scheduler_.head_;
        }
      record->next = NULL;

      ++scheduler_.pending_;
      pthread_mutex_unlock (&scheduler_.list_mutex_);

      /* // TODO:  Comment this out. */
      /* if (rand () % 100 < 50) */
      /*   { */
      /*     push (runtime->runtime, record); */

      /*     pthread_mutex_lock (&scheduler.list_mutex); */
      /*     --scheduler.pending; */
      /*     pthread_cond_signal (&scheduler.list_cond); */
      /*     pthread_mutex_unlock (&scheduler.list_mutex); */
      /*     continue; */
      /*   } */

      /* printf ("BEGIN schedule after pop\n"); */
      /* dump_schedule (runtime); */
      /* printf ("END schedule after pop\n"); */

      // Try all the actions.
      for (instance_t::InstanceSetsType::const_iterator pos = record->instance->instance_sets.begin (),
             limit = record->instance->instance_sets.end ();
           pos != limit;
           ++pos)
        {
          /* printf ("BEGIN instances before enabled\n"); */
          /* dump_instances (runtime); */
          /* printf ("END instances before enabled\n"); */

          const action_t* action = pos->action;

          scheduler_.lock (pos->set);
          runtime::exec (*this, record->instance->ptr (), action, pos->iota);
          scheduler_.unlock (pos->set);

          /* // TODO: Comment this out. */
          /* { */
          /*   instance_t** pos; */
          /*   instance_t** limit; */
          /*   for (pos = instance_table_begin (scheduler.instance_table), limit = instance_table_end (scheduler.instance_table); */
          /*        pos != limit; */
          /*        pos = instance_table_next (pos)) */
          /*     { */
          /*       instance_t* instance = *pos; */
          /*       printf ("BEFORE FIRST\n"); */
          /*       heap_dump (instance_get_record (instance)->heap); */
          /*       printf ("BEFORE LAST\n"); */
          /*       instance_record_collect_garbage (instance_get_record (instance)); */
          /*       printf ("AFTER FIRST\n"); */
          /*       heap_dump (instance_get_record (instance)->heap); */
          /*       printf ("AFTER LAST\n"); */
          /*     } */
          /* } */

          /* printf ("BEGIN instances after enabled\n"); */
          /* dump_instances (runtime); */
          /* printf ("END instances after enabled\n"); */
        }

      // Collect garbage.
      record->collect_garbage ();
      //heap_dump (record->heap);

      pthread_mutex_lock (&scheduler_.list_mutex_);
      --scheduler_.pending_;
      pthread_cond_signal (&scheduler_.list_cond_);
      pthread_mutex_unlock (&scheduler_.list_mutex_);
    }
}

void
instance_scheduler_t::run (instance_table_t& instance_table,
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
      // Set up the scheduling data structure.
      instance_info_t* info = new instance_info_t (instance);
      info_map_[instance] = info;
      // Add the instance to the schedule.
      push (info);
    }

  {
    // Initialize.
    instance_executor_t exec (*this, stack_size);
    for (instance_table_t::InstancesType::const_iterator pos = instance_table.instances.begin (),
           limit = instance_table.instances.end ();
         pos != limit;
         ++pos)
      {
        instance_t* instance = pos->second;
        runtime::initialize (exec, instance);
      }
  }

  std::vector<instance_executor_t*> executors;
  for (size_t idx = 0; idx != thread_count; ++idx)
    {
      instance_executor_t* exec = new instance_executor_t (*this, stack_size);
      executors.push_back (exec);
      exec->spawn ();
    }

  for (size_t idx = 0; idx != thread_count; ++idx)
    {
      instance_executor_t* exec = executors[idx];
      exec->join ();
      delete exec;
    }
}
