#ifndef RC_SRC_EXECUTOR_BASE_HPP
#define RC_SRC_EXECUTOR_BASE_HPP

#include "stack.hpp"
#include "action.hpp"
#include "heap.hpp"

namespace runtime
{

class FileDescriptor
{
public:
  int fd () const
  {
    return fd_;
  }

private:
  FileDescriptor (int fd)
    : fd_ (fd)
  { }
  int const fd_;
  friend class ExecutorBase;
};

  struct ComponentInfoBase {
    ComponentInfoBase (composition::Instance* instance);
    composition::Instance* instance () const { return instance_; }
    Heap* heap () const { return heap_; }
    void heap (Heap* h) { heap_ = h; }
  private:
    composition::Instance* instance_;
    Heap* heap_;
  };

class ExecutorBase
{
public:
  ExecutorBase (size_t stack_size, pthread_mutex_t* stdout_mutex, size_t profile)
    : stack_ (stack_size)
    , current_info_ (NULL)
    , mutable_phase_base_pointer_ (NULL)
    , stdout_mutex_ (stdout_mutex)
    , events_ (profile)
    , event_idx_ (0)
    , event_full_ (false)
  { }

  virtual ~ExecutorBase () { }

  runtime::Stack& stack ()
  {
    return stack_;
  }
  runtime::Heap* heap () const { return current_info_->heap (); }
  void heap (runtime::Heap* heap) { current_info_->heap (heap); }
  ComponentInfoBase* current_info () const
  {
    return current_info_;
  }

  ComponentInfoBase* current_info (ComponentInfoBase* info)
  {
    ComponentInfoBase* retval = current_info_;
    current_info_ = info;
    return retval;
  }

  char* mutable_phase_base_pointer () const
  {
    return mutable_phase_base_pointer_;
  }
  void mutable_phase_base_pointer (char* ptr)
  {
    mutable_phase_base_pointer_ = ptr;
  }

  void lock_stdout ()
  {
    pthread_mutex_lock (stdout_mutex_);
  }
  void unlock_stdout ()
  {
    pthread_mutex_unlock (stdout_mutex_);
  }
  virtual void push () { }

  runtime::FileDescriptor*
  allocateFileDescriptor (int fd)
  {
    void* p = this->heap ()->allocate (sizeof (runtime::FileDescriptor));
    return new (p) runtime::FileDescriptor (fd);
  }

  virtual void checkedForReadability (runtime::FileDescriptor* fd) { }
  virtual void checkedForWritability (runtime::FileDescriptor* fd) { }

  bool execute (const composition::Action* action);
  void execute_no_check (const composition::Action* action);
  bool collect_garbage (ComponentInfoBase* info);

  void fini (FILE* profile_out, size_t thread);

private:
  struct Event {
    enum Type {
      Precondition_True,
      Precondition_False,
      Action,
      Garbage_Collection_True,
      Garbage_Collection_False,
    } type;
    union {
      const composition::Action* action;
      ComponentInfoBase* info;
    };
    struct timespec begin;
    struct timespec end;
  };

  Event* begin_event ()
  {
    Event* e = NULL;
    if (!events_.empty ()) {
      e = &events_[event_idx_];
      event_idx_ = (event_idx_ + 1) & (events_.size () - 1);
      event_full_ = event_full_ || (event_idx_ == 0);
      clock_gettime (CLOCK_MONOTONIC, &e->begin);
    }
    return e;
  }

  void end_event (Event* e, Event::Type type, const composition::Action* action)
  {
    if (e) {
      clock_gettime (CLOCK_MONOTONIC, &e->end);
      e->type = type;
      e->action = action;
    }
  }

  void end_event (Event* e, Event::Type type, ComponentInfoBase* info)
  {
    if (e) {
      clock_gettime (CLOCK_MONOTONIC, &e->end);
      e->type = type;
      e->info = info;
    }
  }

  runtime::Stack stack_;
  ComponentInfoBase* current_info_;
  char* mutable_phase_base_pointer_;
  pthread_mutex_t* stdout_mutex_;
  typedef std::vector<Event> EventsType;
  EventsType events_;
  size_t event_idx_;
  bool event_full_;
};

  inline ComponentInfoBase* component_to_info (component_t* component)
  {
    return *reinterpret_cast<ComponentInfoBase**> (component);
  }

}

#endif // RC_SRC_EXECUTOR_BASE_HPP
