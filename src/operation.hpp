#ifndef RC_SRC_OPERATION_HPP
#define RC_SRC_OPERATION_HPP

#include "location.hpp"
#include "executor_base.hpp"
#include "type.hpp"
#include "symbol.hpp"
#include "expression_value.hpp"

namespace runtime
{

enum Control
{
  Control_Return,
  Control_Continue,
};

// Operations
struct Operation
{
  virtual ~Operation() { }
  virtual Control execute (ExecutorBase& exec) const = 0;
  virtual void dump () const = 0;
};

struct Load : public Operation
{
  Load (const Operation* c, const type::Type* t) : child (c), type (t) { }
  virtual Control execute (ExecutorBase& exec) const;
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
  virtual Control execute (ExecutorBase& exec) const;
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
  virtual Control execute (ExecutorBase& exec) const;
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
  virtual Control execute (ExecutorBase& exec) const;
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
  virtual Control execute (ExecutorBase& exec) const;
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
  virtual Control execute (ExecutorBase& exec) const
  {
    exec.stack ().push (value);
    return Control_Continue;
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
  virtual Control execute (ExecutorBase& exec) const;
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
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  const Operation* const left;
  const Operation* const right;
};

struct ListOperation : public Operation
{
  ListOperation () { }
  ListOperation (Operation* o)
  {
    list.push_back (o);
  }
  ListOperation (Operation* o1, Operation* o2)
  {
    list.push_back (o1);
    list.push_back (o2);
  }


