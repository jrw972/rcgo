#ifndef RC_SRC_TYPE_HPP
#define RC_SRC_TYPE_HPP

#include "types.hpp"

#include <vector>

#include "util.hpp"
#include "complex.hpp"

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
  Void_Kind, // First

  Bool_Kind,
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
  Polymorphic_Function_Kind,

  File_Descriptor_Kind, // Last

  // Named types.
  Named_Kind,
};

struct Type
{
  Type ();
  virtual ~Type ();

  virtual void accept (Visitor& visitor) const = 0;
  virtual Kind kind () const = 0;

  // For working with the type under a named type
  virtual const Type* underlying_type () const;
  virtual Kind underlying_kind () const;
  // Debugging
  virtual void print (std::ostream& out = std::cout) const = 0;
  std::string to_string () const;
  // Allocation
  virtual size_t alignment () const = 0;
  virtual size_t size () const = 0;
  // Get the unnamed type for an untyped type
  virtual const Type* default_type () const;
  // Various predicates
  bool is_untyped () const;
  bool is_unnamed () const;
  bool is_named () const;
  bool is_numeric () const;
  // Get a type making this type the base type
  const Pointer* get_pointer () const;
  const Slice* get_slice () const;
  const Array* get_array (long dimension) const;
  const Heap* get_heap () const;
  // Selecting
  Field* find_field (const std::string& name) const;
  virtual decl::Method* find_method (const std::string& identifier) const;
  virtual decl::Initializer* find_initializer (const std::string& identifier) const;
  virtual decl::Getter* find_getter (const std::string& identifier) const;
  virtual decl::Action* find_action (const std::string& identifier) const;
  virtual decl::Reaction* find_reaction (const std::string& identifier) const;
  virtual decl::Bind* find_bind (const std::string& identifier) const;
  virtual decl::Callable* find_callable (const std::string& name) const;
  const Type* find (const std::string& identifier) const;
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
  virtual const PullPort* to_pull_port () const;
  virtual const Interface* to_interface () const;
  virtual const Map* to_map () const;
  virtual const Heap* to_heap () const;

protected:
  virtual Field* find_field_i (const std::string& name) const;

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
struct NamedType : public Type
{
  typedef std::vector<decl::Getter*> GettersType;
  typedef std::vector<decl::Action*> ActionsType;
  typedef std::vector<decl::Reaction*> ReactionsType;
  typedef std::vector<decl::Bind*> BindsType;

