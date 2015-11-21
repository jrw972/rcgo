#ifndef rc_runtime_hpp
#define rc_runtime_hpp

#include "types.hpp"
#include "Template.hpp"
#include "Type.hpp"

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

  void
  evaluate_expression (executor_base_t& exec,
                       const MemoryModel& memoryModel,
                       const ast::Node* node);

  struct New : public ::Template
  {
    New (ast::Node* dn);
    virtual typed_value_t instantiate (TypedValueListType& tvlist);
  };

  struct Move : public ::Template
  {
    Move (ast::Node* dn);
    virtual typed_value_t instantiate (TypedValueListType& tvlist);
  };

  struct Merge : public ::Template
  {
    Merge (ast::Node* dn);
    virtual typed_value_t instantiate (TypedValueListType& tvlist);
  };

  struct Copy : public ::Template
  {
    Copy (ast::Node* dn);
    virtual typed_value_t instantiate (TypedValueListType& tvlist);
  };

  struct Println : public ::Template
  {
    Println (ast::Node* dn);
    virtual typed_value_t instantiate (TypedValueListType& tvlist);
  };

  // Operations
  struct Operation
  {
    virtual ~Operation() { }
    virtual void execute (executor_base_t& exec, const MemoryModel& memoryModel) const = 0;
  };

  struct ConvertStringToSliceOfBytes : public Operation
  {
    static ConvertStringToSliceOfBytes instance;
    virtual void execute (executor_base_t& exec, const MemoryModel& memoryModel) const;
  private:
    ConvertStringToSliceOfBytes () { }
  };

  // This is a transitional type.
  // It should be removed in the future.
  struct EvaluateNode : public Operation
  {
    EvaluateNode (const ast::Node* n) : node (n) { }
    virtual void execute (executor_base_t& exec, const MemoryModel& memoryModel) const;
    const ast::Node* const node;
  };

  struct LoadSlice : public Operation
  {
    LoadSlice (const Operation* c) : child (c) { }
    virtual void execute (executor_base_t& exec, const MemoryModel& memoryModel) const;
    const Operation* const child;
  };

  struct IndexArrayReference : public Operation
  {
    IndexArrayReference (const Location& l, const Operation* b, const Operation* i, const Type::Array& t) : location (l), base (b), index (i), type (t) { }
    virtual void execute (executor_base_t& exec, const MemoryModel& memoryModel) const;
    Location const location;
    const Operation* const base;
    const Operation* const index;
    const Type::Array& type;
  };

  struct IndexArrayValue : public Operation
  {
    IndexArrayValue (const Location& l, const Operation* b, const Operation* i, const Type::Array& t) : location (l), base (b), index (i), type (t) { }
    virtual void execute (executor_base_t& exec, const MemoryModel& memoryModel) const;
    Location const location;
    const Operation* const base;
    const Operation* const index;
    const Type::Array& type;
  };

  struct IndexSlice : public Operation
  {
    IndexSlice (const Location& l, const Operation* b, const Operation* i, const Type::Slice& t) : location (l), base (b), index (i), type (t) { }
    virtual void execute (executor_base_t& exec, const MemoryModel& memoryModel) const;
    Location const location;
    const Operation* const base;
    const Operation* const index;
    const Type::Slice& type;
  };

  Operation* MakeConvertToInt (const Operation* c, const Type::Type* type);

  struct Int : public Operation
  {
    Int (Type::Int::ValueType v) : value (v) { }
    virtual void execute (executor_base_t& exec, const MemoryModel& memoryModel) const;
    Type::Int::ValueType const value;
  };
}

#endif // rc_runtime_hpp
