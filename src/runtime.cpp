#include "runtime.hpp"

#include <error.h>

#include "ast.hpp"
#include "ast_visitor.hpp"
#include "callable.hpp"
#include "symbol_visitor.hpp"
#include "types.hpp"
#include "reaction.hpp"
#include "field.hpp"
#include "composition.hpp"
#include "heap.hpp"
#include "executor_base.hpp"
#include "semantic.hpp"
#include "check_types.hpp"
#include "error_reporter.hpp"

namespace runtime
{
using namespace util;
using namespace type;
using namespace ast;
using namespace decl;
using namespace semantic;

struct port_t
{
  component_t* instance;
  const decl::Reaction* reaction;
  type::Int::ValueType parameter;
  port_t* next;

  port_t (component_t* i, const decl::Reaction* r, type::Int::ValueType p) : instance (i), reaction (r), parameter (p), next (NULL) { }
};

struct heap_link_t
{
  Heap* heap;
  pthread_mutex_t mutex;
  size_t change_count;
};

void
allocate_instances (composition::Composer& instance_table)
{
  for (composition::Composer::InstancesType::const_iterator pos = instance_table.instances_begin (),
       limit = instance_table.instances_end ();
       pos != limit;
       ++pos)
    {
      composition::Instance* instance = pos->second;
      component_t* ptr;
      if (instance->is_top_level ())
        {
          const type::Type* type = instance->type;
          size_t size = type->Size ();
          ptr = static_cast<component_t*> (malloc (size));
          memset (ptr, 0, size);
        }
      else
        {
          ptr = reinterpret_cast<component_t*> (reinterpret_cast<char*> (instance->parent->component) + instance->offset ());
        }
      instance->component = ptr;
    }
}

static void
bind (port_t** output_port, component_t* input_instance, const decl::Reaction* reaction, type::Int::ValueType parameter)
{
  port_t* port = new port_t (input_instance, reaction, parameter);
  port->next = *output_port;
  *output_port = port;
}

static heap_link_t*
make_heap_link (Heap* heap,
                Heap* allocator)
{
  // Allocate a new heap link in the parent.
  heap_link_t* hl = static_cast<heap_link_t*> (allocator->allocate (sizeof (heap_link_t)));
  // Set up the link.
  hl->heap = heap;
  pthread_mutex_init (&hl->mutex, NULL);
  return hl;
}

void
create_bindings (composition::Composer& instance_table)
{
  for (composition::Composer::PushPortsType::const_iterator pp_pos = instance_table.push_ports_begin (),
       pp_limit = instance_table.push_ports_end ();
       pp_pos != pp_limit;
       ++pp_pos)
    {
      composition::PushPort* pp = pp_pos->second;
      composition::Instance* output_instance = pp->instance;
      size_t output_port = pp->address - output_instance->address;

      for (composition::ReactionsType::const_iterator reaction_pos = pp->reactions.begin (),
           reaction_limit = pp->reactions.end ();
           reaction_pos != reaction_limit;
           ++reaction_pos)
        {
          composition::Reaction* r = *reaction_pos;
          bind (reinterpret_cast<port_t**> (reinterpret_cast<char*> (output_instance->component) + output_port),
                r->instance->component,
                r->reaction,
                r->iota);
        }
    }

  for (composition::Composer::PullPortsType::const_iterator pp_pos = instance_table.pull_ports_begin (),
       pp_limit = instance_table.pull_ports_end ();
       pp_pos != pp_limit;
       ++pp_pos)
    {
      composition::Instance* pull_port_instance = pp_pos->second->instance;
      size_t pull_port_address = pp_pos->first - pull_port_instance->address;
      composition::Getter* getter = *pp_pos->second->getters.begin ();
      pull_port_t* pull_port = reinterpret_cast<pull_port_t*> (reinterpret_cast<char*> (pull_port_instance->component) + pull_port_address);
      assert (getter->instance != NULL);
      pull_port->instance = getter->instance->component;
      pull_port->getter = getter->getter;
    }
}

void
initialize (executor_base_t& exec, composition::Instance* instance)
{
  if (instance->is_top_level ())
    {
      // Set up the heap.
      exec.current_instance (instance->component);
      // Call the initializer.
      instance->operation->execute (exec);
      // Clean up any return value.
      exec.stack ().popn (instance->initializer->return_size ());
    }
}

template <typename T>
static void
evaluate (executor_base_t& exec, const MemoryModel& memoryModel, const ast::Binary& node, const T& op)
{
  struct visitor : public type::DefaultVisitor
  {
    executor_base_t& exec;
    const MemoryModel& memoryModel;
    const ast::Binary& node;
    const T& op;

    visitor (executor_base_t& e,
             const MemoryModel& mm,
             const ast::Binary& n,
             const T& o) : exec (e), memoryModel (mm), node (n), op (o) { }

    void default_action (const type::Type& t)
    {
      TYPE_NOT_REACHED (t);
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

    void visit (const type::Int& t)
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
  const type::Type*
  dispatch_type (const ast::Binary& node) const
  {
    UNIMPLEMENTED;
    //return node.typed_value.type;
  }
};

struct LeftDispatch
{
  const type::Type*
  dispatch_type (const ast::Binary& node) const
  {
    UNIMPLEMENTED;
    //return node.left ()->typed_value.type;
  }
};

struct Divide : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int&) const
  {
    UNIMPLEMENTED;
    // evaluate_expression (exec, memoryModel, node.left ());
    // type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left / right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Float64&) const
  {
    UNIMPLEMENTED;
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
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

struct Modulus : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int&) const
  {
    UNIMPLEMENTED;
    // evaluate_expression (exec, memoryModel, node.left ());
    // type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left % right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

template <typename T>
struct LeftShiftVisitor : public type::DefaultVisitor
{
  executor_base_t& exec;
  const MemoryModel& memoryModel;
  const ast::Binary& node;

  LeftShiftVisitor (executor_base_t& e,
                    const MemoryModel& mm,
                    const ast::Binary& n)
    : exec (e)
    , memoryModel (mm)
    , node (n)
  { }

  void default_action (const type::Type& t)
  {
    TYPE_NOT_REACHED (t);
  }

  void visit (const NamedType& t)
  {
    t.UnderlyingType ()->Accept (*this);
  }

  void visit (const type::Int& t)
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
    UNIMPLEMENTED;
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
        const ast::Binary& node,
        const T&) const
  {
    LeftShiftVisitor<T> v (exec, memoryModel, node);
    UNIMPLEMENTED;
    //node.right ()->typed_value.type->Accept (v);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int& t) const
  {
    doit (exec, memoryModel, node, t);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Uint64& t) const
  {
    doit (exec, memoryModel, node, t);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

struct RightShift : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int&) const
  {
    struct visitor : public type::DefaultVisitor
    {
      executor_base_t& exec;
      const MemoryModel& memoryModel;
      const ast::Binary& node;

      visitor (executor_base_t& e,
               const MemoryModel& mm,
               const ast::Binary& n)
        : exec (e)
        , memoryModel (mm)
        , node (n)
      { }

      void default_action (const type::Type& t)
      {
        TYPE_NOT_REACHED (t);
      }

      void visit (const NamedType& t)
      {
        t.UnderlyingType ()->Accept (*this);
      }

      void visit (const type::Int&)
      {
        UNIMPLEMENTED;
        // evaluate_expression (exec, memoryModel, node.left ());
        // type::Int::ValueType left;
        // exec.stack ().pop (left);
        // evaluate_expression (exec, memoryModel, node.right ());
        // Uint::ValueType right;
        // exec.stack ().pop (right);
        // exec.stack ().push (left >> right);
      }

      void visit (const Uint&)
      {
        UNIMPLEMENTED;
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
    UNIMPLEMENTED;
    //node.right ()->typed_value.type->Accept (v);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

struct BitAnd : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int&) const
  {
    UNIMPLEMENTED;
    // evaluate_expression (exec, memoryModel, node.left ());
    // type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left & right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

struct BitAndNot : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int&) const
  {
    UNIMPLEMENTED;
    // evaluate_expression (exec, memoryModel, node.left ());
    // type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left & (~right));
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

struct Add : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Uint& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  template <typename T>
  void
  doit (executor_base_t& exec,
        const MemoryModel& memoryModel,
        const ast::Binary& node,
        const T&) const
  {
    UNIMPLEMENTED;
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
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

struct Subtract : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int&) const
  {
    UNIMPLEMENTED;
    // evaluate_expression (exec, memoryModel, node.left ());
    // type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left - right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

struct BitOr : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int&) const
  {
    UNIMPLEMENTED;
    // evaluate_expression (exec, memoryModel, node.left ());
    // type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left | right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

struct BitXor : public RetvalDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int&) const
  {
    UNIMPLEMENTED;
    // evaluate_expression (exec, memoryModel, node.left ());
    // type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left ^ right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

struct Equal : public LeftDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Bool& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Boolean& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Uint& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Uint8& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Pointer& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  template <typename T>
  void doit (executor_base_t& exec,
             const MemoryModel& memoryModel,
             const ast::Binary& node,
             const T&) const
  {
    UNIMPLEMENTED;
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
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

struct NotEqual : public LeftDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Bool&) const
  {
    doit<Bool> (exec, memoryModel, node);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int&) const
  {
    doit<type::Int> (exec, memoryModel, node);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Uint&) const
  {
    doit<Uint> (exec, memoryModel, node);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Uint64&) const
  {
    doit<Uint64> (exec, memoryModel, node);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Pointer&) const
  {
    doit<Pointer> (exec, memoryModel, node);
  }

  template <typename T>
  void
  doit (executor_base_t& exec, const MemoryModel& memoryModel, const ast::Binary& node) const
  {
    UNIMPLEMENTED;
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
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

struct LessThan : public LeftDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int&) const
  {
    UNIMPLEMENTED;
    // evaluate_expression (exec, memoryModel, node.left ());
    // type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left < right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Int8&) const
  {
    UNIMPLEMENTED;
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
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

struct LessEqual : public LeftDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int&) const
  {
    UNIMPLEMENTED;
    // evaluate_expression (exec, memoryModel, node.left ());
    // type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left <= right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

struct MoreThan : public LeftDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int&) const
  {
    UNIMPLEMENTED;
    // evaluate_expression (exec, memoryModel, node.left ());
    // type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left > right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

struct MoreEqual : public LeftDispatch
{
  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int&) const
  {
    UNIMPLEMENTED;
    // evaluate_expression (exec, memoryModel, node.left ());
    // type::Int::ValueType left;
    // exec.stack ().pop (left);
    // evaluate_expression (exec, memoryModel, node.right ());
    // type::Int::ValueType right;
    // exec.stack ().pop (right);
    // exec.stack ().push (left >= right);
  }

  void
  operator() (executor_base_t& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

bool
enabled (executor_base_t& exec,
         component_t* instance,
         const decl::Action* action,
         type::Int::ValueType iota)
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
         const decl::Action* action,
         component_t* instance)
{
  // Reset the mutable phase base pointer.
  exec.mutable_phase_base_pointer (0);

  exec.stack ().setup (action->memory_model.locals_size ());

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

bool execute (executor_base_t& exec, component_t* instance, const decl::Action* action, type::Int::ValueType iota)
{
  if (enabled (exec, instance, action, iota))
    {
      return execute_no_check (exec, instance, action, iota);
    }

  return false;
}

#ifdef PROFILE_LATENCY
FILE* latency_file;
#endif

bool execute_no_check (executor_base_t& exec, component_t* instance, const decl::Action* action, type::Int::ValueType iota)
{
  assert (exec.stack ().empty ());

#ifdef PROFILE_LATENCY
  {
    struct timespec ts;
    clock_gettime (CLOCK_MONOTONIC, &ts);
    fprintf (latency_file, "%s %ld.%09ld\n", action->name.c_str (), ts.tv_sec, ts.tv_nsec);
  }
#endif

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
  NewImpl (const type::Type* t, const util::Location& loc)
    : type_ (t)
    , function_type_ (makeFunctionType (t, loc))
  {
    allocate_parameters (memory_model, function_type_->GetSignature ());
    allocate_symbol (memory_model, function_type_->GetReturnParameter ());
  }

  virtual void call (executor_base_t& exec) const
  {
    // Allocate a new instance of the type.
    const type::Heap* heap_type = type_cast<type::Heap> (type_);
    if (heap_type == NULL)
      {
        char** r = static_cast<char**> (exec.stack ().get_address (function_type_->GetReturnParameter ()->offset ()));
        *r = static_cast<char*> (exec.heap ()->allocate (type_->Size ()));
      }
    else
      {
        const type::Type* t = heap_type->Base ();
        // Allocate a new heap and root object.
        Heap* h = new Heap (t->Size ());
        // Insert it into its parent.
        Heap* h2 = exec.heap ();
        h2->insert_child (h);
        // Allocate a new heap link in the parent.
        heap_link_t* hl = make_heap_link (h, h2);
        heap_link_t** r = static_cast<heap_link_t**> (exec.stack ().get_address (function_type_->GetReturnParameter ()->offset ()));
        *r = hl;
      }
  }

  virtual const type::Type* type () const
  {
    return function_type_;
  }
  const type::Type* const type_;
  const type::Function* const function_type_;
  MemoryModel memory_model;
  static const type::Function* makeFunctionType (const type::Type* type, const util::Location& loc)
  {
    const type::Type* return_type = type->get_pointer ();
    return new type::Function (type::Function::FUNCTION, (new Signature ()),
                               ParameterSymbol::makeReturn (loc, ReturnSymbol, return_type, Mutable));
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
  virtual const type::Signature* signature () const
  {
    NOT_REACHED;
  }
  virtual void check_types (ast::List* args) const
  {
    // Do nothing.
  }
  virtual void check_references (ast::List* args) const
  {
    semantic::require_type (args->at (0));
  }
  virtual void check_mutability (ast::List* args) const
  {
    // Do nothing.
  }
  virtual void compute_receiver_access (ast::List* args, ReceiverAccess& receiver_access, bool& flag) const
  {
    // Do nothing.
  }

};

New::New (const util::Location& loc)
  : Template ("new",
              loc,
              new type::Template ())
{ }

Callable*
New::instantiate (ErrorReporter& er,
                  const std::vector<const type::Type*>& argument_types) const
{
  const type::Type* type;
  if (argument_types.size () == 1)
    {
      type = argument_types.front ();
    }
  else
    {
      type = type::Error::Instance ();
      er.func_expects_count (location, "new", 1, argument_types.size ());
    }
  return new NewImpl (type, location);
}

struct MoveImpl : public Callable
{
  MoveImpl (const type::Type* in, const type::Type* out, const util::Location& loc)
    : function_type_ (makeFunctionType (in, out, loc))
  {
    allocate_parameters (memory_model, function_type_->GetSignature ());
    allocate_symbol (memory_model, function_type_->GetReturnParameter ());
  }

  virtual void call (executor_base_t& exec) const
  {
    heap_link_t** r = static_cast<heap_link_t**> (exec.stack ().get_address (function_type_->GetReturnParameter ()->offset ()));
    ParameterSymbol* p = *function_type_->GetSignature ()->begin ();
    heap_link_t* hl = static_cast<heap_link_t*> (exec.stack ().read_pointer (p->offset ()));
    if (hl != NULL)
      {
        pthread_mutex_lock (&hl->mutex);
        if (hl->heap != NULL && hl->change_count == 0)
          {
            // Break the link.
            Heap* h = hl->heap;
            hl->heap = NULL;
            pthread_mutex_unlock (&hl->mutex);

            // Remove from parent.
            h->remove_from_parent ();
            // Insert into the new parent.
            Heap* h2 = exec.heap ();
            h2->insert_child (h);

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

  virtual const type::Type* type () const
  {
    return function_type_;
  }
  const type::Function* const function_type_;
  MemoryModel memory_model;
  static const type::Function* makeFunctionType (const type::Type* in, const type::Type* out, const util::Location& loc)
  {
    // TODO:  The mutabilities may need to be adjusted.
    return new type::Function (type::Function::FUNCTION, (new Signature ())
                               ->Append (ParameterSymbol::make (loc, "h", in, Mutable, Foreign)),
                               ParameterSymbol::makeReturn (loc, ReturnSymbol, out, Mutable));
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
  virtual const type::Signature* signature () const
  {
    return function_type_->GetSignature ();
  }
};

Move::Move (const util::Location& loc)
  : Template ("move",
              loc,
              new type::Template ())
{ }

Callable*
Move::instantiate (util::ErrorReporter& er,
                   const std::vector<const type::Type*>& argument_types) const
{
  const type::Type* in;

  if (argument_types.size () == 1)
    {
      in = argument_types.front ();
    }
  else
    {
      in = type::Error::Instance ();
      er.func_expects_count (location, "move", 1, argument_types.size ());
    }

  const type::Type* out = in->move ();
  if (in->underlying_kind () != kError &&
      out->underlying_kind () == kError)
    {
      er.cannot_be_applied (location, "move", in);
    }

  return new MoveImpl (in, out, location);
}

struct MergeImpl : public Callable
{
  MergeImpl (const type::Type* in, const type::Type* out, const util::Location& loc)
    : function_type_ (makeFunctionType (in, out, loc))
  {
    allocate_parameters (memory_model, function_type_->GetSignature ());
    allocate_symbol (memory_model, function_type_->GetReturnParameter ());
  }

  virtual void call (executor_base_t& exec) const
  {
    char** r = static_cast<char**> (exec.stack ().get_address (function_type_->GetReturnParameter ()->offset ()));
    ParameterSymbol* p = *function_type_->GetSignature ()->begin ();
    heap_link_t* hl = static_cast<heap_link_t*> (exec.stack ().read_pointer (p->offset ()));
    if (hl != NULL)
      {
        pthread_mutex_lock (&hl->mutex);
        if (hl->heap != NULL && hl->change_count == 0)
          {
            // Break the link.
            Heap* h = hl->heap;
            hl->heap = NULL;
            pthread_mutex_unlock (&hl->mutex);

            // Get the heap root.
            char* root = static_cast<char*> (h->root ());

            // Remove from parent.
            h->remove_from_parent ();

            // Merge into the new parent.
            exec.heap ()->merge (h);

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

  virtual const type::Type* type () const
  {
    return function_type_;
  }
  const type::Function* const function_type_;
  MemoryModel memory_model;
  static const type::Function* makeFunctionType (const type::Type* in, const type::Type* out, const util::Location& loc)
  {
    // TODO:  Adjust mutability.
    return new type::Function (type::Function::FUNCTION, (new Signature ())
                               ->Append (ParameterSymbol::make (loc, "h", in, Mutable, Foreign)),
                               ParameterSymbol::makeReturn (loc, ReturnSymbol, out, Mutable));
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
  virtual const type::Signature* signature () const
  {
    return function_type_->GetSignature ();
  }
};

Merge::Merge (const util::Location& loc)
  : Template ("merge",
              loc,
              new type::Template ())
{ }

Callable*
Merge::instantiate (util::ErrorReporter& er,
                    const std::vector<const type::Type*>& argument_types) const
{
  const type::Type* in;

  if (argument_types.size () == 1)
    {
      in = argument_types.front ();
    }
  else
    {
      in = type::Error::Instance ();
      er.func_expects_count (location, "merge", 1, argument_types.size ());
    }

  const type::Type* out = in->merge_change ();
  if (in->underlying_kind () != kError &&
      out->underlying_kind () == kError)
    {
      er.cannot_be_applied (location, "merge", in);
    }

  return new MergeImpl (in, out, location);
}

struct LenImpl : public Callable
{
  LenImpl (const type::Type* type, const util::Location& loc)
    : function_type_ (makeFunctionType (type, loc))
  {
    allocate_parameters (memory_model, function_type_->GetSignature ());
    allocate_symbol (memory_model, function_type_->GetReturnParameter ());
  }

  virtual void call (executor_base_t& exec) const
  {
    Int::ValueType* retval = static_cast<Int::ValueType*> (exec.stack ().get_address (function_type_->GetReturnParameter ()->offset ()));
    Slice::ValueType* slice = static_cast<Slice::ValueType*> (exec.stack ().get_address (function_type_->GetSignature ()->At (0)->offset ()));
    *retval = slice->length;
  }

  virtual const type::Type* type () const
  {
    return function_type_;
  }
  const type::Function* const function_type_;
  MemoryModel memory_model;
  static const type::Function* makeFunctionType (const type::Type* type,
      const util::Location& loc)
  {
    return new type::Function (type::Function::FUNCTION, (new Signature ())
                               ->Append (ParameterSymbol::make (loc, "s", type, Foreign, Foreign)),
                               ParameterSymbol::makeReturn (loc, ReturnSymbol, &named_int, Immutable));
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
  virtual const type::Signature* signature () const
  {
    return function_type_->GetSignature ();
  }
};

Len::Len (const util::Location& loc)
  : Template ("len",
              loc,
              new type::Template ())
{ }

Callable*
Len::instantiate (util::ErrorReporter& er,
                  const std::vector<const type::Type*>& argument_types) const
{
  const type::Type* type;

  if (argument_types.size () == 1)
    {
      type = argument_types.front ();
    }
  else
    {
      type = type::Error::Instance ();
      er.func_expects_count (location, "len", 1, argument_types.size ());
    }

  if (type->underlying_kind () != kError &&
      type->underlying_kind () != kSlice)
    {
      er.cannot_be_applied (location, "[:]", type);
    }

  return new LenImpl (type, location);
}

struct AppendImpl : public Callable
{
  AppendImpl (const type::Slice* slice_type,
              const type::Type* element_type, const util::Location& loc)
    : function_type_ (makeFunctionType (slice_type, element_type, loc))
    , unit_size_ (slice_type->UnitSize ())
  {
    allocate_parameters (memory_model, function_type_->GetSignature ());
    allocate_symbol (memory_model, function_type_->GetReturnParameter ());
  }

  virtual void call (executor_base_t& exec) const
  {
    Slice::ValueType* retval = static_cast<Slice::ValueType*> (exec.stack ().get_address (function_type_->GetReturnParameter ()->offset ()));
    Slice::ValueType* slice = static_cast<Slice::ValueType*> (exec.stack ().get_address (function_type_->GetSignature ()->At (0)->offset ()));
    const void* el = exec.stack ().get_address (function_type_->GetSignature ()->At (1)->offset ());

    const Uint::ValueType new_length = slice->length + 1;
    if (new_length > slice->capacity)
      {
        const Uint::ValueType new_capacity = 2 * new_length;
        void* ptr = exec.heap ()->allocate (new_capacity * unit_size_);
        memcpy (ptr, slice->ptr, slice->length * unit_size_);
        slice->ptr = ptr;
        slice->capacity = new_capacity;
      }
    memcpy (static_cast<char*> (slice->ptr) + slice->length * unit_size_, el, unit_size_);
    slice->length = new_length;
    *retval = *slice;
  }

  virtual const type::Type* type () const
  {
    return function_type_;
  }
  const type::Function* const function_type_;
  size_t const unit_size_;
  MemoryModel memory_model;
  static const type::Function* makeFunctionType (const type::Type* slice_type,
      const type::Type* element_type,
      const util::Location& loc)
  {
    return new type::Function (type::Function::FUNCTION, (new Signature ())
                               ->Append (ParameterSymbol::make (loc, "s", slice_type, Mutable, Mutable))
                               ->Append (ParameterSymbol::make (loc, "x", element_type, Immutable, Mutable)),
                               ParameterSymbol::makeReturn (loc, ReturnSymbol, slice_type, Mutable));
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
  virtual const type::Signature* signature () const
  {
    return function_type_->GetSignature ();
  }
};

Append::Append (const util::Location& loc)
  : Template ("append",
              loc,
              new type::Template ())
{ }

Callable*
Append::instantiate (util::ErrorReporter& er,
                     const std::vector<const type::Type*>& argument_types) const
{
  const type::Type* slice_type;
  const type::Type* element_type;

  if (argument_types.size () == 2)
    {
      slice_type = argument_types[0];
      element_type = argument_types[1];
    }
  else
    {
      slice_type = type::Error::Instance ();
      slice_type = type::Error::Instance ();
      er.func_expects_count (location, "append", 2, argument_types.size ());
    }

  if (slice_type->underlying_kind () != kError &&
      slice_type->underlying_kind () != kSlice)
    {
      er.cannot_be_applied (location, "append", slice_type);
    }

  const type::Slice* st = type_cast<type::Slice> (slice_type->UnderlyingType ());
  if (st != NULL &&
      element_type->underlying_kind () != kError &&
      !are_identical (st->Base (), element_type))
    {
      er.func_expects_arg (location, "append", 2, st->Base (), element_type);
    }

  return new AppendImpl (st, element_type, location);
}

struct CopyImpl : public Callable
{
  CopyImpl (const type::Type* in, const util::Location& loc)
    : function_type_ (makeFunctionType (in, loc))
  {
    allocate_parameters (memory_model, function_type_->GetSignature ());
    allocate_symbol (memory_model, function_type_->GetReturnParameter ());
  }

  virtual void call (executor_base_t& exec) const
  {
    const Slice* slice_type = type_strip_cast<Slice>(function_type_->GetReturnParameter ()->type);
    if (slice_type != NULL)
      {
        Slice::ValueType* in = static_cast<Slice::ValueType*> (exec.stack ().get_address (function_type_->GetSignature ()->At (0)->offset ()));
        Slice::ValueType* out = static_cast<Slice::ValueType*> (exec.stack ().get_address (function_type_->GetReturnParameter ()->offset ()));
        size_t sz = slice_type->UnitSize () * in->length;
        out->ptr = exec.heap ()->allocate (sz);
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
        out->ptr = exec.heap ()->allocate (in->length);
        memcpy (out->ptr, in->ptr, in->length);
        out->length = in->length;
        return;
      }

    void* in = exec.stack ().get_address (function_type_->GetSignature ()->At (0)->offset ());
    void* out = exec.stack ().get_address (function_type_->GetReturnParameter ()->offset ());
    memcpy (out, in, function_type_->GetReturnType ()->Size ());
  }

  virtual const type::Type* type () const
  {
    return function_type_;
  }
  const type::Function* const function_type_;
  MemoryModel memory_model;
  static const type::Function* makeFunctionType (const type::Type* in, const util::Location& loc)
  {
    return new type::Function (type::Function::FUNCTION, (new Signature ())
                               ->Append (ParameterSymbol::make (loc, "h", in, Immutable, Foreign)),
                               ParameterSymbol::makeReturn (loc, ReturnSymbol, in, Mutable));
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
  virtual const type::Signature* signature () const
  {
    return function_type_->GetSignature ();
  }
};

Copy::Copy (const util::Location& loc)
  : Template ("copy",
              loc,
              new type::Template ())
{ }

Callable*
Copy::instantiate (util::ErrorReporter& er,
                   const std::vector<const type::Type*>& argument_types) const
{
  if (argument_types.size () != 1)
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "copy expects one argument (E123)");
    }

  const type::Type* in = argument_types.front ();

  if (type_strip_cast<Component> (in) != NULL)
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "cannot copy components (E94)");
    }

  {
    const Slice* st = type_strip_cast<Slice> (in);
    if (st != NULL)
      {
        if (type_contains_pointer (st->Base ()))
          {
            error_at_line (-1, 0, location.File.c_str (), location.Line,
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

  return new CopyImpl (in, location);
}

struct PrintlnImpl : public Callable
{
  PrintlnImpl (const util::Location& loc, const TypeList& type_list)
    : function_type_ (makeFunctionType (loc, type_list))
  {
    allocate_parameters (memory_model, function_type_->GetSignature ());
    allocate_symbol (memory_model, function_type_->GetReturnParameter ());
  }

  virtual void call (executor_base_t& exec) const
  {
    struct visitor : public type::DefaultVisitor
    {
      executor_base_t& exec;
      void* ptr;

      visitor (executor_base_t& e, void* p) : exec (e), ptr (p) { }

      void default_action (const type::Type& type)
      {
        TYPE_NOT_REACHED (type);
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

      void visit (const Uint32& type)
      {
        printf ("%u", *static_cast<Uint32::ValueType*> (ptr));
      }

      void visit (const Uint64& type)
      {
        printf ("%lu", *static_cast<Uint64::ValueType*> (ptr));
      }

      void visit (const type::Int& type)
      {
        printf ("%ld", *static_cast<Int::ValueType*> (ptr));
      }

      void visit (const Int8& type)
      {
        printf ("%d", *static_cast<Int8::ValueType*> (ptr));
      }

      void visit (const Int64& type)
      {
        printf ("%ld", *static_cast<Int64::ValueType*> (ptr));
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
    for (type::Signature::const_iterator pos = function_type_->GetSignature ()->begin (), limit = function_type_->GetSignature ()->end ();
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

  virtual const type::Type* type () const
  {
    return function_type_;
  }
  const type::Function* const function_type_;
  MemoryModel memory_model;

  static const type::Function* makeFunctionType (const util::Location& loc, const TypeList& argument_types)
  {
    Signature* sig = new Signature ();
    for (TypeList::const_iterator pos = argument_types.begin (), limit = argument_types.end ();
         pos != limit;
         ++pos)
      {
        const type::Type* t = *pos;
        t = t->DefaultType ();
        sig->Append (ParameterSymbol::make (loc, "", t, Immutable, Foreign));
      }

    return new type::Function (type::Function::FUNCTION, sig,
                               ParameterSymbol::makeReturn (loc, ReturnSymbol, Void::Instance (), Foreign));

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
  virtual const type::Signature* signature () const
  {
    return function_type_->GetSignature ();
  }
};

Println::Println (const util::Location& loc)
  : Template ("println",
              loc,
              new type::Template ())
{ }

Callable*
Println::instantiate (util::ErrorReporter& er,
                      const TypeList& argument_types) const
{
  return new PrintlnImpl (location, argument_types);
}

OpReturn
Load::execute (executor_base_t& exec) const
{
  child->execute (exec);
  void* ptr = exec.stack ().pop_pointer ();
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
  type::Int::ValueType i;
  exec.stack ().pop (i);

  if (i < 0 || static_cast<type::Uint::ValueType> (i) >= s.length)
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
    type::Int::ValueType out = in;
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

//   void NotIntegral (const type::Type& type)
//   {
//     NOT_REACHED;
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
//   void operator() (const type::Int& type)
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
MakeConvertToInt (const Operation* c, const type::Type* type)
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
      TYPE_NOT_REACHED (*type);
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
    type::Uint::ValueType out = in;
    exec.stack ().push (out);
    return make_continue ();
  }
  virtual void dump () const
  {
    std::cout << "ConvertToUint(";
    child->dump ();
    std::cout << ")";
  }
  const Operation* const child;
};

Operation*
MakeConvertToUint (const Operation* c, const type::Type* type)
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
      TYPE_NOT_REACHED (*type);
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

struct MakeLiteralVisitor : public type::DefaultVisitor
{
  const Value& value;
  Operation* op;

  MakeLiteralVisitor (const Value& v) : value (v), op (NULL) { }

  void default_action (const type::Type& type)
  {
    TYPE_NOT_REACHED (type);
  }

  void visit (const Bool& type)
  {
    op = make_literal (value.bool_value);
  }

  void visit (const Uint8& type)
  {
    op = make_literal (value.uint8_value);
  }

  void visit (const Uint16& type)
  {
    op = make_literal (value.uint16_value);
  }

  void visit (const Uint32& type)
  {
    op = make_literal (value.uint32_value);
  }

  void visit (const Uint64& type)
  {
    op = make_literal (value.uint64_value);
  }

  void visit (const Int8& type)
  {
    op = make_literal (value.int8_value);
  }

  void visit (const Int16& type)
  {
    op = make_literal (value.int16_value);
  }

  void visit (const Int32& type)
  {
    op = make_literal (value.int32_value);
  }

  void visit (const Int64& type)
  {
    op = make_literal (value.int64_value);
  }

  void visit (const Uint& type)
  {
    op = make_literal (value.uint_value);
  }

  void visit (const Int& type)
  {
    op = make_literal (value.int_value);
  }

  void visit (const Float64& type)
  {
    op = make_literal (value.float64_value);
  }

  void visit (const StringU& type)
  {
    op = make_literal (value.stringu_value);
  }

  void visit (const Pointer& type)
  {
    op = make_literal (value.pointer_value);
  }

  void visit (const Slice& type)
  {
    op = make_literal (value.slice_value);
  }
};

Operation* make_literal (const type::Type* type, const Value& value)
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
    case type::Function::FUNCTION:
      UNIMPLEMENTED;
    case type::Function::PUSH_PORT:
      UNIMPLEMENTED;
    case type::Function::PULL_PORT:
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

  NOT_REACHED;
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
    UNIMPLEMENTED;
  }
  Operation* const left;
  Operation* const right;
};

Operation* make_add_assign (Operation* l, Operation* r, const type::Type* t)
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
      NOT_REACHED;
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
  type::Int::ValueType i;
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
  Int::ValueType low_val = 0;
  if (low)
    {
      low->execute (exec);
      exec.stack ().pop (low_val);
    }
  Int::ValueType high_val = type->dimension;
  if (high)
    {
      high->execute (exec);
      exec.stack ().pop (high_val);
    }
  Int::ValueType max_val = type->dimension;
  if (max)
    {
      max->execute (exec);
      exec.stack ().pop (max_val);
    }

  // Bounds check.
  if (!(0 <= low_val &&
        low_val <= high_val &&
        high_val <= max_val &&
        max_val <= type->dimension))
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "slice index is out of range (E223)");
    }

  Slice::ValueType slice_val;
  slice_val.length = high_val - low_val;
  slice_val.capacity = max_val - low_val;
  slice_val.ptr = slice_val.length ? ptr + low_val * type->UnitSize () : NULL;
  exec.stack ().push (slice_val);

  return make_continue ();
}

OpReturn
SliceSlice::execute (executor_base_t& exec) const
{
  base->execute (exec);
  Slice::ValueType s;
  exec.stack ().pop (s);
  Int::ValueType low_val = 0;
  if (low)
    {
      low->execute (exec);
      exec.stack ().pop (low_val);
    }
  Int::ValueType high_val = s.length;
  if (high)
    {
      high->execute (exec);
      exec.stack ().pop (high_val);
    }
  Int::ValueType max_val = s.capacity;
  if (max)
    {
      max->execute (exec);
      exec.stack ().pop (max_val);
    }

  // Bounds check.
  if (!(0 <= low_val &&
        low_val <= high_val &&
        high_val <= max_val &&
        max_val <= static_cast<Int::ValueType> (s.capacity)))
    {
      error_at_line (-1, 0, location.File.c_str (), location.Line,
                     "slice index is out of range (E22)");
    }

  s.length = high_val - low_val;
  s.capacity = max_val - low_val;
  s.ptr = s.length ? static_cast<char*> (s.ptr) + low_val * type->UnitSize () : NULL;
  exec.stack ().push (s);

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
      UNIMPLEMENTED;
    }
  pthread_mutex_lock (&hl->mutex);
  ++hl->change_count;
  pthread_mutex_unlock (&hl->mutex);

  // Save the old heap.
  Heap* old_heap = exec.heap ();
  // Set the the new heap.
  exec.heap (hl->heap);

  char** root_value = static_cast<char**> (exec.stack ().get_address (root_offset));

  // Push a pointer to the root object.
  *root_value = static_cast<char*> (hl->heap->root ());

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
    UNIMPLEMENTED;
  }
  Operation* const child;
};

Operation* make_increment (Operation* child, const type::Type* type)
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
      TYPE_NOT_REACHED (*type);
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
          exec.stack ().push<type::Int::ValueType> (port->parameter);
        }
      // Push the arguments.
      exec.stack ().load (top_before, arguments_size);
      // Push an instruction pointer.
      exec.stack ().push_pointer (NULL);

      // Jump to the last frame.
      exec.stack ().base_pointer (exec.mutable_phase_base_pointer ());

      exec.stack ().setup (port->reaction->memory_model.locals_size ());

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
    out.ptr = exec.heap ()->allocate (in.length);
    memcpy (out.ptr, in.ptr, in.length);
    out.length = in.length;
    out.capacity = in.length;
    exec.stack ().push (out);
    return make_continue ();
  }

  virtual void dump () const
  {
    UNIMPLEMENTED;
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
    out.ptr = exec.heap ()->allocate (in.length);
    memcpy (out.ptr, in.ptr, in.length);
    out.length = in.length;
    exec.stack ().push (out);
    return make_continue ();
  }

  virtual void dump () const
  {
    std::cout << "ConvertSliceOfBytesToString(";
    child->dump ();
    std::cout << ")";
  }
  Operation* const child;
};

template<typename FromType, typename ToType>
struct Conversion : public Operation
{
  Conversion (Operation* c) : child (c) { }
  virtual OpReturn execute (executor_base_t& exec) const
  {
    child->execute (exec);
    FromType x;
    exec.stack ().pop (x);
    ToType y = x;
    exec.stack ().push (y);
    return kContinue;
  }
  virtual void dump () const
  {
    std::cout << "Conversion(";
    child->dump ();
    std::cout << ")";
  }
  Operation* const child;
};


template <typename T1>
static Operation* make_conversion1 (Operation* c, const type::Type* to)
{
  switch (to->underlying_kind ())
    {
    case kUint8:
      return new Conversion<T1, Uint8::ValueType> (c);
    case kUint16:
      return new Conversion<T1, Uint16::ValueType> (c);
    case kUint32:
      return new Conversion<T1, Uint32::ValueType> (c);
    case kUint64:
      return new Conversion<T1, Uint64::ValueType> (c);
    case kInt8:
      return new Conversion<T1, Int8::ValueType> (c);
    case kInt16:
      return new Conversion<T1, Int16::ValueType> (c);
    case kInt32:
      return new Conversion<T1, Int32::ValueType> (c);
    case kInt64:
      return new Conversion<T1, Int64::ValueType> (c);
    case kFloat32:
      return new Conversion<T1, Float32::ValueType> (c);
    case kFloat64:
      return new Conversion<T1, Float64::ValueType> (c);
    case kUint:
      return new Conversion<T1, Uint::ValueType> (c);
    case kInt:
      return new Conversion<T1, Int::ValueType> (c);
    default:
      break;
    }
  NOT_REACHED;
}

Operation* make_conversion (Operation* c, const type::Type* from, const type::Type* to)
{
  if (are_identical (from->UnderlyingType (), to->UnderlyingType ()))
    {
      return c;
    }
  else if (from->Level () == type::Type::UNNAMED &&
           to->Level () == type::Type::UNNAMED &&
           from->underlying_kind () == kPointer &&
           to->underlying_kind () == kPointer &&
           are_identical (from->pointer_base_type (), to->pointer_base_type ()))
    {
      UNIMPLEMENTED;
    }
  else if ((is_typed_integer (from) || is_typed_float (from)) &&
           (is_typed_integer (to) || is_typed_float (to)))
    {
      switch (from->underlying_kind ())
        {
        case kUint8:
          return make_conversion1<Uint8::ValueType> (c, to);
        case kUint16:
          return make_conversion1<Uint16::ValueType> (c, to);
        case kUint32:
          return make_conversion1<Uint32::ValueType> (c, to);
        case kUint64:
          return make_conversion1<Uint64::ValueType> (c, to);
        case kInt8:
          return make_conversion1<Int8::ValueType> (c, to);
        case kInt16:
          return make_conversion1<Int16::ValueType> (c, to);
        case kInt32:
          return make_conversion1<Int32::ValueType> (c, to);
        case kInt64:
          return make_conversion1<Int64::ValueType> (c, to);
        case kFloat32:
          return make_conversion1<Float32::ValueType> (c, to);
        case kFloat64:
          return make_conversion1<Float64::ValueType> (c, to);
        case kUint:
          return make_conversion1<Uint::ValueType> (c, to);
        case kInt:
          return make_conversion1<Int::ValueType> (c, to);
        default:
          break;
        }
      NOT_REACHED;
    }
  else if (is_typed_complex (from) && is_typed_complex (to))
    {
      UNIMPLEMENTED;
    }
  else if (is_typed_integer (from) && is_typed_string (to))
    {
      UNIMPLEMENTED;
    }
  else if (is_slice_of_bytes (from) && is_typed_string (to))
    {
      return new ConvertSliceOfBytesToString (c);
    }
  else if (is_slice_of_runes (from) && is_typed_string (to))
    {
      UNIMPLEMENTED;
    }
  else if (is_typed_string (from) && is_slice_of_bytes (to))
    {
      return new ConvertStringToSliceOfBytes (c);
    }
  else if (is_typed_string (from) && is_slice_of_runes (to))
    {
      UNIMPLEMENTED;
    }
  else
    {
      // Assignable.
      UNIMPLEMENTED;
    }
}

OpReturn
Popn::execute (executor_base_t& exec) const
{
  OpReturn r = child->execute (exec);
  exec.stack ().popn (size);
  return r;
}
}

// void
// dump_instances (const runtime_t* runtime)
// {
//   for (Composer::Instancestype::const_iterator pos = runtime->instance_table.instances.begin (),
//          limit = runtime->instance_table.instances.end ();
//        pos != limit;
//        ++pos)
//     {
//       Instance* instance = pos->second;
//       if (instance->is_top_level ())
//         {
//           UNIMPLEMENTED;
//           //void* ptr = instance_record_get_ptr (instance_get_record (instance));
//           //type_print_value (instance_type (instance), ptr);
//           printf ("\n");
//         }
//     }
// }