  NamedType (const std::string& name);
  NamedType (const std::string& name,
             const Type* underlyingType);

  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  void underlying_type (const Type* u);
  const Type* underlying_type () const;
  virtual size_t alignment () const;
  virtual size_t size () const;
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

private:
  std::string const name_;
  const Type* underlyingType_;
  std::vector<decl::Method*> methods_;
  std::vector<decl::Initializer*> initializers_;
  GettersType getters_;
  ActionsType actions_;
  ReactionsType reactions_;
  BindsType binds_;
};

// Void represents the absence of a type
// TODO:  Remove Void.
class Void : public Type
{
public:
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  size_t alignment () const;
  size_t size () const;
  virtual Kind kind () const;
  static const Void* instance ();
private:
  Void ();
};

template <typename T, typename S, Kind k>
class Scalar : public Type
{
public:
  typedef T ValueType;
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const
  {
    out << S () ();
  }
  size_t alignment () const
  {
    return sizeof (T);
  }
  size_t size () const
  {
    return sizeof (T);
  }
  virtual Kind kind () const
  {
    return k;
  }
  static const Scalar<T, S, k>* instance ()
  {
    static Scalar<T, S, k>* instance_ = new Scalar<T, S, k> ();
    return instance_;
  }
private:
  Scalar<T, S, k> () { }
};

STRING_RETURNER(BoolString, "<bool>");
typedef Scalar<bool, BoolString, Bool_Kind> Bool;

STRING_RETURNER(Uint8String, "<uint8>");
typedef Scalar<uint8_t, Uint8String, Uint8_Kind> Uint8;

STRING_RETURNER(Uint16String, "<uint16>");
typedef Scalar<uint16_t, Uint16String, Uint16_Kind> Uint16;

STRING_RETURNER(Uint32String, "<uint32>");
typedef Scalar<uint32_t, Uint32String, Uint32_Kind> Uint32;

STRING_RETURNER(Uint64String, "<uint64>");
typedef Scalar<uint64_t, Uint64String, Uint64_Kind> Uint64;

STRING_RETURNER(Int8String, "<int8>");
typedef Scalar<int8_t, Int8String, Int8_Kind> Int8;

STRING_RETURNER(Int16String, "<int16>");
typedef Scalar<int16_t, Int16String, Int16_Kind> Int16;

STRING_RETURNER(Int32String, "<int32>");
typedef Scalar<int32_t, Int32String, Int32_Kind> Int32;

STRING_RETURNER(Int64String, "<int64>");
typedef Scalar<int64_t, Int64String, Int64_Kind> Int64;

STRING_RETURNER(Float32String, "<float32>");
typedef Scalar<float, Float32String, Float32_Kind> Float32;

STRING_RETURNER(Float64String, "<float64>");
typedef Scalar<double, Float64String, Float64_Kind> Float64;

STRING_RETURNER(Complex64String, "<complex64>");
typedef Scalar<C64, Complex64String, Complex64_Kind> Complex64;

STRING_RETURNER(Complex128String, "<complex128>");
typedef Scalar<C128, Complex128String, Complex128_Kind> Complex128;

STRING_RETURNER(UintString, "<uint>");
typedef Scalar<unsigned long, UintString, Uint_Kind> Uint;

STRING_RETURNER(IntString, "<int>");
typedef Scalar<long, IntString, Int_Kind> Int;

STRING_RETURNER(UintptrString, "<uintptr>");
typedef Scalar<ptrdiff_t, UintptrString, Uintptr_Kind> Uintptr;

STRING_RETURNER(StringString, "<string>");
typedef Scalar<StringRep, StringString, String_Kind> String;

// Helper class for types that have a base type.
struct BaseType
{
  BaseType (const Type* a_base_type);
  const Type* const base_type;
};

class Pointer : public Type, public BaseType
{
public:
  typedef void* ValueType;
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  size_t alignment () const;
  size_t size () const;
  virtual Kind kind () const;
  virtual Field* find_field_i (const std::string& name) const;
  virtual decl::Callable* find_callable (const std::string& name) const;
  virtual const Pointer* to_pointer () const;
private:
  friend class Type;
  Pointer (const Type* base);
};

class Slice : public Type, public BaseType
{
public:
  struct ValueType
  {
    void* ptr;
    Uint::ValueType length;
    Uint::ValueType capacity;
  };
  virtual void accept (Visitor& visitor) const;
  virtual void print (std::ostream& out = std::cout) const;
  virtual size_t alignment () const;
  virtual size_t size () const;
  virtual Kind kind () const;
  virtual const Slice* to_slice () const;
  size_t unit_size () const;
private:
  friend class Type;
  Slice (const Type* base);
};

class Array : public Type, public BaseType
{
public:
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  size_t alignment () const;
  size_t size () const;
  virtual Kind kind () const;
  virtual const Array* to_array () const;
  const Int::ValueType dimension;
  size_t unit_size () const;
private:
  friend class Type;
  Array (Int::ValueType d, const Type* base);
};

struct Map : public Type
{
  typedef std::map<std::pair<const Type*, const Type*>, const Map*> MapType;
  static const Map* make (const Type* key_type, const Type* value_type);
  virtual void accept (Visitor& visitor) const;
  virtual void print (std::ostream& out = std::cout) const;
  virtual size_t alignment () const;
  virtual size_t size () const;
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
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  size_t alignment () const;
  size_t size () const;
  virtual Kind kind () const;
  virtual const Heap* to_heap () const;
private:
  friend class Type;
  Heap (const Type* base);
};

struct Struct : public Type
{
  typedef std::vector<Field*> FieldsType;
  typedef FieldsType::const_iterator const_iterator;
  Struct ();
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  size_t alignment () const;
  size_t size () const;
  virtual Kind kind () const;
  virtual const Struct* to_struct () const;
  const_iterator begin () const;
  const_iterator end () const;
  Struct* append_field (decl::Package* package,
                        bool is_anonymous,
                        const std::string& field_name,
                        const Type* field_type,
                        const TagSet& tags);
  virtual Field* find_field_i (const std::string& name) const;
  size_t field_count () const;
protected:
  FieldsType fields_;
private:
  ptrdiff_t offset_;
  size_t alignment_;
};

struct Component : public Struct
{
  Component (decl::Package* package);
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
};

struct FunctionBase : public Type
{
  FunctionBase (const decl::ParameterList* a_parameter_list,
                const decl::ParameterList* a_return_parameter_list);
  size_t alignment () const;
  decl::ParameterSymbol* get_parameter (const std::string& name) const;
  decl::ParameterSymbol* get_return_parameter (const std::string& name) const;
  const decl::ParameterList* const parameter_list;
  const decl::ParameterList* const return_parameter_list;
};

struct Function : public FunctionBase
{
  Function (const decl::ParameterList* a_parameter_list,
            const decl::ParameterList* a_return_parameter_list);
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  size_t size () const;
  virtual Kind kind () const;
  virtual const Function* to_function () const;
};

struct PushPort : public FunctionBase
{
  PushPort (const decl::ParameterList* a_parameter_list,
            const decl::ParameterList* a_return_parameter_list);
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  size_t size () const;
  virtual Kind kind () const;
  virtual const PushPort* to_push_port () const;
};

struct PullPort : public FunctionBase
{
  PullPort (const decl::ParameterList* a_parameter_list,
            const decl::ParameterList* a_return_parameter_list);
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  size_t size () const;
  virtual Kind kind () const;
  virtual const PullPort* to_pull_port () const;
};

struct MethodBase : public Type
{
  MethodBase (const NamedType* a_named_type,
              decl::ParameterSymbol* a_receiver_parameter,
              const decl::ParameterList* a_parameter_list,
              const decl::ParameterList* a_return_parameter_list);
  size_t alignment () const
  {
    return sizeof (void*);
  }
  size_t size () const
  {
    return sizeof (void*);
  }
  const NamedType* const named_type;
  decl::ParameterSymbol* const receiver_parameter;
  const decl::ParameterList* const parameter_list;
  const decl::ParameterList* const return_parameter_list;
  const Function* const function_type;
private:
  static Function* make_function_type (decl::ParameterSymbol* receiver_parameter,
                                       const decl::ParameterList* parameter_list,
                                       const decl::ParameterList* return_parameter_list);
};

struct Method : public MethodBase
{
  Method (const NamedType* a_named_type,
          decl::ParameterSymbol* a_receiver_parameter,
          const decl::ParameterList* a_parameter_list,
          const decl::ParameterList* a_return_parameter_list);
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
};

struct Initializer : public MethodBase
{
  Initializer (const NamedType* a_named_type,
               decl::ParameterSymbol* a_receiver_parameter,
               const decl::ParameterList* a_parameter_list,
               const decl::ParameterList* a_return_parameter_list);
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
};

struct Getter : public MethodBase
{
  Getter (const NamedType* a_named_type,
          decl::ParameterSymbol* a_receiver_parameter,
          const decl::ParameterList* a_parameter_list,
          const decl::ParameterList* a_return_parameter_list);
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
};

struct Reaction : public MethodBase
{
  Reaction (const NamedType* a_named_type,
            decl::ParameterSymbol* a_receiver_parameter,
            const decl::ParameterList* a_parameter_list,
            const decl::ParameterList* a_return_parameter_list);
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  virtual Kind kind () const;
};

struct Interface : public Type
{
  Interface (decl::Package* a_package);
  virtual void accept (Visitor& visitor) const;
  virtual void print (std::ostream& out = std::cout) const;
  virtual size_t alignment () const;
  virtual size_t size () const;
  virtual Kind kind () const;
  virtual const Interface* to_interface () const;

