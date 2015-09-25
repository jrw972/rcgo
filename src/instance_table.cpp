#include "instance_table.hpp"
#include "ast.hpp"
#include "bind.hpp"
#include "field.hpp"
#include <error.h>
#include "trigger.hpp"
#include "action.hpp"
#include "Callable.hpp"

instance_table_t::ActionsType
instance_table_t::actions () const
{
  ActionsType retval;
  for (instance_table_t::InstancesType::const_iterator instance_pos = instances.begin (),
         instance_limit = instances.end ();
       instance_pos != instance_limit;
       ++instance_pos)
    {
      for (instance_t::InstanceSetsType::const_iterator action_pos = instance_pos->second->instance_sets.begin (),
             action_limit = instance_pos->second->instance_sets.end ();
           action_pos != action_limit;
           ++action_pos)
        {
          retval.push_back (*action_pos);
        }
    }
  return retval;
}

class static_memory_t
{
public:

  void set_value_at_offset (ptrdiff_t offset, size_t value)
  {
    stack_[offset] = value;
  }

  size_t get_value_at_offset (ptrdiff_t offset) const
  {
    return stack_.find (offset)->second;
  }

private:
  typedef std::map<ptrdiff_t, size_t> StackType;
  StackType stack_;
};

struct static_value_t
{
  enum Kind
    {
      STACK_ADDRESS,
      ABSOLUTE_ADDRESS,
      VALUE,
    };

  Kind kind;
  union {
    ptrdiff_t offset; // STACK_ADDRESS
    size_t address; // ABSOLUTE_ADDRESS
    size_t value; // VALUE
  };

  static_value_t ()
    : kind (VALUE)
    , value (0)
  { }

  static static_value_t make_stack_offset (ptrdiff_t offset)
  {
    static_value_t v;
    v.kind = STACK_ADDRESS;
    v.offset = offset;
    return v;
  }

  static static_value_t make_value (size_t value)
  {
    static_value_t v;
    v.kind = VALUE;
    v.value = value;
    return v;
  }

  static static_value_t implicit_dereference (static_value_t in, const static_memory_t& memory)
  {
    static_value_t out;
    switch (in.kind)
      {
      case STACK_ADDRESS:
        out.kind = VALUE;
        out.value = memory.get_value_at_offset (in.offset);
        break;
      case ABSOLUTE_ADDRESS:
        unimplemented;
      case VALUE:
        not_reached;
      }
    return out;
  }

  static static_value_t dereference (static_value_t in)
  {
    static_value_t out;
    switch (in.kind)
      {
      case STACK_ADDRESS:
      case ABSOLUTE_ADDRESS:
        not_reached;
      case VALUE:
        out.kind = ABSOLUTE_ADDRESS;
        out.address = in.value;
        break;
      }
    return out;
  }

  static static_value_t select (static_value_t in, ptrdiff_t offset)
  {
    static_value_t out;
    switch (in.kind)
      {
      case STACK_ADDRESS:
        out = in;
        out.offset += offset;
        break;
      case ABSOLUTE_ADDRESS:
        out = in;
        out.address += offset;
        break;
      case VALUE:
        not_reached;
      }
    return out;
  }

  static static_value_t index (static_value_t in, const array_type_t* type, static_value_t idx)
  {
    static_value_t out;
    switch (in.kind)
      {
      case STACK_ADDRESS:
        out = in;
        out.offset += type->element_size () * idx.value;
        break;
      case ABSOLUTE_ADDRESS:
        out = in;
        out.address += type->element_size () * idx.value;
        break;
      case VALUE:
        not_reached;
      }
    return out;
  }
};

std::ostream&
operator<< (std::ostream& o,
            const static_value_t& v)
{
  switch (v.kind)
    {
    case static_value_t::STACK_ADDRESS:
      o << "STACK " << v.offset;
      break;
    case static_value_t::ABSOLUTE_ADDRESS:
      o << "ABSOLUTE " << v.address;
      break;
    case static_value_t::VALUE:
      o << "VALUE " << v.value;
      break;
    }
  return o;
}

