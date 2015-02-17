#include "instance.hpp"
#include "util.hpp"
#include "type.hpp"
#include "debug.hpp"
#include <error.h>
#include "action.hpp"
#include "trigger.hpp"
#include <vector>
#include <set>
#include "ast.hpp"
#include "symbol.hpp"
#include "bind.hpp"
#include "field.hpp"

instance_table_t *
instance_table_make (void)
{
  instance_table_t *t = new instance_table_t;
  return t;
}

instance_t *
instance_table_insert (instance_table_t * table, instance_t* parent, const named_type_t * type, method_t* method, size_t address)
{
  instance_t *i = new instance_t (parent, address, type, method);
  table->instances.insert (std::make_pair (address, i));
  return i;
}

void
instance_table_insert_port (instance_table_t* table,
                            size_t address,
                            instance_t* output_instance)
{
  table->ports[address] = instance_table_t::PortValueType (output_instance);
}

void
instance_table_enumerate_bindings (instance_table_t * table)
{
  // For each instance.
  for (instance_table_t::InstancesType::const_iterator instance_pos = table->instances.begin (),
         instance_limit = table->instances.end ();
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
          instance_table_t* table;
          const size_t receiver_address;
          size_t computed_address;
          reaction_t* reaction;

          visitor (instance_table_t* t, size_t ra) : table (t), receiver_address (ra), computed_address (0), reaction (NULL) { }

          void default_action (const ast_t& node)
          {
            not_reached;
          }

          void visit (const ast_bind_t& node)
          {
            node.body ()->accept (*this);
          }

          void visit (const ast_bind_statement_list_t& node)
          {
            node.visit_children (*this);
          }

          void visit (const ast_bind_statement_t& node)
          {
            // Process the left to get a port.
            computed_address = -1;
            node.left ()->accept (*this);
            size_t port = computed_address;

            // Process the right to get an instance and a reaction.
            computed_address = -1;
            reaction = NULL;
            node.right ()->accept (*this);
            size_t input = computed_address;
            reaction_t* r = reaction;

            instance_table_t::InputType i (table->instances[input], r);
            table->ports[port].inputs.insert (i);
            table->reverse_ports[i].insert (port);
          }

          void visit (const ast_select_expr_t& node)
          {
            computed_address = -1;
            node.base ()->accept (*this);
            if (node.field)
              {
                computed_address += field_offset (node.field);
              }
            else if (node.reaction)
              {
                reaction = node.reaction;
              }
            else
              {
                not_reached;
              }
          }

          void visit (const ast_dereference_expr_t& node)
          {
            ast_identifier_expr_t* id = dynamic_cast<ast_identifier_expr_t*> (node.child ());
            if (id)
              {
                symbol_t* symbol = id->symbol.symbol ();
                switch (symbol_kind (symbol))
                  {
                  case SymbolParameter:
                    if (symbol_parameter_kind (symbol) == ParameterReceiver)
                      {
                        computed_address = receiver_address;
                      }
                    break;
                  case SymbolFunction:
                  case SymbolInstance:
                  case SymbolType:
                  case SymbolTypedConstant:
                  case SymbolVariable:
                    break;
                  }
              }
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
transitive_closure (const instance_table_t * table,
		    instance_t * instance,
                    const action_reaction_base_t * action)
{
  struct port_call_visitor : public ast_const_visitor_t
  {
    action_set_t set;
    const instance_table_t* table;
    const size_t address;

    port_call_visitor (const instance_table_t* t, size_t a) : table (t), address (a) { }

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

    void visit (const ast_port_call_expr_t& node)
    {
      size_t port = address + field_offset (node.field);
      // Find what is bound to this port.
      instance_table_t::PortsType::const_iterator port_pos = table->ports.find (port);
      assert (port_pos != table->ports.end ());

      instance_table_t::InputsType::const_iterator pos = port_pos->second.inputs.begin ();
      if (pos != table->ports.find (port)->second.inputs.end ())
        {
          assert (port_pos->second.inputs.size () == 1);
          instance_t* inst = pos->first;
          // Merge the sets.
          if (set.merge_no_upgrade (transitive_closure (table, inst, pos->second)))
            {
              error (-1, 0, "system is non-deterministic");
            }
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
          computed_address += field_offset (node.field);
        }
    }

    void visit (const ast_dereference_expr_t& node)
    {
      ast_identifier_expr_t* id = dynamic_cast<ast_identifier_expr_t*> (node.child ());
      if (id)
        {
          symbol_t* symbol = id->symbol.symbol ();
          switch (symbol_kind (symbol))
            {
            case SymbolParameter:
              if (symbol_parameter_kind (symbol) == ParameterReceiver)
                {
                  computed_address = receiver_address;
                }
              break;
            case SymbolFunction:
            case SymbolInstance:
            case SymbolType:
            case SymbolTypedConstant:
            case SymbolVariable:
              break;
            }
        }
    }

  };

  struct immutable_phase_visitor : public ast_const_visitor_t
  {
    const instance_table_t * table;
    size_t receiver_address;
    action_set_t& set;

    immutable_phase_visitor (const instance_table_t * t, size_t ra, action_set_t& s) : table (t), receiver_address (ra), set (s) { }

    void default_action (const ast_t& node)
    {
      not_reached;
    }

    void visit (const ast_action_t& node)
    {
      node.precondition ()->accept (*this);
      node.body ()->accept (*this);
    }

    void visit (const ast_reaction_t& node)
    {
      node.body ()->accept (*this);
    }

    void visit (const ast_list_statement_t& node)
    {
      node.visit_children (*this);
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

    void visit (const ast_not_equal_expr_t& node)
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

    void visit (const ast_identifier_expr_t& node)
    { }

    void visit (const ast_literal_expr_t& node)
    { }

    void visit (const ast_port_call_expr_t& node)
    {
      node.args ()->accept (*this);
    }

    void visit (const ast_println_statement_t& node)
    {
      node.visit_children (*this);
    }

    void visit (const ast_call_expr_t& node)
    {
      // See if the first argument is a component.
      if (!node.args ()->children.empty ())
        {
          offset_visitor v (receiver_address);
          node.args ()->children[0]->accept (v);
          if (v.computed_address != static_cast<size_t> (-1))
            {
              instance_table_t::InstancesType::const_iterator pos = table->instances.find (v.computed_address);
              if (pos != table->instances.end ())
                {
                  set.immutable_phase.insert (std::make_pair (pos->second, TRIGGER_READ));
                }
            }
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
      port_call_visitor v (table, instance->address ());
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
  action->node ()->accept (v);

  return set;
}

void
instance_table_analyze_composition (const instance_table_t * table)
{
  // Check that no reaction is bound more than once.
  for (instance_table_t::ReversePortsType::const_iterator pos1 = table->reverse_ports.begin (),
         limit = table->reverse_ports.end ();
       pos1 != limit;
       ++pos1)
    {
      if (pos1->second.size () > 1)
        {
          error (-1, 0, "reaction bound more than once");
        }
    }

  // For each instance.
  for (instance_table_t::InstancesType::const_iterator instance_pos = table->instances.begin (),
         instance_limit = table->instances.end ();
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
	action_set_t set = transitive_closure (table, instance, *action_pos);
        // Combine the immutable and mutable sets.
        set.mutable_phase.insert (set.immutable_phase.begin (), set.immutable_phase.end ());
        instance->instance_sets.push_back (std::make_pair (*action_pos, set.mutable_phase));
      }
  }
}

void
instance_table_dump (const instance_table_t * table)
{
  {
    printf ("instances\n");
    printf ("%s\t%s\n", "address", "type");
    for (instance_table_t::InstancesType::const_iterator pos = table->instances.begin (),
           limit = table->instances.end ();
         pos != limit;
         ++pos)
    {
      printf ("%zd\t%s\n", pos->first, pos->second->type ()->to_string ().c_str ());
    }
    printf ("\n");
  }

  {
    printf ("ports\n");
    printf ("%s\t%s\n", "address", "inputs");
    for (instance_table_t::PortsType::const_iterator pos = table->ports.begin (),
           limit = table->ports.end ();
         pos != limit;
         ++pos)
    {
      printf ("%zd\t", pos->first);

      for (instance_table_t::InputsType::const_iterator p = pos->second.inputs.begin (),
             l = pos->second.inputs.end ();
           p != l;
           ++p)
        {
          printf ("(%zd,%s) ", p->first->address (), get (p->second->name ()));
        }
      printf ("\n");
    }
    printf ("\n");
  }
}
