#ifndef instance_scheduler_hpp
#define instance_scheduler_hpp

#include "types.hpp"
#include <pthread.h>
#include "heap.hpp"
#include "stack.hpp"
#include "executor_base.hpp"
#include "composition.hpp"

class instance_scheduler_t
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

  void run (Composition::Composer& instance_table, size_t stack_size, size_t thread_count);
  void dump_schedule () const;

private:

  struct instance_info_t
  {
    // Scheduling lock.
    pthread_rwlock_t lock;
    Composition::Instance* instance;
    heap_t* heap;
    // Next instance on the schedule.
    // 0 means this instance is not on the schedule.
    // 1 means this instance is the end of the schedule.
    // ? means this instance is on the schedule.
    instance_info_t* next;

    instance_info_t (Composition::Instance* instance)
      : instance (instance)
      , heap (heap_make (instance->component, instance->type->Size ()))
      , next (NULL)
    {
      pthread_rwlock_init (&lock, NULL);
      // Link the instance to its scheduling information.
      *((instance_info_t**)instance->component) = this;
    }

    char* get_ptr () const
    {
      return static_cast<char*> (heap_instance (heap));
    }

    void collect_garbage ()
    {
      pthread_rwlock_wrlock (&lock);
      heap_collect_garbage (heap);
      pthread_rwlock_unlock (&lock);
    }
  };

  class instance_executor_t : public executor_base_t
  {
  private:
    instance_scheduler_t& scheduler_;
    pthread_t thread_;

  public:
    instance_executor_t (instance_scheduler_t& s, size_t stack_size)
      : executor_base_t (stack_size, &s.stdout_mutex_)
      , scheduler_ (s)
    { }

    virtual heap_t* heap () const
    {
      instance_info_t* info = *reinterpret_cast<instance_info_t**> (current_instance ());
      return info->heap;
    }

    virtual void heap (heap_t* heap)
    {
      instance_info_t* info = *reinterpret_cast<instance_info_t**> (current_instance ());
      info->heap = heap;
    }

    void push ()
    {
      scheduler_.push (*reinterpret_cast<instance_info_t**> (current_instance ()));
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
  void lock (const Composition::InstanceSet& set);
  void unlock (const Composition::InstanceSet& set);

  instance_info_t* head_;
  instance_info_t** tail_;
  size_t pending_;
  pthread_mutex_t list_mutex_;
  pthread_cond_t list_cond_;
  pthread_mutex_t stdout_mutex_;
  // TODO:  Replace this datastructure by translating once.
  std::map<Composition::Instance*, instance_info_t*> info_map_;
};

#endif /* instance_scheduler_hpp */
