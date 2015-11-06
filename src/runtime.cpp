#include "runtime.hpp"
#include "Callable.hpp"
#include "SymbolVisitor.hpp"
#include "types.hpp"

namespace runtime
{
  using namespace Type;

  struct port_t
  {
    component_t* instance;
    const reaction_t* reaction;
    Int::ValueType parameter;
    port_t* next;

    port_t (component_t* i, const reaction_t* r, Int::ValueType p) : instance (i), reaction (r), parameter (p), next (NULL) { }
  };

  struct heap_link_t
  {
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
            const Type::Type* type = instance->type ();
            size_t size = type->Size ();
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
  bind (port_t** output_port, component_t* input_instance, const reaction_t* reaction, Int::ValueType parameter)
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
    for (instance_table_t::PushPortsType::const_iterator output_pos = instance_table.push_ports.begin (),
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
        // Set up the heap.
        exec.current_instance (instance->ptr ());
        // Set up a call expression.
        const unsigned int line = instance->line ();
        ast_identifier_t id (line, "");
        ast_identifier_expr_t expr (line, &id);
        ast_call_expr_t node (line, &expr, instance->node ()->expression_list ());
        // Call the initializer.
        instance->initializer ()->call (exec, node, instance->ptr ());
      }
  }

  template <typename T>
  static void
  evaluate (executor_base_t& exec, const ast_binary_expr_t& node, const T& op)
  {
    struct visitor : public Visitor
    {
      executor_base_t& exec;
      const ast_binary_expr_t& node;
      const T& op;

      visitor (executor_base_t& e,
               const ast_binary_expr_t& n,
               const T& o) : exec (e), node (n), op (o) { }

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
        op (exec, node, t);
      }

      void visit (const Boolean& t)
      {
        op (exec, node, t);
      }

      void visit (const Int& t)
      {
        op (exec, node, t);
      }

      void visit (const Int8& t)
      {
        op (exec, node, t);
      }

      void visit (const Uint& t)
      {
        op (exec, node, t);
      }

      void visit (const Uint64& t)
      {
        op (exec, node, t);
      }

      void visit (const Float64& t)
      {
        op (exec, node, t);
      }

      void visit (const Pointer& t)
      {
        op (exec, node, t);
      }

      void visit (const Enum& t)
      {
        op (exec, node, t);
      }
    };

