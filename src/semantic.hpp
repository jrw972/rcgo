#ifndef RC_SRC_SEMANTIC_HPP
#define RC_SRC_SEMANTIC_HPP

#include <error.h>

#include <utility>

#include "types.hpp"
#include "symbol_visitor.hpp"
#include "symbol_cast.hpp"
#include "symbol_table.hpp"
#include "template.hpp"

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

  static runtime::Operation*
  generate_code (const ExpressionValue& result,
                 const ExpressionValue& left_val,
                 runtime::Operation* left_op,
                 const ExpressionValue& right_val,
                 runtime::Operation* right_op);
};

struct RightShifter
{
  template <typename T, typename U>
  T operator() (const T& x, const U& y) const
  {
    return x >> y;
  }

  static runtime::Operation*
  generate_code (const ExpressionValue& result,
                 const ExpressionValue& left_val,
                 runtime::Operation* left_op,
                 const ExpressionValue& right_val,
                 runtime::Operation* right_op);
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
      error_at_line (-1, 0, location.file.c_str (), location.line,
                     "%s was not declared in this scope (E114)", identifier.c_str ());
    }
  if (symbol->inProgress)
    {
      error_at_line (-1, 0, location.file.c_str (), location.line,
                     "%s is defined recursively (E115)", identifier.c_str ());
    }
  if (!symbol->defined ())
    {
      // Process the definition.
      UNIMPLEMENTED;
    }

  return decl::symbol_cast<T> (symbol);
}

ExpressionValueList collect_evals (ast::Node* node);

// Extract an array dimension or error.
type::Int::ValueType process_array_dimension (ast::Node* ptr, util::ErrorReporter& er, decl::SymbolTable& symtab);

// Check that a signature has +foreign where needed.
void CheckForForeignSafe (util::ErrorReporter& er,
                          const decl::ParameterList* parameter_list,
                          const decl::ParameterList* return_parameter_list);

// Process a type specification.
const type::Type * process_type (ast::Node* node, util::ErrorReporter& er, decl::SymbolTable& symtab, bool force);

// Process a signature.
const decl::ParameterList*
process_parameter_list (ast::Node* node,
                        util::ErrorReporter& er,
                        decl::SymbolTable& symtab,
                        bool is_return);

void
allocate_stack_variables (ast::Node* node);

decl::Method*
get_current_method (const ast::Node * node);

void
allocate_symbol (runtime::MemoryModel& memory_model,
                 decl::Symbol* symbol);

void
allocate_parameters (runtime::MemoryModel& memory_model,
                     const decl::ParameterList* signature);

bool
require_value_or_variable (util::ErrorReporter& er,
                           const util::Location& location,
                           ExpressionValue& result,
                           const ExpressionValue& arg);

bool
require_type (util::ErrorReporter& er,
              const util::Location& location,
              ExpressionValue& result,
              const ExpressionValue& arg);

struct LogicNot : public decl::Template
{
  virtual void check (util::ErrorReporter& er,
                      const util::Location& location,
                      ExpressionValue& result,
                      ExpressionValueList& arguments) const;
  virtual runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& arg_val,
      runtime::Operation* arg_op) const;
};

struct Posate : public decl::Template
{
  virtual void check (util::ErrorReporter& er,
                      const util::Location& location,
                      ExpressionValue& result,
                      ExpressionValueList& arguments) const;
  virtual runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& arg_val,
      runtime::Operation* arg_op) const
  {
    UNIMPLEMENTED;
  }
};

struct Negate : public decl::Template
{
  virtual void check (util::ErrorReporter& er,
                      const util::Location& location,
                      ExpressionValue& result,
                      ExpressionValueList& arguments) const;
  virtual runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& arg_val,
      runtime::Operation* arg_op) const
  {
    UNIMPLEMENTED;
  }
};

struct Complement : public decl::Template
{
  virtual void check (util::ErrorReporter& er,
                      const util::Location& location,
                      ExpressionValue& result,
                      ExpressionValueList& arguments) const;
  virtual runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& arg_val,
      runtime::Operation* arg_op) const
  {
    UNIMPLEMENTED;
  }
};

struct PassThroughPicker
{
  static const type::Type* pick (const type::Type* input_type,
                                 const ExpressionValue& left,
                                 const ExpressionValue& right);
};

struct BooleanPicker
{
  static const type::Type* pick (const type::Type* input_type,
                                 const ExpressionValue& left,
                                 const ExpressionValue& right);
};

