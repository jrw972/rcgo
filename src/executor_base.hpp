#ifndef executor_base_hpp
#define executor_base_hpp

#include "stack_frame.hpp"
#include "action.hpp"

class executor_base_t
{
public:
  executor_base_t (size_t stack_size, pthread_mutex_t* stdout_mutex)
    : stack_ (stack_frame_make (stack_size))
    , current_instance_ (NULL)
    , mutable_phase_base_pointer_ (NULL)
    , stdout_mutex_ (stdout_mutex)
  { }

  virtual ~executor_base_t () { }

  stack_frame_t* stack () const { return stack_; }
  virtual heap_t* heap () const = 0;
  virtual void heap (heap_t* heap) = 0;
  component_t* current_instance () const { return current_instance_; }

  component_t* current_instance (component_t* instance)
  {
    component_t* retval = current_instance_;
    current_instance_ = instance;
    return retval;
  }

  char* mutable_phase_base_pointer () const { return mutable_phase_base_pointer_; }
  void mutable_phase_base_pointer (char* ptr) { mutable_phase_base_pointer_ = ptr; }
  void lock_stdout () { pthread_mutex_lock (stdout_mutex_); }
  void unlock_stdout () { pthread_mutex_unlock (stdout_mutex_); }
  virtual void push () = 0;

private:
  stack_frame_t* stack_;
  component_t* current_instance_;
  char* mutable_phase_base_pointer_;
  pthread_mutex_t* stdout_mutex_;
};

#endif /* executor_base_hpp */
