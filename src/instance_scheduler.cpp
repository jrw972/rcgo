#include "instance_scheduler.hpp"
#include "runtime.hpp"
#include <stdlib.h>
#include <string.h>
#include "instance.hpp"
#include "heap.hpp"
#include "executor.hpp"
#include "stack_frame.hpp"
#include "action.hpp"
#include "method.hpp"

struct instance_record_t {
  // Scheduling lock.
  pthread_rwlock_t lock;
  instance_t* instance;
  heap_t* heap;
  // Next instance on the schedule.
  // 0 means this instance is not on the schedule.
  // 1 means this instance is the end of the schedule.
  // ? means this instance is on the schedule.
  instance_record_t* next;

  instance_record_t (void* ptr,
                     instance_t* instance)
    : instance (instance)
    , heap (heap_make ((char*)ptr, (char*)ptr + instance->type ()->size ()))
    , next (NULL)
  {
    pthread_rwlock_init (&lock, NULL);
    // Link the instance to its scheduling information.
    *((instance_record_t**)ptr) = this;

    instance->record = this;
  }

  char* get_ptr () const
  {
    return static_cast<char*> (heap_instance (heap));
  }
};

static void instance_record_lock (const instance_set_t& set)
{
  for (instance_set_t::const_iterator pos = set.begin (), limit = set.end ();
       pos != limit;
       ++pos)
    {
      instance_record_t* record = pos->first->record;
      switch (pos->second)
        {
        case TRIGGER_READ:
          pthread_rwlock_rdlock (&record->lock);
          break;
        case TRIGGER_WRITE:
          pthread_rwlock_wrlock (&record->lock);
          break;
        }
    }
}

static void instance_record_unlock (const instance_set_t& set)
{
  for (instance_set_t::const_iterator pos = set.begin (), limit = set.end ();
       pos != limit;
       ++pos)
    {
      instance_record_t* record = pos->first->record;
      switch (pos->second)
        {
        case TRIGGER_READ:
        case TRIGGER_WRITE:
          pthread_rwlock_unlock (&record->lock);
          break;
        }
    }
}

static void instance_record_collect_garbage (instance_record_t* record)
{
  pthread_rwlock_wrlock (&record->lock);
  heap_collect_garbage (record->heap);
  pthread_rwlock_unlock (&record->lock);
}

void
instance_scheduler_t::allocate_instances (void)
{
  for (instance_table_t::InstancesType::const_iterator pos = instance_table_.instances.begin (),
         limit = instance_table_.instances.end ();
       pos != limit;
       ++pos)
    {
      instance_t* instance = pos->second;
      void * ptr;
      if (instance->is_top_level ())
        {
          const type_t* type = instance->type ();
          size_t size = type->size ();
          ptr = malloc (size);
          memset (ptr, 0, size);
        }
      else
        {
          ptr = instance->parent ()->record->get_ptr () + instance->offset ();
        }

      // Set up the scheduling data structure.
      instance_record_t* record = new instance_record_t (ptr, instance);
      // Add the instance to the schedule.
      push (record);
    }
}

void
instance_scheduler_t::create_bindings (void)
{
  for (instance_table_t::PortsType::const_iterator output_pos = instance_table_.ports.begin (),
         output_limit = instance_table_.ports.end ();
       output_pos != output_limit;
       ++output_pos)
    {
      instance_t* output_instance = output_pos->second.output_instance;
      size_t output_port = output_pos->first - output_instance->address ();

      for (instance_table_t::InputsType::const_iterator input_pos = output_pos->second.inputs.begin (),
             input_limit = output_pos->second.inputs.end ();
           input_pos != input_limit;
           ++input_pos)
        {
          bind (reinterpret_cast<port_t**> (output_instance->record->get_ptr () + output_port),
                input_pos->instance->record->get_ptr (),
                input_pos->reaction,
                input_pos->parameter);
        }
    }
}

void
instance_scheduler_t::push (instance_record_t* record)
{
  pthread_mutex_lock (&list_mutex_);
  if (record->next == 0)
    {
      // Not on the schedule.
      *tail_ = record;
      tail_ = &(record->next);
      record->next = (instance_record_t*)1;
    }
  pthread_cond_signal (&list_cond_);
  pthread_mutex_unlock (&list_mutex_);
}

