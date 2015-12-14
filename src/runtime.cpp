#include "runtime.hpp"
#include "Callable.hpp"
#include "SymbolVisitor.hpp"
#include "types.hpp"
#include "AstVisitor.hpp"
#include "reaction.hpp"
#include <error.h>
#include "field.hpp"
#include "Composition.hpp"
#include "heap.hpp"
#include "executor_base.hpp"
#include "semantic.hpp"
#include "check_types.hpp"

namespace runtime
{
using namespace Type;
using namespace ast;
using namespace decl;

struct port_t
{
  component_t* instance;
  const reaction_t* reaction;
  Type::Int::ValueType parameter;
  port_t* next;

  port_t (component_t* i, const reaction_t* r, Type::Int::ValueType p) : instance (i), reaction (r), parameter (p), next (NULL) { }
};

struct heap_link_t
{
  heap_t* heap;
  pthread_mutex_t mutex;
  size_t change_count;
};

void
allocate_instances (Composition::Composer& instance_table)
{
  for (Composition::Composer::InstancesType::const_iterator pos = instance_table.InstancesBegin (),
       limit = instance_table.InstancesEnd ();
       pos != limit;
       ++pos)
    {
      Composition::Instance* instance = pos->second;
      component_t* ptr;
      if (instance->IsTopLevel ())
        {
          const Type::Type* type = instance->type;
          size_t size = type->Size ();
          ptr = static_cast<component_t*> (malloc (size));
          memset (ptr, 0, size);
        }
      else
        {
          ptr = reinterpret_cast<component_t*> (reinterpret_cast<char*> (instance->parent->component) + instance->Offset ());
        }
      instance->component = ptr;
    }
}

static void
bind (port_t** output_port, component_t* input_instance, const reaction_t* reaction, Type::Int::ValueType parameter)
{
  port_t* port = new port_t (input_instance, reaction, parameter);
  port->next = *output_port;
  *output_port = port;
}

static heap_link_t*
make_heap_link (heap_t* heap,
                heap_t* allocator)
{
  // Allocate a new heap link in the parent.
  heap_link_t* hl = (heap_link_t*)heap_allocate (allocator, sizeof (heap_link_t));
  // Set up the link.
  hl->heap = heap;
  pthread_mutex_init (&hl->mutex, NULL);
  return hl;
}

void
create_bindings (Composition::Composer& instance_table)
{
  for (Composition::Composer::PushPortsType::const_iterator pp_pos = instance_table.PushPortsBegin (),
       pp_limit = instance_table.PushPortsEnd ();
       pp_pos != pp_limit;
       ++pp_pos)
    {
      Composition::PushPort* pp = pp_pos->second;
      Composition::Instance* output_instance = pp->instance;
      size_t output_port = pp->address - output_instance->address;

      for (Composition::ReactionsType::const_iterator reaction_pos = pp->reactions.begin (),
           reaction_limit = pp->reactions.end ();
           reaction_pos != reaction_limit;
           ++reaction_pos)
        {
          Composition::Reaction* r = *reaction_pos;
          bind (reinterpret_cast<port_t**> (reinterpret_cast<char*> (output_instance->component) + output_port),
                r->instance->component,
                r->reaction,
                r->iota);
        }
    }

  for (Composition::Composer::PullPortsType::const_iterator pp_pos = instance_table.PullPortsBegin (),
       pp_limit = instance_table.PullPortsEnd ();
       pp_pos != pp_limit;
       ++pp_pos)
    {
      Composition::Instance* pull_port_instance = pp_pos->second->instance;
      size_t pull_port_address = pp_pos->first - pull_port_instance->address;
      Composition::Getter* getter = *pp_pos->second->getters.begin ();
      pull_port_t* pull_port = reinterpret_cast<pull_port_t*> (reinterpret_cast<char*> (pull_port_instance->component) + pull_port_address);
      assert (getter->instance != NULL);
      pull_port->instance = getter->instance->component;
      pull_port->getter = getter->getter;
    }
}

void
initialize (executor_base_t& exec, Composition::Instance* instance)
{
  if (instance->IsTopLevel ())
    {
      // Set up the heap.
      exec.current_instance (instance->component);
      // Call the initializer.
      instance->node->operation->execute (exec);
      // Clean up any return value.
      exec.stack ().popn (instance->initializer->return_size ());
    }
}

template <typename T>
static void
evaluate (executor_base_t& exec, const MemoryModel& memoryModel, const ast_binary_expr_t& node, const T& op)
{
  struct visitor : public Type::DefaultVisitor
  {
    executor_base_t& exec;
    const MemoryModel& memoryModel;
    const ast_binary_expr_t& node;
    const T& op;

    visitor (executor_base_t& e,
             const MemoryModel& mm,
             const ast_binary_expr_t& n,
             const T& o) : exec (e), memoryModel (mm), node (n), op (o) { }

    void default_action (const Type::Type& t)
    {
      type_not_reached (t);
    }

    void visit (const NamedType& t)
    {
      t.UnderlyingType ()->Accept (*this);
    }

    void visit (const Bool& t)
    {
      op (exec, memoryModel, node, t);
    }

    void visit (const Boolean& t)
    {
      op (exec, memoryModel, node, t);
    }

    void visit (const Type::Int& t)
    {
      op (exec, memoryModel, node, t);
    }

    void visit (const Int8& t)
    {
      op (exec, memoryModel, node, t);
    }

    void visit (const Uint& t)
    {
      op (exec, memoryModel, node, t);
    }

    void visit (const Uint8& t)
    {
      op (exec, memoryModel, node, t);
    }

    void visit (const Uint64& t)
    {
      op (exec, memoryModel, node, t);
    }

    void visit (const Float64& t)
    {
      op (exec, memoryModel, node, t);
    }

    void visit (const Pointer& t)
    {
      op (exec, memoryModel, node, t);
    }
  };

  visitor v (exec, memoryModel, node, op);
  op.dispatch_type (node)->Accept (v);
}

struct RetvalDispatch
{
  const Type::Type*
  dispatch_type (const ast_binary_expr_t& node) const
  {
    unimplemented;
    //return node.typed_value.type;
  }
};

struct LeftDispatch
{
  const Type::Type*
  dispatch_type (const ast_binary_expr_t& node) const
  {
    unimplemented;
    //return node.left ()->typed_value.type;
  }
};

struct Divide : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Int&) const
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // Type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // Type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left / right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Float64&) const
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // Float64::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // Float64::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left / right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Type& t) const
  {
    type_not_reached (t);
  }
};

struct Modulus : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Int&) const
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // Type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // Type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left % right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Type& t) const
  {
    type_not_reached (t);
  }
};

template <typename T>
struct LeftShiftVisitor : public Type::DefaultVisitor
{
  executor_base_t& exec;
  const MemoryModel& memoryModel;
  const ast_binary_expr_t& node;

  LeftShiftVisitor (executor_base_t& e,
                    const MemoryModel& mm,
                    const ast_binary_expr_t& n)
    : exec (e)
    , memoryModel (mm)
    , node (n)
  { }

  void default_action (const Type::Type& t)
  {
    type_not_reached (t);
  }

  void visit (const NamedType& t)
  {
    t.UnderlyingType ()->Accept (*this);
  }

  void visit (const Type::Int& t)
  {
    doit (t);
  }

  void visit (const Uint& t)
  {
    doit (t);
  }

  void visit (const Uint8& t)
  {
    doit (t);
  }

  template <typename U>
  void doit (const U&)
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // typename T::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // typename U::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left << right);
  }
};

struct LeftShift : public RetvalDispatch
{
  template <typename T>
  void
  doit (executor_base_t& exec,
        const MemoryModel& memoryModel,
        const ast_binary_expr_t& node,
        const T&) const
  {
    LeftShiftVisitor<T> v (exec, memoryModel, node);
    unimplemented;
    //node.right ()->typed_value.type->Accept (v);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Int& t) const
  {
    doit (exec, memoryModel, node, t);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Uint64& t) const
  {
    doit (exec, memoryModel, node, t);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Type& t) const
  {
    type_not_reached (t);
  }
};

struct RightShift : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Int&) const
  {
    struct visitor : public Type::DefaultVisitor
    {
      executor_base_t& exec;
      const MemoryModel& memoryModel;
      const ast_binary_expr_t& node;

      visitor (executor_base_t& e,
               const MemoryModel& mm,
               const ast_binary_expr_t& n)
        : exec (e)
        , memoryModel (mm)
        , node (n)
      { }

      void default_action (const Type::Type& t)
      {
        type_not_reached (t);
      }

      void visit (const NamedType& t)
      {
        t.UnderlyingType ()->Accept (*this);
      }

      void visit (const Type::Int&)
      {
        unimplemented;
        // evaluate_expression (exec, memoryModel, node.left ());
        // Type::Int::ValueType left;
        // exec.stack ().pop (left);
        // evaluate_expression (exec, memoryModel, node.right ());
        // Uint::ValueType right;
        // exec.stack ().pop (right);
        // exec.stack ().push (left >> right);
      }

      void visit (const Uint&)
      {
        unimplemented;
        // evaluate_expression (exec, memoryModel, node.left ());
        // Uint::ValueType left;
        // exec.stack ().pop (left);
        // evaluate_expression (exec, memoryModel, node.right ());
        // Uint::ValueType right;
        // exec.stack ().pop (right);
        // exec.stack ().push (left >> right);
      }
    };

    visitor v (exec, memoryModel, node);
    unimplemented;
    //node.right ()->typed_value.type->Accept (v);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Type& t) const
  {
    type_not_reached (t);
  }
};

struct BitAnd : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Int&) const
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // Type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // Type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left & right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Type& t) const
  {
    type_not_reached (t);
  }
};

struct BitAndNot : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Int&) const
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // Type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // Type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left & (~right));
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Type& t) const
  {
    type_not_reached (t);
  }
};

