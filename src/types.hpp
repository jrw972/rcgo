#ifndef types_h
#define types_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <map>
#include <string>

class action_reaction_base_t;
class action_t;
class ast_t;
class bind_t;
typedef struct component_t component_t;
typedef struct field_t field_t;
typedef struct function_t function_t;
typedef struct method_t method_t;
typedef struct heap_t heap_t;
typedef struct instance_t instance_t;
typedef struct instance_table_t instance_table_t;
typedef struct memory_model_t memory_model_t;
typedef struct parameter_t parameter_t;
class port_t;
class reaction_t;
class scheduler_t;
typedef struct stack_frame_t stack_frame_t;
typedef struct symbol_t symbol_t;
typedef struct symtab_t symtab_t;
typedef struct trigger_t trigger_t;
typedef struct type_t type_t;

// A reference is either mutable, immutable, or foreign.
enum Mutability
  {
    MUTABLE,
    IMMUTABLE,
    FOREIGN,
  };

enum TriggerAction
  {
    /* TODO:  Add TRIGGER_NONE. */
    TRIGGER_READ,  /* Trigger reads. */
    TRIGGER_WRITE, /* Trigger writes (and reads). */
  };

typedef std::map<instance_t*, TriggerAction> instance_set_t;

#endif /* types_h */
