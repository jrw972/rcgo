#ifndef rc_src_executor_base_hpp
#define rc_src_executor_base_hpp

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
  friend executor_base_t;
};

class executor_base_t
{
public:
  executor_base_t (size_t stack_size, pthread_mutex_t* stdout_mutex)
    : stack_ (stack_size)
    , current_instance_ (NULL)
    , mutable_phase_base_pointer_ (NULL)
    , stdout_mutex_ (stdout_mutex)
  { }

  virtual ~executor_base_t () { }

  runtime::Stack& stack ()
  {
    return stack_;
  }
  virtual runtime::heap_t* heap () const = 0;
  virtual void heap (runtime::heap_t* heap) = 0;
  component_t* current_instance () const
  {
    return current_instance_;
  }

  component_t* current_instance (component_t* instance)
  {
    component_t* retval = current_instance_;
    current_instance_ = instance;
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
  virtual void push () = 0;

  runtime::FileDescriptor*
  allocateFileDescriptor (int fd)
  {
    return new (static_cast< runtime::FileDescriptor*> (runtime::heap_allocate (this->heap (), sizeof (runtime::FileDescriptor)))) runtime::FileDescriptor (fd);
  }

  virtual void checkedForReadability (runtime::FileDescriptor* fd) { }
  virtual void checkedForWritability (runtime::FileDescriptor* fd) { }

private:
  runtime::Stack stack_;
  component_t* current_instance_;
  char* mutable_phase_base_pointer_;
  pthread_mutex_t* stdout_mutex_;
};

}

#endif // rc_src_executor_base_hpp
