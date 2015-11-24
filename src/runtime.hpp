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
    virtual Callable* instantiate (const std::vector<const Type::Type*>& argument_types)
    {
      unimplemented;
    }
  };

  struct Move : public ::Template
  {
    Move (ast::Node* dn);
    virtual typed_value_t instantiate (TypedValueListType& tvlist);
    virtual Callable* instantiate (const std::vector<const Type::Type*>& argument_types)
    {
      unimplemented;
    }
  };

  struct Merge : public ::Template
  {
    Merge (ast::Node* dn);
    virtual typed_value_t instantiate (TypedValueListType& tvlist);
    virtual Callable* instantiate (const std::vector<const Type::Type*>& argument_types)
    {
      unimplemented;
    }
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

  template <typename O, typename T>
  struct Binary : public Operation
  {
    Binary (const Location& loc, const Operation* l, const Operation* r) : location (loc), left (l), right (r) { }
    virtual void execute (executor_base_t& exec) const
    {
      left->execute (exec);
      typename T::ValueType left;
      exec.stack ().pop (left);
      right->execute (exec);
      typename T::ValueType right;
      exec.stack ().pop (right);
      exec.stack ().push (O () (location, left, right));
    }

    Location const location;
    const Operation* const left;
    const Operation* const right;
  };

  template <typename O>
  struct BinaryGenerator
  {
    const Location& location;
    const Operation* left;
    const Operation* right;

    BinaryGenerator (const Location& loc, const Operation* l, const Operation* r) : location (loc), left (l), right (r) { }
    Operation* operation;

    template <typename T>
    void operator() (const T& type)
    {
      operation = new Binary<O, T> (location, left, right);
    }

    void NotArithmetic (const Type::Type& type)
    {
      type_not_reached (type);
    }

    void NotIntegral (const Type::Type& t)
    {
      type_not_reached (t);
    }
  };

  template <template <typename S> class Visitor, typename T>
  Operation* make_binary (const Type::Type* type, const Location& location, const Operation* left, const Operation* right)
  {
    BinaryGenerator<T> g (location, left, right);
    Visitor<BinaryGenerator<T> > visitor (g);
    type->Accept (visitor);
    return g.operation;
  }

  struct ListOperation : public Operation
  {
    virtual void execute (executor_base_t& exec) const;
    typedef std::vector<Operation*> ListType;
    ListType list;
  };

  struct CallableOperation : public Operation
  {
    CallableOperation (Callable* c, Operation* o) : callable (c), arguments (o) { }
    virtual void execute (executor_base_t& exec) const;
    Callable* callable;
    Operation* arguments;
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
}

#endif // rc_runtime_hpp
