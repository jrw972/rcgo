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
class ast_call_expr_t;
class ast_function_t;
class ast_getter_t;
class ast_initializer_t;
class ast_method_t;
class bind_t;
class Callable;
class component_t;
class executor_base_t;
class field_t;
class Function;
class Getter;
class Initializer;
class Method;
typedef struct heap_t heap_t;
typedef struct instance_t instance_t;
typedef struct instance_table_t instance_table_t;
typedef struct parameter_t parameter_t;
class port_t;
class reaction_t;
class scheduler_t;
typedef struct stack_frame_t stack_frame_t;
class Symbol;
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

struct pull_port_t {
  component_t* instance;
  Getter* getter;
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

struct Location
{
   static std::string StaticFile;
   std::string const File;
   unsigned int const Line;

   Location ()
      : File (StaticFile)
      , Line (0)
   { }

   Location (unsigned int aLine)
      : File (StaticFile)
      , Line (aLine)
   { }
};

#endif /* types_hpp */