void
instance_scheduler_t::dump_schedule () const
{
  if (head_ != NULL)
    {
      instance_record_t* record = head_;
      while (record != (instance_record_t*)1)
        {
          printf ("%p instance=%p type=%p\n", record, heap_instance (record->heap), record->instance->type ());
          record = record->next;
        }
    }
}

class instance_executor_t : public executor_t
{
private:
  instance_scheduler_t& scheduler_;
  pthread_t thread_;
  stack_frame_t* stack_;
  char* mutable_phase_base_pointer_;
  instance_record_t* current_instance_;

public:
  instance_executor_t (instance_scheduler_t& s, size_t stack_size)
    : scheduler_ (s)
    , stack_ (stack_frame_make (stack_size))
    , mutable_phase_base_pointer_ (NULL)
    , current_instance_ (NULL)
  { }

  virtual stack_frame_t* stack () const { return stack_; }
  virtual heap_t* heap () const { return current_instance_->heap; }
  virtual void heap (heap_t* heap) { current_instance_->heap = heap; }
  virtual char* get_ptr () const { return current_instance_->get_ptr (); }
  virtual instance_record_t* current_instance () const { return current_instance_; }
  virtual void current_instance (instance_record_t* record)
  {
    assert (record != NULL);
    current_instance_ = record;
  }
  virtual char* mutable_phase_base_pointer () const { return mutable_phase_base_pointer_; }
  virtual void mutable_phase_base_pointer (char* ptr) { mutable_phase_base_pointer_ = ptr; }
  virtual void lock_stdout ()
  {
    pthread_mutex_lock (&scheduler_.stdout_mutex_);
  }
  virtual void unlock_stdout ()
  {
    pthread_mutex_unlock (&scheduler_.stdout_mutex_);
  }
  virtual void push (instance_record_t* record)
  {
    scheduler_.push (record);
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

  void run_i ()
  {
    for (;;)
      {
        pthread_mutex_lock (&scheduler_.list_mutex_);
        while (!(scheduler_.head_ != NULL ||
                 (scheduler_.head_ == NULL && scheduler_.pending_ == 0)))
          {
            pthread_cond_wait (&scheduler_.list_cond_, &scheduler_.list_mutex_);
          }

        if (scheduler_.head_ == NULL)
          {
            // Implies that runtime->pending == 0.
            // Signal so other threads can exit.
            pthread_cond_signal (&scheduler_.list_cond_);
            pthread_mutex_unlock (&scheduler_.list_mutex_);
            pthread_exit (NULL);
          }

        /* printf ("BEGIN schedule before pop\n"); */
        /* dump_schedule (runtime); */
        /* printf ("END schedule before pop\n"); */

        // Get an instance to execute.
        instance_record_t* record = scheduler_.head_;
        scheduler_.head_= record->next;
        if (scheduler_.head_== (instance_record_t*)1)
          {
            scheduler_.head_= NULL;
            scheduler_.tail_= &scheduler_.head_;
          }
        record->next = NULL;

        ++scheduler_.pending_;
        pthread_mutex_unlock (&scheduler_.list_mutex_);

        /* // TODO:  Comment this out. */
        /* if (rand () % 100 < 50) */
        /*   { */
        /*     push (runtime->runtime, record); */

        /*     pthread_mutex_lock (&scheduler.list_mutex); */
        /*     --scheduler.pending; */
        /*     pthread_cond_signal (&scheduler.list_cond); */
        /*     pthread_mutex_unlock (&scheduler.list_mutex); */
        /*     continue; */
        /*   } */

        /* printf ("BEGIN schedule after pop\n"); */
        /* dump_schedule (runtime); */
        /* printf ("END schedule after pop\n"); */

        // Try all the actions.
        for (instance_t::InstanceSetsType::const_iterator pos = record->instance->instance_sets.begin (),
               limit = record->instance->instance_sets.end ();
             pos != limit;
             ++pos)
          {
            /* printf ("BEGIN instances before enabled\n"); */
            /* dump_instances (runtime); */
            /* printf ("END instances before enabled\n"); */

            const action_t* action = pos->action;

            instance_record_lock (pos->set);
            if (enabled (*this, record, action, pos->iota))
              {
                assert (stack_frame_empty (stack_));
                /* printf ("BEGIN instances before execute\n"); */
                /* dump_instances (runtime); */
                /* printf ("END instances before execute\n"); */

                // Push the instance.
                stack_frame_push_pointer (stack_, record->get_ptr ());
                // Push iota.
                if (action->has_dimension ())
                  {
                    stack_frame_push_uint (stack_, pos->iota);
                  }
                // Push the instruction pointer.
                stack_frame_push_pointer (stack_, NULL);
                // Execute.
                execute (*this, action, record);
                // Pop the instruction pointer.
                stack_frame_pop_pointer (stack_);
                // Pop iota.
                if (action->has_dimension ())
                  {
                    stack_frame_pop_uint (stack_);
                  }
                // Pop the instance.
                stack_frame_pop_pointer (stack_);

                /* type_print_value (record->type, record->instance); */
                /* printf ("\n"); */

                /* printf ("BEGIN instances after execute\n"); */
                /* dump_instances (runtime); */
                /* printf ("END instances after execute\n"); */
                assert (stack_frame_empty (stack_));
              }
            instance_record_unlock (pos->set);

            /* // TODO: Comment this out. */
            /* { */
            /*   instance_t** pos; */
            /*   instance_t** limit; */
            /*   for (pos = instance_table_begin (scheduler.instance_table), limit = instance_table_end (scheduler.instance_table); */
            /*        pos != limit; */
            /*        pos = instance_table_next (pos)) */
            /*     { */
            /*       instance_t* instance = *pos; */
            /*       printf ("BEFORE FIRST\n"); */
            /*       heap_dump (instance_get_record (instance)->heap); */
            /*       printf ("BEFORE LAST\n"); */
            /*       instance_record_collect_garbage (instance_get_record (instance)); */
            /*       printf ("AFTER FIRST\n"); */
            /*       heap_dump (instance_get_record (instance)->heap); */
            /*       printf ("AFTER LAST\n"); */
            /*     } */
            /* } */

            /* printf ("BEGIN instances after enabled\n"); */
            /* dump_instances (runtime); */
            /* printf ("END instances after enabled\n"); */
          }

        // Collect garbage.
        instance_record_collect_garbage (record);
        //heap_dump (record->heap);

        pthread_mutex_lock (&scheduler_.list_mutex_);
        --scheduler_.pending_;
        pthread_cond_signal (&scheduler_.list_cond_);
        pthread_mutex_unlock (&scheduler_.list_mutex_);
      }
  }

  void join ()
  {
    pthread_join (thread_, NULL);
  }
};

#define NUM_THREADS 2

void
instance_scheduler_t::run (size_t stack_size)
{
  instance_executor_t exec (*this, stack_size);

  for (instance_table_t::InstancesType::const_iterator pos = instance_table_.instances.begin (),
         limit = instance_table_.instances.end ();
       pos != limit;
       ++pos)
    {
      instance_t* instance = pos->second;
      if (instance->is_top_level ())
        {
          char* top_before = stack_frame_top (exec.stack ());
          // Push this.
          stack_frame_push_pointer (exec.stack (), instance->record->get_ptr ());
          // Push a fake instruction pointer.
          stack_frame_push_pointer (exec.stack (), NULL);
          char* top_after = stack_frame_top (exec.stack ());
          stack_frame_push_pointer (exec.stack (), instance->method ()->node);
          call (exec);
          stack_frame_pop (exec.stack (), top_after - top_before);
        }
    }

  std::vector<instance_executor_t*> executors;
  for (size_t idx = 0; idx != NUM_THREADS; ++idx)
    {
      instance_executor_t* exec = new instance_executor_t (*this, stack_size);
      executors.push_back (exec);
      exec->spawn ();
    }

  for (size_t idx = 0; idx != NUM_THREADS; ++idx)
    {
      instance_executor_t* exec = executors[idx];
      exec->join ();
      delete exec;
    }
}
