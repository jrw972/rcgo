#ifndef RC_SRC_TYPES_HPP
#define RC_SRC_TYPES_HPP

#include <stddef.h>
#include <stdint.h>

#include <vector>
#include <set>
#include <map>

#include "debug.hpp"

namespace util
{
class Location;
class ErrorReporter;
}

namespace ast
{
class Node;
class List;

class Identifier;
class IdentifierList;
class Receiver;
class ArrayTypeSpec;
class SliceTypeSpec;
class MapTypeSpec;
class EmptyTypeSpec;
class FieldListTypeSpec;
class HeapTypeSpec;
class IdentifierListTypeSpec;
class IdentifierTypeSpec;
class PointerTypeSpec;
class PushPortTypeSpec;
class PullPortTypeSpec;
class ParameterList;
class TypeExpression;
class BinaryArithmeticExpr;
class AddressOfExpr;
class CallExpr;
class ConversionExpr;
class DereferenceExpr;
class IdentifierExpr;
class IndexExpr;
class SliceExpr;
class EmptyExpr;
class IndexedPushPortCallExpr;
class ListExpr;
class LiteralExpr;
class UnaryArithmeticExpr;
class PushPortCallExpr;
class SelectExpr;
class EmptyStatement;
class AddAssignStatement;
class ChangeStatement;
class AssignStatement;
class ExpressionStatement;
class IfStatement;
class WhileStatement;
class ListStatement;
class ReturnStatement;
class IncrementDecrementStatement;
class SubtractAssignStatement;
class ActivateStatement;
class VarStatement;
class BindPushPortStatement;
class BindPushPortParamStatement;
class BindPullPortStatement;
class ForIotaStatement;
class Action;
class Const;
class DimensionedAction;
class Bind;
class Function;
class Getter;
class Initializer;
class Instance;
class Method;
class Reaction;
class DimensionedReaction;
class Type;
class SourceFile;
class ElementList;
class Element;
class CompositeLiteral;

class NodeVisitor;
class DefaultNodeVisitor;
}

namespace semantic
{
class Value;
class ExpressionValue;

enum ExpressionKind
{
  UnknownExpressionKind,
  ErrorExpressionKind,
  ValueExpressionKind,
  VariableExpressionKind,
  TypeExpressionKind,
  VoidExpressionKind,
};

}

namespace type
{
class Array;
class Field;
class Function;
class Getter;
class Heap;
class Initializer;
class Interface;
class Map;
class Method;
class NamedType;
class Pointer;
class PullPort;
class PushPort;
class Reaction;
class Slice;
class Struct;
class Type;
class UntypedString;
class Void;

typedef std::set<std::string> TagSet;
}

namespace decl
{
class Action;
class Bind;
class BuiltinFunction;
class Callable;
class ConstantSymbol;
class Function;
class Getter;
class HiddenSymbol;
class Initializer;
class InstanceSymbol;
class Method;
class Package;
class ParameterList;
class ParameterSymbol;
class Reaction;
class Symbol;
class SymbolTable;
class Template;
class TemplateSymbol;
class TypeSymbol;
class VariableSymbol;

typedef std::vector<const type::Type*> TypeList;
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
class ExecutorBase;
}

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

inline const char* unary_arithmetic_external_symbol (UnaryArithmetic ua)
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

inline const char* binary_arithmetic_external_symbol (BinaryArithmetic ba)
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
