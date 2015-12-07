#ifndef rc_runtime_hpp
#define rc_runtime_hpp

#include "types.hpp"
#include "Template.hpp"
#include "Type.hpp"
#include "executor_base.hpp"
#include "Location.hpp"

namespace runtime
{
void
allocate_instances (Composition::Composer& instance_table);

void
create_bindings (Composition::Composer& instance_table);

void
initialize (executor_base_t& exec, Composition::Instance* instance);

// Returns true if the action is enabled.
bool enabled (executor_base_t& exec,
              component_t* instance,
              const decl::Action* action,
              Type::Int::ValueType iota);

// Returns true if the action was executed.
bool execute (executor_base_t& exec,
              component_t* instance,
              const decl::Action* action,
              Type::Int::ValueType iota);

// Execute the action without checking the precondition.
// Returns true.
bool execute_no_check (executor_base_t& exec,
                       component_t* instance,
                       const decl::Action* action,
                       Type::Int::ValueType iota);

enum ControlAction
{
  kReturn,
  kContinue,
};

struct New : public ::Template
{
  New (ast::Node* dn);
  virtual Callable* instantiate (const std::vector<const Type::Type*>& argument_types) const;
};

struct Move : public ::Template
{
  Move (ast::Node* dn);
  virtual Callable* instantiate (const std::vector<const Type::Type*>& argument_types) const;
};

struct Merge : public ::Template
{
  Merge (ast::Node* dn);
  virtual Callable* instantiate (const std::vector<const Type::Type*>& argument_types) const;
};

struct Copy : public ::Template
{
  Copy (ast::Node* dn);
  virtual Callable* instantiate (const std::vector<const Type::Type*>& argument_types) const;
};

struct Println : public ::Template
{
  Println (ast::Node* dn);
  virtual Callable* instantiate (const std::vector<const Type::Type*>& argument_types) const;
};

// Operations
struct Operation
{
  virtual ~Operation() { }
  virtual ControlAction execute (executor_base_t& exec) const = 0;
  virtual void dump () const = 0;
};

struct Load : public Operation
{
  Load (const Operation* c, const Type::Type* t) : child (c), type (t) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    std::cout << "Load(";
    child->dump ();
    std::cout << *type << ")\n";
  }
  const Operation* const child;
  const Type::Type* const type;
};

struct IndexArray : public Operation
{
  IndexArray (const Location& l, Operation* b, Operation* i, const Type::Array* t) : location (l), base (b), index (i), type (t) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    std::cout << "IndexArray (";
    base->dump ();
    std::cout << ", ";
    index->dump ();
    std::cout << ")\n";
  }
  Location const location;
  Operation* const base;
  Operation* const index;
  const Type::Array* type;
};

struct IndexSlice : public Operation
{
  IndexSlice (const Location& l, const Operation* b, const Operation* i, const Type::Slice* t) : location (l), base (b), index (i), type (t) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  Location const location;
  const Operation* const base;
  const Operation* const index;
  const Type::Slice* type;
};

struct SliceArray : public Operation
{
  SliceArray (const Location& loc, Operation* b, Operation* l, Operation* h, const Type::Array* t) : location (loc), base (b), low (l), high (h), type (t) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  Location const location;
  Operation* const base;
  Operation* const low;
  Operation* const high;
  const Type::Array* type;
};

Operation* MakeConvertToInt (const Operation* c, const Type::Type* type);
Operation* MakeConvertToUint (const Operation* c, const Type::Type* type);

template <typename T>
struct Literal : public Operation
{
  Literal (T v) : value (v) { }
  virtual ControlAction execute (executor_base_t& exec) const
  {
    exec.stack ().push (value);
    return kContinue;
  }
  virtual void dump () const
  {
    std::cout << "Literal value=" << value << '\n';
  }
  T const value;
};

template <typename T>
inline Operation*
make_literal (T v)
{
  return new Literal<T> (v);
}

Operation* make_literal (const Type::Type* type, const value_t& value);

struct LogicOr : public Operation
{
  LogicOr (const Operation* l, const Operation* r) : left (l), right (r) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  const Operation* const left;
  const Operation* const right;
};

struct LogicAnd : public Operation
{
  LogicAnd (const Operation* l, const Operation* r) : left (l), right (r) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  const Operation* const left;
  const Operation* const right;
};

