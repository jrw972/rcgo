#ifndef runtime_hpp
#define runtime_hpp

#include "types.hpp"
#include "executor.hpp"

void bind (port_t** output_port,
           void* input_instance,
           const reaction_t* reaction,
           int64_t parameter);

bool
enabled (executor_t& exec,
         instance_record_t* record,
         const action_t* action,
         size_t iota);

void
execute (executor_t& exec,
         const action_t* action,
         instance_record_t* instance);

void
call (executor_t& exec);

#endif /* runtime_hpp */
