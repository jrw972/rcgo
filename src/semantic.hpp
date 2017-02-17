#ifndef RCGO_SRC_SEMANTIC_HPP
#define RCGO_SRC_SEMANTIC_HPP

#include "types.hpp"
// Borrow some types from runtime.
#include "runtime_types.hpp"
#include "location.hpp"

#include <cstddef>
#include <map>

namespace semantic
{

// Enter types, constants, functions, and instances.
void populate_package_block (ast::Node* root,
                             util::ErrorReporter& er,
                             decl::SymbolTable& symbol_table);

// Enter imported packages.
void generate_symbols (ast::Node* root,
                       util::ErrorReporter& er,
                       decl::SymbolTable& symbol_table);

// Enter method like identifiers into the corresponding named types.
void enter_method_identifiers (ast::Node* root, util::ErrorReporter& er, decl::SymbolTable& symbol_table);

// Process constants, types, functions, methods, initializers, getters,
// actions, reactions, binders, and instances.
void process_top_level_declarations (ast::Node* root, util::ErrorReporter& er, decl::SymbolTable& symbol_table);

// Process an array dimension.  Returns -1 on error.
long
process_array_dimension (util::ErrorReporter& er,
                         decl::SymbolTable& symbol_table,
                         ast::Node* ptr);

// Check that all parameters in a parameter list have foreign indirection mutability if necessary.
void
check_for_foreign_safe (util::ErrorReporter& er,
                        const decl::ParameterList* parameter_list);

// Process a receiver.
decl::Parameter*
process_receiver (util::ErrorReporter& er,
                  decl::SymbolTable& symbol_table,
                  const type::NamedType* named_type,
                  ast::Receiver* node,
                  bool require_component,
                  bool require_immutable_dereference_mutability);

// Process a signature.
const decl::ParameterList*
process_parameter_list (util::ErrorReporter& er,
                        decl::SymbolTable& symbol_table,
                        ast::ParameterList* node,
                        bool is_return);

void
process_signature (util::ErrorReporter& er,
                   decl::SymbolTable& symbol_table,
                   ast::ParameterList* parameter_list_node,
                   ast::ParameterList* return_parameter_list_node,
                   bool require_foreign_safe,
                   const decl::ParameterList*& parameter_list,
                   const decl::ParameterList*& return_parameter_list);

void
check_unique_parameters (util::ErrorReporter& er,
                         const type::Function* type);

// Process a type specification.
// Pre:  node->eval.expression_kind == UnknownExpressionKind
// Post: node->eval.expression_kind is one of ErrorExpressionKind, TypeExpressionKind, or EmptyTypeExpressionKind
//       If TypeExpressionKind, then node->eval.type contains the type.
void
process_type (ast::Node* node,
              util::ErrorReporter& er,
              decl::SymbolTable& symbol_table,
              bool require_named_types_to_be_defined = true);

struct UntypedComplex
{
  double real;
  double imag;
  static UntypedComplex make (double r, double i);
  bool operator== (const UntypedComplex& other) const;
  UntypedComplex& operator= (const double& x);
  UntypedComplex& operator= (const runtime::Complex64& x);
  UntypedComplex& operator= (const runtime::Complex128& x);
  operator double() const;
};

UntypedComplex operator* (const UntypedComplex&, const UntypedComplex&);
UntypedComplex operator/ (const UntypedComplex&, const UntypedComplex&);
UntypedComplex operator+ (const UntypedComplex&, const UntypedComplex&);
UntypedComplex operator- (const UntypedComplex&, const UntypedComplex&);
UntypedComplex operator- (const UntypedComplex&);

struct Value
{
  union
  {
    bool bool_value;

    uint8_t uint8_value;
    uint16_t uint16_value;
    uint32_t uint32_value;
    uint64_t uint64_value;

    int8_t int8_value;
    int16_t int16_value;
    int32_t int32_value;
    int64_t int64_value;

    float float32_value;
    double float64_value;

    runtime::Complex64 complex64_value;
    runtime::Complex128 complex128_value;

    unsigned long uint_value;
    long int_value;
    size_t uintptr_value;

    runtime::Slice slice_value;
    runtime::String string_value;