// template <typename O, typename T>
// struct Binary : public Operation
// {
//   Binary (const Location& loc, const Operation* l, const Operation* r) : location (loc), left (l), right (r) { }
//   virtual ControlAction execute (executor_base_t& exec) const
//   {
//     left->execute (exec);
//     typename T::ValueType left;
//     exec.stack ().pop (left);
//     right->execute (exec);
//     typename T::ValueType right;
//     exec.stack ().pop (right);
//     exec.stack ().push (O () (location, left, right));
//   }

//   Location const location;
//   const Operation* const left;
//   const Operation* const right;
// };

// template <typename O>
// struct BinaryGenerator
// {
//   const Location& location;
//   const Operation* left;
//   const Operation* right;

//   BinaryGenerator (const Location& loc, const Operation* l, const Operation* r) : location (loc), left (l), right (r) { }
//   Operation* operation;

//   template <typename T>
//   void operator() (const T& type)
//   {
//     operation = new Binary<O, T> (location, left, right);
//   }

//   void NotArithmetic (const Type::Type& type)
//   {
//     type_not_reached (type);
//   }

//   void NotIntegral (const Type::Type& t)
//   {
//     type_not_reached (t);
//   }
// };

// template <template <typename S> class Visitor, typename T>
// Operation* make_binary (const Type::Type* type, const Location& location, const Operation* left, const Operation* right)
// {
//   BinaryGenerator<T> g (location, left, right);
//   Visitor<BinaryGenerator<T> > visitor (g);
//   type->Accept (visitor);
//   return g.operation;
// }

struct ListOperation : public Operation
{
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  typedef std::vector<Operation*> ListType;
  ListType list;
};

struct FunctionCall : public Operation
{
  FunctionCall (const Callable* c, Operation* o) : callable (c), arguments (o) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  const Callable* const callable;
  Operation* const arguments;
};

struct MethodCall : public Operation
{
  MethodCall (const Callable* c, Operation* r, Operation* o) : callable (c), receiver (r), arguments (o) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  const Callable* const callable;
  Operation* const receiver;
  Operation* const arguments;
};

struct DynamicFunctionCall : public Operation
{
  DynamicFunctionCall (const Type::Function* t, Operation* f, Operation* a) : type (t), func (f), arguments (a) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  const Type::Function* type;
  Operation* const func;
  Operation* const arguments;
};

struct Instance : public Operation
{
  Instance (InstanceSymbol* i) : instance (i) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  InstanceSymbol* instance;
};

struct SetRestoreCurrentInstance : public Operation
{
  SetRestoreCurrentInstance (Operation* c, ptrdiff_t o) : child (c), receiver_offset (o) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  Operation* const child;
  ptrdiff_t const receiver_offset;
};

struct Clear : public Operation
{
  Clear (ptrdiff_t o, size_t s) : offset (o), size (s) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  ptrdiff_t const offset;
  size_t const size;
};

struct Assign : public Operation
{
  Assign (Operation* l, Operation* r, const Type::Type* t) : left (l), right (r), size (t->Size ())
  {
    assert (left != NULL);
    assert (right != NULL);
  }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  Operation* const left;
  Operation* const right;
  size_t const size;
};

Operation* make_add_assign (Operation* l, Operation* r, const Type::Type* t);

struct Reference : public Operation
{
  Reference (ptrdiff_t o) : offset (o) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    std::cout << "Reference offset=" << offset << '\n';
  }
  ptrdiff_t const offset;
};

struct Select : public Operation
{
  Select (Operation* b, ptrdiff_t o) : base (b), offset (o) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    std::cout << "Select (";
    base->dump ();
    std::cout << " offset = " << offset << ")\n";
  }
  Operation* const base;
  ptrdiff_t const offset;
};

struct Return : public Operation
{
  Return (Operation* c, const ParameterSymbol* r) : child (c), return_offset (r->offset ()), return_size (r->type->Size ()) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  Operation* const child;
  ptrdiff_t const return_offset;
  size_t const return_size;
};

struct If : public Operation
{
  If (Operation* c, Operation* t, Operation* f) : condition (c), true_branch (t), false_branch (f) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  Operation* const condition;
  Operation* const true_branch;
  Operation* const false_branch;
};