static static_value_t
evaluate_static (const ast_t* node, const static_memory_t& memory)
{
  struct visitor : public ast_const_visitor_t
  {
    const static_memory_t& memory;
    static_value_t result;

    visitor (const static_memory_t& m) : memory (m) { }

    void default_action (const ast_t& node)
    {
      not_reached;
    }

    void visit (const ast_binary_arithmetic_expr_t& node)
    {
      switch (node.arithmetic)
        {
        case MULTIPLY:
        case DIVIDE:
        case MODULUS:
        case LEFT_SHIFT:
        case RIGHT_SHIFT:
        case BIT_AND:
        case BIT_AND_NOT:
          unimplemented;

        case ADD:
        case SUBTRACT:
        case BIT_OR:
        case BIT_XOR:
          unimplemented;

        case EQUAL:
          unimplemented;
        case NOT_EQUAL:
          {
            static_value_t left = evaluate_static (node.left (), memory);
            static_value_t right = evaluate_static (node.right (), memory);
            result = static_value_t::make_value (left.value != right.value);
          }
          break;
        case LESS_THAN:
        case LESS_EQUAL:
        case MORE_THAN:
        case MORE_EQUAL:
          unimplemented;
        case LOGIC_AND:
          unimplemented;
        case LOGIC_OR:
          unimplemented;
        }
    }

    void visit (const ast_literal_expr_t& node)
    {
      typed_value_t tv = node.typed_value;
      struct visitor : public const_type_visitor_t
      {
        typed_value_t tv;
        static_value_t result;

        visitor (typed_value_t t) : tv (t) { }

        void default_action (const type_t& type)
        {
          not_reached;
        }

        void visit (const int_type_t& type)
        {
          unimplemented;
          //result = static_value_t::make_value (tv.int_value);
        }
      };
      visitor v (tv);
      tv.type->accept (v);
      result = v.result;
    }

    void visit (const ast_index_expr_t& node)
    {
      static_value_t base = evaluate_static (node.base (), memory);
      static_value_t index = evaluate_static (node.index (), memory);
      typed_value_t base_tv = node.base ()->typed_value;
      const array_type_t* array_type = type_cast<array_type_t> (base_tv.type);
      result = static_value_t::index (base, array_type, index);
    }

    void visit (const ast_select_expr_t& node)
    {
      typed_value_t tv = node.typed_value;
      assert (tv.has_offset);
      static_value_t v = evaluate_static (node.base (), memory);
      result = static_value_t::select (v, tv.offset);
    }

    void visit (const ast_dereference_expr_t& node)
    {
      static_value_t v = evaluate_static (node.child (), memory);
      result = static_value_t::dereference (v);
    }

    void visit (const ast_implicit_dereference_expr_t& node)
    {
      static_value_t v = evaluate_static (node.child (), memory);
      result = static_value_t::implicit_dereference (v, memory);
    }

    void visit (const ast_identifier_expr_t& node)
    {
      ptrdiff_t offset = node.symbol.symbol ()->offset ();
      result = static_value_t::make_stack_offset (offset);
    }
  };
  visitor v (memory);
  node->accept (v);
  return v.result;
}