    visitor v (exec, node, op);
    op.dispatch_type (node)->Accept (v);
  }

  struct RetvalDispatch
  {
    const Type::Type*
    dispatch_type (const ast_binary_expr_t& node) const
    {
      return node.typed_value.type;
    }
  };

  struct LeftDispatch
  {
    const Type::Type*
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
                const Int& t) const
    {
      doit (exec, node, t);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Int8& t) const
    {
      doit (exec, node, t);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Type::Type& t) const
    {
      type_not_reached (t);
    }
  };

  struct Divide : public RetvalDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Int&) const
    {
      evaluate_expr (exec, node.left ());
      Int::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      Int::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left / right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Float64&) const
    {
      evaluate_expr (exec, node.left ());
      Float64::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      Float64::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left / right);
    }

    void
    operator() (executor_base_t& exec,
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
                const ast_binary_expr_t& node,
                const Int&) const
    {
      evaluate_expr (exec, node.left ());
      Int::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      Int::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left % right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Type::Type& t) const
    {
      type_not_reached (t);
    }
  };

  template <typename T>
  struct LeftShiftVisitor : public Visitor
  {
    executor_base_t& exec;
    const ast_binary_expr_t& node;

    LeftShiftVisitor (executor_base_t& e,
                      const ast_binary_expr_t& n)
      : exec (e)
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

    void visit (const Int& t)
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
      node.right ()->typed_value.type->Accept (v);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Int& t) const
    {
      doit (exec, node, t);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Uint64& t) const
    {
      doit (exec, node, t);
    }

    void
    operator() (executor_base_t& exec,
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
                const ast_binary_expr_t& node,
                const Int&) const
    {
      struct visitor : public Visitor
      {
        executor_base_t& exec;
        const ast_binary_expr_t& node;

        visitor (executor_base_t& e,
                 const ast_binary_expr_t& n)
          : exec (e)
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

        void visit (const Int&)
        {
          evaluate_expr (exec, node.left ());
          Int::ValueType left;
          stack_frame_pop (exec.stack (), left);
          evaluate_expr (exec, node.right ());
          Uint::ValueType right;
          stack_frame_pop (exec.stack (), right);
          stack_frame_push (exec.stack (), left >> right);
        }

        void visit (const Uint&)
        {
          evaluate_expr (exec, node.left ());
          Uint::ValueType left;
          stack_frame_pop (exec.stack (), left);
          evaluate_expr (exec, node.right ());
          Uint::ValueType right;
          stack_frame_pop (exec.stack (), right);
          stack_frame_push (exec.stack (), left >> right);
        }
      };

      visitor v (exec, node);
      node.right ()->typed_value.type->Accept (v);
    }

    void
    operator() (executor_base_t& exec,
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
                const ast_binary_expr_t& node,
                const Int&) const
    {
      evaluate_expr (exec, node.left ());
      Int::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      Int::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left & right);
    }

    void
    operator() (executor_base_t& exec,
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
                const ast_binary_expr_t& node,
                const Int&) const
    {
      evaluate_expr (exec, node.left ());
      Int::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      Int::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left & (~right));
    }

    void
    operator() (executor_base_t& exec,
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
                const ast_binary_expr_t& node,
                const Int& type) const
    {
      doit (exec, node, type);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Uint& type) const
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
                const Type::Type& t) const
    {
      type_not_reached (t);
    }
  };

  struct Subtract : public RetvalDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Int&) const
    {
      evaluate_expr (exec, node.left ());
      Int::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      Int::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left - right);
    }

    void
    operator() (executor_base_t& exec,
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
                const ast_binary_expr_t& node,
                const Int&) const
    {
      evaluate_expr (exec, node.left ());
      Int::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      Int::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left | right);
    }

    void
    operator() (executor_base_t& exec,
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
                const ast_binary_expr_t& node,
                const Int&) const
    {
      evaluate_expr (exec, node.left ());
      Int::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      Int::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left ^ right);
    }

    void
    operator() (executor_base_t& exec,
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
                const ast_binary_expr_t& node,
                const Bool& type) const
    {
      doit (exec, node, type);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Boolean& type) const
    {
      doit (exec, node, type);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Int& type) const
    {
      doit (exec, node, type);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Uint& type) const
    {
      doit (exec, node, type);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Pointer& type) const
    {
      doit (exec, node, type);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Enum& type) const
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
                const Type::Type& t) const
    {
      type_not_reached (t);
    }
  };

  struct NotEqual : public LeftDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Bool&) const
    {
      doit<Bool> (exec, node);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Int&) const
    {
      doit<Int> (exec, node);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Uint&) const
    {
      doit<Uint> (exec, node);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Uint64&) const
    {
      doit<Uint64> (exec, node);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Pointer&) const
    {
      doit<Pointer> (exec, node);
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
                const Type::Type& t) const
    {
      type_not_reached (t);
    }
  };

  struct LessThan : public LeftDispatch
  {
    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Int&) const
    {
      evaluate_expr (exec, node.left ());
      Int::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      Int::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left < right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Int8&) const
    {
      evaluate_expr (exec, node.left ());
      Int8::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      Int8::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left < right);
    }

    void
    operator() (executor_base_t& exec,
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
                const ast_binary_expr_t& node,
                const Int&) const
    {
      evaluate_expr (exec, node.left ());
      Int::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      Int::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left <= right);
    }

    void
    operator() (executor_base_t& exec,
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
                const ast_binary_expr_t& node,
                const Int&) const
    {
      evaluate_expr (exec, node.left ());
      Int::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      Int::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left > right);
    }

    void
    operator() (executor_base_t& exec,
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
                const ast_binary_expr_t& node,
                const Int&) const
    {
      evaluate_expr (exec, node.left ());
      Int::ValueType left;
      stack_frame_pop (exec.stack (), left);
      evaluate_expr (exec, node.right ());
      Int::ValueType right;
      stack_frame_pop (exec.stack (), right);
      stack_frame_push (exec.stack (), left >= right);
    }

    void
    operator() (executor_base_t& exec,
                const ast_binary_expr_t& node,
                const Type::Type& t) const
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
            stack_frame_push_tv (exec.stack (), tv);
            return;
          }
          break;
          case typed_value_t::REFERENCE:
            std::cout << *node;
            unimplemented;
          case typed_value_t::TYPE:
            std::cout << *node;
            unimplemented;
          }
      }

    struct visitor : public ast_const_visitor_t
    {
      executor_base_t& exec;

      visitor (executor_base_t& e)
        : exec (e)
      { }

      void default_action (const ast_t& node)
      {
        ast_not_reached (node);
      }

      void visit (const ast_indexed_port_call_expr_t& node)
      {
        // Determine the push port index.
        evaluate_expr (exec, node.index ());
        Int::ValueType idx;
        stack_frame_pop (exec.stack (), idx);
        if (idx < 0 || idx >= node.array_type->dimension)
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "array index is out of bounds (E1)");
          }

        push_port_call (node, node.args (), node.field, idx * node.array_type->ElementSize ());
      }

      void visit (const ast_index_expr_t& node)
      {
        struct visitor : public Visitor
        {
          executor_base_t& exec;
          const ast_index_expr_t& node;

          visitor (executor_base_t& e, const ast_index_expr_t& n) : exec (e), node (n) { }

          void default_action (const Type::Type& type)
          {
            not_reached;
          }

          void visit (const Array& type)
          {
            evaluate_expr (exec, node.base ());
            void* ptr = stack_frame_pop_pointer (exec.stack ());
            evaluate_expr (exec, node.index ());
            Int::ValueType idx;
            stack_frame_pop (exec.stack (), idx);
            if (idx < 0 || idx >= type.dimension)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "array index is out of bounds (E2)");
              }
            stack_frame_push_pointer (exec.stack (),
                                      static_cast<char*> (ptr) + idx * type.ElementSize ());
          }
        };
        visitor v (exec, node);
        node.base ()->typed_value.type->Accept (v);
      }

      void visit (const ast_slice_expr_t& node)
      {
        evaluate_expr (exec, node.base ());
        typed_value_t base_tv = node.base ()->typed_value;
        stack_frame_pop_tv (exec.stack (), base_tv);

        typed_value_t low_tv = node.low ()->typed_value;
        if (!low_tv.value.present)
          {
            evaluate_expr (exec, node.low ());
            stack_frame_pop_tv (exec.stack (), low_tv);
          }
        Int::ValueType low = low_tv.integral_value ();

        typed_value_t high_tv = node.high ()->typed_value;
        if (!high_tv.value.present)
          {
            evaluate_expr (exec, node.high ());
            stack_frame_pop_tv (exec.stack (), high_tv);
          }
        Int::ValueType high = high_tv.integral_value ();

        const Array* array_type = type_cast<Array> (base_tv.type);
        if (array_type)
          {
            if (low < 0 || low >= array_type->dimension)
              {
                error_at_line (EXIT_FAILURE, 0, node.location.File.c_str (), node.location.Line, "lower limit of slice is out of bounds (E3)");
              }
            if (high < low || high > array_type->dimension)
              {
                error_at_line (EXIT_FAILURE, 0, node.location.File.c_str (), node.location.Line, "upper limit of slice is out of bounds (E4)");
              }

            Slice::ValueType slice;
            slice.ptr = NULL;
            slice.length = high - low;
            slice.capacity = high - low;
            if (slice.length != 0)
              {
                slice.ptr = static_cast<char*> (base_tv.value.reference_value ()) + low * array_type->ElementSize ();
              }

            const Type::Type* base_type = array_type->Base ();

            stack_frame_push_tv (exec.stack (), typed_value_t (base_type->GetSlice (), slice));
          }
        else
          {
            unimplemented;
          }
      }

      void visit (const ast_address_of_expr_t& node)
      {
        evaluate_expr (exec, node.child ());
      }

      void visit (const ast_call_expr_t& node)
      {
        const Type::Function* f = type_cast<Type::Function> (node.expr ()->typed_value.type);
        if (f == NULL || f->kind != Type::Function::PULL_PORT)
          {
            node.expr ()->typed_value.value.callable_value ()->call (exec, node);
          }
        else
          {
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

        // Find the port to activate.
        Symbol* this_ = node.GetReceiverSymbol ();
        stack_frame_push (exec.stack (), this_-> offset (), SymbolCast<ParameterSymbol> (this_)->value.type->Size ());
        port_t* port = *((port_t**)((char*)stack_frame_pop_pointer (exec.stack ()) + field->offset + offset));

        char* base_pointer = stack_frame_base_pointer (exec.stack ());
        component_t* instance = exec.current_instance ();

        // Activate all the reactions bound to the port.
        while (port != NULL)
          {
            // Set up a frame.
            // Push the instance.
            stack_frame_push_pointer (exec.stack (), port->instance);
            // Push the parameter.
            if (port->reaction->has_dimension ())
              {
                stack_frame_push<Int::ValueType> (exec.stack (), port->parameter);
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
        Symbol* symbol = node.symbol;
        ptrdiff_t offset = symbol->offset ();
        stack_frame_push_address (exec.stack (), offset);
      }

      void visit (const ast_unary_arithmetic_expr_t& node)
      {
        evaluate_expr (exec, node.child ());
        switch (node.arithmetic)
          {
          case LogicNot:
          {
            Bool::ValueType b;
            stack_frame_pop (exec.stack (), b);
            stack_frame_push<Bool::ValueType> (exec.stack (), !b);
          }
          return;
          case Negate:
            unimplemented;
          }
        not_reached;
      }

      void visit (const ast_implicit_dereference_expr_t& node)
      {
        evaluate_expr (exec, node.child ());
        void* ptr = stack_frame_pop_pointer (exec.stack ());
        if (ptr == NULL)
          {
            std::cout << node;
            unimplemented;
          }
        typed_value_t tv = node.typed_value;
        stack_frame_load (exec.stack (), ptr, tv.type->Size ());
      }

      void visit (const ast_implicit_conversion_expr_t& node)
      {
        evaluate_expr (exec, node.child ());
      }

      void visit (const ast_binary_arithmetic_expr_t& node)
      {
        switch (node.arithmetic)
          {
          case ::Multiply:
            evaluate (exec, node, Multiply ());
            break;
          case ::Divide:
            evaluate (exec, node, Divide ());
            break;
          case ::Modulus:
            evaluate (exec, node, Modulus ());
            break;
          case ::LeftShift:
            evaluate (exec, node, LeftShift ());
            break;
          case ::RightShift:
            evaluate (exec, node, RightShift ());
            break;
          case ::BitAnd:
            evaluate (exec, node, BitAnd ());
            break;
          case ::BitAndNot:
            evaluate (exec, node, BitAndNot ());
            break;
          case ::Add:
            evaluate (exec, node, Add ());
            break;
          case ::Subtract:
            evaluate (exec, node, Subtract ());
            break;
          case ::BitOr:
            evaluate (exec, node, BitOr ());
            break;
          case ::BitXor:
            evaluate (exec, node, BitXor ());
            break;
          case ::Equal:
            evaluate (exec, node, Equal ());
            break;
          case ::NotEqual:
            evaluate (exec, node, NotEqual ());
            break;
          case ::LessThan:
            evaluate (exec, node, LessThan ());
            break;
          case ::LessEqual:
            evaluate (exec, node, LessEqual ());
            break;
          case ::MoreThan:
            evaluate (exec, node, MoreThan ());
            break;
          case ::MoreEqual:
            evaluate (exec, node, MoreEqual ());
            break;
          case ::LogicAnd:
          {
            evaluate_expr (exec, node.left ());
            Bool::ValueType b;
            stack_frame_pop (exec.stack (), b);
            if (b)
              {
                evaluate_expr (exec, node.right ());
              }
            else
              {
                stack_frame_push<Bool::ValueType> (exec.stack (), false);
              }
          }
          break;
          case ::LogicOr:
          {
            evaluate_expr (exec, node.left ());
            Bool::ValueType b;
            stack_frame_pop (exec.stack (), b);
            if (b)
              {
                stack_frame_push<Bool::ValueType> (exec.stack (), true);
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
        size_t size = right->typed_value.type->Size ();
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
          Symbol* symbol = node.root_symbol;
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
        size_t size = child->typed_value.type->Size ();
        // Evaluate.
        evaluate_expr (exec, child);
        // Remove value.
        stack_frame_popn (exec.stack (), size);
      }

      void visit (const ast_if_statement_t& node)
      {
        evaluate_expr (exec, node.condition ());
        Bool::ValueType c;
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
            Bool::ValueType c;
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
        for (Int::ValueType idx = 0, limit = node.limit.integral_value ();
             idx != limit;
             ++idx)
          {
            size_t* ptr = static_cast<size_t*> (stack_frame_address_for_offset (exec.stack (), node.symbol->offset ()));
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
        const Type::Type* type = node.right ()->typed_value.type;
        // Evaluate the address.
        evaluate_expr (exec, node.left ());
        void* ptr = stack_frame_pop_pointer (exec.stack ());
        // Evaluate the value.
        evaluate_expr (exec, node.right ());

        struct visitor : public Visitor
        {
          executor_base_t& exec;
          void* ptr;
          visitor (executor_base_t& e, void* p) : exec (e), ptr (p) { }

          void visit (const NamedType& type)
          {
            type.UnderlyingType ()->Accept (*this);
          }

          void visit (const Uint& type)
          {
            Uint::ValueType x;
            stack_frame_pop (exec.stack (), x);
            *((Uint::ValueType*)ptr) += x;
          }

          void default_action (const Type::Type& type)
          {
            unimplemented;
          }
        };
        visitor v (exec, ptr);
        type->Accept (v);
      }

      void visit (const ast_subtract_assign_statement_t& node)
      {
        // Determine the size of the value being assigned.
        const Type::Type* type = node.right ()->typed_value.type;
        // Evaluate the address.
        evaluate_expr (exec, node.left ());
        void* ptr = stack_frame_pop_pointer (exec.stack ());
        // Evaluate the value.
        evaluate_expr (exec, node.right ());

        struct visitor : public Visitor
        {
          executor_base_t& exec;
          void* ptr;
          visitor (executor_base_t& e, void* p) : exec (e), ptr (p) { }

          void visit (const NamedType& type)
          {
            type.UnderlyingType ()->Accept (*this);
          }

          void visit (const Uint& type)
          {
            Uint::ValueType x;
            stack_frame_pop (exec.stack (), x);
            *((Uint::ValueType*)ptr) -= x;
          }

          void default_action (const Type::Type& type)
          {
            unimplemented;
          }
        };
        visitor v (exec, ptr);
        type->Accept (v);
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
        stack_frame_store_stack (exec.stack (), node.return_symbol->offset (), SymbolCast<ParameterSymbol> (node.return_symbol)->value.type->Size ());
        retval = RETURN;
        return;
      }

      void visit (const ast_increment_statement_t& node)
      {
        evaluate_expr (exec, node.child ());
        void* ptr = stack_frame_pop_pointer (exec.stack ());

        struct visitor : public Visitor
        {
          void* ptr;

          visitor (void* p) : ptr (p) { }

          void default_action (const Type::Type& type)
          {
            not_reached;
          }

          void visit (const NamedType& type)
          {
            type.UnderlyingType ()->Accept (*this);
          }

          void visit (const Uint& type)
          {
            ++(*static_cast<Uint::ValueType*> (ptr));
          }
        };
        visitor v (ptr);
        node.child ()->typed_value.type->Accept (v);
      }

      void visit (const ast_activate_statement_t& node)
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
        ast_t* expression_list = node.expression_list ();

        if (expression_list->size () == 0)
          {
            // Zero out the variable.
            for (size_t idx = 0, limit = node.symbols.size (); idx != limit; ++idx)
              {
                Symbol* symbol = node.symbols[idx];
                stack_frame_clear_stack (exec.stack (), symbol->offset (), SymbolCast<VariableSymbol> (symbol)->value.type->Size ());
              }
          }
        else
          {
            // Initialize the variables.
            for (size_t idx = 0, limit = node.symbols.size (); idx != limit; ++idx)
              {
                // Evaluate the address.
                Symbol* symbol = node.symbols[idx];
                ptrdiff_t offset = symbol->offset ();
                stack_frame_push_address (exec.stack (), offset);
                void* ptr = stack_frame_pop_pointer (exec.stack ());
                ast_t* initializer = expression_list->at (idx);
                size_t size = initializer->typed_value.type->Size ();
                // Evaluate the value.
                evaluate_expr (exec, initializer);
                // Store.
                stack_frame_store_heap (exec.stack (), ptr, size);
              }
          }
      }
    };
    visitor v (exec);
    node->accept (v);

    return v.retval;
  }

  bool
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
        stack_frame_push<Uint::ValueType> (exec.stack (), iota);
      }
    // Push an instruction pointer.
    stack_frame_push_pointer (exec.stack (), NULL);
    stack_frame_push_base_pointer (exec.stack (), 0 /* No locals. */);
    evaluate_expr (exec, action->precondition);
    Bool::ValueType retval;
    stack_frame_pop (exec.stack (), retval);
    stack_frame_pop_base_pointer (exec.stack ());
    // Pop the instruction pointer.
    stack_frame_pop_pointer (exec.stack ());
    // Pop iota.
    if (action->has_dimension ())
      {
        Uint::ValueType x;
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

    stack_frame_push_base_pointer (exec.stack (), reaction->memory_model.LocalsSize ());

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

    stack_frame_push_base_pointer (exec.stack (), action->memory_model.LocalsSize ());

    evaluate_statement (exec, action->body);

    if (exec.mutable_phase_base_pointer () == 0)
      {
        // No activations.  Pop the base pointer and finish.
        stack_frame_pop_base_pointer (exec.stack ());
        return;
      }

    // Process all of the deferred activation bodies.
    // First, go to the last frame.
    stack_frame_set_base_pointer (exec.stack (), exec.mutable_phase_base_pointer ());

    while (stack_frame_base_pointer (exec.stack ()) != NULL)
      {
        // Get the deferred body.
        ast_t* b = *(ast_t**)stack_frame_ip (exec.stack ());

        // Get the activation.
        Activation *activation = b->GetActivation ();
        assert (activation != NULL);

        // Set the current record.
        Symbol* this_ = b->GetReceiverSymbol ();
        stack_frame_push (exec.stack (), this_->offset (), SymbolCast<ParameterSymbol> (this_)->value.type->Size ());
        exec.current_instance (static_cast<component_t*> (stack_frame_pop_pointer (exec.stack ())));

        // Execute it.
        evaluate_statement (exec, b);

        // Add to the schedule.
        if (activation->mode == ACTIVATION_WRITE)
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
        stack_frame_push<Uint::ValueType> (exec.stack (), iota);
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
        Uint::ValueType x;
        stack_frame_pop (exec.stack (), x);
      }
    // Pop the instance.
    stack_frame_pop_pointer (exec.stack ());

    assert (stack_frame_empty (exec.stack ()));
    return true;
  }

  struct NewImpl : public Callable
  {
    NewImpl (const Type::Type* t, ast_t* definingNode)
      : type_ (t)
      , function_type_ (makeFunctionType (t, definingNode))
    { }

    virtual void call (executor_base_t& exec, const ast_call_expr_t& node) const
    {
      // Allocate a new instance of the type.
      const Heap* heap_type = type_cast<Heap> (type_);
      if (heap_type == NULL)
        {
          void* ptr = heap_allocate (exec.heap (), type_->Size ());
          // Return the instance.
          stack_frame_push_pointer (exec.stack (), ptr);
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
          // Return the heap link.
          stack_frame_push_pointer (exec.stack (), hl);
        }
    }

    virtual const Type::Type* type () const
    {
      return function_type_;
    }
    const Type::Type* const type_;
    const Type::Type* const function_type_;
    static const Type::Type* makeFunctionType (const Type::Type* type, ast_t* definingNode)
    {
      const Type::Type* return_type = type->GetPointer ();
      return new Type::Function (Type::Function::FUNCTION, (new Signature ()),
                                 new parameter_t (definingNode, "0return", typed_value_t::make_value (return_type, MUTABLE, MUTABLE), false));
    }

  };

  New::New (ast_t* dn)
    : Template ("new",
                dn,
                new Type::Template ())
  { }

  typed_value_t
  New::instantiate (TypedValueListType& tvlist)
  {
    if (tvlist.size () != 1)
      {
        error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                       "new expects one argument (E5)");
      }

    typed_value_t tv = tvlist[0];
    tvlist.clear ();

    if (tv.kind != typed_value_t::TYPE)
      {
        error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                       "new expects a type (E6)");
      }

    return typed_value_t (new NewImpl (tv.type, definingNode));
  }

  struct MoveImpl : public Callable
  {
    MoveImpl (const typed_value_t& in, const typed_value_t& out, ast_t* definingNode)
      : function_type_ (makeFunctionType (in, out, definingNode))
    { }

    virtual void call (executor_base_t& exec, const ast_call_expr_t& node) const
    {
      evaluate_expr (exec, node.args ());
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

    virtual const Type::Type* type () const
    {
      return function_type_;
    }
    const Type::Type* const function_type_;
    static const Type::Type* makeFunctionType (const typed_value_t& in, const typed_value_t& out, ast_t* definingNode)
    {
      typed_value_t in2 = in;
      in2.intrinsic_mutability = MUTABLE;
      return new Type::Function (Type::Function::FUNCTION, (new Signature ())
                                 ->Append (new parameter_t (definingNode, "h", in2, false)),
                                 new parameter_t (definingNode, "0return", out, false));
    }

  };

  Move::Move (ast_t* dn)
    : Template ("move",
                dn,
                new Type::Template ())
  { }

  typed_value_t
  Move::instantiate (TypedValueListType& tvlist)
  {
    if (tvlist.size () != 1)
      {
        error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                       "move expects one argument (E7)");
      }

    typed_value_t in = tvlist[0];
    typed_value_t out = typed_value_t::move (in);
    if (out.type == NULL)
      {
        error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                       "cannot move expression of type %s (E8)", in.type->ToString ().c_str ());
      }

    return typed_value_t (new MoveImpl (in, out, definingNode));
  }

  struct MergeImpl : public Callable
  {
    MergeImpl (const typed_value_t& in, const typed_value_t& out, ast_t* definingNode)
      : function_type_ (makeFunctionType (in, out, definingNode))
    { }

    virtual void call (executor_base_t& exec, const ast_call_expr_t& node) const
    {
      evaluate_expr (exec, node.args ());
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

    virtual const Type::Type* type () const
    {
      return function_type_;
    }
    const Type::Type* const function_type_;
    static const Type::Type* makeFunctionType (const typed_value_t& in, const typed_value_t& out, ast_t* definingNode)
    {
      typed_value_t in2 = in;
      in2.intrinsic_mutability = MUTABLE;
      return new Type::Function (Type::Function::FUNCTION, (new Signature ())
                                 ->Append (new parameter_t (definingNode, "h", in2, false)),
                                 new parameter_t (definingNode, "0return", out, false));
    }
  };

  Merge::Merge (ast_t* dn)
    : Template ("merge",
                dn,
                new Type::Template ())
  { }

  typed_value_t
  Merge::instantiate (TypedValueListType& tvlist)
  {
    if (tvlist.size () != 1)
      {
        error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                       "merge expects one argument (E9)");
      }

    typed_value_t in = tvlist[0];
    typed_value_t out = typed_value_t::merge (in);
    if (out.type == NULL)
      {
        error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                       "cannot merge expression of type %s (E10)", in.type->ToString ().c_str ());
      }

    return typed_value_t (new MergeImpl (in, out, definingNode));
  }

  struct CopyImpl : public Callable
  {
    CopyImpl (const typed_value_t& in, const typed_value_t& out, ast_t* definingNode)
      : in_ (in)
      , function_type_ (makeFunctionType (in, out, definingNode))
    { }

    virtual void call (executor_base_t& exec, const ast_call_expr_t& node) const
    {
      evaluate_expr (exec, node.args ());
      typed_value_t tv = in_;
      stack_frame_pop_tv (exec.stack (), tv);
      tv = typed_value_t::copy_exec (tv);
      stack_frame_push_tv (exec.stack (), tv);
    }

    virtual const Type::Type* type () const
    {
      return function_type_;
    }
    const typed_value_t in_;
    const Type::Type* const function_type_;
    static const Type::Type* makeFunctionType (const typed_value_t& in, const typed_value_t& out, ast_t* definingNode)
    {
      typed_value_t in2 = in;
      in2.intrinsic_mutability = MUTABLE;
      return new Type::Function (Type::Function::FUNCTION, (new Signature ())
                                 ->Append (new parameter_t (definingNode, "h", in2, false)),
                                 new parameter_t (definingNode, "0return", out, false));
    }
  };

  Copy::Copy (ast_t* dn)
    : Template ("copy",
                dn,
                new Type::Template ())
  { }

  typed_value_t
  Copy::instantiate (TypedValueListType& tvlist)
  {
    if (tvlist.size () != 1)
      {
        error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                       "copy expects one argument (E11)");
      }

    typed_value_t in = tvlist[0];
    typed_value_t out = typed_value_t::copy (definingNode->location, in);
    if (out.type == NULL)
      {
        error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                       "cannot copy expression of type %s (E12)", in.type->ToString ().c_str ());
      }

    return typed_value_t (new CopyImpl (in, out, definingNode));
  }

  struct PrintlnImpl : public Callable
  {
    PrintlnImpl (const TypedValueListType& tvlist, ast_t* definingNode)
      : function_type_ (makeFunctionType (tvlist, definingNode))
    { }

    virtual void call (executor_base_t& exec, const ast_call_expr_t& node) const
    {
      exec.lock_stdout ();
      ast_t* expr_list = node.args ();
      for (ast_t::const_iterator pos = expr_list->begin (),
           limit = expr_list->end ();
           pos != limit;
           ++pos)
        {
          ast_t* child = *pos;
          evaluate_expr (exec, child);
          struct visitor : public Visitor
          {
            executor_base_t& exec;
            visitor (executor_base_t& e) : exec (e) { }

            void default_action (const Type::Type& type)
            {
              type_not_reached (type);
            }

            void visit (const Bool& type)
            {
              Bool::ValueType b;
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

            void visit (const Pointer& type)
            {
              void* ptr = stack_frame_pop_pointer (exec.stack ());
              printf ("%p", ptr);
            }

            void visit (const Uint& type)
            {
              Uint::ValueType u;
              stack_frame_pop (exec.stack (), u);
              printf ("%lu", u);
            }

            void visit (const Uint8& type)
            {
              Uint8::ValueType u;
              stack_frame_pop (exec.stack (), u);
              printf ("%u", u);
            }

            void visit (const Uint16& type)
            {
              Uint16::ValueType u;
              stack_frame_pop (exec.stack (), u);
              printf ("%u", u);
            }

            void visit (const Uint64& type)
            {
              Uint64::ValueType u;
              stack_frame_pop (exec.stack (), u);
              printf ("%lu", u);
            }

            void visit (const Int& type)
            {
              Int::ValueType u;
              stack_frame_pop (exec.stack (), u);
              printf ("%ld", u);
            }

            void visit (const Int8& type)
            {
              Int8::ValueType u;
              stack_frame_pop (exec.stack (), u);
              printf ("%d", u);
            }

            void visit (const Float64& type)
            {
              Float64::ValueType u;
              stack_frame_pop (exec.stack (), u);
              printf ("%g", u);
            }

            void visit (const Slice& type)
            {
              Slice::ValueType u;
              stack_frame_pop (exec.stack (), u);
              if (type_strip_cast<Uint8> (type.Base ()))
                {
                  fwrite (u.ptr, 1, u.length, stdout);
                }
              else
                {
                  printf ("slice");
                }
            }
          };
          visitor v (exec);
          type_strip (child->typed_value.type)->Accept (v);
        }
      printf ("\n");
      exec.unlock_stdout ();
    }

    virtual const Type::Type* type () const
    {
      return function_type_;
    }
    const typed_value_t in_;
    const Type::Type* const function_type_;
    static const Type::Type* makeFunctionType (const TypedValueListType& tvlist, ast_t* definingNode)
    {
      Signature* sig = new Signature ();
      for (TypedValueListType::const_iterator pos = tvlist.begin (),
           limit = tvlist.end ();
           pos != limit;
           ++pos)
        {
          typed_value_t in = *pos;
          in.intrinsic_mutability = MUTABLE;
          sig->Append (new parameter_t (definingNode, "", in, false));
        }

      return new Type::Function (Type::Function::FUNCTION, sig,
                                 new parameter_t (definingNode, "0return", typed_value_t::make_value (Void::Instance (), IMMUTABLE, IMMUTABLE), false));
    }
  };

  Println::Println (ast_t* dn)
    : Template ("println",
                dn,
                new Type::Template ())
  { }

  typed_value_t
  Println::instantiate (TypedValueListType& tvlist)
  {
    return typed_value_t (new PrintlnImpl (tvlist, definingNode));
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
