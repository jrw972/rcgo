#ifndef instance_scheduler_hpp
#define instance_scheduler_hpp

#include "types.hpp"
#include "scheduler.hpp"
#include <pthread.h>

class instance_scheduler_t : public scheduler_t
{
public:
  instance_scheduler_t (instance_table_t& it)
    : instance_table_ (it)
    , head_ (NULL)
    , tail_ (&head_)
    , pending_ (0)
  {
    pthread_mutex_init (&list_mutex_, NULL);
    pthread_cond_init (&list_cond_, NULL);
    pthread_mutex_init (&stdout_mutex_, NULL);
  }

  void allocate_instances (void);
  void create_bindings (void);
  void run (size_t stack_size);
  void dump_schedule () const;

private:
  void push (instance_record_t* record);

  const instance_table_t& instance_table_;
  instance_record_t* head_;
  instance_record_t** tail_;
  size_t pending_;
  pthread_mutex_t list_mutex_;
  pthread_cond_t list_cond_;
  pthread_mutex_t stdout_mutex_;

  friend class instance_executor_t;
};

#endif /* instance_scheduler_hpp */
