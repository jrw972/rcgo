#include "executor_base.hpp"
#include "runtime.hpp"
#include "composition.hpp"
#include "heap.hpp"

namespace runtime
{

ExecutorBase::ExecutorBase (size_t stack_size, pthread_mutex_t* stdout_mutex, size_t profile)
  : stack_ (stack_size)
  , current_info_ (NULL)
  , mutable_phase_base_pointer_ (NULL)
  , stdout_mutex_ (stdout_mutex)
  , events_ (profile)
  , event_idx_ (0)
  , event_full_ (false)
{ }

ExecutorBase::~ExecutorBase () { }

runtime::Stack& ExecutorBase::stack ()
{
  return stack_;
}
runtime::Heap* ExecutorBase::heap () const
{
  return current_info_->heap ();
}
void ExecutorBase::heap (runtime::Heap* heap)
{
  current_info_->heap (heap);
}
ComponentInfoBase* ExecutorBase::current_info () const
{
  return current_info_;
}

ComponentInfoBase* ExecutorBase::current_info (ComponentInfoBase* info)
{
  ComponentInfoBase* retval = current_info_;
  current_info_ = info;
  return retval;
}

char* ExecutorBase::mutable_phase_base_pointer () const
{
  return mutable_phase_base_pointer_;
}
void ExecutorBase::mutable_phase_base_pointer (char* ptr)
{
  mutable_phase_base_pointer_ = ptr;
}

void ExecutorBase::lock_stdout ()
{
  pthread_mutex_lock (stdout_mutex_);
}
void ExecutorBase::unlock_stdout ()
{
  pthread_mutex_unlock (stdout_mutex_);
}
void ExecutorBase::push () { }

runtime::FileDescriptor*
ExecutorBase::allocate_file_descriptor (int fd)
{
  void* p = this->heap ()->allocate (sizeof (runtime::FileDescriptor));
  return new (p) runtime::FileDescriptor (fd);
}

void ExecutorBase::checked_for_readability (runtime::FileDescriptor* fd) { }
void ExecutorBase::checked_for_writability (runtime::FileDescriptor* fd) { }

bool ExecutorBase::execute (const composition::Action* action)
{
  Event* e = begin_event ();
  bool enabled = runtime::enabled (*this, action->instance->component, action->action, action->iota);
  end_event (e, enabled ? Event::Precondition_True : Event::Precondition_False, action);

  if (enabled)
    {
      e = begin_event ();
      runtime::execute_no_check (*this, action->instance->component, action->action, action->iota);
      end_event (e, Event::Action, action);
    }

  return enabled;
}

void ExecutorBase::execute_no_check (const composition::Action* action)
{
  Event* e = begin_event ();
  runtime::execute_no_check (*this, action->instance->component, action->action, action->iota);
  end_event (e, Event::Action, action);
}

bool ExecutorBase::collect_garbage (ComponentInfoBase* info)
{
  this->current_info (info);
  Event* e = begin_event ();
  bool gc = this->heap ()->collect_garbage ();
  end_event (e, gc ? Event::Garbage_Collection_True : Event::Garbage_Collection_False, info);
  return gc;
}

void ExecutorBase::fini (FILE* profile_out, size_t thread)
{
  if (!events_.empty ())
    {
      fprintf (profile_out, "BEGIN thread %zd%s\n", thread, event_full_ ? " OVERFLOW" : "");
      for (EventsType::const_iterator pos = events_.begin (),
           limit = event_full_ ? events_.end () : events_.begin () + event_idx_;
           pos != limit;
           ++pos)
        {
          switch (pos->type)
            {
            case Event::Precondition_True:
              fprintf (profile_out, "PRECONDITION_TRUE %s %ld.%.09ld %ld.%.09ld\n", pos->action->name.c_str (), pos->begin.tv_sec, pos->begin.tv_nsec, pos->end.tv_sec, pos->end.tv_nsec);
              break;
            case Event::Precondition_False:
              fprintf (profile_out, "PRECONDITION_FALSE %s %ld.%.09ld %ld.%.09ld\n", pos->action->name.c_str (), pos->begin.tv_sec, pos->begin.tv_nsec, pos->end.tv_sec, pos->end.tv_nsec);
              break;
            case Event::Action:
              fprintf (profile_out, "ACTION %s %ld.%.09ld %ld.%.09ld\n", pos->action->name.c_str (), pos->begin.tv_sec, pos->begin.tv_nsec, pos->end.tv_sec, pos->end.tv_nsec);
              break;
            case Event::Garbage_Collection_True:
              fprintf (profile_out, "GARBAGE_COLLECTION_TRUE %s %ld.%.09ld %ld.%.09ld\n", pos->info->instance ()->name.c_str (), pos->begin.tv_sec, pos->begin.tv_nsec, pos->end.tv_sec, pos->end.tv_nsec);
              break;
            case Event::Garbage_Collection_False:
              fprintf (profile_out, "GARBAGE_COLLECTION_FALSE %s %ld.%.09ld %ld.%.09ld\n", pos->info->instance ()->name.c_str (), pos->begin.tv_sec, pos->begin.tv_nsec, pos->end.tv_sec, pos->end.tv_nsec);
              break;
            }
        }
      fprintf (profile_out, "END thread %zd\n", thread);
    }
}

ComponentInfoBase::ComponentInfoBase (composition::Instance* instance)
  : instance_ (instance)
  , heap_ (new Heap (instance->component, arch::size (instance->type)))
{
  // Link the instance to its scheduling information.
  *reinterpret_cast<ComponentInfoBase**> (instance->component) = this;
}

composition::Instance* ComponentInfoBase::instance () const
{
  return instance_;
}
Heap* ComponentInfoBase::heap () const
{
  return heap_;
}
void ComponentInfoBase::heap (Heap* h)
{
  heap_ = h;
}

ExecutorBase::Event* ExecutorBase::begin_event ()
{
  Event* e = NULL;
  if (!events_.empty ())
    {
      e = &events_[event_idx_];
      event_idx_ = (event_idx_ + 1) & (events_.size () - 1);
      event_full_ = event_full_ || (event_idx_ == 0);
      clock_gettime (CLOCK_MONOTONIC, &e->begin);
    }
  return e;
}

void ExecutorBase::end_event (Event* e, Event::Type type, const composition::Action* action)
{
  if (e)
    {
      clock_gettime (CLOCK_MONOTONIC, &e->end);
      e->type = type;
      e->action = action;
    }
}

void ExecutorBase::end_event (Event* e, Event::Type type, ComponentInfoBase* info)
{
  if (e)
    {
      clock_gettime (CLOCK_MONOTONIC, &e->end);
      e->type = type;
      e->info = info;
    }
}

ComponentInfoBase* component_to_info (component_t* component)
{
  return *reinterpret_cast<ComponentInfoBase**> (component);
}

}
