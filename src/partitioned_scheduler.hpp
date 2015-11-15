#ifndef partitioned_scheduler_hpp
#define partitioned_scheduler_hpp

#include "types.hpp"
#include <pthread.h>
#include "heap.hpp"
#include "stack_frame.hpp"
#include "executor_base.hpp"
#include "Composition.hpp"
#include "runtime.hpp"
#include <queue>
#include <poll.h>
#include <sys/eventfd.h>
#include <unistd.h>

class partitioned_scheduler_t
{
public:
  partitioned_scheduler_t ()
  {
    pthread_mutex_init (&stdout_mutex_, NULL);
  }

  void run (Composition::Composer& instance_table,
            size_t stack_size,
            size_t thread_count);

private:
  class task_t;

  class info_t
  {
  public:
    info_t (Composition::Instance* instance)
      : instance_ (instance)
      , heap_ (heap_make (instance->component, instance->type->Size ()))
      , lock_ (0)
      , count_ (0)
      , head_ (NULL)
      , tail_ (&head_)
    {
      // Link the instance to its scheduling information.
      *((info_t**)instance->component) = this;
    }

    component_t* ptr () const
    {
      return instance_->component;
    }

    heap_t* heap () const
    {
      return heap_;
    }

    void heap (heap_t* heap)
    {
      heap_ = heap;
    }

    bool read_lock (task_t* task)
    {
      assert (task->next == NULL);
      bool retval = false;
      while (__sync_lock_test_and_set (&lock_, 1)) while (lock_) ;

      if (count_ == 0)
        {
          // First reader.
          ++count_;
        }
      else if (count_ > 0 && head_ == NULL)
        {
          // Subsequent reader.
          ++count_;
        }
      else
        {
          // Enqueue the read lock.
          task->read_lock = true;
          *tail_ = task;
          tail_ = &task->next;
          retval = true;
        }

      __sync_lock_release (&lock_);
      return retval;
    }

    bool write_lock (task_t* task)
    {
      assert (task->next == NULL);
      bool retval = false;
      while (__sync_lock_test_and_set (&lock_, 1)) while (lock_) ;

      if (count_ == 0)
        {
          --count_;
        }
      else
        {
          // Enqueue the write lock.
          task->read_lock = false;
          *tail_ = task;
          tail_ = &task->next;
          retval = true;
        }

      __sync_lock_release (&lock_);
      return retval;
    }

    void read_unlock ()
    {
      while (__sync_lock_test_and_set (&lock_, 1)) while (lock_) ;
      assert (count_ > 0);
      --count_;
      process_list ();
    }

    void write_unlock ()
    {
      while (__sync_lock_test_and_set (&lock_, 1)) while (lock_) ;
      assert (count_ == -1);
      ++count_;
      process_list ();
    }

  private:
    void
    process_list ()
    {
      task_t* h = NULL;
      if (count_ == 0 && head_ != NULL)
        {
          h = head_;
          if (h->read_lock)
            {
              task_t** t = &h->next;
              size_t size = 1;
              while (t != tail_ && (*t)->read_lock == true)
                {
                  t = &(*t)->next;
                  ++size;
                }
              head_ = *t;
              *t = NULL;
              // Lock.
              count_ = size;
            }
          else
            {
              head_ = h->next;
              h->next = NULL;
              // Lock.
              count_ = -1;
            }

          if (head_ == NULL)
            {
              tail_ = &head_;
            }
        }
      __sync_lock_release (&lock_);
      // Signal.
      while (h != NULL)
        {
          task_t* next = h->next;
          h->next = NULL;
          h->to_ready_list ();
          h = next;
        }
    }

    Composition::Instance* instance_;
    heap_t* heap_;
    volatile size_t lock_;
    ssize_t count_;
    task_t* head_;
    task_t** tail_;
  };

  class executor_t;

  enum ExecutionResult
  {
    NONE,
    SKIP,
    HIT,
    FIRST_SKIP,
    FIRST_HIT,
  };

  class task_t
  {
  public:
    enum ExecutionKind
    {
      HIT,
      SKIP,
    };