void
instance_table_enumerate_bindings (instance_table_t& table)
{
  // For each instance.
  for (instance_table_t::InstancesType::const_iterator instance_pos = table.instances.begin (),
         instance_limit = table.instances.end ();
       instance_pos != instance_limit;
       ++instance_pos)
    {
      instance_t *instance = instance_pos->second;
      const named_type_t *type = instance->type ();
      // Enumerate the bindings.
      for (named_type_t::BindsType::const_iterator bind_pos = type->binds_begin (),
             bind_limit = type->binds_end ();
           bind_pos != bind_limit;
           ++bind_pos)
        {
          struct visitor : public ast_const_visitor_t
          {
            instance_table_t& table;
            const size_t receiver_address;
            static_memory_t memory;

            visitor (instance_table_t& t, size_t ra) : table (t), receiver_address (ra) { }

            void default_action (const ast_t& node)
            {
              not_reached;
            }

            void visit (const ast_if_statement_t& node)
            {
              static_value_t c = evaluate_static (node.condition (), memory);
              if (c.value != 0)
                {
                  node.true_branch ()->accept (*this);
                }
              else
                {
                  node.false_branch ()->accept (*this);
                }
            }

            void visit (const ast_list_statement_t& node)
            {
              node.visit_children (*this);
            }

            void visit (const ast_for_iota_statement_t& node)
            {
              for (int_type_t::ValueType idx = 0, limit = node.limit.integral_value ();
                   idx != limit;
                   ++idx)
                {
                  memory.set_value_at_offset (node.symbol.symbol ()->offset (), idx);
                  node.body ()->accept (*this);
                }
            }

            void visit (const ast_bind_t& node)
            {
              memory.set_value_at_offset (node.this_symbol.symbol ()->offset (), receiver_address);
              node.body ()->accept (*this);
            }

            void bind (ast_t* left, ast_t* right, static_value_t param = static_value_t ())
            {
              static_value_t port = evaluate_static (left, memory);
              // Strip off the implicit dereference and selecting of the reaction.
              static_value_t input = evaluate_static (right->children[0]->children[0], memory);
              typed_value_t reaction = right->typed_value;

              assert (port.kind == static_value_t::ABSOLUTE_ADDRESS);
              assert (input.kind == static_value_t::ABSOLUTE_ADDRESS);
              assert (param.kind == static_value_t::VALUE);

              instance_table_t::InputType i (table.instances[input.address], reaction.value.reaction_value (), param.value);
              table.push_ports[port.address].inputs.insert (i);
              table.reverse_ports[i].insert (port.address);
            }

            void visit (const ast_bind_push_port_statement_t& node)
            {
              bind (node.left (), node.right ());
            }

            void visit (const ast_bind_push_port_param_statement_t& node)
            {
              static_value_t param = evaluate_static (node.param (), memory);
              bind (node.left (), node.right (), param);
            }

            void visit (const ast_bind_pull_port_statement_t& node)
            {
              static_value_t pull_port = evaluate_static (node.left (), memory);
              assert (pull_port.kind == static_value_t::ABSOLUTE_ADDRESS);
              typed_value_t tv = node.right ()->typed_value;
              const getter_type_t* getter_type = type_cast<getter_type_t> (tv.type);
              assert (getter_type != NULL);
              // Strip off the implicit dereference and selecting of the getter.
              static_value_t input = evaluate_static (node.right ()->children[0]->children[0], memory);
              assert (input.kind == static_value_t::ABSOLUTE_ADDRESS);
              instance_table_t::OutputType o (table.instances[input.address], dynamic_cast<Getter*> (tv.value.callable_value ()));
              table.pull_ports[pull_port.address].outputs.insert (o);
              return;
            }
          };
          visitor v (table, instance_pos->first);
          (*bind_pos)->node ()->accept (v);
        }
    }
}

struct action_set_t
{
  // Instances implied by the immutable phase.
  // The TriggerAction should always be READ.
  instance_set_t immutable_phase;
  // Instances implied by the mutable phase.
  // The TriggerAction may be READ or WRITE.
  instance_set_t mutable_phase;

  void merge_with_upgrade (const action_set_t& other)
  {
    // Immutable merges are straight union.
    immutable_phase.insert (other.immutable_phase.begin (), other.immutable_phase.end ());
    // Mutable phase is where the upgrade happens.
    for (instance_set_t::const_iterator pos = other.mutable_phase.begin (),
           limit = other.mutable_phase.end ();
         pos != limit;
         ++pos)
      {
        std::pair<instance_set_t::iterator, bool> x = mutable_phase.insert (*pos);
        if (!x.second && pos->second > x.first->second)
          {
            // Upgrade.
            x.first->second = pos->second;
          }
      }
  }