struct While : public Operation
{
  While (Operation* c, Operation* b) : condition (c), body (b) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  Operation* const condition;
  Operation* const body;
};

struct ForIota : public Operation
{
  ForIota (const VariableSymbol* symbol, Type::Int::ValueType l, Operation* b) : offset (symbol->offset ()), limit (l), body (b) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  ptrdiff_t const offset;
  Type::Int::ValueType const limit;
  Operation* const body;
};

template <typename T>
struct Unary : public Operation
{
  Unary (Operation* c) : child (c) { }
  virtual ControlAction execute (executor_base_t& exec) const
  {
    typename T::ValueType x;
    child->execute (exec);
    exec.stack ().pop (x);
    exec.stack ().push (T () (x));
    return kContinue;
  }
  virtual void dump () const
  {
    unimplemented;
  }
  Operation* const child;
};

template <template <typename S> class T>
Operation* make_unary (const Type::Type* type, Operation* child)
{
  struct visitor : public Type::DefaultVisitor
  {
    Operation* child;
    Operation* operation;

    visitor (Operation* c) : child (c), operation (NULL) { }

    void default_action (const Type::Type& type)
    {
      type_not_reached (type);
    }

    void visit (const Type::Bool& type)
    {
      operation = new Unary<T<Type::Bool::ValueType> > (child);
    }
  };
  visitor v (child);
  type->UnderlyingType ()->Accept (v);
  return v.operation;
}

template <typename V, typename T>
struct Binary : public Operation
{
  Binary (Operation* l, Operation* r) : left (l), right (r) { }
  virtual ControlAction execute (executor_base_t& exec) const
  {
    V x;
    V y;
    left->execute (exec);
    exec.stack ().pop (x);
    right->execute (exec);
    exec.stack ().pop (y);
    exec.stack ().push (T () (x, y));
    return kContinue;
  }
  virtual void dump () const
  {
    unimplemented;
  }
  Operation* const left;
  Operation* const right;
};

template <typename V, typename T>
struct Shift : public Operation
{
  Shift (Operation* l, Operation* r) : left (l), right (r) { }
  virtual ControlAction execute (executor_base_t& exec) const
  {
    V x;
    Type::Uint::ValueType y;
    left->execute (exec);
    exec.stack ().pop (x);
    right->execute (exec);
    exec.stack ().pop (y);
    exec.stack ().push (T () (x, y));
    return kContinue;
  }
  virtual void dump () const
  {
    unimplemented;
  }
  Operation* const left;
  Operation* const right;
};

template <typename T>
Operation* make_binary_arithmetic (const Type::Type* type, Operation* left, Operation* right)
{
  switch (type->underlying_kind ())
    {
    case Type::kBool:
      return new Binary<Type::Bool::ValueType, T> (left, right);
    case Type::kUint8:
      return new Binary<Type::Uint8::ValueType, T> (left, right);
    case Type::kUint16:
      return new Binary<Type::Uint16::ValueType, T> (left, right);
    case Type::kUint32:
      return new Binary<Type::Uint32::ValueType, T> (left, right);
    case Type::kUint64:
      return new Binary<Type::Uint64::ValueType, T> (left, right);
    case Type::kInt8:
      return new Binary<Type::Int8::ValueType, T> (left, right);
    case Type::kInt16:
      return new Binary<Type::Int16::ValueType, T> (left, right);
    case Type::kInt32:
      return new Binary<Type::Int32::ValueType, T> (left, right);
    case Type::kInt64:
      return new Binary<Type::Int64::ValueType, T> (left, right);
    case Type::kFloat32:
      return new Binary<Type::Float32::ValueType, T> (left, right);
    case Type::kFloat64:
      return new Binary<Type::Float64::ValueType, T> (left, right);
    case Type::kComplex64:
      return new Binary<Type::Complex64::ValueType, T> (left, right);
    case Type::kComplex128:
      return new Binary<Type::Complex128::ValueType, T> (left, right);
    case Type::kUint:
      return new Binary<Type::Uint::ValueType, T> (left, right);
    case Type::kInt:
      return new Binary<Type::Int::ValueType, T> (left, right);
    case Type::kUintptr:
      return new Binary<Type::Uintptr::ValueType, T> (left, right);
    case Type::kPointer:
      return new Binary<Type::Uintptr::ValueType, T> (left, right);

    default:
      type_not_reached (*type);
    }
}

