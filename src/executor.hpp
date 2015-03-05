#ifndef executor_hpp
#define executor_hpp

class executor_t {
public:
  virtual ~executor_t () { }
  virtual stack_frame_t* stack () const = 0;
  virtual heap_t* heap () const = 0;
  virtual void heap (heap_t* heap) = 0;
  virtual char* get_ptr () const = 0;
  virtual instance_record_t* current_instance () const = 0;
  virtual void current_instance (instance_record_t* record) = 0;
  virtual char* mutable_phase_base_pointer () const = 0;
  virtual void mutable_phase_base_pointer (char* ptr) = 0;
  virtual void lock_stdout () = 0;
  virtual void unlock_stdout () = 0;
  virtual void push (instance_record_t* record) = 0;
};

#endif /* executor_hpp */
