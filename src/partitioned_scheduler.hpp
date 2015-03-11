#ifndef partitioned_scheduler_hpp
#define partitioned_scheduler_hpp

#include "types.hpp"
#include <pthread.h>
#include "instance.hpp"
#include "heap.hpp"
#include "stack_frame.hpp"
#include "executor_base.hpp"
#include <list>
#include "instance_table.hpp"
#include "runtime.hpp"

class partitioned_scheduler_t
{
public:
  partitioned_scheduler_t ()
  {
    pthread_mutex_init (&stdout_mutex_, NULL);
  }

  void run (instance_table_t& instance_table, size_t stack_size, size_t thread_count);

private:
  struct info_t
  {
    instance_t* instance;
    heap_t* heap;

    info_t (instance_t* instance)
      : instance (instance)
      , heap (heap_make (instance->ptr (), instance->type ()->size ()))
    {
      // Link the instance to its scheduling information.
      *((info_t**)instance->ptr ()) = this;
    }

    char* get_ptr () const
    {
      return static_cast<char*> (heap_instance (heap));
    }
  };

  typedef uint8_t processor_id_t;
  class executor_t;

  struct task_t
  {
    uint64_t mask;
    typedef std::vector<processor_id_t> ProcessorListType;
    ProcessorListType processor_list;

    task_t () : mask (0) { }

    void add_conflict (processor_id_t i)
    {
      mask |= (1 << i);
      processor_list.push_back (i);
    }
    virtual const instance_set_t& set () const = 0;
    virtual void execute (executor_t& exec) const = 0;
  };

  struct action_task_t : public task_t
  {
    action_task_t (instance_t* i, const instance_t::ConcreteAction& a)
      : instance (i)
      , action (a)
    { }

    instance_t* instance;
    instance_t::ConcreteAction action;

    const instance_set_t& set () const { return action.set; }
    virtual void execute (executor_t& exec) const
    {
      runtime::exec (exec, instance->ptr (), action.action, action.iota);
    }
  };

  struct gc_task_t : public task_t
  {
    gc_task_t (processor_id_t id, instance_t* i)
      : instance (i)
    {
      set_[i] = TRIGGER_WRITE;
    }

    instance_t* instance;
    instance_set_t set_;

    const instance_set_t& set () const { return set_; }
    virtual void execute (executor_t& exec) const
    {
      exec.current_instance (instance->ptr ());
      heap_collect_garbage (exec.heap ());
    }
  };

  class executor_t : public executor_base_t
  {
  public:
    executor_t (partitioned_scheduler_t& scheduler,
                processor_id_t id,
                size_t stack_size,
                pthread_mutex_t* stdout_mutex)
      : executor_base_t (stack_size, stdout_mutex)
      , scheduler_ (scheduler)
      , id_ (id)
      , mask_ (0)
    { }

    void add (instance_t* instance, const instance_t::ConcreteAction& a)
    {
      tasks_.push_back (new action_task_t (instance, a));
      actions_.insert (a.set.begin (), a.set.end ());
    }

    void add (instance_t* i)
    {
      tasks_.push_back (new gc_task_t (id_, i));
      actions_.insert (std::make_pair (i, TRIGGER_WRITE));
    }

    void compute_masks ()
    {
      for (ListType::iterator pos = tasks_.begin (), limit = tasks_.end ();
           pos != limit;
           ++pos)
        {
          for (processor_id_t i = 0; i != scheduler_.executors_.size (); ++i)
            {
              if (i != id_)
                {
                  if (!independent (scheduler_.executors_[i]->actions_.begin (), scheduler_.executors_[i]->actions_.end (), (*pos)->set ().begin (), (*pos)->set ().end ()))
                    {
                      (*pos)->add_conflict (i);
                    }
                }
              else
                {
                  (*pos)->add_conflict (i);
                }
            }
        }
    }

    void spawn ()
    {
      pthread_create (&thread_, NULL, executor_t::run, this);
    }

    static void* run (void* arg)
    {
      executor_t* exec = static_cast<executor_t*> (arg);
      exec->run_i ();
      return NULL;
    }

    void run_i ();

    void join ()
    {
      pthread_join (thread_, NULL);
    }

    virtual heap_t* heap () const
    {
      info_t* info = *reinterpret_cast<info_t**> (current_instance ());
      return info->heap;
    }

    virtual void heap (heap_t* heap)
    {
      info_t* info = *reinterpret_cast<info_t**> (current_instance ());
      info->heap = heap;
    }

    virtual void push () { dirty_flag_ = true; }

  private:
    enum Mode
      {
        DIRTY,
        CLEAN
      };
    partitioned_scheduler_t& scheduler_;
    const processor_id_t id_;
    std::multiset<std::pair<instance_t*, TriggerAction> > actions_;
    typedef std::list<task_t*> ListType;
    ListType tasks_;
    pthread_t thread_;
    volatile uint64_t mask_;
    Mode mode_;
    bool dirty_flag_;
  };

  pthread_mutex_t stdout_mutex_;
  std::vector<executor_t*> executors_;
  size_t dirty_count_;
};

#endif /* partitioned_scheduler_hpp */
