#ifndef types_hpp
#define types_hpp

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <map>
#include <string>
#include "debug.hpp"

class action_reaction_base_t;
class action_t;
class ast_t;
class ast_function_t;
class ast_method_t;
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

struct pfunc_t {
  component_t* instance;
  union {
    const method_t* method;
    const function_t* function;
  };
};

enum BinaryArithmetic
  {
    MULTIPLY,
    DIVIDE,
    MODULUS,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    BIT_AND,
    BIT_AND_NOT,

    ADD,
    SUBTRACT,
    BIT_OR,
    BIT_XOR,

    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_EQUAL,
    MORE_THAN,
    MORE_EQUAL,

    LOGIC_OR,

    LOGIC_AND,
  };

inline const char* binary_arithmetic_symbol (BinaryArithmetic ba)
{
  switch (ba)
    {
    case MULTIPLY:
      return "*";
    case DIVIDE:
      return "/";
    case MODULUS:
      return "%";
    case LEFT_SHIFT:
      return "<<";
    case RIGHT_SHIFT:
      return ">>";
    case BIT_AND:
      return "&";
    case BIT_AND_NOT:
      return "&^";

    case ADD:
      return "+";
    case SUBTRACT:
      return "-";
    case BIT_OR:
      return "|";
    case BIT_XOR:
      return "^";

    case EQUAL:
      return "==";
    case NOT_EQUAL:
      return "!=";
    case LESS_THAN:
      return "<";
    case LESS_EQUAL:
      return "<=";
    case MORE_THAN:
      return ">";
    case MORE_EQUAL:
      return ">=";

    case LOGIC_OR:
      return "||";

    case LOGIC_AND:
      return "&&";
    }

  not_reached;
}

struct location_t
{
  const char * const file;
  unsigned int const line;

  location_t (const char* f, unsigned int l)
    : file (f)
    , line (l)
  { }
};

#endif /* types_hpp */
