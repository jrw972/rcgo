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
class Array;
class Slice;
class Map;
class EmptyType;
class FieldList;
class Heap;
class VariableList;
class IdentifierType;
class Pointer;
class PushPort;
class PullPort;
class ParameterList;
class TypeExpression;
class BinaryArithmetic;
class AddressOf;
class Call;
class Conversion;
class Dereference;
class IdentifierExpression;
class Index;
class IndexSlice;
class EmptyExpression;
class IndexedPushPortCall;
class ExpressionList;
class Literal;
class UnaryArithmetic;
class PushPortCall;
class Select;
class EmptyStatement;
class AddAssign;
class Change;
class Assign;
class ExpressionStatement;
class If;
class While;
class StatementList;
class Return;
class IncrementDecrement;
class SubtractAssign;
class Activate;
class Var;
class BindPushPort;
class BindPushPortParameter;
class BindPullPort;
class ForIota;
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
class UntypedComplex;

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
class Callable;
class ConstSymbolVisitor;
class Constant;
class Function;
class FunctionBase;
class Getter;
class Hidden;
class Initializer;
class Instance;
class Method;
class Package;
class ParameterList;
class Parameter;
class PolymorphicFunction;
class Reaction;
class Symbol;
class SymbolTable;
class SymbolVisitor;
class Variable;

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
class ComponentInfoBase;
class ExecutorBase;
class FileDescriptor;
class Heap;
class ListOperation;
class MemoryModel;
class Operation;
class Stack;
}

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

enum UnaryArithmetic
{
  LogicNot,
  Posate,
  Negate,
  Complement,
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

#endif // RC_SRC_TYPES_HPP
