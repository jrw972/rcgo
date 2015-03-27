#include "runtime.hpp"

namespace runtime
{

  struct port_t {
    component_t* instance;
    const reaction_t* reaction;
    int64_t parameter;
    port_t* next;

    port_t (component_t* i, const reaction_t* r, int64_t p) : instance (i), reaction (r), parameter (p), next (NULL) { }
  };

  struct heap_link_t {
    heap_t* heap;
    pthread_mutex_t mutex;
    size_t change_count;
  };

  static void
  call (executor_base_t& exec);

  enum ControlAction {
    RETURN,
    CONTINUE,
  };

  static ControlAction
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
  bind (port_t** output_port, component_t* input_instance, const reaction_t* reaction, int64_t parameter)
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
    for (instance_table_t::PortsType::const_iterator output_pos = instance_table.ports.begin (),
           output_limit = instance_table.ports.end ();
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

    for (instance_table_t::PfuncsType::const_iterator input_pos = instance_table.pfuncs.begin (),
           input_limit = instance_table.pfuncs.end ();
         input_pos != input_limit;
         ++input_pos)
      {
        instance_t* input_instance = input_pos->second.input_instance;
        size_t input_pfunc = input_pos->first - input_instance->address ();
        instance_table_t::OutputType output = *input_pos->second.outputs.begin ();
        pfunc_t* pfunc = reinterpret_cast<pfunc_t*> (reinterpret_cast<char*> (input_instance->ptr ()) + input_pfunc);
        if (output.instance != NULL)
          {
            pfunc->instance = output.instance->ptr ();
            pfunc->method = output.method;
          }
        else
          {
            pfunc->instance = NULL;
            pfunc->function = output.function;
          }
      }
  }

  void
  initialize (executor_base_t& exec, instance_t* instance)
  {
    if (instance->is_top_level ())
      {
        char* top_before = stack_frame_top (exec.stack ());
        // Push this.
        stack_frame_push_pointer (exec.stack (), instance->ptr ());
        // Push a fake instruction pointer.
        stack_frame_push_pointer (exec.stack (), NULL);
        char* top_after = stack_frame_top (exec.stack ());
        stack_frame_push_pointer (exec.stack (), instance->method ()->node);
        call (exec);
        stack_frame_pop (exec.stack (), top_after - top_before);
      }
  }

  static void
  call (executor_base_t& exec)
  {
    const ast_t* node = static_cast<const ast_t*> (stack_frame_pop_pointer (exec.stack ()));

    struct visitor : public ast_const_visitor_t
    {
      executor_base_t& exec;

      visitor (executor_base_t& e) : exec (e) { }

      void default_action (const ast_method_t& node)
      {
        not_reached;
      }

      void visit (const ast_function_t& node)
      {
        function_t* function = get_current_function (&node);
        stack_frame_push_base_pointer (exec.stack (), function->memory_model.locals_size ());
        evaluate_statement (exec, node.at (FUNCTION_BODY));
        stack_frame_pop_base_pointer (exec.stack ());
      }

      void visit (const ast_method_t& node)
      {
        method_t* method = get_current_method (&node);
        stack_frame_push_base_pointer (exec.stack (), method->memory_model.locals_size ());
        evaluate_statement (exec, node.body ());
        stack_frame_pop_base_pointer (exec.stack ());
      }
    };

    visitor v (exec);
    node->accept (v);
  }