    task_t ()
      : executor (NULL)
      , read_lock (false)
      , next (NULL)
      , last_execution_kind_ (HIT)
      , generation_ (0)
    { }

    virtual const Composition::InstanceSet& set () const = 0;

    ExecutionResult execute (size_t generation);
    ExecutionResult resume (size_t generation);
    void to_ready_list ()
    {
      executor->to_ready_list (this);
    }

    void to_idle_list ()
    {
      executor->to_idle_list (this);
    }

    executor_t* executor;
    bool read_lock;
    task_t* next;

  private:
    // Return true if the precondition was true.
    virtual bool execute_i () const = 0;
    Composition::InstanceSet::const_iterator pos_;
    Composition::InstanceSet::const_iterator limit_;
    ExecutionKind last_execution_kind_;
    size_t generation_;
  };

  struct action_task_t : public task_t
  {
    action_task_t (const Composition::Action* a)
      : action (a)
    { }

    const Composition::Action* const action;

    const Composition::InstanceSet& set () const
    {
      return action->GetInstanceSet ();
    }
    virtual bool execute_i () const
    {
      return runtime::exec (*executor, action->instance->component, action->action, action->iota);
    }
  };

  struct always_task_t : public task_t
  {
    always_task_t (const Composition::Action* a)
      : action (a)
    { }

    const Composition::Action* const action;

    const Composition::InstanceSet& set () const
    {
      return action->GetInstanceSet ();
    }
    virtual bool execute_i () const
    {
      return runtime::exec_no_check (*executor, action->instance->component, action->action, action->iota);
    }
  };

  struct gc_task_t : public task_t
  {
    gc_task_t (Composition::Instance* i)
      : instance (i)
    {
      set_.insert (std::make_pair (instance, AccessWrite));
    }

    Composition::Instance* instance;
    Composition::InstanceSet set_;

    const Composition::InstanceSet& set () const
    {
      return set_;
    }
    virtual bool execute_i () const
    {
      executor->current_instance (instance->component);
      return heap_collect_garbage (executor->heap ());
    }
  };

  class executor_t : public executor_base_t
  {
  public:
    executor_t (partitioned_scheduler_t& scheduler,
                size_t id,
                size_t neighbor_id,
                size_t stack_size,
                pthread_mutex_t* stdout_mutex)
      : executor_base_t (stack_size, stdout_mutex)
      , scheduler_ (scheduler)
      , id_ (id)
      , neighbor_id_ (neighbor_id)
      , idle_head_ (NULL)
      , idle_tail_ (&idle_head_)
      , ready_head_ (NULL)
      , ready_tail_ (&ready_head_)
      , task_count_ (0)
      , trackFileDescriptors_ (false)
      , using_eventfd_ (false)
    {
      pthread_mutex_init (&mutex_, NULL);
      pthread_cond_init (&cond_, NULL);
      eventfd_ = eventfd (0, EFD_NONBLOCK);
    }

    void to_idle_list (task_t* task)
    {
      assert (task->next == NULL);
      *idle_tail_ = task;
      idle_tail_ = &task->next;
    }

    void to_ready_list (task_t* task)
    {
      assert (task->next == NULL);
      pthread_mutex_lock (&mutex_);
      if (ready_head_ == NULL)
        {
          if (using_eventfd_)
            {
              const uint64_t v = 1;
              write (eventfd_, &v, sizeof (uint64_t));
            }
          pthread_cond_signal (&cond_);
        }
      *ready_tail_ = task;
      ready_tail_ = &task->next;
      pthread_mutex_unlock (&mutex_);
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
      return info->heap ();
    }

    virtual void heap (heap_t* heap)
    {
      info_t* info = *reinterpret_cast<info_t**> (current_instance ());
      info->heap (heap);
    }

    virtual void push ()
    {
      /*dirty_flag_ = true;*/
    }

    void add_task ()
    {
      ++task_count_;
    }

    virtual void
    checkedForReadability (FileDescriptor* fd)
    {
      if (trackFileDescriptors_)
        {
          std::pair<FileDescriptorMapType::iterator, bool> x =
            fileDescriptorMap_.insert (std::make_pair (fd, 0));
          x.first->second |= POLLIN;
        }
    }

