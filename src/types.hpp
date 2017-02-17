#ifndef RC_SRC_TYPES_HPP
#define RC_SRC_TYPES_HPP

// This files contains declarations of public types.

#include <stddef.h>
#include <stdint.h>

#include <vector>
#include <set>
#include <map>

#include "debug.hpp"

namespace util
{
class ErrorReporter;
}

// The interpretation process is driven by a set of import paths.  The
// import paths are processed to create a set of packages.  Each
// package consists of a set of source files.
namespace source
{
class PackageSet;
class Package;
class SourceFile;
class Location;
class Identifier;
}

// The top-level constructs that may be declared (and defined) in a
// source file include actions, binders, constants, functions,
// getters, initializers, instances, methods, reactions, and types.
// Types are handled in the type namespace.  Parameters are declared
// for functions and methods and variable may declared in bodies of
// functions and methods.  The common idea is that these elements have
// names and are subject to symbol processing.
namespace decl
{
class Action;
class Bind;  // TODO:  Rename to binder.
class Callable;
class Constant;
class Field; // TODO:  Move to type.
class Function;
class FunctionBase;
class Getter;
class Hidden; // TODO:  Get rid of this.
class ImportedSymbol;
class Initializer;
class Instance;
class MethodBase;
class Method;
class Package;
class ParameterList;
class Parameter;
class PolymorphicFunction;
class PolymorphicFunctionVisitor;
class Reaction;
class Variable;

class Scope;
class Symbol;
class SymbolTable;
class SymbolVisitor;
class ConstSymbolVisitor;
}

// Abstract Syntax Tree for RCGo code.
namespace ast
{
class Node;
class List;

class ActionDecl;
class Activate;
class AddAssign;
class AddressOf;
class Array;
class Assign;
class BinderDecl;
class BindPullPort;
class BindPushPort;
class BindPushPortParameter;
class Call;
class Change;
class CompositeLiteral;
class ConstDecl;
class Conversion;
class Dereference;
class Element;
class ElementList;
class EmptyExpression;
class EmptyStatement;
class EmptyType;
class ExpressionList;
class ExpressionStatement;
class FieldList;
class ForIota;
class FunctionDecl;
class GetterDecl;
class Heap;
class IdentifierExpression;
class IdentifierList;
class IdentifierType;
class If;
class IncrementDecrement;
class Index;
class IndexSlice;
class IndexedPushPortCall;
class InitializerDecl;
class InstanceDecl;
class Literal;
class Map;
class MethodDecl;
class ParameterList;
class Pointer;
class PullPort;
class PushPort;
class PushPortCall;
class ReactionDecl;
class Receiver;
class Return;
class Select;
class Slice;
class StatementList;
class SubtractAssign;
  class TopLevelDeclList;
class TypeDecl;
class TypeExpression;
class VarDecl;
class VariableList;
class While;

class NodeVisitor;
class DefaultNodeVisitor;
}

// Type descriptors.
namespace type
{
class Array;
class Component;
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
class String;
class Struct;
class Type;
class UntypedBoolean;
class UntypedComplex;
class UntypedFloat;
class UntypedInteger;
class UntypedNil;
class UntypedRune;
class UntypedString;

typedef std::set<std::string> TagSet;
}

// Classes for performing semantic analysis.
namespace semantic
{
class Value;
class ExpressionValue;
class UntypedComplex;
}

// Class used during the analysis of composition.
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

// Classes for the runtime.
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
// TODO: Move to semantic.
enum Mutability
{
  Mutable,
  Immutable,
  Foreign,
  Unknown_Mutability,
};

inline std::ostream& operator<< (std::ostream& out, Mutability m)
{
  switch (m)
    {
    case Mutable:
      out << "Mutable";
      break;
    case Immutable:
      out << "Immutable";
      break;
    case Foreign:
      out << "Foreign";
      break;
    case Unknown_Mutability:
      out << "Unknown";
      break;
    }
  return out;
}

enum ReceiverAccess
{
  AccessNone,
  AccessRead,
  AccessWrite,
};

#endif // RC_SRC_TYPES_HPP
