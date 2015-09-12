#include "runtime.hpp"
#include "Callable.hpp"

namespace runtime
{
  struct port_t {
    component_t* instance;
    const reaction_t* reaction;
    int_type_t::ValueType parameter;
    port_t* next;

    port_t (component_t* i, const reaction_t* r, int_type_t::ValueType p) : instance (i), reaction (r), parameter (p), next (NULL) { }
  };

  struct heap_link_t {
    heap_t* heap;
    pthread_mutex_t mutex;
    size_t change_count;
  };

  ControlAction
  evaluate_statement (executor_base_t& exec,
                      ast_t* node);

  static void
  execute (executor_base_t& exec,
           const reaction_t* reaction,
           component_t* instance);

  void
  allocate_instances (instance_table_t& instance_table)
  {
    for (instance_table_t::InstancesType::const_iterator pos = instance_table.instances.begin (),
           limit = instance_table.instances.end ();
         pos != limit;
         ++pos)
      {
        instance_t* instance = pos->second;
        component_t* ptr;
        if (instance->is_top_level ())
          {
            const type_t* type = instance->type ();
            size_t size = type->size ();
            ptr = static_cast<component_t*> (malloc (size));
            memset (ptr, 0, size);
          }
        else
          {
            ptr = reinterpret_cast<component_t*> (reinterpret_cast<char*> (instance->parent ()->ptr ()) + instance->offset ());
          }
        instance->ptr (ptr);
      }
  }

  static void
  bind (port_t** output_port, component_t* input_instance, const reaction_t* reaction, int_type_t::ValueType parameter)
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
  create_bindings (instance_table_t& instance_table)
  {
    for (instance_table_t::PortsType::const_iterator output_pos = instance_table.push_ports.begin (),
           output_limit = instance_table.push_ports.end ();
         output_pos != output_limit;
         ++output_pos)
      {
        instance_t* output_instance = output_pos->second.output_instance;
        size_t output_port = output_pos->first - output_instance->address ();

        for (instance_table_t::InputsType::const_iterator input_pos = output_pos->second.inputs.begin (),
               input_limit = output_pos->second.inputs.end ();
             input_pos != input_limit;
             ++input_pos)
          {
            bind (reinterpret_cast<port_t**> (reinterpret_cast<char*> (output_instance->ptr ()) + output_port),
                  input_pos->instance->ptr (),
                  input_pos->reaction,
                  input_pos->parameter);
          }
      }

    for (instance_table_t::PullPortsType::const_iterator input_pos = instance_table.pull_ports.begin (),
           input_limit = instance_table.pull_ports.end ();
         input_pos != input_limit;
         ++input_pos)
      {
        instance_t* input_instance = input_pos->second.input_instance;
        size_t input_pull_port = input_pos->first - input_instance->address ();
        instance_table_t::OutputType output = *input_pos->second.outputs.begin ();
        pull_port_t* pull_port = reinterpret_cast<pull_port_t*> (reinterpret_cast<char*> (input_instance->ptr ()) + input_pull_port);
        assert (output.instance != NULL);
        pull_port->instance = output.instance->ptr ();
        pull_port->getter = output.getter;
      }
  }

  void
  initialize (executor_base_t& exec, instance_t* instance)
  {
    if (instance->is_top_level ())
      {
        // Set up a call expression.
        const unsigned int line = instance->line ();
        ast_identifier_t id (line, "");
        ast_identifier_expr_t expr (line, &id);
        ast_list_expr_t args (line);
        ast_call_expr_t node (line, &expr, &args);
        // Call the initializer.
        instance->initializer ()->call (exec, node, instance->ptr ());
      }
  }

  struct const_visitor : public const_type_visitor_t
  {
    executor_base_t& exec;
    const typed_value_t& tv;

    const_visitor (executor_base_t& e,
                   const typed_value_t& t) : exec (e), tv (t) { }

    void visit (const named_type_t& type)
    {
      type.subtype ()->accept (*this);
    }

    template <typename T>
    void push (const T& t)
    {
      stack_frame_push<typename T::ValueType> (exec.stack (), tv.value.ref (t));
    }

    void visit (const bool_type_t& t) { push (t); }
    void visit (const int_type_t& t) { push (t); }
    void visit (const int8_type_t& t) { push (t); }
    void visit (const uint_type_t& t) { push (t); }
    void visit (const uint8_type_t& t) { push (t); }
    //void visit (const uint32_type_t& t) { push (t); }
    void visit (const uint64_type_t& t) { push (t); }
    void visit (const float64_type_t& t) { push (t); }

    void visit (const string_type_t& type)
    {
      stack_frame_push_string (exec.stack (), rtstring_t (tv.value.ref (type)));
    }

    void visit (const pointer_type_t&)
    {
      stack_frame_push_pointer (exec.stack (), NULL);
    }

    void visit (const nil_type_t&)
    {
      stack_frame_push_pointer (exec.stack (), NULL);
    }

    void visit (const function_type_t&)
    {
      unimplemented;
      //stack_frame_push_pointer (exec.stack (), tv.function_value->node);
    }