struct Add : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Int& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Uint& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  template <typename T>
  void
  doit (executor_base_t& exec,
        const MemoryModel& memoryModel,
        const ast_binary_expr_t& node,
        const T&) const
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // typename T::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // typename T::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left + right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Type& t) const
  {
    type_not_reached (t);
  }
};

struct Subtract : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Int&) const
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // Type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // Type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left - right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Type& t) const
  {
    type_not_reached (t);
  }
};

struct BitOr : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Int&) const
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // Type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // Type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left | right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Type& t) const
  {
    type_not_reached (t);
  }
};

struct BitXor : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Int&) const
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // Type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // Type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left ^ right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Type& t) const
  {
    type_not_reached (t);
  }
};

struct Equal : public LeftDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Bool& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Boolean& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Int& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Uint& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Uint8& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Pointer& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  template <typename T>
  void doit (executor_base_t& exec,
             const MemoryModel& memoryModel,
             const ast_binary_expr_t& node,
             const T&) const
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // typename T::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // typename T::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left == right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Type& t) const
  {
    type_not_reached (t);
  }
};

struct NotEqual : public LeftDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Bool&) const
  {
    doit<Bool> (exec, memoryModel, node);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Int&) const
  {
    doit<Type::Int> (exec, memoryModel, node);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Uint&) const
  {
    doit<Uint> (exec, memoryModel, node);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Uint64&) const
  {
    doit<Uint64> (exec, memoryModel, node);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Pointer&) const
  {
    doit<Pointer> (exec, memoryModel, node);
  }

  template <typename T>
  void
  doit (executor_base_t& exec, const MemoryModel& memoryModel, const ast_binary_expr_t& node) const
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // typename T::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // typename T::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left != right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Type& t) const
  {
    type_not_reached (t);
  }
};

struct LessThan : public LeftDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Int&) const
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // Type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // Type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left < right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Int8&) const
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // Int8::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // Int8::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left < right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Type& t) const
  {
    type_not_reached (t);
  }
};

struct LessEqual : public LeftDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Int&) const
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // Type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // Type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left <= right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Type& t) const
  {
    type_not_reached (t);
  }
};

struct MoreThan : public LeftDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Int&) const
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // Type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // Type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left > right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Type& t) const
  {
    type_not_reached (t);
  }
};

struct MoreEqual : public LeftDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Int&) const
  {
    unimplemented;
    // evaluate_expression (exec, memoryModel, node.left ());
    // Type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // Type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left >= right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast_binary_expr_t& node,
              const Type::Type& t) const
  {
    type_not_reached (t);
  }
};

// void
// evaluate_expression (executor_base_t& exec,
//                      const MemoryModel& memoryModel,
//                      const ast::Node* node)
// {
//   unimplemented;
// typed_value_t tv = node->typed_value;
// if (tv.value.present)
//   {
//     switch (tv.kind)
//       {
//       case typed_value_t::VALUE:
//       {
//         exec.stack ().push_tv (tv);
//         return;
//       }
//       break;
//       case typed_value_t::REFERENCE:
//         std::cout << *node;
//         unimplemented;
//       case typed_value_t::TYPE:
//         std::cout << *node;
//         unimplemented;
//       }
//   }

// struct visitor : public ast::DefaultConstVisitor
// {
//   executor_base_t& exec;
//   const MemoryModel& memoryModel;

//   visitor (executor_base_t& e,
//            const MemoryModel& mm)
//     : exec (e)
//     , memoryModel (mm)
//   { }

//   void default_action (const Node& node)
//   {
//     ast_not_reached (node);
//   }

//   void visit (const ast_indexed_port_call_expr_t& node)
//   {
//     // Determine the push port index.
//     node.index_op->execute (exec, memoryModel);
//     Type::Int::ValueType idx;
//     exec.stack ().pop (idx);
//     if (idx < 0 || idx >= node.array_type->dimension)
//       {
//         error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
//                        "array index is out of bounds (E1)");
//       }

//     push_port_call (node, node.args (), memoryModel, node.field, idx * node.array_type->UnitSize ());
//   }

//   void visit (const ast_index_expr_t& node)
//   {
//     node.operation->execute (exec, memoryModel);
//   }

//   void visit (const ast_slice_expr_t& node)
//   {
//     evaluate_expression (exec, memoryModel, node.base ());
//     typed_value_t base_tv = node.base ()->typed_value;
//     exec.stack ().pop_tv (base_tv);

//     typed_value_t low_tv = node.low ()->typed_value;
//     if (!low_tv.value.present)
//       {
//         evaluate_expression (exec, memoryModel, node.low ());
//         exec.stack ().pop_tv (low_tv);
//       }
//     Type::Int::ValueType low = low_tv.integral_value ();

//     typed_value_t high_tv = node.high ()->typed_value;
//     if (!high_tv.value.present)
//       {
//         evaluate_expression (exec, memoryModel, node.high ());
//         exec.stack ().pop_tv (high_tv);
//       }
//     Type::Int::ValueType high = high_tv.integral_value ();

//     const Array* array_type = type_cast<Array> (base_tv.type);
//     if (array_type)
//       {
//         if (low < 0 || low >= array_type->dimension)
//           {
//             error_at_line (EXIT_FAILURE, 0, node.location.File.c_str (), node.location.Line, "lower limit of slice is out of bounds (E3)");
//           }
//         if (high < low || high > array_type->dimension)
//           {
//             error_at_line (EXIT_FAILURE, 0, node.location.File.c_str (), node.location.Line, "upper limit of slice is out of bounds (E4)");
//           }

//         Slice::ValueType slice;
//         slice.ptr = NULL;
//         slice.length = high - low;
//         slice.capacity = high - low;
//         if (slice.length != 0)
//           {
//             slice.ptr = static_cast<char*> (base_tv.value.reference_value ()) + low * array_type->UnitSize ();
//           }

//         exec.stack ().push (slice);
//       }
//     else
//       {
//         unimplemented;
//       }
//   }

//   void visit (const ast_address_of_expr_t& node)
//   {
//     if (!node.address_of_dereference)
//       {
//         evaluate_expression (exec, memoryModel, node.child ());
//       }
//     else
//       {
//         evaluate_expression (exec, memoryModel, node.child ()->At (0));
//       }
//   }

//   void visit (const ast_call_expr_t& node)
//   {
//     if (node.IsCall)
//       {
//         const Type::Function* f = type_cast<Type::Function> (node.expr ()->typed_value.type);
//         assert (f != NULL);
//         if (f->kind != Type::Function::PULL_PORT)
//           {
//             node.expr ()->typed_value.value.callable_value ()->call (exec, memoryModel, node);
//           }
//         else
//           {
//             // Evaluate the pull port.
//             pull_port_t pull_port;
//             evaluate_expression (exec, memoryModel, node.expr ());
//             exec.stack ().store (&pull_port, sizeof (pull_port_t));
//             // Execute the call.
//             pull_port.getter->call (exec, node, pull_port.instance);
//           }
//       }
//     else
//       {
//         evaluate_expression (exec, memoryModel, node.args ());
//         node.operation->execute (exec, memoryModel);
//       }
//   }

//   void push_port_call (const Node& node,
//                        ast::Node* args,
//                        const MemoryModel& memoryModel,
//                        const field_t* field,
//                        size_t offset = 0)
//   {
//     // Push all of the arguments first and measure their size.
//     char* top_before = exec.stack ().top ();
//     evaluate_expression (exec, memoryModel, args);
//     char* top_after = exec.stack ().top ();
//     ptrdiff_t arguments_size = top_after - top_before; // Assumes stack grows up.

//     // Find the port to activate.
//     void* receiverPtr = exec.stack ().read_pointer (memoryModel.ReceiverOffset ());
//     port_t* port = *((port_t**)((char*)receiverPtr + field->offset + offset));

//     char* base_pointer = exec.stack ().base_pointer ();
//     component_t* instance = exec.current_instance ();

//     // Activate all the reactions bound to the port.
//     while (port != NULL)
//       {
//         // Set up a frame.
//         // Push the instance.
//         exec.stack ().push_pointer (port->instance);
//         // Push the parameter.
//         if (port->reaction->has_dimension ())
//           {
//             exec.stack ().push<Type::Int::ValueType> (port->parameter);
//           }
//         // Push the arguments.
//         exec.stack ().load (top_before, arguments_size);
//         // Push an instruction pointer.
//         exec.stack ().push_pointer (NULL);

//         // Jump to the last frame.
//         exec.stack ().base_pointer (exec.mutable_phase_base_pointer ());

//         execute (exec, port->reaction, port->instance);

//         // Move to our frame and instance.
//         exec.stack ().base_pointer (base_pointer);
//         exec.current_instance (instance);

//         port = port->next;
//       }
//   }

//   void visit (const ast_push_port_call_expr_t& node)
//   {
//     push_port_call (node, node.args (), memoryModel, node.field);
//   }

//   void visit (const ast_list_expr_t& node)
//   {
//     for (Node::ConstIterator pos = node.Begin (), limit = node.End ();
//          pos != limit;
//          ++pos)
//       {
//         evaluate_expression (exec, memoryModel, *pos);
//       }
//   }

//   void visit (const ast_select_expr_t& node)
//   {
//     evaluate_expression (exec, memoryModel, node.base ());
//     char* ptr = static_cast<char*> (exec.stack ().pop_pointer ());
//     typed_value_t tv = node.typed_value;
//     assert (tv.has_offset);
//     exec.stack ().push_pointer (ptr + tv.offset);
//   }

//   void visit (const ast_identifier_expr_t& node)
//   {
//     // Get the address of the identifier.
//     Symbol* symbol = node.symbol;
//     ptrdiff_t offset = symbol->offset ();
//     exec.stack ().push_address (offset);
//   }

