#include "executor_base.hpp"
#include "runtime.hpp"
#include "composition.hpp"

namespace runtime
{

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
  , heap_ (new Heap (instance->component, instance->type->Size ()))
{
  // Link the instance to its scheduling information.
  *reinterpret_cast<ComponentInfoBase**> (instance->component) = this;
}

}
