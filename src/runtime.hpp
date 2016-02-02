#ifndef RC_SRC_RUNTIME_HPP
#define RC_SRC_RUNTIME_HPP

#include "types.hpp"
#include "template.hpp"
#include "executor_base.hpp"
#include "location.hpp"

namespace runtime
{
void
allocate_instances (composition::Composer& instance_table);

void
create_bindings (composition::Composer& instance_table);

void
initialize (executor_base_t& exec, composition::Instance* instance);

#ifdef PROFILE_LATENCY
  extern FILE* latency_file;
#endif

// Returns true if the action is enabled.
bool enabled (executor_base_t& exec,
              component_t* instance,
              const decl::Action* action,
              type::Int::ValueType iota);

// Returns true if the action was executed.
bool execute (executor_base_t& exec,
              component_t* instance,
              const decl::Action* action,
              type::Int::ValueType iota);

// Execute the action without checking the precondition.
// Returns true.
bool execute_no_check (executor_base_t& exec,
                       component_t* instance,
                       const decl::Action* action,
                       type::Int::ValueType iota);

enum ControlAction
{
  kReturn,
  kContinue,
};

struct New : public decl::Template
{
  New (const util::Location& loc);
  virtual decl::Callable* instantiate (util::ErrorReporter& er,
                                       const std::vector<const type::Type*>& argument_types) const;
};

struct Move : public decl::Template
{
  Move (const util::Location& loc);
  virtual decl::Callable* instantiate (util::ErrorReporter& er,
                                       const std::vector<const type::Type*>& argument_types) const;
};

struct Merge : public decl::Template
{
  Merge (const util::Location& loc);
  virtual decl::Callable* instantiate (util::ErrorReporter& er,
                                       const std::vector<const type::Type*>& argument_types) const;
};

struct Len : public decl::Template
{
  Len (const util::Location& loc);
  virtual decl::Callable* instantiate (util::ErrorReporter& er,
                                       const std::vector<const type::Type*>& argument_types) const;
};

struct Append : public decl::Template
{
  Append (const util::Location& loc);
  virtual decl::Callable* instantiate (util::ErrorReporter& er,
                                       const std::vector<const type::Type*>& argument_types) const;
};

struct Copy : public decl::Template
{
  Copy (const util::Location& loc);
  virtual decl::Callable* instantiate (util::ErrorReporter& er,
                                       const std::vector<const type::Type*>& argument_types) const;
};

struct Println : public decl::Template
{
  Println (const util::Location& loc);
  virtual decl::Callable* instantiate (util::ErrorReporter& er,
                                       const std::vector<const type::Type*>& argument_types) const;
};


typedef ControlAction OpReturn;

inline OpReturn make_continue ()
{
  return kContinue;
}

inline OpReturn make_return ()
{
  return kReturn;
}

// Operations
struct Operation
{
  virtual ~Operation() { }
  virtual OpReturn execute (executor_base_t& exec) const = 0;
  virtual void dump () const = 0;
};

struct Load : public Operation
{
  Load (const Operation* c, const type::Type* t) : child (c), type (t) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    std::cout << "Load(";
    child->dump ();
    std::cout << *type << ")\n";
  }
  const Operation* const child;
  const type::Type* const type;
};

struct IndexArray : public Operation
{
  IndexArray (const util::Location& l, Operation* b, Operation* i, const type::Array* t) : location (l), base (b), index (i), type (t) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    std::cout << "IndexArray (";
    base->dump ();
    std::cout << ", ";
    index->dump ();
    std::cout << ")\n";
  }
  util::Location const location;
  Operation* const base;
  Operation* const index;
  const type::Array* type;
};

struct IndexSlice : public Operation
{
  IndexSlice (const util::Location& l, const Operation* b, const Operation* i, const type::Slice* t) : location (l), base (b), index (i), type (t) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    std::cout << "IndexSlice(";
    base->dump ();
    std::cout << ",";
    index->dump ();
    std::cout << ")";
  }
  util::Location const location;
  const Operation* const base;
  const Operation* const index;
  const type::Slice* type;
};

struct SliceArray : public Operation
{
  SliceArray (const util::Location& loc,
              Operation* b,
              Operation* l,
              Operation* h,
              Operation* m,
              const type::Array* t)
    : location (loc)
    , base (b)
    , low (l)
    , high (h)
    , max (m)
    , type (t)
  { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    std::cout << "SliceArray(";
    base->dump ();
    std::cout << ",";
    if (low)
      {
        low->dump ();
      }
    std::cout << ",";
    if (high)
      {
        high->dump ();
      }
    std::cout << ",";
    if (max)
      {
        max->dump ();
      }
    std::cout << ")";
  }
  util::Location const location;
  Operation* const base;
  Operation* const low;
  Operation* const high;
  Operation* const max;
  const type::Array* type;
};