  decl::Package* const package;
  typedef std::map<std::string, const Function*> MethodsType;
  MethodsType methods;
};

struct Untyped : public Type
{
  size_t alignment () const;
  size_t size () const;
};

struct UntypedNil : public Untyped
{
  virtual Kind kind () const;
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  static const UntypedNil* instance ();
private:
  UntypedNil ();
};

struct UntypedBoolean : public Untyped
{
  typedef bool ValueType;
  virtual Kind kind () const;
  virtual const Type* default_type () const;
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  static const UntypedBoolean* instance ();
private:
  UntypedBoolean ();
};

struct UntypedRune : public Untyped
{
  typedef int32_t ValueType;
  virtual Kind kind () const;
  virtual const Type* default_type () const;
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  static const UntypedRune* instance ();
private:
  UntypedRune ();
};

struct UntypedInteger : public Untyped
{
  typedef long long ValueType;
  virtual Kind kind () const;
  virtual const Type* default_type () const;
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  static const UntypedInteger* instance ();
private:
  UntypedInteger ();
};

struct UntypedFloat : public Untyped
{
  typedef double ValueType;
  virtual Kind kind () const;
  virtual const Type* default_type () const;
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  static const UntypedFloat* instance ();
private:
  UntypedFloat ();
};

struct UntypedComplex : public Untyped
{
  typedef CU ValueType;
  virtual Kind kind () const;
  virtual const Type* default_type () const;
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  static const UntypedComplex* instance ();
private:
  UntypedComplex ();
};

struct UntypedString : public Untyped
{
  typedef StringRep ValueType;
  virtual Kind kind () const;
  virtual const Type* default_type () const;
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  static const UntypedString* instance ();
private:
  UntypedString ();
};

struct PolymorphicFunction : public Type
{
  void accept (Visitor& visitor) const;
  virtual void print (std::ostream& out = std::cout) const;
  virtual size_t alignment () const;
  virtual size_t size () const;
  virtual Kind kind () const;
  static const PolymorphicFunction* instance ();
private:
  PolymorphicFunction ();
};

struct FileDescriptor : public Type
{
  void accept (Visitor& visitor) const;
  void print (std::ostream& out = std::cout) const;
  size_t alignment () const;
  size_t size () const;
  virtual Kind kind () const;
  static const FileDescriptor* instance ();
private:
  FileDescriptor ();
};

struct Visitor
{
  virtual ~Visitor () { }
  virtual void visit (const Array& type) = 0;
  virtual void visit (const Bool& type) = 0;
  virtual void visit (const Complex128& type) = 0;
  virtual void visit (const Complex64& type) = 0;
  virtual void visit (const Component& type) = 0;
  virtual void visit (const FileDescriptor& type) = 0;
  virtual void visit (const Float32& type) = 0;
  virtual void visit (const Float64& type) = 0;
  virtual void visit (const Function& type) = 0;
  virtual void visit (const Getter& type) = 0;
  virtual void visit (const Heap& type) = 0;
  virtual void visit (const Initializer& type) = 0;
  virtual void visit (const Int& type) = 0;
  virtual void visit (const Int16& type) = 0;
  virtual void visit (const Int32& type) = 0;
  virtual void visit (const Int64& type) = 0;
  virtual void visit (const Int8& type) = 0;
  virtual void visit (const Interface& type) = 0;
  virtual void visit (const Map& type) = 0;
  virtual void visit (const Method& type) = 0;
  virtual void visit (const NamedType& type) = 0;
  virtual void visit (const Pointer& type) = 0;
  virtual void visit (const PullPort& type) = 0;
  virtual void visit (const PushPort& type) = 0;
  virtual void visit (const Reaction& type) = 0;
  virtual void visit (const Slice& type) = 0;
  virtual void visit (const String& type) = 0;
  virtual void visit (const Struct& type) = 0;
  virtual void visit (const PolymorphicFunction& type) = 0;
  virtual void visit (const Uint& type) = 0;
  virtual void visit (const Uint16& type) = 0;
  virtual void visit (const Uint32& type) = 0;
  virtual void visit (const Uint64& type) = 0;
  virtual void visit (const Uint8& type) = 0;
  virtual void visit (const Uintptr& type) = 0;
  virtual void visit (const UntypedBoolean& type) = 0;
  virtual void visit (const UntypedComplex& type) = 0;
  virtual void visit (const UntypedFloat& type) = 0;
  virtual void visit (const UntypedInteger& type) = 0;
  virtual void visit (const UntypedNil& type) = 0;
  virtual void visit (const UntypedRune& type) = 0;
  virtual void visit (const UntypedString& type) = 0;
  virtual void visit (const Void& type) = 0;
};

struct DefaultVisitor : public Visitor
{
  virtual void visit (const Array& type)
  {
    default_action (type);
  }
  virtual void visit (const Map& type)
  {
    default_action (type);
  }
  virtual void visit (const Interface& type)
  {
    default_action (type);
  }
  virtual void visit (const Slice& type)
  {
    default_action (type);
  }
  virtual void visit (const Bool& type)
  {
    default_action (type);
  }
  virtual void visit (const Component& type)
  {
    default_action (type);
  }
  virtual void visit (const Function& type)
  {
    default_action (type);
  }
  virtual void visit (const Method& type)
  {
    default_action (type);
  }
  virtual void visit (const Heap& type)
  {
    default_action (type);
  }
  virtual void visit (const FileDescriptor& type)
  {
    default_action (type);
  }
  virtual void visit (const NamedType& type)
  {
    default_action (type);
  }
  virtual void visit (const Pointer& type)
  {
    default_action (type);
  }
  virtual void visit (const Struct& type)
  {
    default_action (type);
  }
  virtual void visit (const Int& type)
  {
    default_action (type);
  }
  virtual void visit (const Int8& type)
  {
    default_action (type);
  }
  virtual void visit (const Int16& type)
  {
    default_action (type);
  }
  virtual void visit (const Int32& type)
  {
    default_action (type);
  }
  virtual void visit (const Int64& type)
  {
    default_action (type);
  }
  virtual void visit (const Uint& type)
  {
    default_action (type);
  }
  virtual void visit (const Uint8& type)
  {
    default_action (type);
  }
  virtual void visit (const Uint16& type)
  {
    default_action (type);
  }
  virtual void visit (const Uint32& type)
  {
    default_action (type);
  }
  virtual void visit (const Uint64& type)
  {
    default_action (type);
  }
  virtual void visit (const Float32& type)
  {
    default_action (type);
  }
  virtual void visit (const Float64& type)
  {
    default_action (type);
  }
  virtual void visit (const Complex64& type)
  {
    default_action (type);
  }
  virtual void visit (const Complex128& type)
  {
    default_action (type);
  }
  virtual void visit (const String& type)
  {
    default_action (type);
  }
  virtual void visit (const UntypedNil& type)
  {
    default_action (type);
  }
  virtual void visit (const UntypedBoolean& type)
  {
    default_action (type);
  }
  virtual void visit (const UntypedRune& type)
  {
    default_action (type);
  }
  virtual void visit (const UntypedInteger& type)
  {
    default_action (type);
  }
  virtual void visit (const UntypedFloat& type)
  {
    default_action (type);
  }
  virtual void visit (const UntypedComplex& type)
  {
    default_action (type);
  }
  virtual void visit (const UntypedString& type)
  {
    default_action (type);
  }
  virtual void visit (const Void& type)
  {
    default_action (type);
  }
  virtual void visit (const PolymorphicFunction& type)
  {
    default_action (type);
  }
  virtual void visit (const Uintptr& type)
  {
    default_action (type);
  }
  virtual void visit (const PushPort& type)
  {
    default_action (type);
  }
  virtual void visit (const PullPort& type)
  {
    default_action (type);
  }
  virtual void visit (const Initializer& type)
  {
    default_action (type);
  }
  virtual void visit (const Getter& type)
  {
    default_action (type);
  }
  virtual void visit (const Reaction& type)
  {
    default_action (type);
  }

