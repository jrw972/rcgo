#ifndef RC_SRC_INSTANCE_SCHEDULER_HPP
#define RC_SRC_INSTANCE_SCHEDULER_HPP

#include <pthread.h>

#include "types.hpp"
#include "heap.hpp"
#include "stack.hpp"
#include "executor_base.hpp"
#include "composition.hpp"
#include "scheduler.hpp"

namespace runtime
{

class instance_scheduler_t : public Scheduler
{
public:
  instance_scheduler_t ()
    : head_ (NULL)
    , tail_ (&head_)
    , pending_ (0)
  {
    pthread_mutex_init (&list_mutex_, NULL);
    pthread_cond_init (&list_cond_, NULL);
    pthread_mutex_init (&stdout_mutex_, NULL);
  }

  void init (composition::Composer& instance_table, size_t stack_size, size_t thread_count, size_t profile);
  void run ();
  void fini (FILE* profile_out);
  void dump_schedule () const;

private:

  struct instance_info_t : public ComponentInfoBase
  {
    // Scheduling lock.
    pthread_rwlock_t lock;
    // Next instance on the schedule.
    // 0 means this instance is not on the schedule.
    // 1 means this instance is the end of the schedule.
    // ? means this instance is on the schedule.
    instance_info_t* next;

    instance_info_t (composition::Instance* instance)
      : ComponentInfoBase (instance)
      , next (NULL)
    {
      pthread_rwlock_init (&lock, NULL);
    }
  };

  class instance_executor_t : public ExecutorBase
  {
  private:
    instance_scheduler_t& scheduler_;
    pthread_t thread_;

  public:
    instance_executor_t (instance_scheduler_t& s, size_t stack_size, size_t profile)
      : ExecutorBase (stack_size, &s.stdout_mutex_, profile)
      , scheduler_ (s)
    { }

    virtual void push ()
    {
      instance_info_t* info = static_cast<instance_info_t*> (current_info ());
      assert (info != NULL);
      scheduler_.push (info);

      for (composition::Instance::LinkedInstancesType::const_iterator pos = current_info ()->instance ()->linked_instances.begin (),
             limit = current_info ()->instance ()->linked_instances.end ();
           pos != limit;
           ++pos)
        {
          instance_info_t* info = static_cast<instance_info_t*> (component_to_info ((*pos)->component));
          scheduler_.push (info);
        }
    }

    void spawn ()
    {
      pthread_create (&thread_, NULL, instance_executor_t::run, this);
    }

    static void* run (void* arg)
    {
      instance_executor_t* exec = static_cast<instance_executor_t*> (arg);
      exec->run_i ();
      return NULL;
    }

    void run_i ();

    void join ()
    {
      pthread_join (thread_, NULL);
    }
  };

  void push (instance_info_t* info);
  void lock (const composition::InstanceSet& set);
  void unlock (const composition::InstanceSet& set);

  std::vector<instance_executor_t*> executors_;
  instance_info_t* head_;
  instance_info_t** tail_;
  size_t pending_;
  pthread_mutex_t list_mutex_;
  pthread_cond_t list_cond_;
  pthread_mutex_t stdout_mutex_;
  // TODO:  Replace this datastructure by translating once.
  typedef std::map<composition::Instance*, instance_info_t*> InfoMapType;
  InfoMapType info_map_;
};

}

#endif // RC_SRC_INSTANCE_SCHEDULER_HPP
