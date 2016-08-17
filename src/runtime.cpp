#include "runtime.hpp"

#include "node.hpp"
#include "node_visitor.hpp"
#include "callable.hpp"
#include "symbol_visitor.hpp"
#include "types.hpp"
#include "field.hpp"
#include "composition.hpp"
#include "heap.hpp"
#include "executor_base.hpp"
#include "semantic.hpp"
#include "check_types.hpp"
#include "error_reporter.hpp"
#include "parameter_list.hpp"
#include "action.hpp"
#include "operation.hpp"

namespace runtime
{
using namespace util;
using namespace type;
using namespace ast;
using namespace decl;
using namespace semantic;

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
          size_t size = arch::size (type);
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
bind (PushPort** output_port, component_t* input_instance, const decl::Reaction* reaction, long parameter)
{
  PushPort* port = new PushPort;
  port->instance = input_instance;
  port->reaction = reaction;
  port->parameter = parameter;
  port->next = *output_port;
  *output_port = port;
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
          bind (reinterpret_cast<PushPort**> (reinterpret_cast<char*> (output_instance->component) + output_port),
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
      PullPort* pull_port = reinterpret_cast<PullPort*> (reinterpret_cast<char*> (pull_port_instance->component) + pull_port_address);
      assert (getter->instance != NULL);
      pull_port->instance = getter->instance->component;
      pull_port->getter = getter->getter;
    }
}

void
initialize (ExecutorBase& exec, ComponentInfoBase* info)
{
  if (info->instance ()->is_top_level ())
    {
      // Set up the heap.
      exec.current_info (info);
      // Call the initializer.
      info->instance ()->operation->execute (exec);
      // Clean up any return value.
      exec.stack ().popn (arch::size_on_stack (info->instance ()->initializer->return_parameter_list ()));
    }
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

struct BitAnd : public RetvalDispatch
{
  void
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (ExecutorBase& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Uint& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  template <typename T>
  void
  doit (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Bool& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (ExecutorBase& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const UntypedBoolean& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (ExecutorBase& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (ExecutorBase& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Uint& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (ExecutorBase& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Uint8& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  void
  operator() (ExecutorBase& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Pointer& type) const
  {
    doit (exec, memoryModel, node, type);
  }

  template <typename T>
  void doit (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Bool&) const
  {
    doit<Bool> (exec, memoryModel, node);
  }

  void
  operator() (ExecutorBase& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Int&) const
  {
    doit<type::Int> (exec, memoryModel, node);
  }

  void
  operator() (ExecutorBase& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Uint&) const
  {
    doit<Uint> (exec, memoryModel, node);
  }

  void
  operator() (ExecutorBase& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const Uint64&) const
  {
    doit<Uint64> (exec, memoryModel, node);
  }

  void
  operator() (ExecutorBase& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Pointer&) const
  {
    doit<type::Pointer> (exec, memoryModel, node);
  }

  template <typename T>
  void
  doit (ExecutorBase& exec, const MemoryModel& memoryModel, const ast::Binary& node) const
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
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
  operator() (ExecutorBase& exec,
              const MemoryModel& memoryModel,
              const ast::Binary& node,
              const type::Type& t) const
  {
    TYPE_NOT_REACHED (t);
  }
};

bool
enabled (ExecutorBase& exec,
         component_t* instance,
         const decl::Action* action,
         long iota)
{
  assert (exec.stack ().empty ());

  // Push iota.
  if (action->has_dimension ())
    {
      exec.stack ().push<unsigned long> (iota);
    }
  // Push receiver.
  exec.stack ().push_pointer (instance);
  // Push an instruction pointer.
  exec.stack ().push_pointer (NULL);
  exec.stack ().setup (0 /* No locals. */);
  action->precondition->operation->execute (exec);
  bool retval;
  exec.stack ().pop (retval);
  exec.stack ().teardown ();
  // Pop the instruction pointer.
  exec.stack ().pop_pointer ();
  // Pop this.
  exec.stack ().pop_pointer ();
  // Pop iota.
  if (action->has_dimension ())
    {
      unsigned long x;
      exec.stack ().pop (x);
    }
  assert (exec.stack ().empty ());
  return retval;
}

static void
execute (ExecutorBase& exec,
         const decl::Action* action,
         component_t* instance)
{
  // Reset the mutable phase base pointer.
  exec.mutable_phase_base_pointer (0);

  exec.stack ().setup (action->memory_model.locals_size_on_stack ());

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

void execute_no_check (ExecutorBase& exec, component_t* instance, const decl::Action* action, long iota)
{
  assert (exec.stack ().empty ());

  // Push iota.
  if (action->has_dimension ())
    {
      exec.stack ().push<unsigned long> (iota);
    }
  // Push the instance.
  exec.stack ().push_pointer (instance);
  // Push the instruction pointer.
  exec.stack ().push_pointer (NULL);
  // Execute.
  runtime::execute (exec, action, instance);
  // Pop the instruction pointer.
  exec.stack ().pop_pointer ();
  // Pop the instance.
  exec.stack ().pop_pointer ();
  // Pop iota.
  if (action->has_dimension ())
    {
      unsigned long x;
      exec.stack ().pop (x);
    }

  assert (exec.stack ().empty ());
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