template <typename Op>
struct BinaryValueComputer
{
  static void compute (ExpressionValue& result,
                       const type::Type* in_type,
                       const ExpressionValue& left,
                       const ExpressionValue& right);
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

struct LogicOrComputer
{
  static void compute (ExpressionValue& result,
                       const type::Type* in_type,
                       const ExpressionValue& left,
                       const ExpressionValue& right);
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

struct LogicAndComputer
{
  static void compute (ExpressionValue& result,
                       const type::Type* in_type,
                       const ExpressionValue& left,
                       const ExpressionValue& right);
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

template <typename InputPicker, typename OutputPicker, typename Computer, ::BinaryArithmetic ba>
struct BinaryArithmetic : public decl::Template
{
  virtual void check (util::ErrorReporter& er,
                      const util::Location& loc,
                      ExpressionValue& result,
                      ExpressionValueList& arguments) const;
  virtual runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op) const;

};

template <typename B, ::BinaryArithmetic ba>
struct BinaryShift : public decl::Template
{
  virtual void check (util::ErrorReporter& er,
                      const util::Location& loc,
                      ExpressionValue& result,
                      ExpressionValueList& arguments) const;
  virtual runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op) const;
};

struct SMultiply
{
  void operator() (Value&, const type::Type*, const Value&, const Value&) const;
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

typedef BinaryArithmetic<type::Arithmetic, PassThroughPicker, BinaryValueComputer<SMultiply>, ::Multiply> Multiply;

struct SDivide
{
  void operator() (Value&, const type::Type*, const Value&, const Value&) const;
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

typedef BinaryArithmetic<type::Arithmetic, PassThroughPicker, BinaryValueComputer<SDivide>, ::Divide> Divide;

struct SModulus
{
  void operator() (Value&, const type::Type*, const Value&, const Value&) const;
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

typedef BinaryArithmetic<type::Integral, PassThroughPicker, BinaryValueComputer<SModulus>, ::Modulus> Modulus;

typedef BinaryShift<LeftShifter, ::LeftShift> LeftShift;
typedef BinaryShift<RightShifter, ::RightShift> RightShift;

struct SBitAnd
{
  void operator() (Value&, const type::Type*, const Value&, const Value&) const;
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

typedef BinaryArithmetic<type::Integral, PassThroughPicker, BinaryValueComputer<SBitAnd>, ::BitAnd> BitAnd;

struct SBitAndNot
{
  void operator() (Value&, const type::Type*, const Value&, const Value&) const;
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

typedef BinaryArithmetic<type::Integral, PassThroughPicker, BinaryValueComputer<SBitAndNot>, ::BitAndNot> BitAndNot;

struct SAdd
{
  void operator() (Value&, const type::Type*, const Value&, const Value&) const;
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

typedef BinaryArithmetic<type::Arithmetic, PassThroughPicker, BinaryValueComputer<SAdd>, ::Add> Add;

struct SSubtract
{
  void operator() (Value&, const type::Type*, const Value&, const Value&) const;
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

typedef BinaryArithmetic<type::Arithmetic, PassThroughPicker, BinaryValueComputer<SSubtract>, ::Subtract> Subtract;

struct SBitOr
{
  void operator() (Value&, const type::Type*, const Value&, const Value&) const;
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

typedef BinaryArithmetic<type::Integral, PassThroughPicker, BinaryValueComputer<SBitOr>, ::BitOr> BitOr;

struct SBitXor
{
  void operator() (Value&, const type::Type*, const Value&, const Value&) const;
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

typedef BinaryArithmetic<type::Integral, PassThroughPicker, BinaryValueComputer<SBitXor>, ::BitXor> BitXor;

struct SEqual
{
  void operator() (Value&, const type::Type*, const Value&, const Value&) const;
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

typedef BinaryArithmetic<type::Comparable, BooleanPicker, BinaryValueComputer<SEqual>, ::Equal> Equal;

struct SNotEqual
{
  void operator() (Value&, const type::Type*, const Value&, const Value&) const;
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

typedef BinaryArithmetic<type::Comparable, BooleanPicker, BinaryValueComputer<SNotEqual>, ::NotEqual> NotEqual;

struct SLessThan
{
  void operator() (Value&, const type::Type*, const Value&, const Value&) const;
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

typedef BinaryArithmetic<type::Orderable, BooleanPicker, BinaryValueComputer<SLessThan>, ::LessThan> LessThan;

struct SLessEqual
{
  void operator() (Value&, const type::Type*, const Value&, const Value&) const;
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

typedef BinaryArithmetic<type::Orderable, BooleanPicker, BinaryValueComputer<SLessEqual>, ::LessEqual> LessEqual;

struct SMoreThan
{
  void operator() (Value&, const type::Type*, const Value&, const Value&) const;
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

typedef BinaryArithmetic<type::Orderable, BooleanPicker, BinaryValueComputer<SMoreThan>, ::MoreThan> MoreThan;

struct SMoreEqual
{
  void operator() (Value&, const type::Type*, const Value&, const Value&) const;
  static runtime::Operation* generate_code (const ExpressionValue& result,
      const ExpressionValue& left_val,
      runtime::Operation* left_op,
      const ExpressionValue& right_val,
      runtime::Operation* right_op);
};

typedef BinaryArithmetic<type::Orderable, BooleanPicker, BinaryValueComputer<SMoreEqual>, ::MoreEqual> MoreEqual;

typedef BinaryArithmetic<type::Logical, BooleanPicker, LogicOrComputer, ::LogicOr> LogicOr;
typedef BinaryArithmetic<type::Logical, BooleanPicker, LogicAndComputer, ::LogicAnd> LogicAnd;

struct New : public decl::TemplateSymbol
{
  New (const util::Location& loc);
  virtual void check (util::ErrorReporter& er,
                      const util::Location& loc,
                      ExpressionValue& result,
                      ExpressionValueList& arguments) const;
  virtual void compute_receiver_access (const semantic::ExpressionValueList& args,
                                        ReceiverAccess& receiver_access,
                                        bool& flag) const;
  virtual runtime::Operation* generate_code (const semantic::ExpressionValue& result,
      const semantic::ExpressionValueList& arg_vals,
      runtime::Operation* arg_ops) const;
};

struct Move : public decl::TemplateSymbol
{
  Move (const util::Location& loc);
  virtual void check (util::ErrorReporter& er,
                      const util::Location& loc,
                      ExpressionValue& result,
                      ExpressionValueList& arguments) const;
  virtual void compute_receiver_access (const semantic::ExpressionValueList& args,
                                        ReceiverAccess& receiver_access,
                                        bool& flag) const;
  virtual runtime::Operation* generate_code (const semantic::ExpressionValue& result,
      const semantic::ExpressionValueList& arg_vals,
      runtime::Operation* arg_ops) const;
};

struct Merge : public decl::TemplateSymbol
{
  Merge (const util::Location& loc);
  virtual void check (util::ErrorReporter& er,
                      const util::Location& loc,
                      ExpressionValue& result,
                      ExpressionValueList& arguments) const;
  virtual void compute_receiver_access (const semantic::ExpressionValueList& args,
                                        ReceiverAccess& receiver_access,
                                        bool& flag) const;
  virtual runtime::Operation* generate_code (const semantic::ExpressionValue& result,
      const semantic::ExpressionValueList& arg_vals,
      runtime::Operation* arg_ops) const;
};

struct Len : public decl::TemplateSymbol
{
  Len (const util::Location& loc);
  virtual void check (util::ErrorReporter& er,
                      const util::Location& loc,
                      ExpressionValue& result,
                      ExpressionValueList& arguments) const;
  virtual runtime::Operation* generate_code (const semantic::ExpressionValue& result,
      const semantic::ExpressionValueList& arg_vals,
      runtime::Operation* arg_ops) const;
};

struct Append : public decl::TemplateSymbol
{
  Append (const util::Location& loc);
  virtual void check (util::ErrorReporter& er,
                      const util::Location& loc,
                      ExpressionValue& result,
                      ExpressionValueList& arguments) const;
  virtual runtime::Operation* generate_code (const semantic::ExpressionValue& result,
      const semantic::ExpressionValueList& arg_vals,
      runtime::Operation* arg_ops) const;
};

struct Copy : public decl::TemplateSymbol
{
  Copy (const util::Location& loc);
  virtual void check (util::ErrorReporter& er,
                      const util::Location& loc,
                      ExpressionValue& result,
                      ExpressionValueList& arguments) const;
  virtual void compute_receiver_access (const semantic::ExpressionValueList& args,
                                        ReceiverAccess& receiver_access,
                                        bool& flag) const;
  virtual runtime::Operation* generate_code (const semantic::ExpressionValue& result,
      const semantic::ExpressionValueList& arg_vals,
      runtime::Operation* arg_ops) const;
};

struct Println : public decl::TemplateSymbol
{
  Println (const util::Location& loc);
  virtual void check (util::ErrorReporter& er,
                      const util::Location& loc,
                      ExpressionValue& result,
                      ExpressionValueList& arguments) const;
  virtual void compute_receiver_access (const semantic::ExpressionValueList& args,
                                        ReceiverAccess& receiver_access,
                                        bool& flag) const;
  virtual runtime::Operation* generate_code (const semantic::ExpressionValue& result,
      const semantic::ExpressionValueList& arg_vals,
      runtime::Operation* arg_ops) const;
};

extern Posate posate_temp;
extern Negate negate_temp;
extern LogicNot logic_not_temp;
extern Complement complement_temp;

extern Multiply multiply_temp;
extern Divide divide_temp;
extern Modulus modulus_temp;
extern LeftShift left_shift_temp;
extern RightShift right_shift_temp;
extern BitAnd bit_and_temp;
extern BitAndNot bit_and_not_temp;
extern Add add_temp;
extern Subtract subtract_temp;
extern BitOr bit_or_temp;
extern BitXor bit_xor_temp;
extern Equal equal_temp;
extern NotEqual not_equal_temp;
extern LessThan less_than_temp;
extern LessEqual less_equal_temp;
extern MoreThan more_than_temp;
extern MoreEqual more_equal_temp;
extern LogicOr logic_or_temp;
extern LogicAnd logic_and_temp;

}

#endif // RC_SRC_SEMANTIC_HPP