//   void visit (const ast_unary_arithmetic_expr_t& node)
//   {
//     evaluate_expression (exec, memoryModel, node.child ());
//     switch (node.arithmetic)
//       {
//       case LogicNot:
//       {
//         Bool::ValueType b;
//         exec.stack ().pop (b);
//         exec.stack ().push<Bool::ValueType> (!b);
//       }
//       return;
//       case Negate:
//         unimplemented;
//       }
//     not_reached;
//   }

//   void visit (const ast_implicit_dereference_expr_t& node)
//   {
//     evaluate_expression (exec, memoryModel, node.child ());
//     void* ptr = exec.stack ().pop_pointer ();
//     if (ptr == NULL)
//       {
//         std::cout << node;
//         unimplemented;
//       }
//     typed_value_t tv = node.typed_value;
//     exec.stack ().load (ptr, tv.type->Size ());
//   }

//   void visit (const ast_implicit_conversion_expr_t& node)
//   {
//     evaluate_expression (exec, memoryModel, node.child ());
//   }

//   void visit (const ast_binary_arithmetic_expr_t& node)
//   {
//     switch (node.arithmetic)
//       {
//       case ::Multiply:
//       case ::Divide:
//       case ::Modulus:
//         node.operation->execute (exec, memoryModel);
//         break;
//       case ::LeftShift:
//         evaluate (exec, memoryModel, node, LeftShift ());
//         break;
//       case ::RightShift:
//         evaluate (exec, memoryModel, node, RightShift ());
//         break;
//       case ::BitAnd:
//         evaluate (exec, memoryModel, node, BitAnd ());
//         break;
//       case ::BitAndNot:
//         evaluate (exec, memoryModel, node, BitAndNot ());
//         break;
//       case ::Add:
//         evaluate (exec, memoryModel, node, Add ());
//         break;
//       case ::Subtract:
//         evaluate (exec, memoryModel, node, Subtract ());
//         break;
//       case ::BitOr:
//         evaluate (exec, memoryModel, node, BitOr ());
//         break;
//       case ::BitXor:
//         evaluate (exec, memoryModel, node, BitXor ());
//         break;
//       case ::Equal:
//         evaluate (exec, memoryModel, node, Equal ());
//         break;
//       case ::NotEqual:
//         evaluate (exec, memoryModel, node, NotEqual ());
//         break;
//       case ::LessThan:
//         evaluate (exec, memoryModel, node, LessThan ());
//         break;
//       case ::LessEqual:
//         evaluate (exec, memoryModel, node, LessEqual ());
//         break;
//       case ::MoreThan:
//         evaluate (exec, memoryModel, node, MoreThan ());
//         break;
//       case ::MoreEqual:
//         evaluate (exec, memoryModel, node, MoreEqual ());
//         break;
//       case ::LogicAnd:
//       case ::LogicOr:
//         node.operation->execute (exec, memoryModel);
//       break;
//       }
//   }
// };
// visitor v (exec, memoryModel);
// node->Accept (v);
// }

// ControlAction
// evaluate_statement (executor_base_t& exec,
//                     const MemoryModel& memoryModel,
//                     ast::Node* node)
// {
//   struct visitor : public DefaultConstVisitor
//   {
//     ControlAction retval;
//     executor_base_t& exec;
//     const MemoryModel& memoryModel;

//     visitor (executor_base_t& e, const MemoryModel& mm) : retval (Continue), exec (e), memoryModel (mm) { }

//     void default_action (const Node& node)
//     {
//       ast_not_reached (node);
//     }

//     void visit (const ast_const_t& node)
//     { }

//     void visit (const ast_empty_statement_t& node)
//     { }

//     void visit (const ast_assign_statement_t& node)
//     {
//       unimplemented;
//       // ast::Node* left = node.left ();
//       // ast::Node* right = node.right ();
//       // // Determine the size of the value being assigned.
//       // size_t size = right->typed_value.type->Size ();
//       // // Evaluate the address.
//       // evaluate_expression (exec, memoryModel, left);
//       // void* ptr = exec.stack ().pop_pointer ();
//       // // Evaluate the value.
//       // evaluate_expression (exec, memoryModel, right);
//       // // Store.
//       // exec.stack ().store (ptr, size);
//     }

//     void visit (const ast_change_statement_t& node)
//     {
//       unimplemented;
//       // ast::Node* expr = node.expr ();
//       // ast::Node* body = node.body ();
//       // // Evaluate the pointer to the heap link.
//       // evaluate_expression (exec, memoryModel, expr);
//       // heap_link_t* hl = (heap_link_t*)exec.stack ().pop_pointer ();
//       // if (hl == NULL)
//       //   {
//       //     // Heap link is null.
//       //     unimplemented;
//       //   }
//       // pthread_mutex_lock (&hl->mutex);
//       // ++hl->change_count;
//       // pthread_mutex_unlock (&hl->mutex);

//       // // Save the old heap.
//       // heap_t* old_heap = exec.heap ();
//       // // Set the the new heap.
//       // exec.heap (hl->heap);

//       // {
//       //   // Evaluate the address of the heap root.
//       //   Symbol* symbol = node.root_symbol;
//       //   assert (symbol != NULL);
//       //   ptrdiff_t offset = symbol->offset ();
//       //   exec.stack ().push_address (offset);
//       // }

//       // char** root_value = (char**)exec.stack ().pop_pointer ();
//       // // Push a pointer to the root object.
//       // *root_value = static_cast<char*> (heap_instance (hl->heap));

//       // evaluate_statement (exec, memoryModel, body);

//       // // Restore the old heap.
//       // exec.heap (old_heap);

//       // pthread_mutex_lock (&hl->mutex);
//       // --hl->change_count;
//       // pthread_mutex_unlock (&hl->mutex);
//     }

//     void visit (const ast_expression_statement_t& node)
//     {
//       unimplemented;
//       // ast::Node* child = node.child ();
//       // // Determine the size of the value being generated.
//       // size_t size = child->typed_value.type->Size ();
//       // // Evaluate.
//       // evaluate_expression (exec, memoryModel, child);
//       // // Remove value.
//       // exec.stack ().popn (size);
//     }

//     void visit (const ast_if_statement_t& node)
//     {
//       unimplemented;
//       // evaluate_expression (exec, memoryModel, node.condition ());
//       // Bool::ValueType c;
//       // exec.stack ().pop (c);
//       // if (c)
//       //   {
//       //     if (evaluate_statement (exec, memoryModel, node.true_branch ()) == Return)
//       //       {
//       //         retval = Return;
//       //         return;
//       //       }
//       //   }
//       // else
//       //   {
//       //     if (evaluate_statement (exec, memoryModel, node.false_branch ()) == Return)
//       //       {
//       //         retval = Return;
//       //         return;
//       //       }
//       //   }
//     }

//     void visit (const ast_while_statement_t& node)
//     {
//       unimplemented;
//       // for (;;)
//       //   {
//       //     evaluate_expression (exec, memoryModel, node.condition ());
//       //     Bool::ValueType c;
//       //     exec.stack ().pop (c);
//       //     if (c)
//       //       {
//       //         if (evaluate_statement (exec, memoryModel, node.body ()) == Return)
//       //           {
//       //             retval = Return;
//       //             return;
//       //           }
//       //       }
//       //     else
//       //       {
//       //         return;
//       //       }
//       //   }
//     }

//     void visit (const ast_for_iota_statement_t& node)
//     {
//       for (Type::Int::ValueType idx = 0, limit = node.limit.integral_value ();
//            idx != limit;
//            ++idx)
//         {
//           exec.stack ().write (node.symbol->offset (), &idx, sizeof (idx));
//           if (evaluate_statement (exec, memoryModel, node.body ()) == Return)
//             {
//               retval = Return;
//               return;
//             }
//         }
//     }

//     void visit (const ast_add_assign_statement_t& node)
//     {
//       unimplemented;
//       // // Determine the size of the value being assigned.
//       // const Type::Type* type = node.right ()->typed_value.type;
//       // // Evaluate the address.
//       // evaluate_expression (exec, memoryModel, node.left ());
//       // void* ptr = exec.stack ().pop_pointer ();
//       // // Evaluate the value.
//       // evaluate_expression (exec, memoryModel, node.right ());

//       // struct visitor : public Type::DefaultVisitor
//       // {
//       //   executor_base_t& exec;
//       //   void* ptr;
//       //   visitor (executor_base_t& e, void* p) : exec (e), ptr (p) { }

//       //   void visit (const NamedType& type)
//       //   {
//       //     type.UnderlyingType ()->Accept (*this);
//       //   }

//       //   void visit (const Type::Int& type)
//       //   {
//       //     Type::Int::ValueType x;
//       //     exec.stack ().pop (x);
//       //     *((Type::Int::ValueType*)ptr) += x;
//       //   }

//       //   void visit (const Uint& type)
//       //   {
//       //     Uint::ValueType x;
//       //     exec.stack ().pop (x);
//       //     *((Uint::ValueType*)ptr) += x;
//       //   }

//       //   void default_action (const Type::Type& type)
//       //   {
//       //     unimplemented;
//       //   }
//       // };
//       // visitor v (exec, ptr);
//       // type->Accept (v);
//     }

//     void visit (const ast_subtract_assign_statement_t& node)
//     {
//       unimplemented;
//       // // Determine the size of the value being assigned.
//       // const Type::Type* type = node.right ()->typed_value.type;
//       // // Evaluate the address.
//       // evaluate_expression (exec, memoryModel, node.left ());
//       // void* ptr = exec.stack ().pop_pointer ();
//       // // Evaluate the value.
//       // evaluate_expression (exec, memoryModel, node.right ());

//       // struct visitor : public Type::DefaultVisitor
//       // {
//       //   executor_base_t& exec;
//       //   void* ptr;
//       //   visitor (executor_base_t& e, void* p) : exec (e), ptr (p) { }