struct SliceSlice : public Operation
{
  SliceSlice (const util::Location& loc,
              Operation* b,
              Operation* l,
              Operation* h,
              Operation* m,
              const type::Slice* t)
    : location (loc)
    , base (b)
    , low (l)
    , high (h)
    , max (m)
    , type (t)
  { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    std::cout << "SliceSlice(";
    base->dump ();
    std::cout << ",";
    if (low)
      {
        low->dump ();
      }
    std::cout << ",";
    if (high)
      {
        high->dump ();
      }
    std::cout << ",";
    if (max)
      {
        max->dump ();
      }
    std::cout << ")";
  }
  util::Location const location;
  Operation* const base;
  Operation* const low;
  Operation* const high;
  Operation* const max;
  const type::Slice* type;
};

Operation* MakeConvertToInt (const Operation* c, const type::Type* type);
Operation* MakeConvertToUint (const Operation* c, const type::Type* type);

template <typename T>
struct Literal : public Operation
{
  Literal (T v) : value (v) { }
  virtual OpReturn execute (executor_base_t& exec) const
  {
    exec.stack ().push (value);
    return make_continue ();
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

Operation* make_literal (const type::Type* type, const semantic::Value& value);

struct LogicOr : public Operation
{
  LogicOr (const Operation* l, const Operation* r) : left (l), right (r) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  const Operation* const left;
  const Operation* const right;
};

struct LogicAnd : public Operation
{
  LogicAnd (const Operation* l, const Operation* r) : left (l), right (r) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  const Operation* const left;
  const Operation* const right;
};

struct ListOperation : public Operation
{
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    std::cout << "List(";
    for (ListType::const_iterator pos = list.begin (), limit = list.end ();
         pos != limit;
         ++pos)
      {
        (*pos)->dump ();
        std::cout << ",";
      }
    std::cout << ")";
  }
  typedef std::vector<Operation*> ListType;
  ListType list;
};

struct FunctionCall : public Operation
{
  FunctionCall (const decl::Callable* c, Operation* o) : callable (c), arguments (o) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    std::cout << "Function(";
    arguments->dump ();
    std::cout << ")";
  }
  const decl::Callable* const callable;
  Operation* const arguments;
};

struct MethodCall : public Operation
{
  MethodCall (const decl::Callable* c, Operation* r, Operation* o) : callable (c), receiver (r), arguments (o) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    std::cout << "Method(";
    receiver->dump ();
    std::cout << ", ";
    arguments->dump ();
    std::cout << ")";
  }
  const decl::Callable* const callable;
  Operation* const receiver;
  Operation* const arguments;
};

struct DynamicFunctionCall : public Operation
{
  DynamicFunctionCall (const type::Function* t, Operation* f, Operation* a) : type (t), func (f), arguments (a) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  const type::Function* type;
  Operation* const func;
  Operation* const arguments;
};

struct Instance : public Operation
{
  Instance (decl::InstanceSymbol* i) : instance (i) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    std::cout << "Instance(" << instance << ")";
  }
  decl::InstanceSymbol* instance;
};

struct SetRestoreCurrentInstance : public Operation
{
  SetRestoreCurrentInstance (Operation* c, ptrdiff_t o) : child (c), receiver_offset (o) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    std::cout << "SetRestoreCurrentInstance(";
    child->dump ();
    std::cout << "," << receiver_offset << ")";
  }
  Operation* const child;
  ptrdiff_t const receiver_offset;
};

struct Clear : public Operation
{
  Clear (ptrdiff_t o, size_t s) : offset (o), size (s) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  ptrdiff_t const offset;
  size_t const size;
};

struct Assign : public Operation
{
  Assign (Operation* l, Operation* r, const type::Type* t) : left (l), right (r), size (t->Size ())
  {
    assert (left != NULL);
    assert (right != NULL);
  }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const left;
  Operation* const right;
  size_t const size;
};

Operation* make_add_assign (Operation* l, Operation* r, const type::Type* t);

struct Reference : public Operation
{
  Reference (ptrdiff_t o) : offset (o) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    std::cout << "Reference offset=" << offset << '\n';
  }
  ptrdiff_t const offset;
};