  bool merge_no_upgrade (const action_set_t& other)
  {
    // Immutable merges are straight union.
    immutable_phase.insert (other.immutable_phase.begin (), other.immutable_phase.end ());
    // Mutable phase is where the upgrade happens.
    for (instance_set_t::const_iterator pos = other.mutable_phase.begin (),
           limit = other.mutable_phase.end ();
         pos != limit;
         ++pos)
      {
        std::pair<instance_set_t::iterator, bool> x = mutable_phase.insert (*pos);
        if (!x.second && (pos->second == TRIGGER_WRITE || x.first->second == TRIGGER_WRITE))
          {
            // Two writers for same instance.
            return true;
          }
      }

    return false;
  }

};

/*
  Given an <instance, action> pair, compute the action_set_t checking
  for determinism.  The system is non-deterministic if two different
  actions attempt to mutate the same instance.
*/
static action_set_t
transitive_closure (const instance_table_t& table,
                    instance_t * instance,
                    const action_reaction_base_t * action,
                    size_t iota = 0)
{
  struct port_call_visitor : public ast_const_visitor_t
  {
    action_set_t set;
    const instance_table_t& table;
    const size_t address;
    size_t iota;
    bool have_port_index;
    ssize_t port_index;

    port_call_visitor (const instance_table_t& t, size_t a, size_t i) : table (t), address (a), iota (i) { }

    void default_action (const ast_t& node)
    {
      not_reached;
    }

    void visit (const ast_trigger_statement_t& node)
    {
      node.expr_list ()->accept (*this);
    }

    void visit (const ast_list_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_push_port_call_expr_t& node)
    {
      size_t port = address + node.field->offset;
      // Find what is bound to this port.
      instance_table_t::PortsType::const_iterator port_pos = table.push_ports.find (port);
      assert (port_pos != table.push_ports.end ());

      for (instance_table_t::InputsType::const_iterator pos = port_pos->second.inputs.begin (),
             limit = port_pos->second.inputs.end ();
           pos != limit;
           ++pos)
        {
          instance_t* inst = pos->instance;
          // Merge the sets.
          if (set.merge_no_upgrade (transitive_closure (table, inst, pos->reaction)))
            {
              error_at_line (-1, 0, node.location.file, node.location.line,
                             "system is non-deterministic");

            }
        }
    }

    void visit (const ast_indexed_port_call_expr_t& node)
    {
      have_port_index = false;
      node.index ()->accept (*this);

      if (!have_port_index)
        {
          error_at_line (-1, 0, node.location.file, node.location.line,
                         "port index is not constant");

        }

      if (port_index < 0)
        {
          error_at_line (-1, 0, node.location.file, node.location.line,
                         "port index is negative");
        }

      if (port_index >= node.array_type->dimension)
        {
          error_at_line (-1, 0, node.location.file, node.location.line,
                         "port index out of bounds");
        }

      size_t port = address + node.field->offset + port_index * node.array_type->element_size ();

      // Find what is bound to this port.
      instance_table_t::PortsType::const_iterator port_pos = table.push_ports.find (port);
      assert (port_pos != table.push_ports.end ());

      instance_table_t::InputsType::const_iterator pos = port_pos->second.inputs.begin ();
      if (pos != table.push_ports.find (port)->second.inputs.end ())
        {
          assert (port_pos->second.inputs.size () == 1);
          instance_t* inst = pos->instance;
          // Merge the sets.
          if (set.merge_no_upgrade (transitive_closure (table, inst, pos->reaction)))
            {
              error_at_line (-1, 0, node.location.file, node.location.line,
                             "system is non-deterministic");

            }
        }
    }

    void visit (const ast_implicit_dereference_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_identifier_expr_t& node)
    {
      const std::string& id = ast_get_identifier (node.child ());
      if (id == "IOTA")
        {
          have_port_index = true;
          port_index = iota;
        }
    }
  };

  struct offset_visitor : public ast_const_visitor_t
  {
    const size_t receiver_address;
    size_t computed_address;

    offset_visitor (size_t ra) : receiver_address (ra), computed_address (-1) { }

    void default_action (const ast_t& node)
    {
      not_reached;
    }

    void visit (const ast_identifier_expr_t& node)
    {
      ParameterSymbol* symbol = SymbolCast<ParameterSymbol> (node.symbol.symbol ());
      if (symbol != NULL &&
          symbol->kind == ParameterSymbol::Receiver) {
        computed_address = receiver_address;
      }
    }

    void visit (const ast_implicit_dereference_expr_t& node)
    {
      computed_address = -1;
      node.visit_children (*this);
    }

    void visit (const ast_address_of_expr_t& node)
    {
      computed_address = -1;
      node.visit_children (*this);
    }

    void visit (const ast_select_expr_t& node)
    {
      computed_address = -1;
      node.base ()->accept (*this);
      if (computed_address != static_cast<size_t> (-1))
        {
          typed_value_t tv = node.typed_value;
          assert (tv.has_offset);
          computed_address += tv.offset;
        }
    }

    void visit (const ast_dereference_expr_t& node)
    {
      computed_address = -1;
      node.child ()->accept (*this);
    }

  };

  struct immutable_phase_visitor : public ast_const_visitor_t
  {
    const instance_table_t& table;
    size_t receiver_address;
    action_set_t& set;

    immutable_phase_visitor (const instance_table_t& t, size_t ra, action_set_t& s) : table (t), receiver_address (ra), set (s) { }

    void default_action (const ast_t& node)
    {
      ast_not_reached (node);
    }

    void visit (const ast_empty_statement_t& node)
    { }

    void visit (const ast_address_of_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_action_t& node)
    {
      node.precondition ()->accept (*this);
      node.body ()->accept (*this);
    }

    void visit (const ast_dimensioned_action_t& node)
    {
      node.precondition ()->accept (*this);
      node.body ()->accept (*this);
    }

    void visit (const ast_reaction_t& node)
    {
      node.body ()->accept (*this);
    }

    void visit (const ast_dimensioned_reaction_t& node)
    {
      node.body ()->accept (*this);
    }

    void visit (const ast_list_statement_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_if_statement_t& node)
    {
      node.condition ()->accept (*this);
      node.true_branch ()->accept (*this);
      node.false_branch ()->accept (*this);
    }

    void visit (const ast_trigger_statement_t& node)
    {
      node.expr_list ()->accept (*this);
    }

    void visit (const ast_var_statement_t& node)
    {
    }

    void visit (const ast_assign_statement_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_change_statement_t& node)
    {
      node.expr ()->accept (*this);
      node.body ()->accept (*this);
    }

    void visit (const ast_new_expr_t& node)
    {
    }

    void visit (const ast_move_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_merge_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_list_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_logic_not_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_select_expr_t& node)
    {
      node.base ()->accept (*this);
    }

    void visit (const ast_dereference_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_implicit_dereference_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_identifier_expr_t& node)
    { }

    void visit (const ast_literal_expr_t& node)
    { }

    void visit (const ast_push_port_call_expr_t& node)
    {
      node.args ()->accept (*this);
    }

    void visit (const ast_indexed_port_call_expr_t& node)
    {
      node.index ()->accept (*this);
      node.args ()->accept (*this);
    }

    void visit (const ast_expression_statement_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_println_statement_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_index_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_slice_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_binary_arithmetic_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_call_expr_t& node)
    {
      node.visit_children (*this);

      const type_t* t = node.expr ()->typed_value.type;
      if (type_cast<getter_type_t> (t) != NULL) {
        offset_visitor v (receiver_address);
        node.expr ()->children[0]->children[0]->accept (v);
        instance_table_t::InstancesType::const_iterator pos = table.instances.find (v.computed_address);
        assert (pos != table.instances.end ());
        set.immutable_phase.insert (std::make_pair (pos->second, TRIGGER_READ));
      } else if (type_cast<pull_port_type_t> (t) != NULL) {
        offset_visitor v (receiver_address);
        node.expr ()->children[0]->accept (v);
        instance_table_t::PullPortsType::const_iterator pos = table.pull_ports.find (v.computed_address);
        assert (pos != table.pull_ports.end ());
        instance_table_t::OutputType out = *pos->second.outputs.begin ();
        set.immutable_phase.insert (std::make_pair (out.instance, TRIGGER_READ));
      }
    }
  };

  action_set_t set;
  TriggerAction local_action = TRIGGER_READ;

  // For each trigger in the action.
  for (action_reaction_base_t::TriggersType::const_iterator pos = action->begin (),
         limit = action->end ();
       pos != limit;
       ++pos)
    {
      // Determine the set for this trigger.
      port_call_visitor v (table, instance->address (), iota);
      (*pos)->node.accept (v);
      // Merge the sets from this trigger.
      set.merge_with_upgrade (v.set);
      // Determine if this action mutates the receiver.
      local_action = std::max (local_action, (*pos)->action);
    }

  // Add this action.
  if (!set.mutable_phase.insert (std::make_pair (instance, local_action)).second)
    {
      if (local_action == TRIGGER_WRITE)
        {
          // We are not the first writer of this instance.
          error (-1, 0, "system is non-deterministic");
        }
    }

  // Add instances accessed in the immutable phase.
  immutable_phase_visitor v (table, instance->address (), set);
  action->node->accept (v);

  return set;
}

