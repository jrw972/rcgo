#ifndef RC_SRC_SEMANTIC_HPP
#define RC_SRC_SEMANTIC_HPP

#include <error.h>

#include <utility>

#include "types.hpp"
#include "symbol_visitor.hpp"
#include "symbol_table.hpp"

namespace semantic
{

using namespace std::rel_ops;

struct Multiplier
{
  template <typename T>
  T operator() (const T& x, const T& y) const
  {
    return x * y;
  }
};

struct Divider
{
  template <typename T>
  T operator() (const T& x, const T& y) const
  {
    return x / y;
  }
};

struct Modulizer
{
  template <typename T>
  T operator() (const T& x, const T& y) const
  {
    return x % y;
  }
};

struct LeftShifter
{
  template <typename T, typename U>
  T operator() (const T& x, const U& y) const
  {
    return x << y;
  }
};

struct RightShifter
{
  template <typename T, typename U>
  T operator() (const T& x, const U& y) const
  {
    return x >> y;
  }
};

struct BitAnder
{
  template <typename T>
  T operator() (const T& x, const T& y) const
  {
    return x & y;
  }
};

struct BitAndNotter
{
  template <typename T>
  T operator() (const T& x, const T& y) const
  {
    return x & (~y);
  }
};

struct Adder
{
  template <typename T>
  T operator() (const T& x, const T& y) const
  {
    return x + y;
  }
};

struct Subtracter
{
  template <typename T>
  T operator() (const T& x, const T& y) const
  {
    return x - y;
  }
};

struct BitOrer
{
  template <typename T>
  T operator() (const T& x, const T& y) const
  {
    return x | y;
  }
};

struct BitXorer
{
  template <typename T>
  T operator() (const T& x, const T& y) const
  {
    return x ^ y;
  }
};

struct Equalizer
{
  template <typename T>
  bool operator() (const T& x, const T& y) const
  {
    return x == y;
  }
};

struct NotEqualizer
{
  template <typename T>
  bool operator() (const T& x, const T& y) const
  {
    return x != y;
  }
};

struct LessThaner
{
  template <typename T>
  bool operator() (const T& x, const T& y) const
  {
    return x < y;
  }
};

struct LessEqualizer
{
  template <typename T>
  bool operator() (const T& x, const T& y) const
  {
    return x <= y;
  }
};

struct MoreThaner
{
  template <typename T>
  bool operator() (const T& x, const T& y) const
  {
    return x > y;
  }
};

struct MoreEqualizer
{
  template <typename T>
  bool operator() (const T& x, const T& y) const
  {
    return x >= y;
  }
};

template <typename T>
struct LogicNotter
{
  typedef T ValueType;
  bool operator() (const T& x) const
  {
    return !x;
  }
};

template <typename T>
struct Negater
{
  typedef T ValueType;
  T operator() (const T& x) const
  {
    return -x;
  }
};

// Enter builtin types, constants, etc.
void enter_symbols (decl::SymbolTable& symtab);

// Look up a symbol.  If it is not defined, process its definition.
template<typename T>
T* processAndLookup (decl::SymbolTable& symtab, const std::string& identifier, const util::Location& location)
{
  decl::Symbol *symbol = symtab.find_global_symbol (identifier);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "%s was not declared in this scope (E114)", identifier.c_str ());
    }
  if (symbol->inProgress)
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "%s is defined recursively (E115)", identifier.c_str ());
    }
  if (!symbol->defined ())
    {
      // Process the definition.
      UNIMPLEMENTED;
    }

  return decl::SymbolCast<T> (symbol);
}

// Extract an array dimension or error.
type::Int::ValueType process_array_dimension (ast::Node* ptr, decl::SymbolTable& symtab);

// Check that a signature has +foreign where needed.
void CheckForForeignSafe (const type::Signature* signature, const decl::ParameterSymbol* return_parameter);

// Process a type specification.
const type::Type * process_type (ast::Node* node, decl::SymbolTable& symtab, bool force);

// Type check the expression, insert an implicit dereference if necessary, and convert to the given type if necessary.
typed_Value
CheckAndImplicitlyDereferenceAndConvert (ast::Node*& expr, const type::Type* type);

void
check_assignment (typed_Value left_tv,
                  typed_Value right_tv,
                  const ast::Node& node,
                  const char* conversion_message,
                  const char* leak_message);

void
allocate_stack_variables (ast::Node* node);

decl::Method*
get_current_method (const ast::Node * node);

ReceiverAccess ComputeReceiverAccess (const ast::Node* node);

void
allocate_symbol (runtime::MemoryModel& memory_model,
                 decl::Symbol* symbol);

void
allocate_parameters (runtime::MemoryModel& memory_model,
                     const type::Signature* signature);

}

#endif // RC_SRC_SEMANTIC_HPP