struct Select : public Operation
{
  Select (Operation* b, ptrdiff_t o) : base (b), offset (o) { }
  virtual OpReturn execute (executor_base_t& exec) const;
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
  Return (Operation* c, const decl::ParameterSymbol* r) : child (c), return_offset (r->offset ()), return_size (r->type->Size ()) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const child;
  ptrdiff_t const return_offset;
  size_t const return_size;
};

struct If : public Operation
{
  If (Operation* c, Operation* t, Operation* f) : condition (c), true_branch (t), false_branch (f) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const condition;
  Operation* const true_branch;
  Operation* const false_branch;
};

struct While : public Operation
{
  While (Operation* c, Operation* b) : condition (c), body (b) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const condition;
  Operation* const body;
};

struct ForIota : public Operation
{
  ForIota (const decl::VariableSymbol* symbol, type::Int::ValueType l, Operation* b) : offset (symbol->offset ()), limit (l), body (b) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  ptrdiff_t const offset;
  type::Int::ValueType const limit;
  Operation* const body;
};

template <typename T>
struct Unary : public Operation
{
  Unary (Operation* c) : child (c) { }
  virtual OpReturn execute (executor_base_t& exec) const
  {
    typename T::ValueType x;
    child->execute (exec);
    exec.stack ().pop (x);
    exec.stack ().push (T () (x));
    return make_continue ();
  }
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const child;
};