void
instance_table_analyze_composition (const instance_table_t& table)
{
  // Check that no reaction is bound more than once.
  for (instance_table_t::ReversePortsType::const_iterator pos1 = table.reverse_ports.begin (),
         limit = table.reverse_ports.end ();
       pos1 != limit;
       ++pos1)
    {
      if (pos1->second.size () > 1)
        {
          error (-1, 0, "reaction bound more than once");
        }
    }

  // Check that every pull port is bound exactly once.
  for (instance_table_t::PullPortsType::const_iterator pos = table.pull_ports.begin (),
         limit = table.pull_ports.end ();
       pos != limit;
       ++pos)
    {
      if (pos->second.outputs.size () != 1)
        {
          error (-1, 0, "unbound pull port");
        }
    }

  // For each instance.
  for (instance_table_t::InstancesType::const_iterator instance_pos = table.instances.begin (),
         instance_limit = table.instances.end ();
       instance_pos != instance_limit;
       ++instance_pos)
    {
      instance_t *instance = instance_pos->second;
      const named_type_t *type = instance->type ();
      // For each action in the type.
      for (named_type_t::ActionsType::const_iterator action_pos = type->actions_begin (),
             action_end = type->actions_end ();
           action_pos != action_end;
           ++action_pos)
        {
          action_t* action = *action_pos;
          if (action->has_dimension ())
            {
              for (int_type_t::ValueType iota = 0, limit = action->dimension ().integral_value();
                   iota != limit;
                   ++iota)
                {
                  action_set_t set = transitive_closure (table, instance, action, iota);
                  // Combine the immutable and mutable sets.
                  set.mutable_phase.insert (set.immutable_phase.begin (), set.immutable_phase.end ());
                  instance->instance_sets.push_back (instance_t::ConcreteAction (action, set.mutable_phase, iota));
                }
            }
          else
            {
              action_set_t set = transitive_closure (table, instance, action);
              // Combine the immutable and mutable sets.
              set.mutable_phase.insert (set.immutable_phase.begin (), set.immutable_phase.end ());
              instance->instance_sets.push_back (instance_t::ConcreteAction (action, set.mutable_phase));
            }
        }
    }
}

