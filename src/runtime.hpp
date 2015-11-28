#ifndef rc_runtime_hpp
#define rc_runtime_hpp

#include "types.hpp"
#include "Template.hpp"
#include "Type.hpp"
#include "executor_base.hpp"

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
    Return,
    Continue,
  };

  ControlAction
  evaluate_statement (executor_base_t& exec,
                      const MemoryModel& memoryModel,
                      ast::Node* node);

  // void
  // evaluate_expression (executor_base_t& exec,
  //                      const MemoryModel& memoryModel,
  //                      const ast::Node* node);

  struct New : public ::Template
  {
    New (ast::Node* dn);
    virtual typed_value_t instantiate (TypedValueListType& tvlist);
    virtual Callable* instantiate (const std::vector<const Type::Type*>& argument_types);
  };

  struct Move : public ::Template
  {
    Move (ast::Node* dn);
    virtual typed_value_t instantiate (TypedValueListType& tvlist);
    virtual Callable* instantiate (const std::vector<const Type::Type*>& argument_types);
  };

  struct Merge : public ::Template
  {
    Merge (ast::Node* dn);
    virtual typed_value_t instantiate (TypedValueListType& tvlist);
    virtual Callable* instantiate (const std::vector<const Type::Type*>& argument_types);
  };

  struct Copy : public ::Template
  {
    Copy (ast::Node* dn);
    virtual typed_value_t instantiate (TypedValueListType& tvlist);
    virtual Callable* instantiate (const std::vector<const Type::Type*>& argument_types)
    {
      unimplemented;
    }
  };

  struct Println : public ::Template
  {
    Println (ast::Node* dn);
    virtual typed_value_t instantiate (TypedValueListType& tvlist);
    virtual Callable* instantiate (const std::vector<const Type::Type*>& argument_types);
  };

  // Operations
  struct Operation
  {
    virtual ~Operation() { }
    virtual void execute (executor_base_t& exec) const = 0;
  };

  struct ConvertStringToSliceOfBytes : public Operation
  {
    static ConvertStringToSliceOfBytes instance;
    virtual void execute (executor_base_t& exec) const;
  private:
    ConvertStringToSliceOfBytes () { }
  };

  struct Load : public Operation
  {
    Load (const Operation* c, const Type::Type* t) : child (c), type (t) { }
    virtual void execute (executor_base_t& exec) const;
    const Operation* const child;
    const Type::Type* const type;
  };

  struct IndexArrayReference : public Operation
  {
    IndexArrayReference (const Location& l, const Operation* b, const Operation* i, const Type::Array& t) : location (l), base (b), index (i), type (t) { }
    virtual void execute (executor_base_t& exec) const;
    Location const location;
    const Operation* const base;
    const Operation* const index;
    const Type::Array& type;
  };

  struct IndexArrayValue : public Operation
  {
    IndexArrayValue (const Location& l, const Operation* b, const Operation* i, const Type::Array& t) : location (l), base (b), index (i), type (t) { }
    virtual void execute (executor_base_t& exec) const;
    Location const location;
    const Operation* const base;
    const Operation* const index;
    const Type::Array& type;
  };

  struct IndexSlice : public Operation
  {
    IndexSlice (const Location& l, const Operation* b, const Operation* i, const Type::Slice& t) : location (l), base (b), index (i), type (t) { }
    virtual void execute (executor_base_t& exec) const;
    Location const location;
    const Operation* const base;
    const Operation* const index;
    const Type::Slice& type;
  };

  Operation* MakeConvertToInt (const Operation* c, const Type::Type* type);
  Operation* MakeConvertToUint (const Operation* c, const Type::Type* type);

  template <typename T>
  struct Literal : public Operation
  {
    Literal (T v) : value (v) { }
    virtual void execute (executor_base_t& exec) const
    {
      exec.stack ().push (value);
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
    virtual void execute (executor_base_t& exec) const;
    const Operation* const left;
    const Operation* const right;
  };

  struct LogicAnd : public Operation
  {
    LogicAnd (const Operation* l, const Operation* r) : left (l), right (r) { }
    virtual void execute (executor_base_t& exec) const;
    const Operation* const left;
    const Operation* const right;
  };

  // template <typename O, typename T>
  // struct Binary : public Operation
  // {
  //   Binary (const Location& loc, const Operation* l, const Operation* r) : location (loc), left (l), right (r) { }
  //   virtual void execute (executor_base_t& exec) const
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
    virtual void execute (executor_base_t& exec) const;
    typedef std::vector<Operation*> ListType;
    ListType list;
  };

  struct CallableOperation : public Operation
  {
    CallableOperation (const Callable* c, Operation* o) : callable (c), arguments (o) { }
    virtual void execute (executor_base_t& exec) const;
    const Callable* const callable;
    Operation* const arguments;
  };

  struct Instance : public Operation
  {
    Instance (InstanceSymbol* i) : instance (i) { }
    virtual void execute (executor_base_t& exec) const;
    InstanceSymbol* instance;
  };

  struct SetRestoreCurrentInstance : public Operation
  {
    SetRestoreCurrentInstance (Operation* c, ptrdiff_t o) : child (c), receiver_offset (o) { }
    virtual void execute (executor_base_t& exec) const;
    Operation* const child;
    ptrdiff_t const receiver_offset;
  };

  struct Clear : public Operation
  {
    Clear (ptrdiff_t o, size_t s) : offset (o), size (s) { }
    virtual void execute (executor_base_t& exec) const;
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
    virtual void execute (executor_base_t& exec) const;
    Operation* const left;
    Operation* const right;
    size_t const size;
  };

  struct Reference : public Operation
  {
    Reference (ptrdiff_t o) : offset (o) { }
    virtual void execute (executor_base_t& exec) const;
    ptrdiff_t const offset;
  };

  struct Select : public Operation
  {
    Select (Operation* b, ptrdiff_t o) : base (b), offset (o) { }
    virtual void execute (executor_base_t& exec) const;
    Operation* const base;
    ptrdiff_t const offset;
  };

  struct Index : public Operation
  {
    Index (Operation* b, Operation* i) : base (b), index (i) { }
    virtual void execute (executor_base_t& exec) const;
    Operation* const base;
    Operation* const index;
  };

  struct ReturnJ : public Operation
  {
    ReturnJ (Operation* c) : child (c) { }
    virtual void execute (executor_base_t& exec) const;
    Operation* const child;
  };

  struct If : public Operation
  {
    If (Operation* c, Operation* t, Operation* f) : condition (c), true_branch (t), false_branch (f) { }
    virtual void execute (executor_base_t& exec) const;
    Operation* const condition;
    Operation* const true_branch;
    Operation* const false_branch;
  };

  template <typename T>
  struct Unary : public Operation
  {
    Unary (Operation* c) : child (c) { }
    virtual void execute (executor_base_t& exec) const
    {
      typename T::ValueType x;
      child->execute (exec);
      exec.stack ().pop (x);
      exec.stack ().push (T () (x));
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
    virtual void execute (executor_base_t& exec) const
    {
      V x;
      V y;
      left->execute (exec);
      exec.stack ().pop (x);
      right->execute (exec);
      exec.stack ().pop (y);
      exec.stack ().push (T () (x, y));
    }
    Operation* const left;
    Operation* const right;
  };

  template <typename V, typename T>
  struct Shift : public Operation
  {
    Shift (Operation* l, Operation* r) : left (l), right (r) { }
    virtual void execute (executor_base_t& exec) const
    {
      V x;
      Type::Uint::ValueType y;
      left->execute (exec);
      exec.stack ().pop (x);
      right->execute (exec);
      exec.stack ().pop (y);
      exec.stack ().push (T () (x, y));
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
    virtual void execute (executor_base_t& exec) const;
    Operation* const root;
    ptrdiff_t const root_offset;
    Operation* const body;
  };

  Operation* make_increment (Operation* child, const Type::Type* type);
}

#endif // rc_runtime_hpp
