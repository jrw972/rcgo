#ifndef RC_SRC_EXECUTOR_BASE_HPP
#define RC_SRC_EXECUTOR_BASE_HPP

#include "stack.hpp"
#include "runtime_types.hpp"

namespace runtime
{

struct ComponentInfoBase
{
  ComponentInfoBase (composition::Instance* instance);
  composition::Instance* instance () const;
  Heap* heap () const;
  void heap (Heap* h);

private:
  composition::Instance* instance_;
  Heap* heap_;
};

struct ExecutorBase
{
  ExecutorBase (size_t stack_size,
                pthread_mutex_t* stdout_mutex,
                size_t profile);
  virtual ~ExecutorBase ();
  Stack& stack ();
  Heap* heap () const;
  void heap (Heap* heap);
  ComponentInfoBase* current_info () const;
  ComponentInfoBase* current_info (ComponentInfoBase* info);
  char* mutable_phase_base_pointer () const;
  void mutable_phase_base_pointer (char* ptr);
  void lock_stdout ();
  void unlock_stdout ();
  virtual void push ();
  FileDescriptor* allocate_file_descriptor (int fd);
  virtual void checked_for_readability (FileDescriptor* fd);
  virtual void checked_for_writability (FileDescriptor* fd);
  bool execute (const composition::Action* action);
  void execute_no_check (const composition::Action* action);
  bool collect_garbage (ComponentInfoBase* info);
  void fini (FILE* profile_out, size_t thread);

private:
  struct Event
  {
    enum Type
    {
      Precondition_True,
      Precondition_False,
      Action,
      Garbage_Collection_True,
      Garbage_Collection_False,
    } type;
    union
    {
      const composition::Action* action;
      ComponentInfoBase* info;
    };
    struct timespec begin;
    struct timespec end;
  };

  Event* begin_event ();
  void end_event (Event* e, Event::Type type, const composition::Action* action);
  void end_event (Event* e, Event::Type type, ComponentInfoBase* info);

  Stack stack_;
  ComponentInfoBase* current_info_;
  char* mutable_phase_base_pointer_;
  pthread_mutex_t* stdout_mutex_;
  typedef std::vector<Event> EventsType;
  EventsType events_;
  size_t event_idx_;
  bool event_full_;
};

ComponentInfoBase* component_to_info (component_t* component);

}

#endif // RC_SRC_EXECUTOR_BASE_HPP