    void visit (const method_type_t&)
    {
      unimplemented;
      //stack_frame_push_pointer (exec.stack (), tv.method_value->node);
    }

    void visit (const enum_type_t& t) { push (t); }

    void default_action (const type_t& type)
    {
      type_not_reached (type);
    }
  };

  template <typename T>
  static void
  evaluate (executor_base_t& exec, const ast_binary_expr_t& node, const T& op)
  {
    struct visitor : public const_type_visitor_t
    {
      executor_base_t& exec;
      const ast_binary_expr_t& node;
      const T& op;

      visitor (executor_base_t& e,
               const ast_binary_expr_t& n,
               const T& o) : exec (e), node (n), op (o) { }

      void default_action (const type_t& t)
      {
        type_not_reached (t);
      }

      void visit (const named_type_t& t)
      {
        t.subtype ()->accept (*this);
      }

      void visit (const bool_type_t& t)
      {
        op (exec, node, t);
      }

      void visit (const int_type_t& t)
      {
        op (exec, node, t);
      }

      void visit (const int8_type_t& t)
      {
        op (exec, node, t);
      }

      void visit (const uint_type_t& t)
      {
        op (exec, node, t);
      }

      void visit (const uint64_type_t& t)
      {
        op (exec, node, t);
      }

      void visit (const float64_type_t& t)
      {
        op (exec, node, t);
      }

      void visit (const pointer_type_t& t)
      {
        op (exec, node, t);
      }

      void visit (const enum_type_t& t)
      {
        op (exec, node, t);
      }
    };

    visitor v (exec, node, op);
    op.dispatch_type (node)->accept (v);
  }

  struct RetvalDispatch
  {
    const type_t*
    dispatch_type (const ast_binary_expr_t& node) const
    {
      return node.typed_value.type;
    }
  };

  struct LeftDispatch
  {
    const type_t*
    dispatch_type (const ast_binary_expr_t& node) const
    {
      return node.left ()->typed_value.type;
    }
  };

  struct Multiply : public RetvalDispatch
  {
    template <typename T>
    void
    doit (executor_base_t& exec,
          const ast_binary_expr_t& node,
          const T&) const
    {
      evaluate_expr (exec, node.left ());
      typename T::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      typename T::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left * right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t& t) const
    {
      doit (exec, node, t);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int8_type_t& t) const
    {
      doit (exec, node, t);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  struct Divide : public RetvalDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t&) const
    {
      evaluate_expr (exec, node.left ());
      int_type_t::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      int_type_t::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left / right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const float64_type_t&) const
    {
      evaluate_expr (exec, node.left ());
      float64_type_t::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      float64_type_t::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left / right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  struct Modulus : public RetvalDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t&) const
    {
      evaluate_expr (exec, node.left ());
      int_type_t::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      int_type_t::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left % right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  template <typename T>
  struct LeftShiftVisitor : public const_type_visitor_t
  {
    executor_base_t& exec;
    const ast_binary_expr_t& node;

    LeftShiftVisitor (executor_base_t& e,
                      const ast_binary_expr_t& n)
      : exec (e)
      , node (n)
    { }

    void default_action (const type_t& t)
    {
      type_not_reached (t);
    }

    void visit (const named_type_t& t)
    {
      t.subtype ()->accept (*this);
    }

    void visit (const uint_type_t& t)
    {
      doit (t);
    }

    void visit (const uint8_type_t& t)
    {
      doit (t);
    }

    template <typename U>
    void doit (const U&)
    {
      evaluate_expr (exec, node.left ());
      typename T::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      typename U::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left << right);
    }
  };

  struct LeftShift : public RetvalDispatch
  {
    template <typename T>
    void
    doit (executor_base_t& exec,
          const ast_binary_expr_t& node,
          const T&) const
    {
      LeftShiftVisitor<T> v (exec, node);
      node.right ()->typed_value.type->accept (v);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t& t) const
    {
      doit (exec, node, t);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const uint64_type_t& t) const
    {
      doit (exec, node, t);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  struct RightShift : public RetvalDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t&) const
    {
      struct visitor : public const_type_visitor_t
      {
        executor_base_t& exec;
        const ast_binary_expr_t& node;

        visitor (executor_base_t& e,
                 const ast_binary_expr_t& n)
          : exec (e)
          , node (n)
        { }

        void default_action (const type_t& t)
        {
          type_not_reached (t);
        }

        void visit (const named_type_t& t)
        {
          t.subtype ()->accept (*this);
        }

        void visit (const uint_type_t&)
        {
          evaluate_expr (exec, node.left ());
          int_type_t::ValueType left;
          stack_frame_pop (exec.stack (), left);
          evaluate_expr (exec, node.right ());
          uint_type_t::ValueType right;
          stack_frame_pop (exec.stack (), right);
          stack_frame_push (exec.stack (), left >> right);
        }
      };

      visitor v (exec, node);
      node.right ()->typed_value.type->accept (v);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  struct BitAnd : public RetvalDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t&) const
    {
      evaluate_expr (exec, node.left ());
      int_type_t::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      int_type_t::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left & right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  struct BitAndNot : public RetvalDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t&) const
    {
      evaluate_expr (exec, node.left ());
      int_type_t::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      int_type_t::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left & (~right));
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  struct Add : public RetvalDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t& type) const
    {
      doit (exec, node, type);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const uint_type_t& type) const
    {
      doit (exec, node, type);
    }