    virtual void
    checkedForWritability (FileDescriptor* fd)
    {
      if (trackFileDescriptors_)
        {
          std::pair<FileDescriptorMapType::iterator, bool> x =
            fileDescriptorMap_.insert (std::make_pair (fd, 0));
          x.first->second |= POLLOUT;
        }
    }

  private:

    void
    disableFileDescriptorTracking ()
    {
      trackFileDescriptors_ = false;
    }

    void
    enableFileDescriptorTracking ()
    {
      trackFileDescriptors_ = true;
      fileDescriptorMap_.clear ();
    }

    struct Message
    {
      enum Kind
      {
        START_SHOOT_DOWN,
        START_WAITING1,
        START_DOUBLE_CHECK,
        START_WAITING2,
        TERMINATE,
        RESET,
      };
      Kind kind;
      size_t id;

      static Message make_start_shoot_down (size_t id)
      {
        Message m;
        m.kind = START_SHOOT_DOWN;
        m.id = id;
        return m;
      }

      static Message make_start_waiting1 (size_t id)
      {
        Message m;
        m.kind = START_WAITING1;
        m.id = id;
        return m;
      }

      static Message make_start_double_check (size_t id)
      {
        Message m;
        m.kind = START_DOUBLE_CHECK;
        m.id = id;
        return m;
      }

      static Message make_start_waiting2 (size_t id)
      {
        Message m;
        m.kind = START_WAITING2;
        m.id = id;
        return m;
      }

      static Message make_terminate ()
      {
        Message m;
        m.kind = TERMINATE;
        return m;
      }

      static Message make_reset (size_t id)
      {
        Message m;
        m.kind = RESET;
        m.id = id;
        return m;
      }
    };

    bool get_ready_task_and_message (task_t*& task, Message& message)
    {
      pthread_mutex_lock (&mutex_);
      task = ready_head_;
      if (task != NULL)
        {
          ready_head_ = task->next;
          if (ready_head_ == NULL)
            {
              ready_tail_ = &ready_head_;
            }
        }
      bool flag = false;
      if (!message_queue_.empty ())
        {
          flag = true;
          message = message_queue_.front ();
          message_queue_.pop ();
        }

      if (using_eventfd_)
        {
          uint64_t v;
          read (eventfd_, &v, sizeof (uint64_t));
        }

      pthread_mutex_unlock (&mutex_);

      return flag;
    }

    void send (Message m) const
    {
      scheduler_.executors_[neighbor_id_]->receive (m);
    }

    void receive (Message m)
    {
      pthread_mutex_lock (&mutex_);
      if (message_queue_.empty ())
        {
          if (using_eventfd_)
            {
              const uint64_t v = 1;
              write (eventfd_, &v, sizeof (uint64_t));
            }
          pthread_cond_signal (&cond_);
        }
      message_queue_.push (m);
      pthread_mutex_unlock (&mutex_);
    }

    void sleep ()
    {
      pthread_mutex_lock (&mutex_);
      while (ready_head_ == NULL && message_queue_.empty ())
        {
          pthread_cond_wait (&cond_, &mutex_);
        }
      pthread_mutex_unlock (&mutex_);
    }

    bool poll ();

    partitioned_scheduler_t& scheduler_;
    const size_t id_;
    const size_t neighbor_id_;
    pthread_t thread_;

    task_t* idle_head_;
    task_t** idle_tail_;
    pthread_mutex_t mutex_;
    pthread_cond_t cond_;
    int eventfd_;
    task_t* ready_head_;
    task_t** ready_tail_;
    std::queue<Message> message_queue_;
    size_t task_count_;
    bool trackFileDescriptors_;
    typedef std::map<FileDescriptor*, short> FileDescriptorMapType;
    FileDescriptorMapType fileDescriptorMap_;
    bool using_eventfd_;
  };

  void
  initialize_task (task_t* task, size_t thread_count);

  pthread_mutex_t stdout_mutex_;
  std::vector<executor_t*> executors_;
};

#endif /* partitioned_scheduler_hpp */
