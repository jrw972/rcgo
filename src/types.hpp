#ifndef RC_SRC_TYPES_HPP
#define RC_SRC_TYPES_HPP

#include <stddef.h>
#include <stdint.h>

#include <map>
#include <vector>

#include "debug.hpp"

namespace ast
{
class ast_receiver_t;
class ast_activate_statement_t;
class Node;
class ast_call_expr_t;
class ast_function_t;
class ast_getter_t;
class ast_initializer_t;
class ast_instance_t;
class ast_method_t;
class ast_list_expr_t;

class Visitor;
}

namespace semantic
{
class Value;
}

namespace type
{
class Type;
class NamedType;
class Signature;
class field_t;
}

namespace decl
{
class Action;
class reaction_t;
class Template;
class Symbol;
class ParameterSymbol;
class VariableSymbol;
class TypeSymbol;
class ConstantSymbol;
class InstanceSymbol;
class Callable;
class Function;
class Getter;
class Initializer;
class Method;
class bind_t;
}

namespace composition
{
struct Composer;
struct Instance;
struct Node;
struct Action;
struct Reaction;
struct Activation;
struct PushPort;
struct PullPort;
struct Getter;
}

namespace runtime
{
class Operation;
class Stack;
class MemoryModel;
class Heap;
class executor_base_t;
}

enum ExpressionKind
{
  kValue,
  kVariable,
  kType
};

typedef std::vector<const type::Type*> TypeList;

class typed_Value;
class component_t;
class port_t;
class scheduler_t;

// A reference is either mutable, immutable, or foreign.
// Ordered by strictness for <.
enum Mutability
{
  Mutable,
  Immutable,
  Foreign,
};

enum ReceiverAccess
{
  AccessNone,
  AccessRead,
  AccessWrite,
};

struct pull_port_t
{
  component_t* instance;
  decl::Getter* getter;
};

enum UnaryArithmetic
{
  LogicNot,
  Posate,
  Negate,
  Complement,
};

inline const char* unary_arithmetic_symbol (UnaryArithmetic ua)
{
  switch (ua)
    {
    case LogicNot:
      return "!";
    case Posate:
      return "+";
    case Negate:
      return "-";
    case Complement:
      return "^";
    }

  NOT_REACHED;
}

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

  NOT_REACHED;
}

#endif // RC_SRC_TYPES_HPP