template <template <typename S> class T>
Operation* make_unary (const type::Type* type, Operation* child)
{
  struct visitor : public type::DefaultVisitor
  {
    Operation* child;
    Operation* operation;

    visitor (Operation* c) : child (c), operation (NULL) { }

    void default_action (const type::Type& type)
    {
      TYPE_NOT_REACHED (type);
    }

    void visit (const type::Bool& type)
    {
      operation = new Unary<T<type::Bool::ValueType> > (child);
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
  virtual OpReturn execute (executor_base_t& exec) const
  {
    V x;
    V y;
    left->execute (exec);
    exec.stack ().pop (x);
    right->execute (exec);
    exec.stack ().pop (y);
    exec.stack ().push (T () (x, y));
    return make_continue ();
  }
  virtual void dump () const
  {
    std::cout << "Binary(";
    left->dump ();
    std::cout << ",";
    right->dump ();
    std::cout << ")";
  }
  Operation* const left;
  Operation* const right;
};

template <typename V, typename T>
struct Shift : public Operation
{
  Shift (Operation* l, Operation* r) : left (l), right (r) { }
  virtual OpReturn execute (executor_base_t& exec) const
  {
    V x;
    type::Uint::ValueType y;
    left->execute (exec);
    exec.stack ().pop (x);
    right->execute (exec);
    exec.stack ().pop (y);
    exec.stack ().push (T () (x, y));
    return make_continue ();
  }
  virtual void dump () const
  {
    std::cout << "Shift(";
    left->dump ();
    std::cout << ",";
    right->dump ();
    std::cout << ")";
  }
  Operation* const left;
  Operation* const right;
};

template <typename T>
Operation* make_binary_arithmetic (const type::Type* type, Operation* left, Operation* right)
{
  switch (type->underlying_kind ())
    {
    case type::kBool:
      return new Binary<type::Bool::ValueType, T> (left, right);
    case type::kUint8:
      return new Binary<type::Uint8::ValueType, T> (left, right);
    case type::kUint16:
      return new Binary<type::Uint16::ValueType, T> (left, right);
    case type::kUint32:
      return new Binary<type::Uint32::ValueType, T> (left, right);
    case type::kUint64:
      return new Binary<type::Uint64::ValueType, T> (left, right);
    case type::kInt8:
      return new Binary<type::Int8::ValueType, T> (left, right);
    case type::kInt16:
      return new Binary<type::Int16::ValueType, T> (left, right);
    case type::kInt32:
      return new Binary<type::Int32::ValueType, T> (left, right);
    case type::kInt64:
      return new Binary<type::Int64::ValueType, T> (left, right);
    case type::kFloat32:
      return new Binary<type::Float32::ValueType, T> (left, right);
    case type::kFloat64:
      return new Binary<type::Float64::ValueType, T> (left, right);
    case type::kComplex64:
      return new Binary<type::Complex64::ValueType, T> (left, right);
    case type::kComplex128:
      return new Binary<type::Complex128::ValueType, T> (left, right);
    case type::kUint:
      return new Binary<type::Uint::ValueType, T> (left, right);
    case type::kInt:
      return new Binary<type::Int::ValueType, T> (left, right);
    case type::kUintptr:
      return new Binary<type::Uintptr::ValueType, T> (left, right);
    case type::kPointer:
      return new Binary<type::Uintptr::ValueType, T> (left, right);

    default:
      TYPE_NOT_REACHED (*type);
    }
}

template <typename T>
Operation* make_binary_integral (const type::Type* type, Operation* left, Operation* right)
{
  switch (type->underlying_kind ())
    {
    case type::kUint8:
      return new Binary<type::Uint8::ValueType, T> (left, right);
    case type::kUint16:
      return new Binary<type::Uint16::ValueType, T> (left, right);
    case type::kUint32:
      return new Binary<type::Uint32::ValueType, T> (left, right);
    case type::kUint64:
      return new Binary<type::Uint64::ValueType, T> (left, right);
    case type::kInt8:
      return new Binary<type::Int8::ValueType, T> (left, right);
    case type::kInt16:
      return new Binary<type::Int16::ValueType, T> (left, right);
    case type::kInt32:
      return new Binary<type::Int32::ValueType, T> (left, right);
    case type::kInt64:
      return new Binary<type::Int64::ValueType, T> (left, right);
    case type::kUint:
      return new Binary<type::Uint::ValueType, T> (left, right);
    case type::kInt:
      return new Binary<type::Int::ValueType, T> (left, right);
    case type::kUintptr:
      return new Binary<type::Uintptr::ValueType, T> (left, right);

    default:
      TYPE_NOT_REACHED (*type);
    }
}

template <typename T>
Operation* make_shift (const type::Type* type, Operation* left, Operation* right)
{
  switch (type->underlying_kind ())
    {
    case type::kUint8:
      return new Shift<type::Uint8::ValueType, T> (left, right);
    case type::kUint16:
      return new Shift<type::Uint16::ValueType, T> (left, right);
    case type::kUint32:
      return new Shift<type::Uint32::ValueType, T> (left, right);
    case type::kUint64:
      return new Shift<type::Uint64::ValueType, T> (left, right);
    case type::kInt8:
      return new Shift<type::Int8::ValueType, T> (left, right);
    case type::kInt16:
      return new Shift<type::Int16::ValueType, T> (left, right);
    case type::kInt32:
      return new Shift<type::Int32::ValueType, T> (left, right);
    case type::kInt64:
      return new Shift<type::Int64::ValueType, T> (left, right);
    case type::kUint:
      return new Shift<type::Uint::ValueType, T> (left, right);
    case type::kInt:
      return new Shift<type::Int::ValueType, T> (left, right);
    case type::kUintptr:
      return new Shift<type::Uintptr::ValueType, T> (left, right);

    default:
      TYPE_NOT_REACHED (*type);
    }
}

struct Change : public Operation
{
  Change (Operation* r, ptrdiff_t o, Operation* b) : root (r), root_offset (o), body (b) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const root;
  ptrdiff_t const root_offset;
  Operation* const body;
};

Operation* make_increment (Operation* child, const type::Type* type);

struct Activate : public Operation
{
  Activate (Operation* pc, Operation* b) : port_calls (pc), body (b) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const port_calls;
  Operation* const body;
};

struct PushPortCall : public Operation
{
  PushPortCall (ptrdiff_t ro, ptrdiff_t po, Operation* o) : receiver_offset (ro), port_offset (po), args (o) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  ptrdiff_t const receiver_offset;
  ptrdiff_t const port_offset;
  Operation* const args;
};

struct IndexedPushPortCall : public Operation
{
  IndexedPushPortCall (ptrdiff_t ro, ptrdiff_t po, Operation* i, Operation* o, const type::Array* a) : receiver_offset (ro), port_offset (po), index (i), args (o), array_type (a) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  ptrdiff_t const receiver_offset;
  ptrdiff_t const port_offset;
  Operation* const index;
  Operation* const args;
  const type::Array* const array_type;
};

struct Push : public Operation
{
  Push (Operation* b) : body (b) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const body;
};

struct Noop : public Operation
{
  virtual OpReturn execute (executor_base_t& exec) const
  {
    return make_continue ();
  }
  virtual void dump () const
  {
    std::cout << "Noop()";
  }
};

Operation* make_conversion (Operation* c, const type::Type* from, const type::Type* to);

struct Popn : public Operation
{
  Popn (Operation* c, size_t s) : child (c), size (s) { }
  virtual OpReturn execute (executor_base_t& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const child;
  size_t const size;
};

}

#endif // RC_SRC_RUNTIME_HPP