  static void
  evaluate_expr (executor_base_t& exec,
                 ast_t* node)
  {
    typed_value_t tv = ast_get_typed_value (node);
    if (tv.has_value)
      {
        switch (tv.kind)
          {
          case typed_value_t::VALUE:
            {
              struct visitor : public const_type_visitor_t
              {
                executor_base_t& exec;
                const typed_value_t& tv;

                visitor (executor_base_t& e,
                         const typed_value_t& t) : exec (e), tv (t) { }

                void visit (const named_type_t& type)
                {
                  type.subtype ()->accept (*this);
                }

                void visit (const bool_type_t&)
                {
                  stack_frame_push_bool (exec.stack (), tv.bool_value);
                }

                void visit (const int_type_t&)
                {
                  stack_frame_push_int (exec.stack (), tv.int_value);
                }

                void visit (const uint_type_t&)
                {
                  stack_frame_push_uint (exec.stack (), tv.uint_value);
                }

                void visit (const string_type_t&)
                {
                  stack_frame_push_string (exec.stack (), tv.string_value);
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
                  stack_frame_push_pointer (exec.stack (), tv.function_value->node);
                }

                void visit (const method_type_t&)
                {
                  stack_frame_push_pointer (exec.stack (), tv.method_value->node);
                }

                void visit (const enum_type_t&)
                {
                  stack_frame_push_uint (exec.stack (), tv.enum_value);
                }

                void default_action (const type_t& type)
                {
                  not_reached;
                }
              };

              visitor v (exec, tv);
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
        not_reached;
      }

      void visit (const ast_indexed_port_call_expr_t& node)
      {
        // Determine the trigger index.
        evaluate_expr (exec, node.index ());
        int64_t idx = stack_frame_pop_int (exec.stack ());
        if (idx < 0 || static_cast<size_t> (idx) >= node.array_type->dimension ())
          {
            error_at_line (-1, 0, node.file, node.line,
                           "array index is out of bounds");
          }

        port_call (node, node.args (), node.field, idx * node.array_type->element_size ());
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
            int64_t idx = stack_frame_pop_int (exec.stack ());
            if (idx < 0 || static_cast<size_t> (idx) >= type.dimension ())
              {
                error_at_line (-1, 0, node.file, node.line,
                               "array index is out of bounds");
              }
            stack_frame_push_pointer (exec.stack (),
                                      static_cast<char*> (ptr) + idx * type.element_size ());
          }
        };
        visitor v (exec, node);
        ast_get_typed_value (node.base ()).type->accept (v);
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
        typed_value_t tv = node.get_type ();
        const type_t* type = dynamic_cast<const pointer_type_t*> (tv.type)->base_type ();
        if (!type_cast<heap_type_t> (type))
          {
            void* ptr = heap_allocate (exec.heap (), type->size ());
            // Return the instance.
            stack_frame_push_pointer (exec.stack (), ptr);
          }
        else
          {
            const type_t* t = dynamic_cast<const heap_type_t*> (type)->base_type ();
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
        // Create space for the return.
        typed_value_t return_tv = node.get_type ();
        stack_frame_reserve (exec.stack (), return_tv.type->size ());

        // Sample the top of the stack.
        char* top_before = stack_frame_top (exec.stack ());

        switch (node.kind)
          {
          case ast_call_expr_t::NONE:
            not_reached;
          case ast_call_expr_t::FUNCTION:
            break;
          case ast_call_expr_t::METHOD:
            evaluate_expr (exec, node.expr ()->children[0]->children[0]);
            break;
          case ast_call_expr_t::PFUNC:
            unimplemented;
          }

        // Push the arguments.
        evaluate_expr (exec, node.args ());

        // Push a fake instruction pointer.
        stack_frame_push_pointer (exec.stack (), NULL);

        // Sample the top.
        char* top_after = stack_frame_top (exec.stack ());

        // Push the thing to call.
        evaluate_expr (exec, node.expr ());

        // Perform the call.
        call (exec);

        // Pop the arguments.
        stack_frame_pop (exec.stack (), top_after - top_before);
      }

      void port_call (const ast_t& node, ast_t* args, const field_t* field, size_t offset = 0)
      {
        // Push all of the arguments first and measure their size.
        char* top_before = stack_frame_top (exec.stack ());
        evaluate_expr (exec, args);
        char* top_after = stack_frame_top (exec.stack ());
        ptrdiff_t arguments_size = top_after - top_before; // Assumes stack grows up.

        // Find the port to trigger.
        symbol_t* this_ = node.symtab->get_this ();
        stack_frame_push (exec.stack (), symbol_get_offset (this_), symbol_parameter_type (this_)->size ());
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
                stack_frame_push_int (exec.stack (), port->parameter);
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

      void visit (const ast_port_call_expr_t& node)
      {
        port_call (node, node.args (), node.field);
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
        typed_value_t tv = node.get_type ();
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
        symbol_t* symbol = node.symbol.symbol ();
        ptrdiff_t offset = symbol_get_offset (symbol);
        stack_frame_push_address (exec.stack (), offset);
      }

      void visit (const ast_logic_not_expr_t& node)
      {
        evaluate_expr (exec, node.child ());
        bool b = stack_frame_pop_bool (exec.stack ());
        stack_frame_push_bool (exec.stack (), !b);
      }

      void visit (const ast_implicit_dereference_expr_t& node)
      {
        evaluate_expr (exec, node.child ());
        void* ptr = stack_frame_pop_pointer (exec.stack ());
        typed_value_t tv = node.get_type ();
        stack_frame_load (exec.stack (), ptr, tv.type->size ());
      }

      void visit (const ast_binary_arithmetic_expr_t& node)
      {
        switch (node.arithmetic)
          {
          case ast_binary_arithmetic_expr_t::EQUAL:
            {
              evaluate_expr (exec, node.left ());
              evaluate_expr (exec, node.right ());
              typed_value_t tv = ast_get_typed_value (node.left ());
              stack_frame_equal (exec.stack (), tv.type->size ());
            }
            break;
          case ast_binary_arithmetic_expr_t::NOT_EQUAL:
            {
              evaluate_expr (exec, node.left ());
              evaluate_expr (exec, node.right ());
              typed_value_t tv = ast_get_typed_value (node.left ());
              stack_frame_not_equal (exec.stack (), tv.type->size ());
            }
            break;
          case ast_binary_arithmetic_expr_t::LOGIC_OR:
            {
              evaluate_expr (exec, node.left ());
              bool b = stack_frame_pop_bool (exec.stack ());
              if (b)
                {
                  stack_frame_push_bool (exec.stack (), true);
                }
              else
                {
                  evaluate_expr (exec, node.right ());
                }
            }
            break;
          case ast_binary_arithmetic_expr_t::LOGIC_AND:
            {
              evaluate_expr (exec, node.left ());
              bool b = stack_frame_pop_bool (exec.stack ());
              if (b)
                {
                  evaluate_expr (exec, node.right ());
                }
              else
                {
                  stack_frame_push_bool (exec.stack (), false);
                }
            }
            break;
          case ast_binary_arithmetic_expr_t::ADD:
            {
              evaluate_expr (exec, node.left ());
              evaluate_expr (exec, node.right ());

              struct visitor : public const_type_visitor_t
              {
                executor_base_t& exec;
                const ast_binary_arithmetic_expr_t& node;

                visitor (executor_base_t& e, const ast_binary_arithmetic_expr_t& n) : exec (e), node (n) { }

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
                  uint64_t right = stack_frame_pop_uint (exec.stack ());
                  uint64_t left = stack_frame_pop_uint (exec.stack ());
                  switch (node.arithmetic)
                    {
                    case ast_binary_arithmetic_expr_t::EQUAL:
                    case ast_binary_arithmetic_expr_t::NOT_EQUAL:
                    case ast_binary_arithmetic_expr_t::LOGIC_OR:
                    case ast_binary_arithmetic_expr_t::LOGIC_AND:
                      not_reached;
                    case ast_binary_arithmetic_expr_t::ADD:
                      stack_frame_push_uint (exec.stack (), left + right);
                      break;
                    case ast_binary_arithmetic_expr_t::SUBTRACT:
                      stack_frame_push_uint (exec.stack (), left - right);
                      break;
                    }
                }

                void visit (const int_type_t& type)
                {
                  int64_t right = stack_frame_pop_int (exec.stack ());
                  int64_t left = stack_frame_pop_int (exec.stack ());
                  switch (node.arithmetic)
                    {
                    case ast_binary_arithmetic_expr_t::EQUAL:
                    case ast_binary_arithmetic_expr_t::NOT_EQUAL:
                    case ast_binary_arithmetic_expr_t::LOGIC_OR:
                    case ast_binary_arithmetic_expr_t::LOGIC_AND:
                      not_reached;
                    case ast_binary_arithmetic_expr_t::ADD:
                      stack_frame_push_int (exec.stack (), left + right);
                      break;
                    case ast_binary_arithmetic_expr_t::SUBTRACT:
                      stack_frame_push_int (exec.stack (), left - right);
                      break;
                    }
                }
              };
              visitor v (exec, node);
              node.get_type ().type->accept (v);
            }
            break;
          case ast_binary_arithmetic_expr_t::SUBTRACT:
            unimplemented;
          }
      }
    };
    visitor v (exec);
    node->accept (v);
  }