    bool untyped_boolean_value;
    int32_t untyped_rune_value;
    long long untyped_integer_value;
    double untyped_float_value;
    UntypedComplex untyped_complex_value;
    runtime::String untyped_string_value;
  };
};

struct ExpressionValue
{
  static ExpressionValue make_error ();
  static ExpressionValue make_void ();
  static ExpressionValue make_variable (const type::Type* type);
  static ExpressionValue make_value (const type::Type* a_type,
                                     Mutability a_intrinsic_mutability,
                                     Mutability a_indirection_mutability,
                                     bool a_variadic = false);
  static ExpressionValue make_bool (const type::Type* type, bool v);
  static ExpressionValue make_uint8 (const type::Type* type, uint8_t v);
  static ExpressionValue make_uint16 (const type::Type* type, uint16_t v);
  static ExpressionValue make_uint32 (const type::Type* type, uint32_t v);
  static ExpressionValue make_uint64 (const type::Type* type, uint64_t v);
  static ExpressionValue make_int8 (const type::Type* type, int8_t v);
  static ExpressionValue make_int16 (const type::Type* type, int16_t v);
  static ExpressionValue make_int32 (const type::Type* type, int32_t v);
  static ExpressionValue make_int64 (const type::Type* type, int64_t v);
  static ExpressionValue make_float32 (const type::Type* type, float v);
  static ExpressionValue make_float64 (const type::Type* type, double v);
  static ExpressionValue make_complex64 (const type::Type* type, runtime::Complex64 v);
  static ExpressionValue make_complex128 (const type::Type* type, runtime::Complex128 v);
  static ExpressionValue make_uint (const type::Type* type, unsigned long v);
  static ExpressionValue make_int (const type::Type* type, long v);
  static ExpressionValue make_uintptr (const type::Type* type, size_t v);
  static ExpressionValue make_string (const type::Type* type, const runtime::String& s);
  static ExpressionValue make_pointer (const type::Type* type);
  static ExpressionValue make_nil ();
  static ExpressionValue make_boolean (bool v);
  static ExpressionValue make_rune (int32_t v);
  static ExpressionValue make_integer (long long v);
  static ExpressionValue make_float (double v);
  static ExpressionValue make_complex (UntypedComplex v);
  static ExpressionValue make_string_untyped (const runtime::String& s);

  static ExpressionValue make_polymorphic_function (decl::PolymorphicFunction* func);
  static ExpressionValue make_type (const type::Type* t);

  enum Kind
  {
    Unknown,
    Error,
    Constant,
    Value,      // A computed value.
    Variable,   // A computed location.
    Void,       // Expression yields no value.
    Type,       // A type.
    EmptyType,  // No type was specified.
    Polymorphic_Function
  };
  Kind kind;
  const type::Type* type;
  semantic::Value value;
  // TODO:  This could be part of value.
  const decl::PolymorphicFunction* polymorphic_function;
  Mutability intrinsic_mutability;
  Mutability indirection_mutability;
  bool variadic;

  ReceiverAccess receiver_access;
  bool receiver_state;

  ExpressionValue ();

  bool is_unknown () const;
  bool is_error () const;
  bool is_value () const;
  bool is_variable () const;
  bool is_void () const;
  bool is_type () const;
  bool is_rvalue () const;
  bool is_rvalue_or_void () const;
  bool is_constant () const;
  bool is_polymorphic_function () const;
  bool is_variadic () const;

  bool is_assignable (const type::Type* to) const;

