#ifndef RC_SRC_TYPE_HPP
#define RC_SRC_TYPE_HPP

#include "types.hpp"

#include <vector>

#include "symbol.hpp"
#include "util.hpp"

#define TYPE_NOT_REACHED(type) do { std::cerr << '\n' << type << std::endl; NOT_REACHED; } while (0);
#define STRING_RETURNER(name, string) struct name { const char* operator() () const { return string; } }

namespace type
{

// Ordered.  See the choose function.
enum Kind
{
  // Untyped types.
  Untyped_Nil_Kind,  // First
  Untyped_Boolean_Kind,
  Untyped_Rune_Kind,
  Untyped_Integer_Kind,
  Untyped_Float_Kind,
  Untyped_Complex_Kind,
  Untyped_String_Kind, // Last

  // Typed types.
  Bool_Kind, // First
  Uint8_Kind,
  Uint16_Kind,
  Uint32_Kind,
  Uint64_Kind,
  Int8_Kind,
  Int16_Kind,
  Int32_Kind,
  Int64_Kind,
  Float32_Kind,
  Float64_Kind,
  Complex64_Kind,
  Complex128_Kind,
  Uint_Kind,
  Int_Kind,
  Uintptr_Kind,
  String_Kind,

  Struct_Kind,
  Component_Kind,
  Array_Kind,
  Map_Kind,
  Pointer_Kind,
  Slice_Kind,
  Heap_Kind,

  Function_Kind,
  Push_Port_Kind,
  Pull_Port_Kind,

  Method_Kind,
  Initializer_Kind,
  Getter_Kind,
  Reaction_Kind,

  Interface_Kind,

  File_Descriptor_Kind, // Last

  // Named types.
  Named_Kind,
};

struct Type
{
  Type ();
  virtual ~Type ();

  virtual Kind kind () const = 0;

  // For working with the type under a named type
  virtual const Type* underlying_type () const;
  virtual Kind underlying_kind () const;
  // Debugging
  virtual void print (std::ostream& out = std::cout) const = 0;
  // Allocation
  // Get the unnamed type for an untyped type
  virtual const Type* default_type () const;
  // Various predicates
  bool is_untyped () const;
  bool is_unnamed () const;
  bool is_named () const;
  bool is_numeric () const;
  bool contains_pointer () const;
  bool is_typed_integer () const;
  bool is_typed_float () const;
  bool is_typed_unsigned_integer () const;
  bool is_typed_complex () const;
  bool is_typed_numeric () const;
  bool is_untyped_numeric () const;
  bool is_typed_string () const;
  bool is_any_string () const;
  bool is_slice_of_bytes () const;
  bool is_slice_of_runes () const;
  const Pointer* pointer_to_array () const;
  bool is_arithmetic () const;
  bool is_integral () const;
  bool is_any_boolean () const;
  // Get a type making this type the base type
  const Pointer* get_pointer () const;
  const Slice* get_slice () const;
  const Array* get_array (long dimension) const;
  const Heap* get_heap () const;
  // Selecting
  decl::Field* find_field (const std::string& name) const;
  virtual decl::Method* find_method (const std::string& identifier) const;
  virtual decl::Initializer* find_initializer (const std::string& identifier) const;
  virtual decl::Getter* find_getter (const std::string& identifier) const;
  virtual decl::Action* find_action (const std::string& identifier) const;
  virtual decl::Reaction* find_reaction (const std::string& identifier) const;
  virtual decl::Bind* find_bind (const std::string& identifier) const;
  virtual decl::Callable* find_callable (const std::string& name) const;
  bool has_member (const std::string& identifier) const;
  // Heap operations.
  const Type* move () const;
  const Type* merge_change () const;
  // Casting
  virtual const NamedType* to_named_type () const;
  virtual const Array* to_array () const;
  virtual const Slice* to_slice () const;
  virtual const Struct* to_struct () const;
  virtual const Pointer* to_pointer () const;
  virtual const Function* to_function () const;
  virtual const PushPort* to_push_port () const;
  virtual const Reaction* to_reaction () const;
  virtual const PullPort* to_pull_port () const;
  virtual const Interface* to_interface () const;
  virtual const Map* to_map () const;
  virtual const Heap* to_heap () const;
  virtual const Method* to_method () const;
  virtual const Initializer* to_initializer () const;
  virtual const Getter* to_getter () const;

protected:
  virtual decl::Field* find_field_i (const std::string& name) const;
  virtual bool contains_pointer_i () const;

private:
  const Pointer* pointer_;
  const Slice* slice_;
  typedef std::map<long, const Array*> ArraysType;
  ArraysType arrays_;
  const Heap* heap_;
};

std::ostream&
operator<< (std::ostream& o, const Type& type);

// A NamedType is created via type name ...
struct NamedType : public Type, public decl::Symbol
{
  typedef std::vector<decl::Getter*> GettersType;
  typedef std::vector<decl::Action*> ActionsType;
  typedef std::vector<decl::Reaction*> ReactionsType;
  typedef std::vector<decl::Bind*> BindsType;