//       //   void visit (const NamedType& type)
//       //   {
//       //     type.UnderlyingType ()->Accept (*this);
//       //   }

//       //   void visit (const Uint& type)
//       //   {
//       //     Uint::ValueType x;
//       //     exec.stack ().pop (x);
//       //     *((Uint::ValueType*)ptr) -= x;
//       //   }

//       //   void default_action (const Type::Type& type)
//       //   {
//       //     unimplemented;
//       //   }
//       // };
//       // visitor v (exec, ptr);
//       // type->Accept (v);
//     }

//     void visit (const ast_list_statement_t& node)
//     {
//       for (Node::ConstIterator pos = node.Begin (), limit = node.End ();
//            pos != limit;
//            ++pos)
//         {
//           if (evaluate_statement (exec, memoryModel, *pos) == Return)
//             {
//               retval = Return;
//               return;
//             }
//         }
//     }

//     void visit (const ast_return_statement_t& node)
//     {
//       unimplemented;
//       // // Evaluate the expression.
//       // evaluate_expression (exec, memoryModel, node.child ());
//       // // Store in the return parameter.
//       // unimplemented;
//       // // exec.stack ().move (node.return_symbol->offset (), SymbolCast<ParameterSymbol> (node.return_symbol)->value.type->Size ());
//       // // retval = Return;
//       // // return;
//     }

//     void visit (const ast_increment_statement_t& node)
//     {
//       unimplemented;
//       // evaluate_expression (exec, memoryModel, node.child ());
//       // void* ptr = exec.stack ().pop_pointer ();

//       // struct visitor : public Type::DefaultVisitor
//       // {
//       //   void* ptr;

//       //   visitor (void* p) : ptr (p) { }

//       //   void default_action (const Type::Type& type)
//       //   {
//       //     not_reached;
//       //   }

//       //   void visit (const NamedType& type)
//       //   {
//       //     type.UnderlyingType ()->Accept (*this);
//       //   }

//       //   void visit (const Type::Int& type)
//       //   {
//       //     ++(*static_cast<Type::Int::ValueType*> (ptr));
//       //   }

//       //   void visit (const Uint& type)
//       //   {
//       //     ++(*static_cast<Uint::ValueType*> (ptr));
//       //   }
//       // };
//       // visitor v (ptr);
//       // node.child ()->typed_value.type->Accept (v);
//     }

//     void visit (const ast_activate_statement_t& node)
//     {
//       unimplemented;
//       // // Need to keep track of the largest base pointer so we can process the mutable section.
//       // char* base_pointer = exec.stack ().base_pointer ();
//       // if (base_pointer > exec.mutable_phase_base_pointer ())
//       //   {
//       //     exec.mutable_phase_base_pointer (base_pointer);
//       //   }

//       // // The caller pushed an instruction pointer which is just
//       // // before the base pointer.  Overwrite it with the body.
//       // const ast::Node* p = &node;
//       // memcpy (exec.stack ().pointer_to_instruction_pointer (), &p, sizeof (void*));
//       // // Execute the expression list.
//       // evaluate_expression (exec, memoryModel, node.expr_list ());

//       // // Stop execution.
//       // retval = Return;
//       // return;
//     }

//     void visit (const ast_var_statement_t& node)
//     {
//       ast::Node* expression_list = node.expression_list ();

//       if (expression_list->Size () == 0)
//         {
//           // Zero out the variable.
//           for (size_t idx = 0, limit = node.symbols.size (); idx != limit; ++idx)
//             {
//               unimplemented;
//               // Symbol* symbol = node.symbols[idx];
//               //exec.stack ().clear (symbol->offset (), SymbolCast<VariableSymbol> (symbol)->value.type->Size ());
//             }
//         }
//       else
//         {
//           // Initialize the variables.
//           for (size_t idx = 0, limit = node.symbols.size (); idx != limit; ++idx)
//             {
//               unimplemented;
//               // // Evaluate the address.
//               // Symbol* symbol = node.symbols[idx];
//               // ptrdiff_t offset = symbol->offset ();
//               // exec.stack ().push_address (offset);
//               // void* ptr = exec.stack ().pop_pointer ();
//               // ast::Node* initializer = expression_list->At (idx);
//               // size_t size = initializer->typed_value.type->Size ();
//               // // Evaluate the value.
//               // evaluate_expression (exec, memoryModel, initializer);
//               // // Store.
//               // exec.stack ().store (ptr, size);
//             }
//         }
//     }
//   };
//   visitor v (exec, memoryModel);
//   node->Accept (v);

//   return v.retval;
// }

bool
enabled (executor_base_t& exec,
         component_t* instance,
         const Action* action,
         Type::Int::ValueType iota)
{
  assert (exec.stack ().empty ());

  // Push receiver.
  exec.stack ().push_pointer (instance);
  // Push iota.
  if (action->has_dimension ())
    {
      exec.stack ().push<Uint::ValueType> (iota);
    }
  // Push an instruction pointer.
  exec.stack ().push_pointer (NULL);
  exec.stack ().setup (0 /* No locals. */);
  action->precondition->operation->execute (exec);
  Bool::ValueType retval;
  exec.stack ().pop (retval);
  exec.stack ().teardown ();
  // Pop the instruction pointer.
  exec.stack ().pop_pointer ();
  // Pop iota.
  if (action->has_dimension ())
    {
      Uint::ValueType x;
      exec.stack ().pop (x);
    }
  // Pop this.
  exec.stack ().pop_pointer ();
  assert (exec.stack ().empty ());
  return retval;
}

static void
execute (executor_base_t& exec,
         const Action* action,
         component_t* instance)
{
  // Reset the mutable phase base pointer.
  exec.mutable_phase_base_pointer (0);

  exec.stack ().setup (action->memory_model.LocalsSize ());

  action->body->operation->execute (exec);

  if (exec.mutable_phase_base_pointer () == 0)
    {
      // No activations.  Pop the base pointer and finish.
      exec.stack ().teardown ();
      return;
    }

  // Process all of the deferred activation bodies.
  // First, go to the last frame.
  exec.stack ().base_pointer (exec.mutable_phase_base_pointer ());

  while (exec.stack ().base_pointer () != NULL)
    {
      // Get the deferred body.
      Operation* o = *static_cast<Operation**> (exec.stack ().pointer_to_instruction_pointer ());

      // Execute it.
      o->execute (exec);

      // Pop the base pointer.
      exec.stack ().teardown ();
    }
}

bool execute (executor_base_t& exec, component_t* instance, const Action* action, Type::Int::ValueType iota)
{
  if (enabled (exec, instance, action, iota))
    {
      return execute_no_check (exec, instance, action, iota);
    }

  return false;
}

bool execute_no_check (executor_base_t& exec, component_t* instance, const Action* action, Type::Int::ValueType iota)
{
  assert (exec.stack ().empty ());

  // Push the instance.
  exec.stack ().push_pointer (instance);
  // Push iota.
  if (action->has_dimension ())
    {
      exec.stack ().push<Uint::ValueType> (iota);
    }
  // Push the instruction pointer.
  exec.stack ().push_pointer (NULL);
  // Execute.
  runtime::execute (exec, action, instance);
  // Pop the instruction pointer.
  exec.stack ().pop_pointer ();
  // Pop iota.
  if (action->has_dimension ())
    {
      Uint::ValueType x;
      exec.stack ().pop (x);
    }
  // Pop the instance.
  exec.stack ().pop_pointer ();

  assert (exec.stack ().empty ());
  return true;
}

struct NewImpl : public Callable
{
  NewImpl (const Type::Type* t, ast::Node* definingNode)
    : type_ (t)
    , function_type_ (makeFunctionType (t, definingNode))
  {
    allocate_parameter (memory_model, function_type_->GetSignature ()->Begin (), function_type_->GetSignature ()->End ());
    allocate_symbol (memory_model, function_type_->GetReturnParameter ());
  }

  virtual void call (executor_base_t& exec) const
  {
    // Allocate a new instance of the type.
    const Heap* heap_type = type_cast<Heap> (type_);
    if (heap_type == NULL)
      {
        char** r = static_cast<char**> (exec.stack ().get_address (function_type_->GetReturnParameter ()->offset ()));
        *r = static_cast<char*> (heap_allocate (exec.heap (), type_->Size ()));
      }
    else
      {
        const Type::Type* t = heap_type->Base ();
        // Allocate a new heap and root object.
        heap_t* h = heap_make_size (t->Size ());
        // Insert it into its parent.
        heap_t* h2 = exec.heap ();
        heap_insert_child (h2, h);
        // Allocate a new heap link in the parent.
        heap_link_t* hl = make_heap_link (h, h2);
        heap_link_t** r = static_cast<heap_link_t**> (exec.stack ().get_address (function_type_->GetReturnParameter ()->offset ()));
        *r = hl;
      }
  }

  virtual const Type::Type* type () const
  {
    return function_type_;
  }
  const Type::Type* const type_;
  const Type::Function* const function_type_;
  MemoryModel memory_model;
  static const Type::Function* makeFunctionType (const Type::Type* type, ast::Node* definingNode)
  {
    const Type::Type* return_type = type->GetPointer ();
    return new Type::Function (Type::Function::FUNCTION, (new Signature ()),
                               ParameterSymbol::makeReturn (definingNode, ReturnSymbol, return_type, MUTABLE));
  }

  virtual size_t return_size () const
  {
    return function_type_->GetReturnType ()->Size ();
  }
  virtual size_t receiver_size () const
  {
    return 0;
  }
  virtual size_t arguments_size () const
  {
    return function_type_->GetSignature ()->Size ();
  }
  virtual size_t locals_size () const
  {
    return 0;
  }
  virtual const Type::Signature* signature () const
  {
    not_reached;
  }
  virtual void check_types (ast::Node* args) const
  {
    // Do nothing.
  }
  virtual void check_references (ast::Node* args) const
  {
    semantic::require_type (args->At (0));
  }
  virtual void check_mutability (ast::Node* args) const
  {
    // Do nothing.
  }
  virtual void compute_receiver_access (ast::Node* args, ReceiverAccess& receiver_access, bool& flag) const
  {
    // Do nothing.
  }

};

