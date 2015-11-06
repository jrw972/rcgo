#ifndef types_hpp
#define types_hpp

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <map>
#include "debug.hpp"

class action_reaction_base_t;
class action_t;
class ast_t;
class ast_call_expr_t;
class ast_function_t;
class ast_getter_t;
class ast_initializer_t;
class ast_instance_t;
class ast_method_t;
class bind_t;
class Callable;
class Template;
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
class Activation;

// A reference is either mutable, immutable, or foreign.
// Ordered by strictness for <.
enum Mutability
{
  MUTABLE,
  IMMUTABLE,
  FOREIGN,
};

enum ActivationMode
{
  ACTIVATION_READ,  /* Activation reads. */
  ACTIVATION_WRITE, /* Activation writes (and reads). */
};

typedef std::map<instance_t*, ActivationMode> instance_set_t;

struct pull_port_t
{
  component_t* instance;
  Getter* getter;
};

enum BinaryArithmetic
{
  Multiply,
  Divide,
  Modulus,
  LeftShift,
  RightShift,
  BitAnd,
  BitAndNot,

  Add,
  Subtract,
  BitOr,
  BitXor,

  Equal,
  NotEqual,
  LessThan,
  LessEqual,
  MoreThan,
  MoreEqual,

  LogicOr,

  LogicAnd,
};

inline const char* binary_arithmetic_symbol (BinaryArithmetic ba)
{
  switch (ba)
    {
    case Multiply:
      return "*";
    case Divide:
      return "/";
    case Modulus:
      return "%";
    case LeftShift:
      return "<<";
    case RightShift:
      return ">>";
    case BitAnd:
      return "&";
    case BitAndNot:
      return "&^";

    case Add:
      return "+";
    case Subtract:
      return "-";
    case BitOr:
      return "|";
    case BitXor:
      return "^";

    case Equal:
      return "==";
    case NotEqual:
      return "!=";
    case LessThan:
      return "<";
    case LessEqual:
      return "<=";
    case MoreThan:
      return ">";
    case MoreEqual:
      return ">=";

    case LogicOr:
      return "||";

    case LogicAnd:
      return "&&";
    }

  not_reached;
}

#endif /* types_hpp */