    template <typename T>
    void
    doit (executor_base_t& exec,
          const ast_binary_expr_t& node,
          const T&) const
    {
      evaluate_expr (exec, node.left ());
      typename T::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      typename T::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left + right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  struct Subtract : public RetvalDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t&) const
    {
      evaluate_expr (exec, node.left ());
      int_type_t::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      int_type_t::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left - right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  struct BitOr : public RetvalDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t&) const
    {
      evaluate_expr (exec, node.left ());
      int_type_t::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      int_type_t::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left | right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  struct BitXor : public RetvalDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t&) const
    {
      evaluate_expr (exec, node.left ());
      int_type_t::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      int_type_t::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left ^ right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  struct Equal : public LeftDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t& type) const
    {
      doit (exec, node, type);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const uint_type_t& type) const
    {
      doit (exec, node, type);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const pointer_type_t& type) const
    {
      doit (exec, node, type);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const enum_type_t& type) const
    {
      doit (exec, node, type);
    }

    template <typename T>
    void doit (executor_base_t& exec,
               const ast_binary_expr_t& node,
               const T&) const
    {
      evaluate_expr (exec, node.left ());
      typename T::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      typename T::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left == right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  struct NotEqual : public LeftDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t&) const
    {
      doit<int_type_t> (exec, node);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const uint_type_t&) const
    {
      doit<uint_type_t> (exec, node);
    }

    template <typename T>
    void
    doit (executor_base_t& exec, const ast_binary_expr_t& node) const
    {
      evaluate_expr (exec, node.left ());
      typename T::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      typename T::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left != right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  struct LessThan : public LeftDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t&) const
    {
      evaluate_expr (exec, node.left ());
      int_type_t::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      int_type_t::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left < right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int8_type_t&) const
    {
      evaluate_expr (exec, node.left ());
      int8_type_t::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      int8_type_t::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left < right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  struct LessEqual : public LeftDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t&) const
    {
      evaluate_expr (exec, node.left ());
      int_type_t::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      int_type_t::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left <= right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  struct MoreThan : public LeftDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t&) const
    {
      evaluate_expr (exec, node.left ());
      int_type_t::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      int_type_t::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left > right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  struct MoreEqual : public LeftDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const int_type_t&) const
    {
      evaluate_expr (exec, node.left ());
      int_type_t::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      int_type_t::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left >= right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const type_t& t) const
    {
      type_not_reached (t);
    }
  };

  void
  evaluate_expr (executor_base_t& exec,
                 ast_t* node)
  {
    typed_value_t tv = node->typed_value;
    if (tv.value.present)
      {
        switch (tv.kind)
          {
          case typed_value_t::VALUE:
            {
              const_visitor v (exec, tv);
              tv.type->accept (v);

              return;
            }
            break;
          case typed_value_t::REFERENCE:
            unimplemented;
          }
      }

    struct visitor : public ast_const_visitor_t
    {
      executor_base_t& exec;

      visitor (executor_base_t& e) : exec (e) { }

      void default_action (const ast_t& node)
      {
        ast_not_reached (node);
      }

      void visit (const ast_indexed_port_call_expr_t& node)
      {
        // Determine the trigger index.
        evaluate_expr (exec, node.index ());
        int_type_t::ValueType idx;
        stack_frame_pop (exec.stack (), idx);
        if (idx < 0 || idx >= node.array_type->dimension)
          {
            error_at_line (-1, 0, node.location.file, node.location.line,
                           "array index is out of bounds");
          }

        push_port_call (node, node.args (), node.field, idx * node.array_type->element_size ());
      }

      void visit (const ast_index_expr_t& node)
      {
        struct visitor : public const_type_visitor_t
        {
          executor_base_t& exec;
          const ast_index_expr_t& node;

          visitor (executor_base_t& e, const ast_index_expr_t& n) : exec (e), node (n) { }

          void default_action (const type_t& type)
          {
            not_reached;
          }

          void visit (const array_type_t& type)
          {
            evaluate_expr (exec, node.base ());
            void* ptr = stack_frame_pop_pointer (exec.stack ());
            evaluate_expr (exec, node.index ());
            int_type_t::ValueType idx;
            stack_frame_pop (exec.stack (), idx);
            if (idx < 0 || idx >= type.dimension)
              {
                error_at_line (-1, 0, node.location.file, node.location.line,
                               "array index is out of bounds");
              }
            stack_frame_push_pointer (exec.stack (),
                                      static_cast<char*> (ptr) + idx * type.element_size ());
          }
        };
        visitor v (exec, node);
        node.base ()->typed_value.type->accept (v);
      }

      void visit (const ast_merge_expr_t& node)
      {
        ast_t* child = node.child ();
        evaluate_expr (exec, child);
        heap_link_t* hl = (heap_link_t*)stack_frame_pop_pointer (exec.stack ());
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
                void* root = heap_instance (h);

                // Remove from parent.
                heap_remove_from_parent (h);

                // Merge into the new parent.
                heap_merge (exec.heap (), h);

                // Return the root.
                stack_frame_push_pointer (exec.stack (), root);
              }
            else
              {
                pthread_mutex_unlock (&hl->mutex);
                stack_frame_push_pointer (exec.stack (), NULL);
              }
          }
        else
          {
            stack_frame_push_pointer (exec.stack (), NULL);
          }
      }