template <typename T>
Operation* make_binary_integral (const Type::Type* type, Operation* left, Operation* right)
{
  switch (type->underlying_kind ())
    {
    case Type::kUint8:
      return new Binary<Type::Uint8::ValueType, T> (left, right);
    case Type::kUint16:
      return new Binary<Type::Uint16::ValueType, T> (left, right);
    case Type::kUint32:
      return new Binary<Type::Uint32::ValueType, T> (left, right);
    case Type::kUint64:
      return new Binary<Type::Uint64::ValueType, T> (left, right);
    case Type::kInt8:
      return new Binary<Type::Int8::ValueType, T> (left, right);
    case Type::kInt16:
      return new Binary<Type::Int16::ValueType, T> (left, right);
    case Type::kInt32:
      return new Binary<Type::Int32::ValueType, T> (left, right);
    case Type::kInt64:
      return new Binary<Type::Int64::ValueType, T> (left, right);
    case Type::kUint:
      return new Binary<Type::Uint::ValueType, T> (left, right);
    case Type::kInt:
      return new Binary<Type::Int::ValueType, T> (left, right);
    case Type::kUintptr:
      return new Binary<Type::Uintptr::ValueType, T> (left, right);

    default:
      type_not_reached (*type);
    }
}

template <typename T>
Operation* make_shift (const Type::Type* type, Operation* left, Operation* right)
{
  switch (type->underlying_kind ())
    {
    case Type::kUint8:
      return new Shift<Type::Uint8::ValueType, T> (left, right);
    case Type::kUint16:
      return new Shift<Type::Uint16::ValueType, T> (left, right);
    case Type::kUint32:
      return new Shift<Type::Uint32::ValueType, T> (left, right);
    case Type::kUint64:
      return new Shift<Type::Uint64::ValueType, T> (left, right);
    case Type::kInt8:
      return new Shift<Type::Int8::ValueType, T> (left, right);
    case Type::kInt16:
      return new Shift<Type::Int16::ValueType, T> (left, right);
    case Type::kInt32:
      return new Shift<Type::Int32::ValueType, T> (left, right);
    case Type::kInt64:
      return new Shift<Type::Int64::ValueType, T> (left, right);
    case Type::kUint:
      return new Shift<Type::Uint::ValueType, T> (left, right);
    case Type::kInt:
      return new Shift<Type::Int::ValueType, T> (left, right);
    case Type::kUintptr:
      return new Shift<Type::Uintptr::ValueType, T> (left, right);

    default:
      type_not_reached (*type);
    }
}

struct Change : public Operation
{
  Change (Operation* r, ptrdiff_t o, Operation* b) : root (r), root_offset (o), body (b) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  Operation* const root;
  ptrdiff_t const root_offset;
  Operation* const body;
};

Operation* make_increment (Operation* child, const Type::Type* type);

struct Activate : public Operation
{
  Activate (Operation* pc, Operation* b) : port_calls (pc), body (b) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  Operation* const port_calls;
  Operation* const body;
};

struct PushPortCall : public Operation
{
  PushPortCall (ptrdiff_t ro, ptrdiff_t po, Operation* o) : receiver_offset (ro), port_offset (po), args (o) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  ptrdiff_t const receiver_offset;
  ptrdiff_t const port_offset;
  Operation* const args;
};

struct IndexedPushPortCall : public Operation
{
  IndexedPushPortCall (ptrdiff_t ro, ptrdiff_t po, Operation* i, Operation* o, const Type::Array* a) : receiver_offset (ro), port_offset (po), index (i), args (o), array_type (a) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  ptrdiff_t const receiver_offset;
  ptrdiff_t const port_offset;
  Operation* const index;
  Operation* const args;
  const Type::Array* const array_type;
};

struct Push : public Operation
{
  Push (Operation* b) : body (b) { }
  virtual ControlAction execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    unimplemented;
  }
  Operation* const body;
};

struct Noop : public Operation
{
  virtual ControlAction execute (executor_base_t& exec) const
  {
    return kContinue;
  }
  virtual void dump () const
  {
    unimplemented;
  }
};

Operation* make_conversion (Operation* c, const Type::Type* from, const Type::Type* to);
}

#endif // rc_runtime_hpp