New::New (ast::Node* dn)
  : Template ("new",
              dn,
              new Type::Template ())
{ }

Callable*
New::instantiate (const std::vector<const Type::Type*>& argument_types) const
{
  if (argument_types.size () != 1)
    {
      error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                     "new expects one argument (E220)");
    }

  const Type::Type* type = argument_types.front ();

  // TODO.
  // if (tv.kind != typed_value_t::TYPE)
  //   {
  //     error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
  //                    "new expects a type (E219)");
  //   }

  return new NewImpl (type, definingNode);
}

struct MoveImpl : public Callable
{
  MoveImpl (const Type::Type* in, const Type::Type* out, ast::Node* definingNode)
    : function_type_ (makeFunctionType (in, out, definingNode))
  {
    allocate_parameter (memory_model, function_type_->GetSignature ()->Begin (), function_type_->GetSignature ()->End ());
    allocate_symbol (memory_model, function_type_->GetReturnParameter ());
  }

  virtual void call (executor_base_t& exec) const
  {
    heap_link_t** r = static_cast<heap_link_t**> (exec.stack ().get_address (function_type_->GetReturnParameter ()->offset ()));
    ParameterSymbol* p = *function_type_->GetSignature ()->Begin ();
    heap_link_t* hl = static_cast<heap_link_t*> (exec.stack ().read_pointer (p->offset ()));
    if (hl != NULL)
      {
        pthread_mutex_lock (&hl->mutex);
        if (hl->heap != NULL && hl->change_count == 0)
          {
            // Break the link.
            heap_t* h = hl->heap;
            hl->heap = NULL;
            pthread_mutex_unlock (&hl->mutex);

            // Remove from parent.
            heap_remove_from_parent (h);
            // Insert into the new parent.
            heap_t* h2 = exec.heap ();
            heap_insert_child (h2, h);

            // Allocate a new heap link in the parent.
            heap_link_t* new_hl = make_heap_link (h, h2);

            // Return the heap link.
            *r = new_hl;
          }
        else
          {
            pthread_mutex_unlock (&hl->mutex);
            *r = NULL;
          }
      }
    else
      {
        *r = NULL;
      }
  }

  virtual const Type::Type* type () const
  {
    return function_type_;
  }
  const Type::Function* const function_type_;
  MemoryModel memory_model;
  static const Type::Function* makeFunctionType (const Type::Type* in, const Type::Type* out, ast::Node* definingNode)
  {
    // TODO:  The mutabilities may need to be adjusted.
    return new Type::Function (Type::Function::FUNCTION, (new Signature ())
                               ->Append (ParameterSymbol::make (definingNode, "h", in, MUTABLE, FOREIGN)),
                               ParameterSymbol::makeReturn (definingNode, ReturnSymbol, out, MUTABLE));
  }

  virtual size_t return_size () const
  {
    return function_type_->GetReturnType ()->Size ();
  }
  virtual size_t receiver_size () const
  {
    return 0;
  }
  virtual size_t arguments_size () const
  {
    return function_type_->GetSignature ()->Size ();
  }
  virtual size_t locals_size () const
  {
    return 0;
  }
  virtual const Type::Signature* signature () const
  {
    return function_type_->GetSignature ();
  }
};

Move::Move (ast::Node* dn)
  : Template ("move",
              dn,
              new Type::Template ())
{ }

Callable*
Move::instantiate (const std::vector<const Type::Type*>& argument_types) const
{
  if (argument_types.size () != 1)
    {
      error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                     "move expects one argument (E218)");
    }

  const Type::Type* in = argument_types.front ();
  const Type::Type* out = type_move (in);
  if (out == NULL)
    {
      error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                     "cannot move expression of type %s (E217)", in->ToString ().c_str ());
    }

  return new MoveImpl (in, out, definingNode);
}

struct MergeImpl : public Callable
{
  MergeImpl (const Type::Type* in, const Type::Type* out, ast::Node* definingNode)
    : function_type_ (makeFunctionType (in, out, definingNode))
  {
    allocate_parameter (memory_model, function_type_->GetSignature ()->Begin (), function_type_->GetSignature ()->End ());
    allocate_symbol (memory_model, function_type_->GetReturnParameter ());
  }

  virtual void call (executor_base_t& exec) const
  {
    char** r = static_cast<char**> (exec.stack ().get_address (function_type_->GetReturnParameter ()->offset ()));
    ParameterSymbol* p = *function_type_->GetSignature ()->Begin ();
    heap_link_t* hl = static_cast<heap_link_t*> (exec.stack ().read_pointer (p->offset ()));
    if (hl != NULL)
      {
        pthread_mutex_lock (&hl->mutex);
        if (hl->heap != NULL && hl->change_count == 0)
          {
            // Break the link.
            heap_t* h = hl->heap;
            hl->heap = NULL;
            pthread_mutex_unlock (&hl->mutex);

            // Get the heap root.
            char* root = static_cast<char*> (heap_instance (h));

            // Remove from parent.
            heap_remove_from_parent (h);

            // Merge into the new parent.
            heap_merge (exec.heap (), h);

            // Return the root.
            *r = root;
          }
        else
          {
            pthread_mutex_unlock (&hl->mutex);
            *r = NULL;
          }
      }
    else
      {
        *r = NULL;
      }
  }

  virtual const Type::Type* type () const
  {
    return function_type_;
  }
  const Type::Function* const function_type_;
  MemoryModel memory_model;
  static const Type::Function* makeFunctionType (const Type::Type* in, const Type::Type* out, ast::Node* definingNode)
  {
    // TODO:  Adjust mutability.
    return new Type::Function (Type::Function::FUNCTION, (new Signature ())
                               ->Append (ParameterSymbol::make (definingNode, "h", in, MUTABLE, FOREIGN)),
                               ParameterSymbol::makeReturn (definingNode, ReturnSymbol, out, MUTABLE));
  }

  virtual size_t return_size () const
  {
    return function_type_->GetReturnType ()->Size ();
  }
  virtual size_t receiver_size () const
  {
    return 0;
  }
  virtual size_t arguments_size () const
  {
    return function_type_->GetSignature ()->Size ();
  }
  virtual size_t locals_size () const
  {
    return 0;
  }
  virtual const Type::Signature* signature () const
  {
    return function_type_->GetSignature ();
  }
};

Merge::Merge (ast::Node* dn)
  : Template ("merge",
              dn,
              new Type::Template ())
{ }

Callable*
Merge::instantiate (const std::vector<const Type::Type*>& argument_types) const
{
  if (argument_types.size () != 1)
    {
      error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                     "merge expects one argument (E216)");
    }

  const Type::Type* in = argument_types.front ();
  const Type::Type* out = type_merge (in);
  if (out == NULL)
    {
      error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                     "cannot merge expression of type %s (E215)", in->ToString ().c_str ());
    }

  return new MergeImpl (in, out, definingNode);
}

struct CopyImpl : public Callable
{
  CopyImpl (const Type::Type* in, ast::Node* definingNode)
    : function_type_ (makeFunctionType (in, definingNode))
  {
    allocate_parameter (memory_model, function_type_->GetSignature ()->Begin (), function_type_->GetSignature ()->End ());
    allocate_symbol (memory_model, function_type_->GetReturnParameter ());
  }

  virtual void call (executor_base_t& exec) const
  {
    const Slice* slice_type = type_strip_cast<Slice>(function_type_->GetReturnParameter ()->type);
    if (slice_type != NULL)
      {
        Slice::ValueType* in = static_cast<Slice::ValueType*> (exec.stack ().get_address (function_type_->GetSignature ()->At (0)->offset ()));
        Slice::ValueType* out = static_cast<Slice::ValueType*> (exec.stack ().get_address (function_type_->GetReturnParameter ()->offset ()));
        size_t sz = slice_type->Base ()->Size () * in->length;
        out->ptr = malloc (sz);
        memcpy (out->ptr, in->ptr, sz);
        out->length = in->length;
        out->capacity = in->length;
        return;
      }

    const StringU* string_type = type_strip_cast<StringU>(function_type_->GetReturnParameter ()->type);
    if (string_type != NULL)
      {
        StringU::ValueType* in = static_cast<StringU::ValueType*> (exec.stack ().get_address (function_type_->GetSignature ()->At (0)->offset ()));
        StringU::ValueType* out = static_cast<StringU::ValueType*> (exec.stack ().get_address (function_type_->GetReturnParameter ()->offset ()));
        out->ptr = malloc (in->length);
        memcpy (out->ptr, in->ptr, in->length);
        out->length = in->length;
        return;
      }

    void* in = exec.stack ().get_address (function_type_->GetSignature ()->At (0)->offset ());
    void* out = exec.stack ().get_address (function_type_->GetReturnParameter ()->offset ());
    memcpy (out, in, function_type_->GetReturnType ()->Size ());
  }

  virtual const Type::Type* type () const
  {
    return function_type_;
  }
  const Type::Function* const function_type_;
  MemoryModel memory_model;
  static const Type::Function* makeFunctionType (const Type::Type* in, ast::Node* definingNode)
  {
    return new Type::Function (Type::Function::FUNCTION, (new Signature ())
                               ->Append (ParameterSymbol::make (definingNode, "h", in, IMMUTABLE, FOREIGN)),
                               ParameterSymbol::makeReturn (definingNode, ReturnSymbol, in, MUTABLE));
  }

  virtual size_t return_size () const
  {
    return function_type_->GetReturnType ()->Size ();
  }
  virtual size_t receiver_size () const
  {
    return 0;
  }
  virtual size_t arguments_size () const
  {
    return function_type_->GetSignature ()->Size ();
  }
  virtual size_t locals_size () const
  {
    return 0;
  }
  virtual const Type::Signature* signature () const
  {
    return function_type_->GetSignature ();
  }
};