  void fix_string_indirection_mutability ();
  bool representable (const type::Type* to) const;
  void convert (const type::Type* to);
  long to_int () const;
  void print (std::ostream& out = std::cout) const;
};

std::ostream& operator<< (std::ostream& out, const ExpressionValue& ev);

typedef std::vector<ExpressionValue> ExpressionValueList;

ExpressionValueList collect_evals (ast::Node* node);
source::LocationList collect_locations (ast::Node* node);
void distribute_evals (const ExpressionValueList& evals, ast::Node* node);

void check_polymorphic_function_call (util::ErrorReporter& er,
                                      const decl::PolymorphicFunction* pf,
                                      const source::Location& loc,
                                      ExpressionValue& result,
                                      ExpressionValueList& arguments,
                                      const source::LocationList& locations);

void check_types_arguments (ast::List* node, const decl::ParameterList* signature);
void require_value_or_const_or_variable_list (const ast::List* node);
void check_mutability_arguments (ast::Node* node, const decl::ParameterList* signature);

void check_all (ast::Node* root, util::ErrorReporter& er, decl::SymbolTable& symbol_table);
bool check_constant_expression (ast::Node* root, util::ErrorReporter& er, decl::SymbolTable& symbol_table);

void compute_receiver_access_arguments (const ExpressionValueList& args, const decl::ParameterList* signature, ReceiverAccess& receiver_access, bool& flag);
void compute_receiver_access (ast::Node* root);

class static_memory_t
{
public:

  void set_value_at_offset (ptrdiff_t offset, size_t value)
  {
    stack_[offset] = value;
  }

  size_t get_value_at_offset (ptrdiff_t offset) const
  {
    return stack_.find (offset)->second;
  }

private:
  typedef std::map<ptrdiff_t, size_t> StackType;
  StackType stack_;
};

struct static_Value
{
  enum Kind
  {
    STACK_ADDRESS,
    ABSOLUTE_ADDRESS,
    VALUE,
  };

  Kind kind;
  union
  {
    ptrdiff_t offset; // STACK_ADDRESS
    size_t address; // ABSOLUTE_ADDRESS
    size_t value; // VALUE
  };

  static_Value ()
    : kind (VALUE)
    , value (0)
  { }

  static static_Value make_stack_offset (ptrdiff_t offset)
  {
    static_Value v;
    v.kind = STACK_ADDRESS;
    v.offset = offset;
    return v;
  }

  static static_Value make_value (size_t value)
  {
    static_Value v;
    v.kind = VALUE;
    v.value = value;
    return v;
  }

  static static_Value implicit_dereference (static_Value in, const static_memory_t& memory)
  {
    static_Value out;
    switch (in.kind)
      {
      case STACK_ADDRESS:
        out.kind = VALUE;
        out.value = memory.get_value_at_offset (in.offset);
        break;
      case ABSOLUTE_ADDRESS:
        UNIMPLEMENTED;
      case VALUE:
        NOT_REACHED;
      }
    return out;
  }

  static static_Value dereference (static_Value in)
  {
    static_Value out;
    switch (in.kind)
      {
      case STACK_ADDRESS:
      case ABSOLUTE_ADDRESS:
        NOT_REACHED;
      case VALUE:
        out.kind = ABSOLUTE_ADDRESS;
        out.address = in.value;
        break;
      }
    return out;
  }

  static static_Value select (static_Value in, ptrdiff_t offset)
  {
    static_Value out;
    switch (in.kind)
      {
      case STACK_ADDRESS:
        out = in;
        out.offset += offset;
        break;
      case ABSOLUTE_ADDRESS:
        out = in;
        out.address += offset;
        break;
      case VALUE:
        NOT_REACHED;
      }
    return out;
  }

  static static_Value index (static_Value in, const type::Array* type, static_Value idx)
  {
    static_Value out;
    switch (in.kind)
      {
      case STACK_ADDRESS:
        out = in;
        UNIMPLEMENTED;
        //out.offset += type->unit_size () * idx.value;
        break;
      case ABSOLUTE_ADDRESS:
        out = in;
        UNIMPLEMENTED;
        //out.address += type->unit_size () * idx.value;
        break;
      case VALUE:
        NOT_REACHED;
      }
    return out;
  }

  static static_Value address_of (static_Value in)
  {
    static_Value out;
    out.kind = VALUE;

    switch (in.kind)
      {
      case STACK_ADDRESS:
        UNIMPLEMENTED;
      case ABSOLUTE_ADDRESS:
        out.value = in.address;
        break;
        UNIMPLEMENTED;
      case VALUE:
        UNIMPLEMENTED;
      }

    return out;
  }
};

std::ostream&
operator<< (std::ostream& o,
            const static_Value& v);

static_Value
EvaluateStatic (const ast::Node* node, const static_memory_t& memory);

}

#endif /* RCGO_SRC_SEMANTIC_HPP */