  virtual Control execute (ExecutorBase& exec) const;
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
  virtual Control execute (ExecutorBase& exec) const;
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
  virtual Control execute (ExecutorBase& exec) const;
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

struct DynamicPullPortCall : public Operation
{
  DynamicPullPortCall (const type::PullPort* t, Operation* f, Operation* a) : type (t), func (f), arguments (a) { }
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  const type::PullPort* type;
  Operation* const func;
  Operation* const arguments;
};

struct Instance : public Operation
{
  Instance (decl::Instance* i) : instance (i) { }
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    std::cout << "Instance(" << instance << ")";
  }
  decl::Instance* instance;
};

struct SetRestoreCurrentInstance : public Operation
{
  SetRestoreCurrentInstance (Operation* c, ptrdiff_t o) : child (c), receiver_offset (o) { }
  virtual Control execute (ExecutorBase& exec) const;
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
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  ptrdiff_t const offset;
  size_t const size;
};

struct Assign : public Operation
{
  Assign (Operation* l, Operation* r, const type::Type* t)
    : left (l)
    , right (r)
    , size (arch::size (t))
  {
    assert (left != NULL);
    assert (right != NULL);
  }
  virtual Control execute (ExecutorBase& exec) const;
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
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    std::cout << "Reference offset=" << offset << '\n';
  }
  ptrdiff_t const offset;
};

struct Select : public Operation
{
  Select (Operation* b, ptrdiff_t o) : base (b), offset (o) { }
  virtual Control execute (ExecutorBase& exec) const;
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
  Return (Operation* c, const decl::Parameter* r)
    : child (c)
    , return_offset (r->offset ())
    , return_size (arch::size (r->type))
  { }
  virtual Control execute (ExecutorBase& exec) const;
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
  virtual Control execute (ExecutorBase& exec) const;
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
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const condition;
  Operation* const body;
};

struct ForIota : public Operation
{
  ForIota (const decl::Variable* symbol, long l, Operation* b) : offset (symbol->offset ()), limit (l), body (b) { }
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  ptrdiff_t const offset;
  long const limit;
  Operation* const body;
};

template <typename T>
struct Unary : public Operation
{
  Unary (Operation* c) : child (c) { }
  virtual Control execute (ExecutorBase& exec) const
  {
    typename T::ValueType x;
    child->execute (exec);
    exec.stack ().pop (x);
    exec.stack ().push (T () (x));
    return Control_Continue;
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
  if (type->underlying_type ()->kind () == type::Bool_Kind)
    {
      return new Unary<T<bool> > (child);
    }
  else
    {
      TYPE_NOT_REACHED (*type);
    }
}

template <typename V, typename T>
struct Binary : public Operation
{
  Binary (Operation* l, Operation* r) : left (l), right (r) { }
  virtual Control execute (ExecutorBase& exec) const
  {
    V x;
    V y;
    left->execute (exec);
    exec.stack ().pop (x);
    right->execute (exec);
    exec.stack ().pop (y);
    exec.stack ().push (T () (x, y));
    return Control_Continue;
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
  virtual Control execute (ExecutorBase& exec) const
  {
    V x;
    unsigned long y;
    left->execute (exec);
    exec.stack ().pop (x);
    right->execute (exec);
    exec.stack ().pop (y);
    exec.stack ().push (T () (x, y));
    return Control_Continue;
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
    case type::Bool_Kind:
      return new Binary<bool, T> (left, right);
    case type::Uint8_Kind:
      return new Binary<uint8_t, T> (left, right);
    case type::Uint16_Kind:
      return new Binary<uint16_t, T> (left, right);
    case type::Uint32_Kind:
      return new Binary<uint32_t, T> (left, right);
    case type::Uint64_Kind:
      return new Binary<uint64_t, T> (left, right);
    case type::Int8_Kind:
      return new Binary<int8_t, T> (left, right);
    case type::Int16_Kind:
      return new Binary<int16_t, T> (left, right);
    case type::Int32_Kind:
      return new Binary<int32_t, T> (left, right);
    case type::Int64_Kind:
      return new Binary<int64_t, T> (left, right);
    case type::Float32_Kind:
      return new Binary<float, T> (left, right);
    case type::Float64_Kind:
      return new Binary<double, T> (left, right);
    case type::Complex64_Kind:
      return new Binary<Complex64, T> (left, right);
    case type::Complex128_Kind:
      return new Binary<Complex128, T> (left, right);
    case type::Uint_Kind:
      return new Binary<unsigned long, T> (left, right);
    case type::Int_Kind:
      return new Binary<long, T> (left, right);
    case type::Uintptr_Kind:
      return new Binary<size_t, T> (left, right);
    case type::Pointer_Kind:
      return new Binary<ptrdiff_t, T> (left, right);

    default:
      TYPE_NOT_REACHED (*type);
    }
}

template <typename T>
Operation* make_binary_integral (const type::Type* type, Operation* left, Operation* right)
{
  switch (type->underlying_kind ())
    {
    case type::Uint8_Kind:
      return new Binary<uint8_t, T> (left, right);
    case type::Uint16_Kind:
      return new Binary<uint16_t, T> (left, right);
    case type::Uint32_Kind:
      return new Binary<uint32_t, T> (left, right);
    case type::Uint64_Kind:
      return new Binary<uint64_t, T> (left, right);
    case type::Int8_Kind:
      return new Binary<int8_t, T> (left, right);
    case type::Int16_Kind:
      return new Binary<int16_t, T> (left, right);
    case type::Int32_Kind:
      return new Binary<int32_t, T> (left, right);
    case type::Int64_Kind:
      return new Binary<int64_t, T> (left, right);
    case type::Uint_Kind:
      return new Binary<unsigned long, T> (left, right);
    case type::Int_Kind:
      return new Binary<long, T> (left, right);
    case type::Uintptr_Kind:
      return new Binary<size_t, T> (left, right);

    default:
      TYPE_NOT_REACHED (*type);
    }
}

template <typename T>
Operation* make_shift (const type::Type* type, Operation* left, Operation* right)
{
  switch (type->underlying_kind ())
    {
    case type::Uint8_Kind:
      return new Shift<uint8_t, T> (left, right);
    case type::Uint16_Kind:
      return new Shift<uint16_t, T> (left, right);
    case type::Uint32_Kind:
      return new Shift<uint32_t, T> (left, right);
    case type::Uint64_Kind:
      return new Shift<uint64_t, T> (left, right);
    case type::Int8_Kind:
      return new Shift<int8_t, T> (left, right);
    case type::Int16_Kind:
      return new Shift<int16_t, T> (left, right);
    case type::Int32_Kind:
      return new Shift<int32_t, T> (left, right);
    case type::Int64_Kind:
      return new Shift<int64_t, T> (left, right);
    case type::Uint_Kind:
      return new Shift<unsigned long, T> (left, right);
    case type::Int_Kind:
      return new Shift<long, T> (left, right);
    case type::Uintptr_Kind:
      return new Shift<size_t, T> (left, right);

    default:
      TYPE_NOT_REACHED (*type);
    }
}

struct Change : public Operation
{
  Change (Operation* r, ptrdiff_t o, Operation* b) : root (r), root_offset (o), body (b) { }
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const root;
  ptrdiff_t const root_offset;
  Operation* const body;
};

Operation* make_increment (Operation* child, const type::Type* type);
Operation* make_decrement (Operation* child, const type::Type* type);

struct Activate : public Operation
{
  Activate (Operation* pc, Operation* b) : port_calls (pc), body (b) { }
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const port_calls;
  Operation* const body;
};

struct PushPortCall : public Operation
{
  PushPortCall (const type::PushPort* a_push_port,
                ptrdiff_t ro,
                ptrdiff_t po,
                Operation* o)
    : arguments_size (arch::size_on_stack (a_push_port->parameter_list))
    , receiver_offset (ro)
    , port_offset (po)
    , args (o)
  { }
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  size_t const arguments_size;
  ptrdiff_t const receiver_offset;
  ptrdiff_t const port_offset;
  Operation* const args;
};

struct IndexedPushPortCall : public Operation
{
  IndexedPushPortCall (const type::PushPort* a_push_port,
                       ptrdiff_t ro,
                       ptrdiff_t po,
                       Operation* i,
                       Operation* o,
                       const type::Array* a)
    : arguments_size (arch::size_on_stack (a_push_port->parameter_list))
    , receiver_offset (ro)
    , port_offset (po)
    , index (i)
    , args (o)
    , array_type (a)
  {
  }
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  size_t const arguments_size;
  ptrdiff_t const receiver_offset;
  ptrdiff_t const port_offset;
  Operation* const index;
  Operation* const args;
  const type::Array* const array_type;
};

struct Push : public Operation
{
  Push (Operation* b) : body (b) { }
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const body;
};

struct Noop : public Operation
{
  virtual Control execute (ExecutorBase& exec) const
  {
    return Control_Continue;
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
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const child;
  size_t const size;
};

struct PrintlnOp : public Operation
{
  PrintlnOp (const semantic::ExpressionValueList& a_evals, Operation* a_args)
    : evals (a_evals)
    , args (a_args)
  { }
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }

  semantic::ExpressionValueList const evals;
  Operation* const args;
};

struct NewOp : public Operation
{
  NewOp (const type::Type* a_type)
    : type (a_type)
  { }
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }

  const type::Type* const type;
};

struct MoveOp : public Operation
{
  MoveOp (Operation* a_arg) : arg (a_arg) { }
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const arg;
};

struct MergeOp : public Operation
{
  MergeOp (Operation* a_arg) : arg (a_arg) { }
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const arg;
};

struct LenOp : public Operation
{
  LenOp (Operation* a_arg) : arg (a_arg) { }
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const arg;
};

Operation* make_append (const type::Slice* slice_type, Operation* args);

struct CopyOp : public Operation
{
  CopyOp (const type::Type* a_type, Operation* a_arg)
    : type (a_type)
    , arg (a_arg) { }
  virtual Control execute (ExecutorBase& exec) const;
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  const type::Type* const type;
  Operation* const arg;
};

}

#endif // RC_SRC_OPERATION_HPP