Copy::Copy (ast::Node* dn)
  : Template ("copy",
              dn,
              new Type::Template ())
{ }

Callable*
Copy::instantiate (const std::vector<const Type::Type*>& argument_types) const
{
  if (argument_types.size () != 1)
    {
      error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                     "copy expects one argument (E123)");
    }

  const Type::Type* in = argument_types.front ();

  if (type_strip_cast<Component> (in) != NULL)
    {
      error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                     "cannot copy components (E94)");
    }

  {
    const Slice* st = type_strip_cast<Slice> (in);
    if (st != NULL)
      {
        if (type_contains_pointer (st->Base ()))
          {
            error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                           "copy leaks pointers (E95)");

          }
        // We will copy so a dereference can mutate the data.
        //tv.intrinsic_mutability = MUTABLE;
        //tv.dereference_mutability = MUTABLE;
      }
  }

  {
    const StringU* st = type_strip_cast<StringU> (in);
    if (st != NULL)
      {
        // We will copy so a dereference can mutate the data.
        //tv.intrinsic_mutability = MUTABLE;
        //tv.dereference_mutability = IMMUTABLE;
      }
  }

  return new CopyImpl (in, definingNode);
}

struct PrintlnImpl : public Callable
{
  PrintlnImpl (const TypeList& type_list)
    : function_type_ (makeFunctionType (type_list))
  {
    allocate_parameter (memory_model, function_type_->GetSignature ()->Begin (), function_type_->GetSignature ()->End ());
    allocate_symbol (memory_model, function_type_->GetReturnParameter ());
  }

  virtual void call (executor_base_t& exec) const
  {
    struct visitor : public Type::DefaultVisitor
    {
      executor_base_t& exec;
      void* ptr;

      visitor (executor_base_t& e, void* p) : exec (e), ptr (p) { }

      void default_action (const Type::Type& type)
      {
        type_not_reached (type);
      }

      void visit (const Bool& type)
      {
        if (*static_cast<Bool::ValueType*> (ptr))
          {
            printf ("true");
          }
        else
          {
            printf ("false");
          }
      }

      void visit (const Pointer& type)
      {
        printf ("%p", *static_cast<Pointer::ValueType*> (ptr));
      }

      void visit (const StringU& type)
      {
        StringU::ValueType* u = static_cast<StringU::ValueType*> (ptr);
        fwrite (u->ptr, 1, u->length, stdout);
      }

      void visit (const Uint& type)
      {
        printf ("%lu", *static_cast<Uint::ValueType*> (ptr));
      }

      void visit (const Uint8& type)
      {
        printf ("%u", *static_cast<Uint8::ValueType*> (ptr));
      }

      void visit (const Uint16& type)
      {
        printf ("%u", *static_cast<Uint16::ValueType*> (ptr));
      }

      void visit (const Uint64& type)
      {
        printf ("%lu", *static_cast<Uint64::ValueType*> (ptr));
      }

      void visit (const Type::Int& type)
      {
        printf ("%ld", *static_cast<Int::ValueType*> (ptr));
      }

      void visit (const Int8& type)
      {
        printf ("%d", *static_cast<Int8::ValueType*> (ptr));
      }

      void visit (const Float64& type)
      {
        printf ("%g", *static_cast<Float64::ValueType*> (ptr));
      }

      void visit (const Slice& type)
      {
        printf ("<slice>");
      }
    };

    exec.lock_stdout ();
    for (Type::Signature::const_iterator pos = function_type_->GetSignature ()->Begin (), limit = function_type_->GetSignature ()->End ();
         pos != limit;
         ++pos)
      {
        ParameterSymbol* parameter = *pos;
        void* ptr = exec.stack ().get_address (parameter->offset ());
        visitor v (exec, ptr);
        parameter->type->UnderlyingType ()->Accept (v);
      }
    printf ("\n");
    exec.unlock_stdout ();
  }

  virtual const Type::Type* type () const
  {
    return function_type_;
  }
  const Type::Function* const function_type_;
  MemoryModel memory_model;

  static const Type::Function* makeFunctionType (const TypeList& argument_types)
  {
    Signature* sig = new Signature ();
    for (TypeList::const_iterator pos = argument_types.begin (), limit = argument_types.end ();
         pos != limit;
         ++pos)
      {
        const Type::Type* t = *pos;
        t = t->DefaultType ();
        sig->Append (ParameterSymbol::make (NULL, "", t, IMMUTABLE, FOREIGN));
      }

    return new Type::Function (Type::Function::FUNCTION, sig,
                               ParameterSymbol::makeReturn (NULL, ReturnSymbol, Void::Instance (), FOREIGN));

  }

  virtual size_t return_size () const
  {
    return function_type_->GetReturnType ()->Size ();
  }
  virtual size_t receiver_size () const
  {
    return 0;
  }
  virtual size_t arguments_size () const
  {
    return function_type_->GetSignature ()->Size ();
  }
  virtual size_t locals_size () const
  {
    return 0;
  }
  virtual const Type::Signature* signature () const
  {
    return function_type_->GetSignature ();
  }
};

Println::Println (ast::Node* dn)
  : Template ("println",
              dn,
              new Type::Template ())
{ }

Callable*
Println::instantiate (const TypeList& argument_types) const
{
  return new PrintlnImpl (argument_types);
}

OpReturn
Load::execute (executor_base_t& exec) const
{
  child->execute (exec);
  void * ptr = exec.stack ().pop_pointer ();
  exec.stack ().load (ptr, type->Size ());
  return make_continue ();
}


OpReturn
IndexSlice::execute (executor_base_t& exec) const
{
  base->execute (exec);
  Slice::ValueType s;
  exec.stack ().pop (s);
  index->execute (exec);
  Type::Int::ValueType i;
  exec.stack ().pop (i);

  if (i < 0 || static_cast<Type::Uint::ValueType> (i) >= s.length)
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "slice index is out of bounds (E35)");

    }
  exec.stack ().push_pointer (static_cast<char*> (s.ptr) + i * type->UnitSize ());
  return make_continue ();
}

template<typename T>
struct ConvertToInt : public Operation
{
  ConvertToInt (const Operation* c) : child (c) { }
  OpReturn execute (executor_base_t& exec) const
  {
    child->execute (exec);
    typename T::ValueType in;
    exec.stack ().pop (in);
    Type::Int::ValueType out = in;
    exec.stack ().push (out);
    return make_continue ();
  }
  virtual void dump () const
  {
    std::cout << "ConvertToInt(";
    child->dump ();
    std::cout << ")\n";
  }
  const Operation* const child;
};

// template<typename T>
// static Operation*
// makeConvertToInt (const T& t, const Operation* child)
// {
//   return new ConvertToInt<T> (child);
// }

// struct ConvertToIntListener
// {
//   const Operation* const child;
//   Operation* operation;
//   ConvertToIntListener (const Operation* c) : child (c), operation (NULL) { }

//   void NotIntegral (const Type::Type& type)
//   {
//     not_reached;
//   }

//   void operator() (const Uint8& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Uint16& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Uint32& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Uint64& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }

//   void operator() (const Int8& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Int16& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Int32& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Int64& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }

//   void operator() (const Uint& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Type::Int& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Uintptr& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Rune& type)
//   {
//     NotIntegral (type);
//   }
//   void operator() (const Integer& type)
//   {
//     NotIntegral (type);
//   }
// };

Operation*
MakeConvertToInt (const Operation* c, const Type::Type* type)
{
  switch (type->underlying_kind ())
    {
    case kUint8:
      return new ConvertToInt<Uint8> (c);
    case kUint16:
      return new ConvertToInt<Uint16> (c);
    case kUint32:
      return new ConvertToInt<Uint32> (c);
    case kUint64:
      return new ConvertToInt<Uint64> (c);
    case kInt8:
      return new ConvertToInt<Int8> (c);
    case kInt16:
      return new ConvertToInt<Int16> (c);
    case kInt32:
      return new ConvertToInt<Int32> (c);
    case kInt64:
      return new ConvertToInt<Int64> (c);
    case kUint:
      return new ConvertToInt<Uint> (c);
    case kInt:
      return new ConvertToInt<Int> (c);
    case kUintptr:
      return new ConvertToInt<Uintptr> (c);

    default:
      type_not_reached (*type);
    }
}

template<typename T>
struct ConvertToUint : public Operation
{
  ConvertToUint (const Operation* c) : child (c) { }
  OpReturn execute (executor_base_t& exec) const
  {
    child->execute (exec);
    typename T::ValueType in;
    exec.stack ().pop (in);
    Type::Uint::ValueType out = in;
    exec.stack ().push (out);
    return make_continue ();
  }
  virtual void dump () const
  {
    unimplemented;
  }
  const Operation* const child;
};

Operation*
MakeConvertToUint (const Operation* c, const Type::Type* type)
{
  switch (type->underlying_kind ())
    {
    case kUint8:
      return new ConvertToUint<Uint8> (c);
    case kUint16:
      return new ConvertToUint<Uint16> (c);
    case kUint32:
      return new ConvertToUint<Uint32> (c);
    case kUint64:
      return new ConvertToUint<Uint64> (c);
    case kInt8:
      return new ConvertToUint<Int8> (c);
    case kInt16:
      return new ConvertToUint<Int16> (c);
    case kInt32:
      return new ConvertToUint<Int32> (c);
    case kInt64:
      return new ConvertToUint<Int64> (c);
    case kUint:
      return new ConvertToUint<Uint> (c);
    case kInt:
      return new ConvertToUint<Int> (c);
    case kUintptr:
      return new ConvertToUint<Uintptr> (c);

    default:
      type_not_reached (*type);
    }
}