std::ostream&
operator<< (std::ostream& o,
            const instance_t& i)
{
  o << '<' << *i.type () << ',' << i.address () << '>';
  return o;
}

std::ostream&
operator<< (std::ostream& o,
            const instance_t::ConcreteAction& ca)
{
  o << '{' << ca.action->node->location.line << ',' << ca.iota;
  for (instance_set_t::const_iterator pos = ca.set.begin (),
         limit = ca.set.end ();
       pos != limit;
       ++pos)
    {
      o << ',' << '(' << *pos->first << ',';
      switch (pos->second)
        {
        case TRIGGER_READ:
          o << "READ";
          break;
        case TRIGGER_WRITE:
          o << "WRITE";
          break;
        }
      o << ')';
    }
  o << '}';
  return o;
}

std::ostream&
operator<< (std::ostream& o,
            const instance_table_t::PortValueType& pv)
{
  o << '[' << *pv.output_instance << ',' << pv.output_field->name << ',' << pv.address << ']' << " ->";

  for (instance_table_t::InputsType::const_iterator p = pv.inputs.begin (),
         l = pv.inputs.end ();
       p != l;
       ++p)
    {
      o << ' ' << '(' << *p->instance << ',' << p->reaction->name << ',' << p->parameter << ')';
    }

  return o;
}