  virtual void default_action (const Type& type) { }
};

// template <typename T, typename T1>
// struct visitor2 : public DefaultVisitor
// {
//   const T1& type1;
//   T& t;

//   visitor2 (const T1& t1, T& t_) : type1 (t1), t (t_) { }

//   void default_action (const Type& type)
//   {
//     TYPE_NOT_REACHED (type);
//   }

//   void visit (const Map& type2)
//   {
//     t (type1, type2);
//   }

//   void visit (const Interface& type2)
//   {
//     t (type1, type2);
//   }

//   void visit (const Array& type2)
//   {
//     t (type1, type2);
//   }

//   void visit (const Bool& type2)
//   {
//     t (type1, type2);
//   }

//   void visit (const Int& type2)
//   {
//     t (type1, type2);
//   }
//   void visit (const Int8& type2)
//   {
//     t (type1, type2);
//   }
//   void visit (const Int16& type2)
//   {
//     t (type1, type2);
//   }
//   void visit (const Int32& type2)
//   {
//     t (type1, type2);
//   }
//   void visit (const Int64& type2)
//   {
//     t (type1, type2);
//   }

//   void visit (const Uint& type2)
//   {
//     t (type1, type2);
//   }
//   void visit (const Uint8& type2)
//   {
//     t (type1, type2);
//   }
//   void visit (const Uint16& type2)
//   {
//     t (type1, type2);
//   }
//   void visit (const Uint32& type2)
//   {
//     t (type1, type2);
//   }
//   void visit (const Uint64& type2)
//   {
//     t (type1, type2);
//   }

//   void visit (const Float32& type2)
//   {
//     t (type1, type2);
//   }
//   void visit (const Float64& type2)
//   {
//     t (type1, type2);
//   }

//   void visit (const Complex64& type2)
//   {
//     t (type1, type2);
//   }
//   void visit (const Complex128& type2)
//   {
//     t (type1, type2);
//   }

//   void visit (const Pointer& type2)
//   {
//     t (type1, type2);
//   }

//   void visit (const Slice& type2)
//   {
//     t (type1, type2);
//   }

//   void visit (const String& type2)
//   {
//     t (type1, type2);
//   }

//   void visit (const Struct& type2)
//   {
//     t (type1, type2);
//   }

//   void visit (const UntypedBoolean& type2)
//   {
//     t (type1, type2);
//   }
//   void visit (const UntypedRune& type2)
//   {
//     t (type1, type2);
//   }
//   void visit (const UntypedInteger& type2)
//   {
//     t (type1, type2);
//   }
//   void visit (const UntypedFloat& type2)
//   {
//     t (type1, type2);
//   }

//   void visit (const UntypedString& type2)
//   {
//     t (type1, type2);
//   }

//   void visit (const UntypedNil& type2)
//   {
//     t (type1, type2);
//   }

//   void visit (const Function& type2)
//   {
//     t (type1, type2);
//   }
// };

// template <typename T, typename T1>
// static void doubleDispatchHelper (const T1& type1, const Type* type2, T& t)
// {
//   visitor2<T, T1> v (type1, t);
//   type2->accept (v);
// }

// template <typename T>
// struct visitor1 : public DefaultVisitor
// {
//   const Type* type2;
//   T& t;
//   visitor1 (const Type* t2, T& t_) : type2 (t2), t (t_) { }

//   void default_action (const Type& type)
//   {
//     TYPE_NOT_REACHED (type);
//   }

//   void visit (const Map& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }

//   void visit (const Interface& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }

//   void visit (const Bool& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }

//   void visit (const Int& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }
//   void visit (const Int8& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }
//   void visit (const Int16& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }
//   void visit (const Int32& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }
//   void visit (const Int64& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }

//   void visit (const Uint& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }
//   void visit (const Uint8& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }
//   void visit (const Uint16& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }
//   void visit (const Uint32& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }
//   void visit (const Uint64& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }
//   void visit (const Float32& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }
//   void visit (const Float64& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }
//   void visit (const Complex64& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }
//   void visit (const Complex128& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }

//   void visit (const Pointer& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }

//   void visit (const Array& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }

//   void visit (const String& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }

//   void visit (const Slice& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }

//   void visit (const Struct& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }

//   void visit (const UntypedBoolean& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }

//   void visit (const UntypedRune& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }
//   void visit (const UntypedInteger& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }
//   void visit (const UntypedFloat& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }

//   void visit (const UntypedString& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }

//   void visit (const UntypedNil& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }

//   void visit (const Function& type)
//   {
//     doubleDispatchHelper (type, type2, t);
//   }
// };

// template <typename T>
// static void DoubleDispatch (const Type* type1, const Type* type2, T& t)
// {
//   visitor1<T> v (type2, t);
//   type1->accept (v);
// }

// Return the type of indexing into the other type.
const Type*
type_index (const Type* base, const Type* index);

bool
are_identical (const Type* x, const Type* y);

bool
assignable (const Type* from, const semantic::Value& from_value, const Type* to);

const Type*
Choose (const Type* x, const Type* y);

// True if any pointer is accessible.
bool
type_contains_pointer (const Type* type);

bool is_typed_boolean (const Type* type);
bool is_untyped_boolean (const Type* type);
bool is_any_boolean (const Type* type);

bool is_typed_string (const Type* type);
bool is_untyped_string (const Type* type);
bool is_any_string (const Type* type);

bool is_typed_integer (const Type* type);
bool is_typed_unsigned_integer (const Type* type);
bool is_typed_float (const Type* type);
bool is_typed_complex (const Type* type);
// True to typed numeric types.
bool is_typed_numeric (const Type* type);
// True for untyped numeric types.
bool is_untyped_numeric (const Type* type);
bool is_slice_of_bytes (const Type* type);
bool is_slice_of_runes (const Type* type);

// True if the type is comparable.
bool comparable (const Type* type);

// True if the type is orderable.
bool orderable (const Type* type);

// True if the types are arithmetic.
bool arithmetic (const Type* type);

bool integral (const Type* type);

// True if type is an integer.
bool
type_is_integral (const Type* type);

// True if type is an unsigned integer.
bool
type_is_unsigned_integral (const Type* type);

// True if type is floating-point.
bool
type_is_floating (const Type* type);

// True if == or != can be applied to values of this type.
bool
type_is_comparable (const Type* type);

// True if <, <=, >, or >= can be applied to values of this type.
bool
type_is_orderable (const Type* type);

// True if index is valid.
bool
type_is_index (const Type* type, Int::ValueType index);

// True if x can be cast to y.
bool
type_is_castable (const Type* x, const Type* y);

bool
type_is_pointer_compare (const Type* left, const Type* right);

// Remove a NamedType.
const Type*
type_strip (const Type* type);

// Cast a type to a specific type.
template<typename T>
const T*
type_cast (const Type * type)
{
  if (type == NULL) return NULL;

  struct visitor : public DefaultVisitor
  {
    const T* retval;

    visitor () : retval (NULL) { }

    void visit (const T& type)
    {
      retval = &type;
    }
  };
  visitor v;
  type->accept (v);
  return v.retval;
}

template<typename T>
const T*
type_strip_cast (const Type* type)
{
  if (type == NULL) return NULL;
  return type_cast<T> (type_strip (type));
}

// Return a Pointer if a pointer to an array or NULL.
const Pointer*
pointer_to_array (const Type* type);

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

inline C64 operator* (const C64&, const C64&)
{
  UNIMPLEMENTED;
}
inline C64 operator/ (const C64&, const C64&)
{
  UNIMPLEMENTED;
}


inline C128 operator* (const C128&, const C128&)
{
  UNIMPLEMENTED;
}
inline C128 operator/ (const C128&, const C128&)
{
  UNIMPLEMENTED;
}

inline UntypedComplex::ValueType operator* (const UntypedComplex::ValueType&, const UntypedComplex::ValueType&)
{
  UNIMPLEMENTED;
}
inline UntypedComplex::ValueType operator/ (const UntypedComplex::ValueType&, const UntypedComplex::ValueType&)
{
  UNIMPLEMENTED;
}

inline C64 operator+ (const C64&, const C64&)
{
  UNIMPLEMENTED;
}
inline C64 operator- (const C64&, const C64&)
{
  UNIMPLEMENTED;
}
inline C64 operator- (const C64&)
{
  UNIMPLEMENTED;
}

inline C128 operator+ (const C128&, const C128&)
{
  UNIMPLEMENTED;
}
inline C128 operator- (const C128&, const C128&)
{
  UNIMPLEMENTED;
}
inline C128 operator- (const C128&)
{
  UNIMPLEMENTED;
}

inline UntypedComplex::ValueType operator+ (const UntypedComplex::ValueType&, const UntypedComplex::ValueType&)
{
  UNIMPLEMENTED;
}
inline UntypedComplex::ValueType operator- (const UntypedComplex::ValueType&, const UntypedComplex::ValueType&)
{
  UNIMPLEMENTED;
}
inline UntypedComplex::ValueType operator- (const UntypedComplex::ValueType&)
{
  UNIMPLEMENTED;
}

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

extern NamedType named_file_descriptor;
extern NamedType named_timespec;

inline std::ostream& operator<< (std::ostream& out, const StringRep& s)
{
  return out << std::string (static_cast<const char*> (s.ptr), s.length);
}

}

#endif // RC_SRC_TYPE_HPP