  NamedType (ast::TypeDecl* a_typedecl);
  NamedType (const std::string& name,
             const util::Location& location,
             const Type* underlyingType);

  void print (std::ostream& out = std::cout) const;
  void underlying_type (const Type* u);
  const Type* underlying_type () const;
  virtual Kind kind () const;
  void insert_method (decl::Method* method);
  decl::Method* find_method (const std::string& identifier) const;
  void insert_initializer (decl::Initializer* initializer);
  decl::Initializer* find_initializer (const std::string& identifier) const;
  void insert_getter (decl::Getter* getter);
  decl::Getter* find_getter (const std::string& identifier) const;
  GettersType::const_iterator getters_begin () const;
  GettersType::const_iterator getters_end () const;
  void insert_action (decl::Action* action);
  decl::Action* find_action (const std::string& identifier) const;
  ActionsType::const_iterator actions_begin () const;
  ActionsType::const_iterator actions_end () const;
  void insert_reaction (decl::Reaction* reaction);
  decl::Reaction* find_reaction (const std::string& identifier) const;
  ReactionsType::const_iterator reactions_begin () const;
  ReactionsType::const_iterator reactions_end () const;
  void insert_bind (decl::Bind* bind);
  decl::Bind* find_bind (const std::string& identifier) const;
  BindsType::const_iterator binds_begin () const;
  BindsType::const_iterator binds_end () const;
  virtual decl::Callable* find_callable (const std::string& name) const;
  virtual const NamedType* to_named_type () const;

  virtual void accept (decl::SymbolVisitor& visitor);
  virtual void accept (decl::ConstSymbolVisitor& visitor) const;
  virtual bool process_declaration_i (util::ErrorReporter& er, decl::SymbolTable& symbol_table);
  virtual void post_process_declaration_i (util::ErrorReporter& er, decl::SymbolTable& symbol_table);

private:
  const Type* underlyingType_;
  std::vector<decl::Method*> methods_;
  std::vector<decl::Initializer*> initializers_;
  GettersType getters_;
  ActionsType actions_;
  ReactionsType reactions_;
  BindsType binds_;
  ast::TypeDecl* const typedecl_;
};

template <typename S, Kind k>
struct Scalar : public Type
{
  void print (std::ostream& out = std::cout) const
  {
    out << S () ();
  }
  virtual Kind kind () const
  {
    return k;
  }
  static const Scalar<S, k>* instance ()
  {
    static Scalar<S, k>* instance_ = new Scalar<S, k> ();
    return instance_;
  }
private:
  Scalar<S, k> () { }
};

STRING_RETURNER(BoolString, "<bool>");
typedef Scalar<BoolString, Bool_Kind> Bool;

STRING_RETURNER(Uint8String, "<uint8>");
typedef Scalar<Uint8String, Uint8_Kind> Uint8;

STRING_RETURNER(Uint16String, "<uint16>");
typedef Scalar<Uint16String, Uint16_Kind> Uint16;

STRING_RETURNER(Uint32String, "<uint32>");
typedef Scalar<Uint32String, Uint32_Kind> Uint32;

STRING_RETURNER(Uint64String, "<uint64>");
typedef Scalar<Uint64String, Uint64_Kind> Uint64;

STRING_RETURNER(Int8String, "<int8>");
typedef Scalar<Int8String, Int8_Kind> Int8;

STRING_RETURNER(Int16String, "<int16>");
typedef Scalar<Int16String, Int16_Kind> Int16;

STRING_RETURNER(Int32String, "<int32>");
typedef Scalar<Int32String, Int32_Kind> Int32;

STRING_RETURNER(Int64String, "<int64>");
typedef Scalar<Int64String, Int64_Kind> Int64;

STRING_RETURNER(Float32String, "<float32>");
typedef Scalar<Float32String, Float32_Kind> Float32;

STRING_RETURNER(Float64String, "<float64>");
typedef Scalar<Float64String, Float64_Kind> Float64;

STRING_RETURNER(Complex64String, "<complex64>");
typedef Scalar<Complex64String, Complex64_Kind> Complex64;

STRING_RETURNER(Complex128String, "<complex128>");
typedef Scalar<Complex128String, Complex128_Kind> Complex128;

STRING_RETURNER(UintString, "<uint>");
typedef Scalar<UintString, Uint_Kind> Uint;

STRING_RETURNER(IntString, "<int>");
typedef Scalar<IntString, Int_Kind> Int;

STRING_RETURNER(UintptrString, "<uintptr>");
typedef Scalar<UintptrString, Uintptr_Kind> Uintptr;

struct String : public Type
{
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
  static const String* instance ();
private:
  bool contains_pointer_i () const;
  String ();
};

// Helper class for types that have a base type.
struct BaseType
{
  BaseType (const Type* a_base_type);
  const Type* const base_type;
};

struct Pointer : public Type, public BaseType
{
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
  virtual decl::Field* find_field_i (const std::string& name) const;
  virtual decl::Callable* find_callable (const std::string& name) const;
  virtual const Pointer* to_pointer () const;
private:
  virtual bool contains_pointer_i () const;
  friend class Type;
  Pointer (const Type* base);
};

struct Slice : public Type, public BaseType
{
  virtual void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
  virtual const Slice* to_slice () const;
private:
  virtual bool contains_pointer_i () const;
  friend class Type;
  Slice (const Type* base);
};

struct Array : public Type, public BaseType
{
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
  virtual const Array* to_array () const;
  long const dimension;
private:
  virtual bool contains_pointer_i () const;
  friend class Type;
  Array (long d, const Type* base);
};

struct Map : public Type
{
  typedef std::map<std::pair<const Type*, const Type*>, const Map*> MapType;
  static const Map* make (const Type* key_type, const Type* value_type);
  virtual void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
  virtual const Map* to_map () const;