      void visit (const ast_move_expr_t& node)
      {
        ast_t* child = node.child ();
        evaluate_expr (exec, child);
        heap_link_t* hl = (heap_link_t*)stack_frame_pop_pointer (exec.stack ());
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
                stack_frame_push_pointer (exec.stack (), new_hl);
              }
            else
              {
                pthread_mutex_unlock (&hl->mutex);
                stack_frame_push_pointer (exec.stack (), NULL);
              }
          }
        else
          {
            stack_frame_push_pointer (exec.stack (), NULL);
          }
      }

      void visit (const ast_new_expr_t& node)
      {
        // Allocate a new instance of the type.
        typed_value_t tv = node.typed_value;
        const type_t* type = type_dereference (tv.type);
        const heap_type_t* heap_type = type_cast<heap_type_t> (type);
        if (heap_type == NULL)
          {
            void* ptr = heap_allocate (exec.heap (), type->size ());
            // Return the instance.
            stack_frame_push_pointer (exec.stack (), ptr);
          }
        else
          {
            const type_t* t = heap_type->base_type ();
            // Allocate a new heap and root object.
            heap_t* h = heap_make_size (t->size ());
            // Insert it into its parent.
            heap_t* h2 = exec.heap ();
            heap_insert_child (h2, h);
            // Allocate a new heap link in the parent.
            heap_link_t* hl = make_heap_link (h, h2);
            // Return the heap link.
            stack_frame_push_pointer (exec.stack (), hl);
          }
      }

      void visit (const ast_address_of_expr_t& node)
      {
        evaluate_expr (exec, node.child ());
      }

      void visit (const ast_call_expr_t& node)
      {
        if (type_cast<pull_port_type_t> (node.expr ()->typed_value.type) == NULL) {
          node.expr ()->typed_value.value.callable_value ()->call (exec, node);
        } else {
          // Evaluate the pull port.
          pull_port_t pull_port;
          evaluate_expr (exec, node.expr ());
          stack_frame_store_heap (exec.stack (), &pull_port, sizeof (pull_port_t));
          // Execute the call.
          pull_port.getter->call (exec, node, pull_port.instance);
        }
      }

      void push_port_call (const ast_t& node,
                           ast_t* args,
                           const field_t* field,
                           size_t offset = 0)
      {
        // Push all of the arguments first and measure their size.
        char* top_before = stack_frame_top (exec.stack ());
        evaluate_expr (exec, args);
        char* top_after = stack_frame_top (exec.stack ());
        ptrdiff_t arguments_size = top_after - top_before; // Assumes stack grows up.

        // Find the port to trigger.
        Symbol* this_ = node.symtab->get_this ();
        stack_frame_push (exec.stack (), this_-> offset (), SymbolCast<ParameterSymbol> (this_)->value.type->size ());
        port_t* port = *((port_t**)((char*)stack_frame_pop_pointer (exec.stack ()) + field->offset + offset));

        char* base_pointer = stack_frame_base_pointer (exec.stack ());
        component_t* instance = exec.current_instance ();

        // Trigger all the reactions bound to the port.
        while (port != NULL)
          {
            // Set up a frame.
            // Push the instance.
            stack_frame_push_pointer (exec.stack (), port->instance);
            // Push the parameter.
            if (port->reaction->has_dimension ())
              {
                stack_frame_push<int_type_t::ValueType> (exec.stack (), port->parameter);
              }
            // Push the arguments.
            stack_frame_load (exec.stack (), top_before, arguments_size);
            // Push an instruction pointer.
            stack_frame_push_pointer (exec.stack (), NULL);

            // Jump to the last frame.
            stack_frame_set_base_pointer (exec.stack (), exec.mutable_phase_base_pointer ());

            execute (exec, port->reaction, port->instance);

            // Move to our frame and instance.
            stack_frame_set_base_pointer (exec.stack (), base_pointer);
            exec.current_instance (instance);

            port = port->next;
          }
      }

      void visit (const ast_push_port_call_expr_t& node)
      {
        push_port_call (node, node.args (), node.field);
      }

      void visit (const ast_list_expr_t& node)
      {
        for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
             pos != limit;
             ++pos)
          {
            evaluate_expr (exec, *pos);
          }
      }

      void visit (const ast_select_expr_t& node)
      {
        evaluate_expr (exec, node.base ());
        char* ptr = static_cast<char*> (stack_frame_pop_pointer (exec.stack ()));
        typed_value_t tv = node.typed_value;
        assert (tv.has_offset);
        stack_frame_push_pointer (exec.stack (), ptr + tv.offset);
      }

      void visit (const ast_dereference_expr_t& node)
      {
        evaluate_expr (exec, node.child ());
      }

      void visit (const ast_identifier_expr_t& node)
      {
        // Get the address of the identifier.
        Symbol* symbol = node.symbol.symbol ();
        ptrdiff_t offset = symbol->offset ();
        stack_frame_push_address (exec.stack (), offset);
      }

      void visit (const ast_logic_not_expr_t& node)
      {
        evaluate_expr (exec, node.child ());
        bool_type_t::ValueType b;
        stack_frame_pop (exec.stack (), b);
        stack_frame_push<bool_type_t::ValueType> (exec.stack (), !b);
      }

      void visit (const ast_implicit_dereference_expr_t& node)
      {
        evaluate_expr (exec, node.child ());
        void* ptr = stack_frame_pop_pointer (exec.stack ());
        typed_value_t tv = node.typed_value;
        stack_frame_load (exec.stack (), ptr, tv.type->size ());
      }

      void visit (const ast_cast_expr_t& node)
      {
        evaluate_expr (exec, node.child ());
        typed_value_t tv = node.child ()->typed_value;
        stack_frame_pop_tv (exec.stack (), tv);
        tv = typed_value_t::cast_exec (node.typed_value.type, tv);
        stack_frame_push_tv (exec.stack (), tv);
      }

      void visit (const ast_binary_arithmetic_expr_t& node)
      {
        switch (node.arithmetic)
          {
          case MULTIPLY:
            evaluate (exec, node, Multiply ());
            break;
          case DIVIDE:
            evaluate (exec, node, Divide ());
            break;
          case MODULUS:
            evaluate (exec, node, Modulus ());
            break;
          case LEFT_SHIFT:
            evaluate (exec, node, LeftShift ());
            break;
          case RIGHT_SHIFT:
            evaluate (exec, node, RightShift ());
            break;
          case BIT_AND:
            evaluate (exec, node, BitAnd ());
            break;
          case BIT_AND_NOT:
            evaluate (exec, node, BitAndNot ());
            break;
          case ADD:
            evaluate (exec, node, Add ());
            break;
          case SUBTRACT:
            evaluate (exec, node, Subtract ());
            break;
          case BIT_OR:
            evaluate (exec, node, BitOr ());
            break;
          case BIT_XOR:
            evaluate (exec, node, BitXor ());
            break;
          case EQUAL:
            evaluate (exec, node, Equal ());
            break;
          case NOT_EQUAL:
            evaluate (exec, node, NotEqual ());
            break;
          case LESS_THAN:
            evaluate (exec, node, LessThan ());
            break;
          case LESS_EQUAL:
            evaluate (exec, node, LessEqual ());
            break;
          case MORE_THAN:
            evaluate (exec, node, MoreThan ());
            break;
          case MORE_EQUAL:
            evaluate (exec, node, MoreEqual ());
            break;
          case LOGIC_AND:
            {
              evaluate_expr (exec, node.left ());
              bool_type_t::ValueType b;
              stack_frame_pop (exec.stack (), b);
              if (b)
                {
                  evaluate_expr (exec, node.right ());
                }
              else
                {
                  stack_frame_push<bool_type_t::ValueType> (exec.stack (), false);
                }
            }
            break;
          case LOGIC_OR:
            {
              evaluate_expr (exec, node.left ());
              bool_type_t::ValueType b;
              stack_frame_pop (exec.stack (), b);
              if (b)
                {
                  stack_frame_push<bool_type_t::ValueType> (exec.stack (), true);
                }
              else
                {
                  evaluate_expr (exec, node.right ());
                }
            }
            break;
          }
      }
    };
    visitor v (exec);
    node->accept (v);
  }

  ControlAction
  evaluate_statement (executor_base_t& exec,
                      ast_t* node)
  {
    struct visitor : public ast_const_visitor_t
    {
      ControlAction retval;
      executor_base_t& exec;

      visitor (executor_base_t& e) : retval (CONTINUE), exec (e) { }

      void default_action (const ast_t& node)
      {
        ast_not_reached (node);
      }

      void visit (const ast_const_t& node)
      { }

      void visit (const ast_empty_statement_t& node)
      { }

      void visit (const ast_assign_statement_t& node)
      {
        ast_t* left = node.left ();
        ast_t* right = node.right ();
        // Determine the size of the value being assigned.
        size_t size = right->typed_value.type->size ();
        // Evaluate the address.
        evaluate_expr (exec, left);
        void* ptr = stack_frame_pop_pointer (exec.stack ());
        // Evaluate the value.
        evaluate_expr (exec, right);
        // Store.
        stack_frame_store_heap (exec.stack (), ptr, size);
      }

      void visit (const ast_change_statement_t& node)
      {
        ast_t* expr = node.expr ();
        ast_t* body = node.body ();
        // Evaluate the pointer to the heap link.
        evaluate_expr (exec, expr);
        heap_link_t* hl = (heap_link_t*)stack_frame_pop_pointer (exec.stack ());
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

        {
          // Evaluate the address of the heap root.
          Symbol* symbol = node.root_symbol.symbol ();
          assert (symbol != NULL);
          ptrdiff_t offset = symbol->offset ();
          stack_frame_push_address (exec.stack (), offset);
        }

        char** root_value = (char**)stack_frame_pop_pointer (exec.stack ());
        // Push a pointer to the root object.
        *root_value = static_cast<char*> (heap_instance (hl->heap));

        evaluate_statement (exec, body);

        // Restore the old heap.
        exec.heap (old_heap);

        pthread_mutex_lock (&hl->mutex);
        --hl->change_count;
        pthread_mutex_unlock (&hl->mutex);
      }

      void visit (const ast_expression_statement_t& node)
      {
        ast_t* child = node.child ();
        // Determine the size of the value being generated.
        size_t size = child->typed_value.type->size ();
        // Evaluate.
        evaluate_expr (exec, child);
        // Remove value.
        stack_frame_popn (exec.stack (), size);
      }

      void visit (const ast_if_statement_t& node)
      {
        evaluate_expr (exec, node.condition ());
        bool_type_t::ValueType c;
        stack_frame_pop (exec.stack (), c);
        if (c)
          {
            if (evaluate_statement (exec, node.true_branch ()) == RETURN)
              {
                retval = RETURN;
                return;
              }
          }
        else
          {
            if (evaluate_statement (exec, node.false_branch ()) == RETURN)
              {
                retval = RETURN;
                return;
              }
          }
      }

      void visit (const ast_while_statement_t& node)
      {
        for (;;)
          {
            evaluate_expr (exec, node.condition ());
            bool_type_t::ValueType c;
            stack_frame_pop (exec.stack (), c);
            if (c)
              {
                if (evaluate_statement (exec, node.body ()) == RETURN)
                  {
                    retval = RETURN;
                    return;
                  }
              }
            else
              {
                return;
              }
          }
      }

      void visit (const ast_for_iota_statement_t& node)
      {
        for (int_type_t::ValueType idx = 0, limit = node.limit.value.integral_value (node.limit.type);
             idx != limit;
             ++idx)
          {
            size_t* ptr = static_cast<size_t*> (stack_frame_address_for_offset (exec.stack (), node.symbol.symbol ()->offset ()));
            *ptr = idx;
            if (evaluate_statement (exec, node.body ()) == RETURN)
              {
                retval = RETURN;
                return;
              }
          }
      }

      void visit (const ast_add_assign_statement_t& node)
      {
        // Determine the size of the value being assigned.
        const type_t* type = node.right ()->typed_value.type;
        // Evaluate the address.
        evaluate_expr (exec, node.left ());
        void* ptr = stack_frame_pop_pointer (exec.stack ());
        // Evaluate the value.
        evaluate_expr (exec, node.right ());

        struct visitor : public const_type_visitor_t
        {
          executor_base_t& exec;
          void* ptr;
          visitor (executor_base_t& e, void* p) : exec (e), ptr (p) { }

          void visit (const named_type_t& type)
          {
            type.subtype ()->accept (*this);
          }

          void visit (const uint_type_t& type)
          {
            uint_type_t::ValueType x;
            stack_frame_pop (exec.stack (), x);
            *((uint_type_t::ValueType*)ptr) += x;
          }

          void default_action (const type_t& type)
          {
            unimplemented;
          }
        };
        visitor v (exec, ptr);
        type->accept (v);
      }

      void visit (const ast_subtract_assign_statement_t& node)
      {
        // Determine the size of the value being assigned.
        const type_t* type = node.right ()->typed_value.type;
        // Evaluate the address.
        evaluate_expr (exec, node.left ());
        void* ptr = stack_frame_pop_pointer (exec.stack ());
        // Evaluate the value.
        evaluate_expr (exec, node.right ());

        struct visitor : public const_type_visitor_t
        {
          executor_base_t& exec;
          void* ptr;
          visitor (executor_base_t& e, void* p) : exec (e), ptr (p) { }

          void visit (const named_type_t& type)
          {
            type.subtype ()->accept (*this);
          }

          void visit (const uint_type_t& type)
          {
            uint_type_t::ValueType x;
            stack_frame_pop (exec.stack (), x);
            *((uint_type_t::ValueType*)ptr) -= x;
          }

          void default_action (const type_t& type)
          {
            unimplemented;
          }
        };
        visitor v (exec, ptr);
        type->accept (v);
      }

      void visit (const ast_list_statement_t& node)
      {
        for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
             pos != limit;
             ++pos)
          {
            if (evaluate_statement (exec, *pos) == RETURN)
              {
                retval = RETURN;
                return;
              }
          }
      }

      void visit (const ast_return_statement_t& node)
      {
        // Evaluate the expression.
        evaluate_expr (exec, node.child ());
        // Store in the return parameter.
        stack_frame_store_stack (exec.stack (), node.return_symbol->offset (), SymbolCast<ParameterSymbol> (node.return_symbol)->value.type->size ());
        retval = RETURN;
        return;
      }

      void visit (const ast_increment_statement_t& node)
      {
        evaluate_expr (exec, node.child ());
        void* ptr = stack_frame_pop_pointer (exec.stack ());

        struct visitor : public const_type_visitor_t
        {
          void* ptr;

          visitor (void* p) : ptr (p) { }

          void default_action (const type_t& type)
          {
            not_reached;
          }

          void visit (const named_type_t& type)
          {
            type.subtype ()->accept (*this);
          }

          void visit (const uint_type_t& type)
          {
            ++(*static_cast<uint_type_t::ValueType*> (ptr));
          }
        };
        visitor v (ptr);
        node.child ()->typed_value.type->accept (v);
      }

      void visit (const ast_trigger_statement_t& node)
      {
        // Need to keep track of the largest base pointer so we can process the mutable section.
        char* base_pointer = stack_frame_base_pointer (exec.stack ());
        if (base_pointer > exec.mutable_phase_base_pointer ())
          {
            exec.mutable_phase_base_pointer (base_pointer);
          }

        // The caller pushed an instruction pointer which is just
        // before the base pointer.  Overwrite it with the body.
        ast_t* body = node.body ();
        memcpy (stack_frame_ip (exec.stack ()), &body, sizeof (void*));
        // Execute the expression list.
        evaluate_expr (exec, node.expr_list ());

        // Stop execution.
        retval = RETURN;
        return;
      }

      void visit (const ast_var_statement_t& node)
      {
        // Zero out the variable.
        for (size_t idx = 0, limit = node.symbols.size (); idx != limit; ++idx)
          {
            Symbol* symbol = node.symbols[idx].symbol ();
            stack_frame_clear_stack (exec.stack (), symbol->offset (), SymbolCast<VariableSymbol> (symbol)->value.type->size ());
          }
      }

      void visit (const ast_var_type_init_statement_t& node)
      {
        ast_t* initializer_list = node.initializer_list ();
        // Initialize the variables.
        for (size_t idx = 0, limit = node.symbols.size (); idx != limit; ++idx)
          {
            // Evaluate the address.
            Symbol* symbol = node.symbols[idx].symbol ();
            ptrdiff_t offset = symbol->offset ();
            stack_frame_push_address (exec.stack (), offset);
            void* ptr = stack_frame_pop_pointer (exec.stack ());
            ast_t* initializer = initializer_list->children.at (idx);
            size_t size = initializer->typed_value.type->size ();
            // Evaluate the value.
            evaluate_expr (exec, initializer);
            // Store.
            stack_frame_store_heap (exec.stack (), ptr, size);
          }
      }

      void visit (const ast_println_statement_t& node)
      {
        exec.lock_stdout ();
        ast_t* expr_list = node.child ();
        for (ast_t::const_iterator pos = expr_list->begin (),
               limit = expr_list->end ();
             pos != limit;
             ++pos)
          {
            ast_t* child = *pos;
            evaluate_expr (exec, child);
            struct visitor : public const_type_visitor_t
            {
              executor_base_t& exec;
              visitor (executor_base_t& e) : exec (e) { }

              void default_action (const type_t& type)
              {
                type_not_reached (type);
              }

              void visit (const bool_type_t& type)
              {
                bool_type_t::ValueType b;
                stack_frame_pop (exec.stack (), b);
                if (b)
                  {
                    printf ("true");
                  }
                else
                  {
                    printf ("false");
                  }
              }

              void visit (const pointer_type_t& type)
              {
                void* ptr = stack_frame_pop_pointer (exec.stack ());
                printf ("%p", ptr);
              }

              void visit (const string_type_t& type)
              {
                rtstring_t s = stack_frame_pop_string (exec.stack ());
                fwrite (s.bytes, 1, s.size, stdout);
              }

              void visit (const uint_type_t& type)
              {
                uint_type_t::ValueType u;
                stack_frame_pop (exec.stack (), u);
                printf ("%lu", u);
              }

              void visit (const uint8_type_t& type)
              {
                uint8_type_t::ValueType u;
                stack_frame_pop (exec.stack (), u);
                printf ("%u", u);
              }

              void visit (const uint64_type_t& type)
              {
                uint64_type_t::ValueType u;
                stack_frame_pop (exec.stack (), u);
                printf ("%lu", u);
              }

              void visit (const int_type_t& type)
              {
                int_type_t::ValueType u;
                stack_frame_pop (exec.stack (), u);
                printf ("%ld", u);
              }

              void visit (const int8_type_t& type)
              {
                int8_type_t::ValueType u;
                stack_frame_pop (exec.stack (), u);
                printf ("%d", u);
              }

              void visit (const float64_type_t& type)
              {
                float64_type_t::ValueType u;
                stack_frame_pop (exec.stack (), u);
                printf ("%g", u);
              }
            };
            visitor v (exec);
            type_strip (child->typed_value.type)->accept (v);
          }
        printf ("\n");
        exec.unlock_stdout ();
      }
    };
    visitor v (exec);
    node->accept (v);

    return v.retval;
  }

  static bool
  enabled (executor_base_t& exec,
           component_t* instance,
           const action_t* action,
           size_t iota)
  {
    assert (stack_frame_empty (exec.stack ()));

    // Set the current instance.
    exec.current_instance (instance);

    // Push this.
    stack_frame_push_pointer (exec.stack (), instance);
    // Push iota.
    if (action->has_dimension ())
      {
        stack_frame_push<uint_type_t::ValueType> (exec.stack (), iota);
      }
    // Push an instruction pointer.
    stack_frame_push_pointer (exec.stack (), NULL);
    stack_frame_push_base_pointer (exec.stack (), 0 /* No locals. */);
    evaluate_expr (exec, action->precondition);
    bool_type_t::ValueType retval;
    stack_frame_pop (exec.stack (), retval);
    stack_frame_pop_base_pointer (exec.stack ());
    // Pop the instruction pointer.
    stack_frame_pop_pointer (exec.stack ());
    // Pop iota.
    if (action->has_dimension ())
      {
        uint_type_t::ValueType x;
        stack_frame_pop (exec.stack (), x);
      }
    // Pop this.
    stack_frame_pop_pointer (exec.stack ());
    assert (stack_frame_empty (exec.stack ()));
    return retval;
  }

  static void
  execute (executor_base_t& exec,
           const reaction_t* reaction,
           component_t* instance)
  {
    // Set the current instance.
    exec.current_instance (instance);

    stack_frame_push_base_pointer (exec.stack (), reaction->memory_model.locals_size ());

    evaluate_statement (exec, reaction->body);
  }

  static void
  execute (executor_base_t& exec,
           const action_t* action,
           component_t* instance)
  {
    // Set the current instance.
    exec.current_instance (instance);

    // Reset the mutable phase base pointer.
    exec.mutable_phase_base_pointer (0);

    stack_frame_push_base_pointer (exec.stack (), action->memory_model.locals_size ());

    evaluate_statement (exec, action->body);

    if (exec.mutable_phase_base_pointer () == 0)
      {
        // No triggers.  Pop the base pointer and finish.
        stack_frame_pop_base_pointer (exec.stack ());
        return;
      }

    // Process all of the deferred trigger bodies.
    // First, go to the last frame.
    stack_frame_set_base_pointer (exec.stack (), exec.mutable_phase_base_pointer ());

    while (stack_frame_base_pointer (exec.stack ()) != NULL)
      {
        // Get the deferred body.
        ast_t* b = *(ast_t**)stack_frame_ip (exec.stack ());

        // Get the trigger.
        trigger_t *trigger = symtab_get_current_trigger (b->symtab);
        assert (trigger != NULL);

        // Set the current record.
        Symbol* this_ = b->symtab->get_this ();
        stack_frame_push (exec.stack (), this_->offset (), SymbolCast<ParameterSymbol> (this_)->value.type->size ());
        exec.current_instance (static_cast<component_t*> (stack_frame_pop_pointer (exec.stack ())));

        // Execute it.
        evaluate_statement (exec, b);

        // Add to the schedule.
        if (trigger->action == TRIGGER_WRITE)
          {
            exec.push ();
          }

        // Pop until the base pointer.
        stack_frame_set_top (exec.stack (), stack_frame_base_pointer (exec.stack ()) + sizeof (void*));
        // Pop the base pointer.
        stack_frame_pop_base_pointer (exec.stack ());
      }
  }

  bool exec (executor_base_t& exec, component_t* instance, const action_t* action, size_t iota)
  {
    if (enabled (exec, instance, action, iota))
      {
        return exec_no_check (exec, instance, action, iota);
      }

    return false;
  }

  bool exec_no_check (executor_base_t& exec, component_t* instance, const action_t* action, size_t iota)
  {
    assert (stack_frame_empty (exec.stack ()));

    // Push the instance.
    stack_frame_push_pointer (exec.stack (), instance);
    // Push iota.
    if (action->has_dimension ())
      {
        stack_frame_push<uint_type_t::ValueType> (exec.stack (), iota);
      }
    // Push the instruction pointer.
    stack_frame_push_pointer (exec.stack (), NULL);
    // Execute.
    runtime::execute (exec, action, instance);
    // Pop the instruction pointer.
    stack_frame_pop_pointer (exec.stack ());
    // Pop iota.
    if (action->has_dimension ())
      {
        uint_type_t::ValueType x;
        stack_frame_pop (exec.stack (), x);
      }
    // Pop the instance.
    stack_frame_pop_pointer (exec.stack ());

    assert (stack_frame_empty (exec.stack ()));
    return true;
  }

}

// void
// dump_instances (const runtime_t* runtime)
// {
//   for (instance_table_t::InstancesType::const_iterator pos = runtime->instance_table.instances.begin (),
//          limit = runtime->instance_table.instances.end ();
//        pos != limit;
//        ++pos)
//     {
//       instance_t* instance = pos->second;
//       if (instance->is_top_level ())
//         {
//           unimplemented;
//           //void* ptr = instance_record_get_ptr (instance_get_record (instance));
//           //type_print_value (instance_type (instance), ptr);
//           printf ("\n");
//         }
//     }
// }