void
instance_table_dump (const instance_table_t& table)
{
  std::cout << "instances\n";
  for (instance_table_t::InstancesType::const_iterator pos = table.instances.begin (),
         limit = table.instances.end ();
       pos != limit;
       ++pos)
    {
      std::cout << *pos->second << '\n';
    }
  std::cout << '\n';

  std::cout << "ports\n";
  for (instance_table_t::PortsType::const_iterator pos = table.push_ports.begin (),
         limit = table.push_ports.end ();
       pos != limit;
       ++pos)
    {
      std::cout << pos->second << '\n';
    }
  std::cout << '\n';

  std::cout << "actions\n";
  typedef std::vector<instance_t::ConcreteAction> ActionsType;
  ActionsType actions;
  for (instance_table_t::InstancesType::const_iterator instance_pos = table.instances.begin (),
         instance_limit = table.instances.end ();
       instance_pos != instance_limit;
       ++instance_pos)
    {
      for (instance_t::InstanceSetsType::const_iterator action_pos = instance_pos->second->instance_sets.begin (),
             action_limit = instance_pos->second->instance_sets.end ();
           action_pos != action_limit;
           ++action_pos)
        {
          std::cout << "Action " << *instance_pos->second << ' ' << *action_pos << '\n';
          actions.push_back (*action_pos);
        }
    }
  std::cout << '\n';

  std::cout << "graph {\n";
  for (ActionsType::const_iterator pos = actions.begin (), limit = actions.end ();
       pos != limit;
       ++pos)
    {
      for (ActionsType::const_iterator pos2 = pos + 1; pos2 != limit; ++pos2)
        {
          if (!independent (pos->set.begin (), pos->set.end (), pos2->set.begin (), pos2->set.end ()))
            {
              std::cout << '"' << *pos << '"' << " -- " << '"' << *pos2 << '"' << '\n';
            }
        }
    }
  std::cout << "}\n";

  for (ActionsType::const_iterator pos = actions.begin (), limit = actions.end ();
       pos != limit;
       ++pos)
    {
      for (ActionsType::const_iterator pos2 = actions.begin (), limit2 = actions.end ();
           pos2 != limit2;
           ++pos2)
        {
          std::cout << *pos << ' ' << *pos2 << ' ' << independent (pos->set.begin (), pos->set.end (), pos2->set.begin (), pos2->set.end ()) << '\n';
        }
    }

}