  const Type* const key_type;
  const Type* const value_type;

private:
  static MapType maps_;
  Map (const Type* a_key_type,
       const Type* a_value_type);
};

struct Heap : public Type, public BaseType
{
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
  virtual const Heap* to_heap () const;
private:
  virtual bool contains_pointer_i () const;
  friend class Type;
  Heap (const Type* base);
};

struct Struct : public Type
{
  typedef std::vector<decl::Field*> FieldsType;
  typedef FieldsType::const_iterator const_iterator;
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
  virtual const Struct* to_struct () const;
  const_iterator begin () const;
  const_iterator end () const;
  Struct* append_field (decl::Package* package,
                        bool is_anonymous,
                        const std::string& field_name,
                        const util::Location& location,
                        const Type* field_type,
                        const TagSet& tags);
  virtual decl::Field* find_field_i (const std::string& name) const;
  size_t field_count () const;
protected:
  FieldsType fields_;
private:
  virtual bool contains_pointer_i () const;
};

struct Component : public Struct
{
  Component (decl::Package* package, const util::Location& location);
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
};

struct FunctionBase : public Type
{
  FunctionBase (const decl::ParameterList* a_parameter_list,
                const decl::ParameterList* a_return_parameter_list);
  const decl::ParameterList* const parameter_list;
  const decl::ParameterList* const return_parameter_list;
};

struct Function : public FunctionBase
{
  Function (const decl::ParameterList* a_parameter_list,
            const decl::ParameterList* a_return_parameter_list);
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
  virtual const Function* to_function () const;
};

struct PushPort : public FunctionBase
{
  PushPort (const decl::ParameterList* a_parameter_list,
            const decl::ParameterList* a_return_parameter_list);
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
  virtual const PushPort* to_push_port () const;
};

struct PullPort : public FunctionBase
{
  PullPort (const decl::ParameterList* a_parameter_list,
            const decl::ParameterList* a_return_parameter_list);
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
  virtual const PullPort* to_pull_port () const;
};

struct MethodBase : public Type
{
  MethodBase (const NamedType* a_named_type,
              decl::Parameter* a_receiver_parameter,
              const decl::ParameterList* a_parameter_list,
              const decl::ParameterList* a_return_parameter_list);
  const NamedType* const named_type;
  decl::Parameter* const receiver_parameter;
  const decl::ParameterList* const parameter_list;
  const decl::ParameterList* const return_parameter_list;
  const Function* const function_type;
private:
  static Function* make_function_type (decl::Parameter* receiver_parameter,
                                       const decl::ParameterList* parameter_list,
                                       const decl::ParameterList* return_parameter_list);
};

struct Method : public MethodBase
{
  Method (const NamedType* a_named_type,
          decl::Parameter* a_receiver_parameter,
          const decl::ParameterList* a_parameter_list,
          const decl::ParameterList* a_return_parameter_list);
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
  virtual const Method* to_method () const;
};

struct Initializer : public MethodBase
{
  Initializer (const NamedType* a_named_type,
               decl::Parameter* a_receiver_parameter,
               const decl::ParameterList* a_parameter_list,
               const decl::ParameterList* a_return_parameter_list);
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
  virtual const Initializer* to_initializer () const;
};

struct Getter : public MethodBase
{
  Getter (const NamedType* a_named_type,
          decl::Parameter* a_receiver_parameter,
          const decl::ParameterList* a_parameter_list,
          const decl::ParameterList* a_return_parameter_list);
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
  virtual const Getter* to_getter () const;
};

struct Reaction : public MethodBase
{
  Reaction (const NamedType* a_named_type,
            decl::Parameter* a_receiver_parameter,
            const decl::ParameterList* a_parameter_list,
            const decl::ParameterList* a_return_parameter_list);
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
  virtual const Reaction* to_reaction () const;
};

struct Interface : public Type
{
  Interface (decl::Package* a_package);
  virtual void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
  virtual const Interface* to_interface () const;