OpReturn
LogicOr::execute (executor_base_t& exec) const
{
  left->execute (exec);
  Bool::ValueType b;
  exec.stack ().pop (b);
  if (b)
    {
      exec.stack ().push (b);
    }
  else
    {
      right->execute (exec);
    }
  return make_continue ();
}

OpReturn
LogicAnd::execute (executor_base_t& exec) const
{
  left->execute (exec);
  Bool::ValueType b;
  exec.stack ().pop (b);
  if (!b)
    {
      exec.stack ().push (b);
    }
  else
    {
      right->execute (exec);
    }
  return make_continue ();
}

struct MakeLiteralVisitor : public Type::DefaultVisitor
{
  const value_t& value;
  Operation* op;

  MakeLiteralVisitor (const value_t& v) : value (v), op (NULL) { }

  void default_action (const Type::Type& type)
  {
    type_not_reached (type);
  }

  void visit (const Bool& type)
  {
    op = make_literal (value.ref (type));
  }

  void visit (const Uint8& type)
  {
    op = make_literal (value.ref (type));
  }

  void visit (const Uint16& type)
  {
    op = make_literal (value.ref (type));
  }

  void visit (const Uint32& type)
  {
    op = make_literal (value.ref (type));
  }

  void visit (const Uint64& type)
  {
    op = make_literal (value.ref (type));
  }

  void visit (const Int8& type)
  {
    op = make_literal (value.ref (type));
  }

  void visit (const Int16& type)
  {
    op = make_literal (value.ref (type));
  }

  void visit (const Int32& type)
  {
    op = make_literal (value.ref (type));
  }

  void visit (const Int64& type)
  {
    op = make_literal (value.ref (type));
  }

  void visit (const Uint& type)
  {
    op = make_literal (value.ref (type));
  }

  void visit (const Int& type)
  {
    op = make_literal (value.ref (type));
  }

  void visit (const StringU& type)
  {
    op = make_literal (value.ref (type));
  }

  void visit (const Pointer& type)
  {
    op = make_literal (value.ref (type));
  }
};

Operation* make_literal (const Type::Type* type, const value_t& value)
{
  assert (value.present);
  MakeLiteralVisitor visitor (value);
  type->UnderlyingType ()->Accept (visitor);
  return visitor.op;
}

OpReturn
ListOperation::execute (executor_base_t& exec) const
{
  for (ListType::const_iterator pos = list.begin (), limit = list.end ();
       pos != limit;
       ++pos)
    {
      OpReturn r = (*pos)->execute (exec);
      if (r == kReturn)
        {
          return r;
        }
    }
  return make_continue ();
}

OpReturn
FunctionCall::execute (executor_base_t& exec) const
{
  // Create space for the return.
  exec.stack ().reserve (callable->return_size ());

  // Push the arguments.
  arguments->execute (exec);

  // Push a fake instruction pointer.
  exec.stack ().push_pointer (NULL);

  // Setup the frame.
  exec.stack ().setup (callable->locals_size ());

  // Do the call.
  callable->call (exec);

  // Tear down the frame.
  exec.stack ().teardown ();

  // Pop the fake instruction pointer.
  exec.stack ().pop_pointer ();

  // Pop the arguments.
  exec.stack ().popn (callable->arguments_size ());

  return make_continue ();
}

OpReturn
MethodCall::execute (executor_base_t& exec) const
{
  // Create space for the return.
  exec.stack ().reserve (callable->return_size ());

  // Push the receiver.
  receiver->execute (exec);

  // Push the arguments.
  arguments->execute (exec);

  // Push a fake instruction pointer.
  exec.stack ().push_pointer (NULL);

  // Setup the frame.
  exec.stack ().setup (callable->locals_size ());

  // Do the call.
  callable->call (exec);

  // Tear down the frame.
  exec.stack ().teardown ();

  // Pop the fake instruction pointer.
  exec.stack ().pop_pointer ();

  // Pop the arguments.
  exec.stack ().popn (callable->arguments_size ());

  exec.stack ().popn (callable->receiver_size ());

  return make_continue ();
}

OpReturn
DynamicFunctionCall::execute (executor_base_t& exec) const
{
  switch (type->function_kind)
    {
    case Type::Function::FUNCTION:
      unimplemented;
    case Type::Function::PUSH_PORT:
      unimplemented;
    case Type::Function::PULL_PORT:
    {
      func->execute (exec);
      pull_port_t pp;
      exec.stack ().pop (pp);

      // Create space for the return.
      exec.stack ().reserve (pp.getter->return_size ());

      // Push the arguments.
      exec.stack ().push_pointer (pp.instance);
      arguments->execute (exec);

      // Push a fake instruction pointer.
      exec.stack ().push_pointer (NULL);

      // Setup the frame.
      exec.stack ().setup (pp.getter->locals_size ());

      // Do the call.
      pp.getter->call (exec);

      // Tear down the frame.
      exec.stack ().teardown ();

      // Pop the fake instruction pointer.
      exec.stack ().pop_pointer ();

      // Pop the arguments.
      exec.stack ().popn (pp.getter->arguments_size ());
      exec.stack ().pop_pointer ();

      return make_continue ();
    }
    break;
    }

  not_reached;
}

OpReturn
Instance::execute (executor_base_t& exec) const
{
  exec.stack ().push_pointer (instance->instance->component);
  return make_continue ();
}

OpReturn
SetRestoreCurrentInstance::execute (executor_base_t& exec) const
{
  component_t* new_receiver = static_cast<component_t*> (exec.stack ().read_pointer (receiver_offset));
  component_t* old_receiver = exec.current_instance (new_receiver);
  OpReturn ca = child->execute (exec);
  exec.current_instance (old_receiver);
  return ca;
}

OpReturn
Clear::execute (executor_base_t& exec) const
{
  exec.stack ().clear (offset, size);
  return make_continue ();
}

OpReturn
Assign::execute (executor_base_t& exec) const
{
  left->execute (exec);
  void* ptr = exec.stack ().pop_pointer ();
  right->execute (exec);
  exec.stack ().store (ptr, size);
  return make_continue ();
}

template <typename T>
struct AddAssign : public Operation
{
  AddAssign (Operation* l, Operation* r) : left (l), right (r)
  {
    assert (left != NULL);
    assert (right != NULL);
  }
  OpReturn
  execute (executor_base_t& exec) const
  {
    left->execute (exec);
    T* ptr = static_cast<T*> (exec.stack ().pop_pointer ());
    right->execute (exec);
    T v;
    exec.stack ().pop (v);
    *ptr += v;
    return make_continue ();
  }
  virtual void dump () const
  {
    unimplemented;
  }
  Operation* const left;
  Operation* const right;
};

Operation* make_add_assign (Operation* l, Operation* r, const Type::Type* t)
{
  switch (t->underlying_kind ())
    {
    case kUint8:
      return new AddAssign<Uint8::ValueType> (l, r);
    case kUint16:
      return new AddAssign<Uint16::ValueType> (l, r);
    case kUint32:
      return new AddAssign<Uint32::ValueType> (l, r);
    case kUint64:
      return new AddAssign<Uint64::ValueType> (l, r);
    case kInt8:
      return new AddAssign<Int8::ValueType> (l, r);
    case kInt16:
      return new AddAssign<Int16::ValueType> (l, r);
    case kInt32:
      return new AddAssign<Int32::ValueType> (l, r);
    case kInt64:
      return new AddAssign<Int64::ValueType> (l, r);
    case kFloat32:
      return new AddAssign<Float32::ValueType> (l, r);
    case kFloat64:
      return new AddAssign<Float64::ValueType> (l, r);
    case kComplex64:
      return new AddAssign<Complex64::ValueType> (l, r);
    case kComplex128:
      return new AddAssign<Complex128::ValueType> (l, r);
    case kUint:
      return new AddAssign<Uint::ValueType> (l, r);
    case kInt:
      return new AddAssign<Int::ValueType> (l, r);
    case kUintptr:
      return new AddAssign<Uintptr::ValueType> (l, r);
    default:
      not_reached;
    }
}

OpReturn
Reference::execute (executor_base_t& exec) const
{
  exec.stack ().push_address (offset);
  return make_continue ();
}

OpReturn
Select::execute (executor_base_t& exec) const
{
  base->execute (exec);
  char* ptr = static_cast<char*> (exec.stack ().pop_pointer ());
  ptr += offset;
  exec.stack ().push_pointer (ptr);
  return make_continue ();
}

OpReturn
IndexArray::execute (executor_base_t& exec) const
{
  base->execute (exec);
  void* ptr = exec.stack ().pop_pointer ();
  index->execute (exec);
  Type::Int::ValueType i;
  exec.stack ().pop (i);
  if (i < 0 || i >= type->dimension)
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "array index is out of bounds (E148)");
    }
  exec.stack ().push_pointer (static_cast<char*> (ptr) + i * type->UnitSize ());
  return make_continue ();
}

OpReturn
SliceArray::execute (executor_base_t& exec) const
{
  base->execute (exec);
  char* ptr = static_cast<char*> (exec.stack ().pop_pointer ());
  low->execute (exec);
  Int::ValueType low_val;
  exec.stack ().pop (low_val);
  high->execute (exec);
  Int::ValueType high_val;
  exec.stack ().pop (high_val);
  // Bounds check.
  if (!(0 <= low_val &&
        low_val <= high_val &&
        high_val <= type->dimension))
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "slice index is out of range (E223)");
    }

  Slice::ValueType slice_val;
  slice_val.length = high_val - low_val;
  slice_val.capacity = type->dimension - low_val;
  slice_val.ptr = slice_val.length ? ptr + low_val * type->UnitSize () : NULL;
  exec.stack ().push (slice_val);

  return make_continue ();
}

OpReturn
Return::execute (executor_base_t& exec) const
{
  child->execute (exec);
  exec.stack ().move (return_offset, return_size);
  return make_return ();
}