  static ControlAction
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
        not_reached;
      }

      void visit (const ast_empty_statement_t& node)
      { }

      void visit (const ast_assign_statement_t& node)
      {
        ast_t* left = node.left ();
        ast_t* right = node.right ();
        // Determine the size of the value being assigned.
        size_t size = ast_get_typed_value (right).type->size ();
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
          symbol_t* symbol = node.root_symbol.symbol ();
          assert (symbol != NULL);
          ptrdiff_t offset = symbol_get_offset (symbol);
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
        ast_t* child = node.at (UNARY_CHILD);
        // Determine the size of the value being generated.
        size_t size = ast_get_typed_value (child).type->size ();
        // Evaluate.
        evaluate_expr (exec, child);
        // Remove value.
        stack_frame_pop (exec.stack (), size);
      }

      void visit (const ast_if_statement_t& node)
      {
        evaluate_expr (exec, node.condition ());
        bool c = stack_frame_pop_bool (exec.stack ());
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
            bool c = stack_frame_pop_bool (exec.stack ());
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
        for (size_t idx = 0; idx != node.limit; ++idx)
          {
            size_t* ptr = static_cast<size_t*> (stack_frame_address_for_offset (exec.stack (), symbol_get_offset (node.symbol.symbol ())));
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
        const type_t* type = ast_get_typed_value (node.right ()).type;
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
            *((uint64_t*)ptr) += stack_frame_pop_uint (exec.stack ());
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
        const type_t* type = ast_get_typed_value (node.right ()).type;
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
            *((uint64_t*)ptr) -= stack_frame_pop_uint (exec.stack ());
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
        AST_FOREACH (child, &node)
          {
            if (evaluate_statement (exec, child) == RETURN)
              {
                retval = RETURN;
                return;
              }
          }
      }

      void visit (const ast_return_statement_t& node)
      {
        // Evaluate the expression.
        evaluate_expr (exec, node.at (UNARY_CHILD));
        // Store in the return parameter.
        stack_frame_store_stack (exec.stack (), symbol_get_offset (node.return_symbol), symbol_parameter_type (node.return_symbol)->size ());
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
            ++(*static_cast<uint64_t*> (ptr));
          }
        };
        visitor v (ptr);
        ast_get_typed_value (node.child ()).type->accept (v);
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
            symbol_t* symbol = node.symbols[idx].symbol ();
            stack_frame_clear_stack (exec.stack (), symbol_get_offset (symbol), symbol_variable_type (symbol)->size ());
          }
      }

      void visit (const ast_println_statement_t& node)
      {
        exec.lock_stdout ();
        ast_t* expr_list = node.at (UNARY_CHILD);
        AST_FOREACH (child, expr_list)
          {
            evaluate_expr (exec, child);
            struct visitor : public const_type_visitor_t
            {
              executor_base_t& exec;
              visitor (executor_base_t& e) : exec (e) { }

              void default_action (const type_t& type)
              {
                not_reached;
              }

              void visit (const named_type_t& type)
              {
                type.subtype ()->accept (*this);
              }

              void visit (const bool_type_t& type)
              {
                bool b = stack_frame_pop_bool (exec.stack ());
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
                uint64_t u = stack_frame_pop_uint (exec.stack ());
                printf ("%lu", u);
              }

              void visit (const int_type_t& type)
              {
                int64_t u = stack_frame_pop_int (exec.stack ());
                printf ("%lu", u);
              }

              void visit (const iota_type_t& type)
              {
                int64_t u = stack_frame_pop_int (exec.stack ());
                printf ("%lu", u);
              }
            };
            visitor v (exec);
            ast_get_typed_value (child).type->accept (v);
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
    ast_t* precondition;
    if (dynamic_cast<ast_action_t*> (action->node ()))
      {
        precondition = dynamic_cast<ast_action_t*> (action->node ())->precondition ();
      }
    else
      {
        precondition = dynamic_cast<ast_dimensioned_action_t*> (action->node ())->precondition ();
      }

    // Set the current instance.
    exec.current_instance (instance);

    // Push this.
    stack_frame_push_pointer (exec.stack (), instance);
    // Push iota.
    if (action->has_dimension ())
      {
        stack_frame_push_uint (exec.stack (), iota);
      }
    // Push an instruction pointer.
    stack_frame_push_pointer (exec.stack (), NULL);
    stack_frame_push_base_pointer (exec.stack (), 0 /* No locals. */);
    evaluate_expr (exec, precondition);
    bool retval = stack_frame_pop_bool (exec.stack ());
    stack_frame_pop_base_pointer (exec.stack ());
    // Pop the instruction pointer.
    stack_frame_pop_pointer (exec.stack ());
    // Pop iota.
    if (action->has_dimension ())
      {
        stack_frame_pop_uint (exec.stack ());
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

    ast_t* body;
    if (dynamic_cast<ast_reaction_t*> (reaction->node ()))
      {
        body = dynamic_cast<ast_reaction_t*> (reaction->node ())->body ();
      }
    else
      {
        body = dynamic_cast<ast_dimensioned_reaction_t*> (reaction->node ())->body ();
      }

    stack_frame_push_base_pointer (exec.stack (), reaction->memory_model.locals_size ());

    evaluate_statement (exec, body);
  }

  static void
  execute (executor_base_t& exec,
           const action_t* action,
           component_t* instance)
  {
    // Set the current instance.
    exec.current_instance (instance);

    ast_t* body;
    if (dynamic_cast<ast_action_t*> (action->node ()))
      {
        body = dynamic_cast<ast_action_t*> (action->node ())->body ();
      }
    else
      {
        body = dynamic_cast<ast_dimensioned_action_t*> (action->node ())->body ();
      }

    // Reset the mutable phase base pointer.
    exec.mutable_phase_base_pointer (0);

    stack_frame_push_base_pointer (exec.stack (), action->memory_model.locals_size ());

    evaluate_statement (exec, body);

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
        symbol_t* this_ = b->symtab->get_this ();
        stack_frame_push (exec.stack (), symbol_get_offset (this_), symbol_parameter_type (this_)->size ());
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
        stack_frame_push_uint (exec.stack (), iota);
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
        stack_frame_pop_uint (exec.stack ());
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