  decl::Package* const package;
  typedef std::map<std::string, const Function*> MethodsType;
  MethodsType methods;
};

struct UntypedNil : public Type
{
  virtual Kind kind () const;
  void print (std::ostream& out = std::cout) const;
  static const UntypedNil* instance ();
private:
  UntypedNil ();
};

struct UntypedBoolean : public Type
{
  virtual Kind kind () const;
  virtual const Type* default_type () const;
  void print (std::ostream& out = std::cout) const;
  static const UntypedBoolean* instance ();
private:
  UntypedBoolean ();
};

struct UntypedRune : public Type
{
  virtual Kind kind () const;
  virtual const Type* default_type () const;
  void print (std::ostream& out = std::cout) const;
  static const UntypedRune* instance ();
private:
  UntypedRune ();
};

struct UntypedInteger : public Type
{
  virtual Kind kind () const;
  virtual const Type* default_type () const;
  void print (std::ostream& out = std::cout) const;
  static const UntypedInteger* instance ();
private:
  UntypedInteger ();
};

struct UntypedFloat : public Type
{
  virtual Kind kind () const;
  virtual const Type* default_type () const;
  void print (std::ostream& out = std::cout) const;
  static const UntypedFloat* instance ();
private:
  UntypedFloat ();
};

struct UntypedComplex : public Type
{
  virtual Kind kind () const;
  virtual const Type* default_type () const;
  void print (std::ostream& out = std::cout) const;
  static const UntypedComplex* instance ();
private:
  UntypedComplex ();
};

struct UntypedString : public Type
{
  virtual Kind kind () const;
  virtual const Type* default_type () const;
  void print (std::ostream& out = std::cout) const;
  static const UntypedString* instance ();
private:
  UntypedString ();
};

struct FileDescriptor : public Type
{
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
  static const FileDescriptor* instance ();
private:
  virtual bool contains_pointer_i () const;
  FileDescriptor ();
};

bool
are_identical (const Type* x, const Type* y);

const Type*
choose (const Type* x, const Type* y);

// Select the type for *, /, +, -.
// NULL means no suitable type.
struct Arithmetic
{
  static const Type* pick (const Type* left_type, const Type* right_type);
};

// Select the type for %.
// NULL means no suitable type.
struct Integral
{
  static const Type* pick (const Type* left_type, const Type* right_type);
};

// Select the type for == and !=.
// NULL means no suitable type.
struct Comparable
{
  static const type::Type* pick (const type::Type* left_type, const type::Type* right_type);
};

// Select the type for <, <=, >, and >=.
// NULL means no suitable type.
struct Orderable
{
  static const type::Type* pick (const type::Type* left_type, const type::Type* right_type);
};

// Select the type for || and &&.
// NULL means no suitable type.
struct Logical
{
  static const type::Type* pick (const type::Type* left_type, const type::Type* right_type);
};

extern NamedType named_bool;

extern NamedType named_uint8;
extern NamedType named_uint16;
extern NamedType named_uint32;
extern NamedType named_uint64;

extern NamedType named_int8;
extern NamedType named_int16;
extern NamedType named_int32;
extern NamedType named_int64;

extern NamedType named_float32;
extern NamedType named_float64;

extern NamedType named_complex64;
extern NamedType named_complex128;

extern NamedType named_uint;
extern NamedType named_int;
extern NamedType named_uintptr;

extern NamedType named_rune;
extern NamedType named_byte;
extern NamedType named_string;

extern NamedType named_error;
extern NamedType named_file_descriptor;
extern NamedType named_timespec;

}

#endif // RC_SRC_TYPE_HPP
