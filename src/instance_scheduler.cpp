#include "instance_scheduler.hpp"

#include <cstdlib>
#include <cstring>

#include "runtime.hpp"

namespace runtime
{
using namespace composition;

void
instance_scheduler_t::lock (const InstanceSet& set)
{
  for (InstanceSet::const_iterator pos = set.begin (), limit = set.end ();
       pos != limit;
       ++pos)
    {
      instance_info_t* record = info_map_[pos->first];
      switch (pos->second)
        {
        case AccessNone:
          break;
        case AccessRead:
          pthread_rwlock_rdlock (&record->lock);
          break;
        case AccessWrite:
          pthread_rwlock_wrlock (&record->lock);
          break;
        }
    }
}

void
instance_scheduler_t::unlock (const InstanceSet& set)
{
  for (InstanceSet::const_iterator pos = set.begin (), limit = set.end ();
       pos != limit;
       ++pos)
    {
      instance_info_t* record = info_map_[pos->first];
      switch (pos->second)
        {
        case AccessNone:
          break;
        case AccessRead:
        case AccessWrite:
          pthread_rwlock_unlock (&record->lock);
          break;
        }
    }
}

void
instance_scheduler_t::push (instance_info_t* info)
{
  pthread_mutex_lock (&list_mutex_);
  if (info->next == NULL)
    {
      // Not on the schedule.
      *tail_ = info;
      tail_ = &(info->next);
      info->next = reinterpret_cast<instance_info_t*> (1);
    }
  pthread_cond_signal (&list_cond_);
  pthread_mutex_unlock (&list_mutex_);
}

void
instance_scheduler_t::dump_schedule () const
{
  if (head_ != NULL)
    {
      for (instance_info_t* record = head_;
           record != reinterpret_cast<instance_info_t*> (1);
           record = record->next)
        {
          std::cout << record << " instance=" << record->heap ()->root () << " type=" << record->instance ()->type << '\n';
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

      // Get an instance to execute.
      instance_info_t* record = scheduler_.head_;
      scheduler_.head_= record->next;
      if (scheduler_.head_== reinterpret_cast<instance_info_t*> (1))
        {
          scheduler_.head_= NULL;
          scheduler_.tail_= &scheduler_.head_;
        }
      record->next = NULL;

      ++scheduler_.pending_;
      pthread_mutex_unlock (&scheduler_.list_mutex_);

      // Try all the actions.
      for (composition::ActionsType::const_iterator pos = record->instance ()->actions.begin (),
           limit = record->instance ()->actions.end ();
           pos != limit;
           ++pos)
        {
          const Action* action = *pos;
          scheduler_.lock (action->instance_set ());
          this->execute (action);
          scheduler_.unlock (action->instance_set ());
        }

      // Collect garbage.
      pthread_rwlock_wrlock (&record->lock);
      this->collect_garbage (record);
      pthread_rwlock_unlock (&record->lock);

      pthread_mutex_lock (&scheduler_.list_mutex_);
      --scheduler_.pending_;
      pthread_cond_signal (&scheduler_.list_cond_);
      pthread_mutex_unlock (&scheduler_.list_mutex_);
    }
}

void
instance_scheduler_t::init (Composer& instance_table,
                            size_t stack_size,
                            size_t thread_count,
                            size_t profile)
{
  // Set up data structures.
  for (Composer::InstancesType::const_iterator pos = instance_table.instances_begin (),
       limit = instance_table.instances_end ();
       pos != limit;
       ++pos)
    {
      composition::Instance* instance = pos->second;
      // Set up the scheduling data structure.
      instance_info_t* info = new instance_info_t (instance);
      info_map_[instance] = info;
      // Add the instance to the schedule.
      push (info);
    }

  {
    // Initialize.
    instance_executor_t exec (*this, stack_size, 0);
    for (InfoMap::const_iterator pos = info_map_.begin (),
         limit = info_map_.end ();
         pos != limit;
         ++pos)
      {
        runtime::initialize (exec, pos->second);
      }
  }

  for (size_t idx = 0; idx != thread_count; ++idx)
    {
      instance_executor_t* exec = new instance_executor_t (*this, stack_size, profile);
      executors_.push_back (exec);
    }
}

void instance_scheduler_t::run ()
{
  for (size_t idx = 0; idx != executors_.size (); ++idx)
    {
      executors_[idx]->spawn ();
    }

  for (size_t idx = 0; idx != executors_.size (); ++idx)
    {
      executors_[idx]->join ();
    }
}

void
instance_scheduler_t::fini (FILE* profile_out)
{
  for (size_t idx = 0; idx != executors_.size (); ++idx)
    {
      executors_[idx]->fini (profile_out, idx);
      delete executors_[idx];
    }
}

}