OpReturn
If::execute (executor_base_t& exec) const
{
  condition->execute (exec);
  Bool::ValueType c;
  exec.stack ().pop (c);
  if (c)
    {
      return true_branch->execute (exec);
    }
  else
    {
      return false_branch->execute (exec);
    }
}

OpReturn
While::execute (executor_base_t& exec) const
{
  for (;;)
    {
      condition->execute (exec);
      Bool::ValueType c;
      exec.stack ().pop (c);
      if (c)
        {
          OpReturn r = body->execute (exec);
          if (r == kReturn)
            {
              return r;
            }
        }
      else
        {
          return make_continue ();
        }
    }
}

OpReturn
Change::execute (executor_base_t& exec) const
{
  root->execute (exec);
  heap_link_t* hl = static_cast<heap_link_t*> (exec.stack ().pop_pointer ());
  if (hl == NULL)
    {
      // Heap link is null.
      unimplemented;
    }
  pthread_mutex_lock (&hl->mutex);
  ++hl->change_count;
  pthread_mutex_unlock (&hl->mutex);

  // Save the old heap.
  heap_t* old_heap = exec.heap ();
  // Set the the new heap.
  exec.heap (hl->heap);

  char** root_value = static_cast<char**> (exec.stack ().get_address (root_offset));

  // Push a pointer to the root object.
  *root_value = static_cast<char*> (heap_instance (hl->heap));

  OpReturn ca = body->execute (exec);

  // Restore the old heap.
  exec.heap (old_heap);

  pthread_mutex_lock (&hl->mutex);
  --hl->change_count;
  pthread_mutex_unlock (&hl->mutex);

  return ca;
}

template <typename T>
struct Increment : public Operation
{
  Increment (Operation* c) : child (c) { }
  virtual OpReturn execute (executor_base_t& exec) const
  {
    child->execute (exec);
    T* ptr = static_cast<T*> (exec.stack ().pop_pointer ());
    ++*ptr;
    return make_continue ();
  }
  virtual void dump () const
  {
    unimplemented;
  }
  Operation* const child;
};

Operation* make_increment (Operation* child, const Type::Type* type)
{
  switch (type->underlying_kind ())
    {
    case kUint8:
      return new Increment<Uint8::ValueType> (child);
    case kUint16:
      return new Increment<Uint16::ValueType> (child);
    case kUint32:
      return new Increment<Uint32::ValueType> (child);
    case kUint64:
      return new Increment<Uint64::ValueType> (child);
    case kInt8:
      return new Increment<Int8::ValueType> (child);
    case kInt16:
      return new Increment<Int16::ValueType> (child);
    case kInt32:
      return new Increment<Int32::ValueType> (child);
    case kInt64:
      return new Increment<Int64::ValueType> (child);
    case kFloat32:
      return new Increment<Float32::ValueType> (child);
    case kFloat64:
      return new Increment<Float64::ValueType> (child);
    case kComplex64:
      return new Increment<Complex64::ValueType> (child);
    case kComplex128:
      return new Increment<Complex128::ValueType> (child);
    case kUint:
      return new Increment<Uint::ValueType> (child);
    case kInt:
      return new Increment<Int::ValueType> (child);
    case kUintptr:
      return new Increment<Uintptr::ValueType> (child);
    default:
      type_not_reached (*type);
    }
}

OpReturn
Activate::execute (executor_base_t& exec) const
{
  // Save the base pointer to return to.
  // Currently, this is done in the port call.
  // char* return_bp = *reinterpret_cast<char**> (exec.stack ().base_pointer ());
  // Insert into the mutable phase list.
  *reinterpret_cast<char**> (exec.stack ().base_pointer ()) = exec.mutable_phase_base_pointer ();
  exec.mutable_phase_base_pointer (exec.stack ().base_pointer ());

  // The caller pushed a (fake) instruction pointer which is just
  // before the base pointer.  Overwrite it with the body.
  Operation** ip = static_cast<Operation**> (exec.stack ().pointer_to_instruction_pointer ());
  *ip = body;
  // Execute the expression list.
  port_calls->execute (exec);

  return make_return ();
}

static void push_port_call (executor_base_t& exec, Operation* args, ptrdiff_t receiver_offset, ptrdiff_t port_offset, ptrdiff_t array_offset)
{
  // TODO:  The port knows the size of the arguments.  No need to measure.
  // Push all of the arguments first and measure their size.
  char* top_before = exec.stack ().top ();
  args->execute (exec);
  char* top_after = exec.stack ().top ();
  ptrdiff_t arguments_size = top_after - top_before; // Assumes stack grows up.

  // Find the port to activate.
  port_t* port = *reinterpret_cast<port_t**> (static_cast<char*> (exec.stack ().read_pointer (receiver_offset)) + port_offset + array_offset);

  char* base_pointer = exec.stack ().base_pointer ();

  // Activate all the reactions bound to the port.
  while (port != NULL)
    {
      // Set up a frame.
      // Push the instance.
      exec.stack ().push_pointer (port->instance);
      // Push the parameter.
      if (port->reaction->has_dimension ())
        {
          exec.stack ().push<Type::Int::ValueType> (port->parameter);
        }
      // Push the arguments.
      exec.stack ().load (top_before, arguments_size);
      // Push an instruction pointer.
      exec.stack ().push_pointer (NULL);

      // Jump to the last frame.
      exec.stack ().base_pointer (exec.mutable_phase_base_pointer ());

      exec.stack ().setup (port->reaction->memory_model.LocalsSize ());

      port->reaction->call (exec);

      // Move back to our frame.
      exec.stack ().base_pointer (base_pointer);

      port = port->next;
    }
}

OpReturn
PushPortCall::execute (executor_base_t& exec) const
{
  push_port_call (exec, args, receiver_offset, port_offset, 0);
  return make_continue ();
}

OpReturn
IndexedPushPortCall::execute (executor_base_t& exec) const
{
  index->execute (exec);
  Int::ValueType idx;
  exec.stack ().pop (idx);
  push_port_call (exec, args, receiver_offset, port_offset, idx * array_type->UnitSize ());
  return make_continue ();
}

OpReturn
Push::execute (executor_base_t& exec) const
{
  OpReturn ca = body->execute (exec);
  exec.push ();
  return ca;
}

OpReturn
ForIota::execute (executor_base_t& exec) const
{
  for (Int::ValueType idx = 0; idx != limit; ++idx)
    {
      Int::ValueType* i = static_cast<Int::ValueType*> (exec.stack ().get_address (offset));
      *i = idx;
      OpReturn r = body->execute (exec);
      if (r == kReturn)
        {
          return r;
        }
    }
  return make_continue ();
}

struct ConvertStringToSliceOfBytes : public Operation
{
  ConvertStringToSliceOfBytes (Operation* c) : child (c) { }
  OpReturn
  execute (executor_base_t& exec) const
  {
    child->execute (exec);
    StringU::ValueType in;
    exec.stack ().pop (in);
    Slice::ValueType out;
    out.ptr = heap_allocate (exec.heap (), in.length);
    memcpy (out.ptr, in.ptr, in.length);
    out.length = in.length;
    out.capacity = in.length;
    exec.stack ().push (out);
    return make_continue ();
  }

  virtual void dump () const
  {
    unimplemented;
  }
  Operation* const child;
};

struct ConvertSliceOfBytesToString : public Operation
{
  ConvertSliceOfBytesToString (Operation* c) : child (c) { }
  OpReturn
  execute (executor_base_t& exec) const
  {
    child->execute (exec);
    Slice::ValueType in;
    exec.stack ().pop (in);
    StringU::ValueType out;
    out.ptr = heap_allocate (exec.heap (), in.length);
    memcpy (out.ptr, in.ptr, in.length);
    out.length = in.length;
    exec.stack ().push (out);
    return make_continue ();
  }

  virtual void dump () const
  {
    unimplemented;
  }
  Operation* const child;
};

Operation* make_conversion (Operation* c, const Type::Type* from, const Type::Type* to)
{
  if (Identical (from->UnderlyingType (), to->UnderlyingType ()))
    {
      unimplemented;
    }
  else if (from->Level () == Type::Type::UNNAMED &&
           to->Level () == Type::Type::UNNAMED &&
           from->underlying_kind () == kPointer &&
           to->underlying_kind () == kPointer &&
           Identical (from->pointer_base_type (), to->pointer_base_type ()))
    {
      unimplemented;
    }
  else if ((is_typed_integer (from) || is_typed_float (from)) &&
           (is_typed_integer (to) || is_typed_float (from)))
    {
      unimplemented;
    }
  else if (is_typed_complex (from) && is_typed_complex (to))
    {
      unimplemented;
    }
  else if (is_typed_integer (from) && is_typed_string (to))
    {
      unimplemented;
    }
  else if (is_slice_of_bytes (from) && is_typed_string (to))
    {
      return new ConvertSliceOfBytesToString (c);
    }
  else if (is_slice_of_runes (from) && is_typed_string (to))
    {
      unimplemented;
    }
  else if (is_typed_string (from) && is_slice_of_bytes (to))
    {
      return new ConvertStringToSliceOfBytes (c);
    }
  else if (is_typed_string (from) && is_slice_of_runes (to))
    {
      unimplemented;
    }
  else
    {
      // Assignable.
      unimplemented;
    }
}
}

// void
// dump_instances (const runtime_t* runtime)
// {
//   for (Composer::InstancesType::const_iterator pos = runtime->instance_table.instances.begin (),
//          limit = runtime->instance_table.instances.end ();
//        pos != limit;
//        ++pos)
//     {
//       Instance* instance = pos->second;
//       if (instance->is_top_level ())
//         {
//           unimplemented;
//           //void* ptr = instance_record_get_ptr (instance_get_record (instance));
//           //type_print_value (instance_type (instance), ptr);
//           printf ("\n");
//         }
//     }
// }
