#include "composition.hpp"

#include <error.h>

#include <set>

#include "ast.hpp"
#include "ast_visitor.hpp"
#include "bind.hpp"
#include "field.hpp"
#include "action.hpp"
#include "callable.hpp"
#include "semantic.hpp"
#include "evaluate_static.hpp"
#include "reaction.hpp"
#include "stack.hpp"
#include "runtime.hpp"

namespace composition
{
using namespace type;
using namespace ast;
using namespace decl;
using namespace runtime;

Instance::Instance (Instance* p,
                    size_t a,
                    const type::NamedType* t,
                    Initializer* i,
                    ast_instance_t* n,
                    const std::string& aName)
  : parent (p)
  , address (a)
  , type (t)
  , component (NULL)
  , initializer (i)
  , node (n)
  , name (aName)
{ }

size_t
Instance::Offset () const
{
  if (parent == NULL)
    {
      return 0;
    }
  else
    {
      return address - parent->address;
    }
}

void
InstanceSet::Insert (const value_type& v)
{
  std::pair<iterator, bool> p = insert (v);
  p.first->second = std::max (p.first->second, v.second);
}

bool
InstanceSet::Compatible (const InstanceSet& other) const
{
  const_iterator pos1, pos2, limit1, limit2;
  pos1 = begin ();
  limit1 = end ();
  pos2 = other.begin ();
  limit2 = other.end ();
  while (pos1 != limit1 && pos2 != limit2)
    {
      if (*pos1 == *pos2 &&
          pos1->second == AccessWrite)
        {
          return false;
        }

      if (pos1->first < pos2->first)
        {
          ++pos1;
        }
      else if (pos1->first > pos2->first)
        {
          ++pos2;
        }
      else
        {
          ++pos1;
          ++pos2;
        }
    }

  return true;
}

void
InstanceSet::Union (const InstanceSet& other)
{
  for (const_iterator pos = other.begin (), limit = other.end ();
       pos != limit;
       ++pos)
    {
      Insert (*pos);
    }
}

Action::Action (Instance* i,
                decl::Action* a,
                type::Int::ValueType p)
  : Node (getname (i, a, p))
  , instance (i)
  , action (a)
  , iota (p)
{ }

size_t
Action::OutgoingCount () const
{
  return nodes.size ();
}

Node*
Action::OutgoingNode (size_t i) const
{
  return nodes.at (i);
}

const InstanceSet&
Action::GetInstanceSet ()
{
  if (instance_set_computed)
    {
      return instance_set;
    }

  for (NodesType::const_iterator pos = nodes.begin (),
       limit = nodes.end ();
       pos != limit;
       ++pos)
    {
      instance_set.Union ((*pos)->GetInstanceSet ());
    }

  // Is the instance read in the precondition?
  instance_set.Insert (std::make_pair (instance, action->precondition_access));
  // Is the instance read in the immutable phase?
  instance_set.Insert (std::make_pair (instance, action->immutable_phase_access));

  instance_set_computed = true;
  return instance_set;
}

std::string
Action::getname (Instance* i, decl::Action* a, type::Int::ValueType p)
{
  std::stringstream str;
  str << i->name << '.' << a->name;
  if (a->has_dimension ())
    {
      str << '[' << p << ']';
    }
  return str.str ();
}

ReactionKey::ReactionKey (Instance* i, const reaction_t* a, type::Int::ValueType p)
  : instance (i)
  , reaction (a)
  , iota (p)
{ }

bool
ReactionKey::operator< (const ReactionKey& other) const
{
  if (this->instance != other.instance)
    {
      return this->instance < other.instance;
    }
  if (this->reaction != other.reaction)
    {
      return this->reaction < other.reaction;
    }
  return this->iota < other.iota;
}

Reaction::Reaction (Instance* i,
                    reaction_t* a,
                    type::Int::ValueType p)
  : Node (getname (i, a, p))
  , instance (i)
  , reaction (a)
  , iota (p)
{ }

size_t
Reaction::OutgoingCount () const
{
  return nodes.size ();
}

Node*
Reaction::OutgoingNode (size_t i) const
{
  return nodes.at (i);
}

const InstanceSet&
Reaction::GetInstanceSet ()
{
  if (instance_set_computed)
    {
      return instance_set;
    }

  for (NodesType::const_iterator pos = nodes.begin (),
       limit = nodes.end ();
       pos != limit;
       ++pos)
    {
      instance_set.Union ((*pos)->GetInstanceSet ());
    }

  // Is the instance read in the immutable phase?
  instance_set.Insert (std::make_pair (instance, reaction->immutable_phase_access));

  instance_set_computed = true;
  return instance_set;
}

std::string
Reaction::getname (Instance* i, reaction_t* a, type::Int::ValueType p)
{
  std::stringstream str;
  str << i->name << '.' << a->name;
  if (a->has_dimension ())
    {
      str << '[' << p << ']';
    }
  return str.str ();
}

GetterKey::GetterKey (Instance* i, const Callable* g)
  : instance (i)
  , getter (g)
{ }

bool
GetterKey::operator< (const GetterKey& other) const
{
  if (this->instance != other.instance)
    {
      return this->instance < other.instance;
    }
  return this->getter < other.getter;
}

Getter::Getter (Instance* i,
                decl::Getter* g)
  : Node (i->name + "." + g->name)
  , instance (i)
  , getter (g)
{ }

size_t
Getter::OutgoingCount () const
{
  return nodes.size ();
}

Node*
Getter::OutgoingNode (size_t i) const
{
  return nodes.at (i);
}

const InstanceSet&
Getter::GetInstanceSet ()
{
  if (instance_set_computed)
    {
      return instance_set;
    }

  for (NodesType::const_iterator pos = nodes.begin (),
       limit = nodes.end ();
       pos != limit;
       ++pos)
    {
      instance_set.Union ((*pos)->GetInstanceSet ());
    }

  // Is the instance read in the immutable phase?
  instance_set.Insert (std::make_pair (instance, getter->immutable_phase_access));

  instance_set_computed = true;
  return instance_set;
}

Activation::Activation (Instance* i, const ast_activate_statement_t* as)
  : Node (getname (this))
  , instance (i)
  , activate_statement (as)
{ }

size_t
Activation::OutgoingCount () const
{
  return nodes.size ();
}

Node*
Activation::OutgoingNode (size_t i) const
{
  return nodes.at (i);
}

const InstanceSet&
Activation::GetInstanceSet ()
{
  if (instance_set_computed)
    {
      return instance_set;
    }

  for (NodesType::const_iterator pos = nodes.begin (),
       limit = nodes.end ();
       pos != limit;
       ++pos)
    {
      const InstanceSet& s = (*pos)->GetInstanceSet ();
      if (instance_set.Compatible (s))
        {
          instance_set.Union (s);
        }
      else
        {
          error (-1, 0, "system is non-deterministic (E137)");
        }
    }

  instance_set.Insert (std::make_pair (instance, activate_statement->mutable_phase_access));

  instance_set_computed = true;
  return instance_set;
}

std::string
Activation::getname (Activation* a)
{
  std::stringstream str;
  str << a;
  return str.str ();
}

PushPort::PushPort (size_t a, Instance* oi, field_t* of, const std::string& name)
  : Node (name)
  , address (a)
  , instance (oi)
  , field (of)
{ }

size_t
PushPort::OutgoingCount () const
{
  return reactions.size ();
}

Node*
PushPort::OutgoingNode (size_t i) const
{
  return reactions.at (i);
}

const InstanceSet&
PushPort::GetInstanceSet ()
{
  if (instance_set_computed)
    {
      return instance_set;
    }

  for (ReactionsType::const_iterator pos = reactions.begin (),
       limit = reactions.end ();
       pos != limit;
       ++pos)
    {
      const InstanceSet& s = (*pos)->GetInstanceSet ();
      if (instance_set.Compatible (s))
        {
          instance_set.Union (s);
        }
      else
        {
          error (-1, 0, "system is non-deterministic (E138)");
        }
    }

  instance_set_computed = true;
  return instance_set;
}

PullPort::PullPort (size_t a, Instance* oi, field_t* of, const std::string& name)
  : Node (name)
  , address (a)
  , instance (oi)
  , field (of)
{ }

size_t
PullPort::OutgoingCount () const
{
  return getters.size ();
}

Node*
PullPort::OutgoingNode (size_t i) const
{
  return getters.at (i);
}

const InstanceSet&
PullPort::GetInstanceSet ()
{
  if (instance_set_computed)
    {
      return instance_set;
    }

  for (GettersType::const_iterator pos = getters.begin (),
       limit = getters.end ();
       pos != limit;
       ++pos)
    {
      const InstanceSet& s = (*pos)->GetInstanceSet ();
      if (instance_set.Compatible (s))
        {
          instance_set.Union (s);
        }
      else
        {
          error (-1, 0, "system is non-deterministic (E120)");
        }
    }

  instance_set_computed = true;
  return instance_set;
}

void
Composer::AddInstance (Instance* instance)
{
  instances[instance->address] = instance;
}

void
Composer::AddPushPort (size_t address,
                       Instance* output_instance,
                       field_t* output_field,
                       const std::string& name)
{
  push_ports[address] = new PushPort (address, output_instance, output_field, name);
}

void
Composer::AddPullPort (size_t address,
                       Instance* input_instance,
                       field_t* input_field,
                       const std::string& name)
{
  pull_ports[address] = new PullPort (address, input_instance, input_field, name);
}

struct Executor : public executor_base_t
{
  Executor () : executor_base_t (1024, NULL) { }
  virtual heap_t* heap () const
  {
    unimplemented;
  }
  virtual void heap (heap_t* heap)
  {
    unimplemented;
  }
  virtual void push ()
  {
    unimplemented;
  }
};

void
Composer::elaborateBindings ()
{
  // For each instance.
  for (Composer::InstancesType::const_iterator instance_pos = this->instances.begin (),
       instance_limit = this->instances.end ();
       instance_pos != instance_limit;
       ++instance_pos)
    {
      Instance *instance = instance_pos->second;
      const NamedType *type = instance->type;
      // Enumerate the bindings.
      for (NamedType::BindsType::const_iterator bind_pos = type->BindsBegin (),
           bind_limit = type->BindsEnd ();
           bind_pos != bind_limit;
           ++bind_pos)
        {
          struct visitor : public DefaultConstVisitor
          {
            Composer& table;
            Executor exec;

            visitor (Composer& t, size_t receiver_address, const bind_t* b) : table (t)
            {
              // Build a stack frame.
              exec.stack ().push_pointer(reinterpret_cast<void*> (receiver_address));
              exec.stack ().push_pointer (NULL);
              exec.stack ().setup (b->memory_model.LocalsSize ());
            }

            void default_action (const Node& node)
            {
              not_reached;
            }

            void visit (const ast_if_statement_t& node)
            {
              node.condition ()->operation->execute (exec);
              if (node.condition ()->expression_kind == kVariable)
                {
                  void* ptr = exec.stack ().pop_pointer ();
                  exec.stack ().load (ptr, node.condition ()->type->Size ());
                }
              Bool::ValueType c;
              exec.stack ().pop (c);
              if (c)
                {
                  node.true_branch ()->Accept (*this);
                }
              else
                {
                  node.false_branch ()->Accept (*this);
                }
            }

            void visit (const ast_list_statement_t& node)
            {
              node.VisitChildren (*this);
            }

            void visit (const ast_for_iota_statement_t& node)
            {
              for (Int::ValueType idx = 0, limit = node.limit;
                   idx != limit;
                   ++idx)
                {
                  Int::ValueType* i = static_cast<Int::ValueType*> (exec.stack ().get_address (node.symbol->offset ()));
                  *i = idx;
                  node.body ()->Accept (*this);
                }
            }

            void visit (const ast_bind_t& node)
            {
              node.body ()->Accept (*this);
            }

            void bind (ast::Node* left, ast::Node* right, Int::ValueType param = 0)
            {
              left->operation->execute (exec);
              void* port = exec.stack ().pop_pointer ();
              right->At (0)->operation->execute (exec);
              void* reaction_component = exec.stack ().pop_pointer ();
              if (type_dereference (right->At (0)->type))
                {
                  exec.stack ().load (reaction_component, right->At (0)->type->Size ());
                  reaction_component = exec.stack ().pop_pointer ();
                }
              const reaction_t* reaction = static_cast<const reaction_t*> (right->callable);
              PushPortsType::const_iterator pp_pos = table.push_ports.find (reinterpret_cast<size_t> (port));
              assert (pp_pos != table.push_ports.end ());
              PushPort* pp = pp_pos->second;
              InstancesType::const_iterator i_pos = table.instances.find (reinterpret_cast<size_t> (reaction_component));
              assert (i_pos != table.instances.end ());
              Instance* i = i_pos->second;
              ReactionsType::const_iterator r_pos = table.reactions.find (ReactionsType::key_type (i, reaction, param));
              assert (r_pos != table.reactions.end ());
              Reaction* r = r_pos->second;
              pp->reactions.push_back (r);
              r->push_ports.push_back (pp);
            }

            void visit (const ast_bind_push_port_statement_t& node)
            {
              bind (node.left (), node.right ());
            }

            void visit (const ast_bind_push_port_param_statement_t& node)
            {
              node.param ()->operation->execute (exec);
              if (node.param ()->expression_kind == kVariable)
                {
                  void* ptr = exec.stack ().pop_pointer ();
                  exec.stack ().load (ptr, node.param ()->type->Size ());
                }
              Int::ValueType idx;
              exec.stack ().pop (idx);
              bind (node.left (), node.right (), idx);
            }

            void visit (const ast_bind_pull_port_statement_t& node)
            {
              node.left ()->operation->execute (exec);
              void* port = exec.stack ().pop_pointer ();
              node.right ()->At (0)->operation->execute (exec);
              void* getter_component = exec.stack ().pop_pointer ();
              const decl::Getter* getter = static_cast<const decl::Getter*> (node.right ()->callable);

              PullPortsType::const_iterator pp_pos = table.pull_ports.find (reinterpret_cast<size_t> (port));
              assert (pp_pos != table.pull_ports.end ());
              PullPort* pp = pp_pos->second;
              InstancesType::const_iterator i_pos = table.instances.find (reinterpret_cast<size_t> (getter_component));
              assert (i_pos != table.instances.end ());
              Instance* i = i_pos->second;
              GettersType::const_iterator g_pos = table.getters.find (GettersType::key_type (i, getter));
              assert (g_pos != table.getters.end ());
              Getter* g = g_pos->second;
              pp->getters.push_back (g);
            }
          };
          visitor v (*this, instance_pos->first, *bind_pos);
          (*bind_pos)->node ()->Accept (v);
        }
    }
}

static void tarjan (Node* n)
{
  switch (n->state)
    {
    case Node::Unmarked:
      n->state = Node::Temporary;
      for (size_t i = 0; i != n->OutgoingCount (); ++i)
        {
          Node* m = n->OutgoingNode (i);
          tarjan (m);
        }
      n->state = Node::Marked;
      break;
    case Node::Temporary:
      error (-1, 0, "composition is recursive (E72)");
      break;
    case Node::Marked:
      // Do nothing.
      break;
    }
}

void
Composer::enumerateActions ()
{
  for (InstancesType::const_iterator pos = instances.begin (),
       limit = instances.end ();
       pos != limit;
       ++pos)
    {
      Instance* instance = pos->second;
      const NamedType* type = instance->type;
      for (NamedType::ActionsType::const_iterator pos = type->ActionsBegin (),
           limit = type->ActionsEnd ();
           pos != limit;
           ++pos)
        {
          decl::Action* action = *pos;
          if (action->has_dimension ())
            {
              for (type::Int::ValueType idx = 0; idx != action->dimension; ++idx)
                {
                  instance->actions.push_back (new Action (instance, action, idx));
                }
            }
          else
            {
              instance->actions.push_back (new Action (instance, action));
            }
        }
    }
}

// Determine what relationship the given entity has with other entities.
// These relationships are created through activate statements and calls to
// getters and pull ports.
struct Composer::ElaborationVisitor : public DefaultConstVisitor
{
  Executor exec;
  Composer& table;
  Action* action;
  Reaction* reaction;
  Getter* getter;
  Activation* activation;

  ElaborationVisitor (Composer& t, Action* a)
    : table (t)
    , action (a)
    , reaction (NULL)
    , getter (NULL)
    , activation (NULL)
  {
    exec.stack ().push_pointer (reinterpret_cast<void*> (action->instance->address));
    if (action->action->has_dimension ())
      {
        exec.stack ().push (action->iota);
      }
    exec.stack ().push_pointer (NULL);
    exec.stack ().setup (action->action->memory_model.LocalsSize ());
  }

  ElaborationVisitor (Composer& t, Reaction* r)
    : table (t)
    , action (NULL)
    , reaction (r)
    , getter (NULL)
    , activation (NULL)
  {
    exec.stack ().push_pointer (reinterpret_cast<void*> (reaction->instance->address));
    if (reaction->reaction->has_dimension ())
      {
        exec.stack ().push (reaction->iota);
      }
    exec.stack ().reserve (reaction->reaction->signature ()->Size ());
    exec.stack ().push_pointer (NULL);
    exec.stack ().setup (reaction->reaction->locals_size ());
  }

  ElaborationVisitor (Composer& t, Getter* g)
    : table (t)
    , action (NULL)
    , reaction (NULL)
    , getter (g)
    , activation (NULL)
  {
    exec.stack ().push_pointer (reinterpret_cast<void*> (getter->instance->address));
    exec.stack ().reserve (getter->getter->signature ()->Size ());
    exec.stack ().push_pointer (NULL);
    exec.stack ().setup (getter->getter->locals_size ());
  }

  Instance* get_instance () const
  {
    if (action != NULL)
      {
        return action->instance;
      }
    else if (reaction != NULL)
      {
        return reaction->instance;
      }
    else
      {
        not_reached;
      }
  }

  void addCall (Node* n)
  {
    assert (n != NULL);
    if (action != NULL)
      {
        action->nodes.push_back (n);
      }
    else if (reaction != NULL)
      {
        reaction->nodes.push_back (n);
      }
    else if (getter != NULL)
      {
        getter->nodes.push_back (n);
      }
    else
      {
        not_reached;
      }
  }

  void default_action (const ast::Node& node)
  {
    ast_not_reached (node);
  }

  void visit (const ast_list_statement_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (const ast_expression_statement_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (const ast_return_statement_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (const ast_activate_statement_t& node)
  {
    if (action != NULL)
      {
        activation = new Activation (action->instance, &node);
        action->nodes.push_back (activation);
      }
    else if (reaction != NULL)
      {
        activation = new Activation (reaction->instance, &node);
        reaction->nodes.push_back (activation);
      }
    else
      {
        not_reached;
      }
    node.expr_list ()->Accept (*this);
  }

  void visit (const ast_list_expr_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (const ast_push_port_call_expr_t& node)
  {
    size_t port = activation->instance->address + node.field->offset;
    // Find what is bound to this port.
    Composer::PushPortsType::const_iterator port_pos = table.push_ports.find (port);
    assert (port_pos != table.push_ports.end ());
    activation->nodes.push_back (port_pos->second);
    node.args ()->Accept (*this);
  }

  void visit (const ast_indexed_port_call_expr_t& node)
  {
    node.index ()->operation->execute (exec);
    if (node.index ()->expression_kind == kVariable)
      {
        void* ptr = exec.stack ().pop_pointer ();
        exec.stack ().load (ptr, node.index ()->type->Size ());
      }

    Int::ValueType idx;
    exec.stack ().pop (idx);
    if (idx < 0)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "port index is negative (E100)");
      }
    if (idx >= node.array_type->dimension)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "port index is negative (E75)");
      }

    size_t port = activation->instance->address + node.field->offset + idx * node.array_type->UnitSize ();

    // Find what is bound to this port.
    Composer::PushPortsType::const_iterator port_pos = table.push_ports.find (port);
    assert (port_pos != table.push_ports.end ());
    activation->nodes.push_back (port_pos->second);
    node.args ()->Accept (*this);
  }

  void visit (const ast_call_expr_t& node)
  {
    if (node.expr ()->expression_kind != kType)
      {
        // Are we calling a getter or pull port.
        const type::Method* method = node.method_type;
        if (method != NULL && method->method_kind == type::Method::GETTER)
          {
            node.expr ()->At (0)->operation->execute (exec);
            if (node.expr ()->At (0)->expression_kind == kVariable &&
                type_dereference (node.expr ()->At (0)->type))
              {
                void* ptr = exec.stack ().pop_pointer ();
                exec.stack ().load (ptr, node.expr ()->At (0)->type->Size ());
              }
            size_t inst_addr = reinterpret_cast<size_t> (exec.stack ().pop_pointer ());
            InstancesType::const_iterator i_pos = table.instances.find (inst_addr);
            assert (i_pos != table.instances.end ());
            Instance* i = i_pos->second;
            const Callable* g = node.callable;
            assert (g != NULL);
            GettersType::const_iterator g_pos = table.getters.find (GetterKey (i, g));
            assert (g_pos != table.getters.end ());
            addCall (g_pos->second);
          }

        const type::Function* function = node.function_type;
        if (function != NULL && function->function_kind == type::Function::PULL_PORT)
          {
            size_t port = get_instance ()->address + node.field->offset;
            Composer::PullPortsType::const_iterator port_pos = table.pull_ports.find (port);
            assert (port_pos != table.pull_ports.end ());
            addCall (port_pos->second);
          }
      }

    node.VisitChildren (*this);
  }

  void visit (const ast_implicit_dereference_expr_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (const ast_identifier_expr_t& node)
  {
    // Do nothing.
  }

  void visit (const ast_implicit_conversion_expr_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (const ast_literal_expr_t& node)
  {
    // Do nothing.
  }

  void visit (const ast_binary_arithmetic_expr_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (const ast_unary_arithmetic_expr_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (const ast_address_of_expr_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (const ast_select_expr_t& node)
  {
    node.base ()->Accept (*this);
  }

  void visit (const ast_dereference_expr_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (const ast_var_statement_t& node)
  {
    node.expression_list ()->Accept (*this);
  }

  void visit (const ast_assign_statement_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (const TypeExpression& node)
  {
  }

  void visit (const ast_change_statement_t& node)
  {
    node.expr ()->Accept (*this);
    node.body ()->Accept (*this);
  }

  void visit (const ast_empty_statement_t& node)
  {
  }

  void visit (const ast_index_expr_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (const ast_if_statement_t& node)
  {
    node.VisitChildren (*this);
  }
};

void
Composer::elaborateActions ()
{
  for (InstancesType::const_iterator ipos = instances.begin (),
       ilimit = instances.end ();
       ipos != ilimit;
       ++ipos)
    {
      for (ActionsType::const_iterator pos = ipos->second->actions.begin (),
           limit = ipos->second->actions.end ();
           pos != limit;
           ++pos)
        {
          Action* action = *pos;
          ElaborationVisitor v (*this, action);
          action->action->body->Accept (v);
        }
    }
}

void
Composer::enumerateReactions ()
{
  for (InstancesType::const_iterator pos = instances.begin (),
       limit = instances.end ();
       pos != limit;
       ++pos)
    {
      Instance* instance = pos->second;
      const NamedType* type = instance->type;
      for (NamedType::ReactionsType::const_iterator pos = type->ReactionsBegin (),
           limit = type->ReactionsEnd ();
           pos != limit;
           ++pos)
        {
          reaction_t* reaction = *pos;
          if (reaction->has_dimension ())
            {
              for (type::Int::ValueType idx = 0; idx != reaction->dimension (); ++idx)
                {
                  reactions.insert (std::make_pair (ReactionKey (instance, reaction, idx), new Reaction (instance, reaction, idx)));
                }
            }
          else
            {
              reactions.insert (std::make_pair (ReactionKey (instance, reaction), new Reaction (instance, reaction)));
            }
        }
    }
}

void
Composer::elaborateReactions ()
{
  for (ReactionsType::const_iterator pos = reactions.begin (),
       limit = reactions.end ();
       pos != limit;
       ++pos)
    {
      Reaction* reaction = pos->second;
      ElaborationVisitor v (*this, reaction);
      reaction->reaction->body->Accept (v);
    }
}

void
Composer::enumerateGetters ()
{
  for (InstancesType::const_iterator pos = instances.begin (),
       limit = instances.end ();
       pos != limit;
       ++pos)
    {
      Instance* instance = pos->second;
      const NamedType* type = instance->type;
      for (NamedType::GettersType::const_iterator pos = type->GettersBegin (),
           limit = type->GettersEnd ();
           pos != limit;
           ++pos)
        {
          decl::Getter* getter = *pos;
          getters.insert (std::make_pair (GetterKey (instance, getter), new Getter (instance, getter)));
        }
    }
}

void
Composer::elaborateGetters ()
{
  for (GettersType::const_iterator pos = getters.begin (),
       limit = getters.end ();
       pos != limit;
       ++pos)
    {
      Getter* getter = pos->second;
      ElaborationVisitor v (*this, getter);
      getter->getter->node->body ()->Accept (v);
    }
}

void
Composer::checkStructure ()
{
  // Check that no reaction is bound more than once.
  for (ReactionsType::const_iterator pos = reactions.begin (),
       limit = reactions.end ();
       pos != limit;
       ++pos)
    {
      if (pos->second->push_ports.size () > 1)
        {
          // TODO:  Better error message.  Something like
          // reaction XXXX is activated by AAA, BBB, and CCC.
          error (-1, 0, "reaction bound more than once (E71)");
        }
    }

  // Check that every pull port is bound exactly once.
  for (Composer::PullPortsType::const_iterator pos = pull_ports.begin (),
       limit = pull_ports.end ();
       pos != limit;
       ++pos)
    {
      if (pos->second->getters.empty ())
        {
          error (-1, 0, "unbound pull port (E118)");
        }
      else if (pos->second->getters.size () > 1)
        {
          error (-1, 0, "multiply bound pull port (E119)");
        }
    }

  // Prove that the graph is acyclic.
  // We'll use Tarjan's algorithm on the actions since they are the roots.
  for (InstancesType::const_iterator ipos = instances.begin (),
       ilimit = instances.end ();
       ipos != ilimit;
       ++ipos)
    {
      for (ActionsType::const_iterator pos = ipos->second->actions.begin (),
           limit = ipos->second->actions.end ();
           pos != limit;
           ++pos)
        {
          tarjan (*pos);
        }
    }
}

void
Composer::computeInstanceSets ()
{
  for (InstancesType::const_iterator ipos = instances.begin (),
       ilimit = instances.end ();
       ipos != ilimit;
       ++ipos)
    {
      for (ActionsType::const_iterator pos = ipos->second->actions.begin (),
           limit = ipos->second->actions.end ();
           pos != limit;
           ++pos)
        {
          (*pos)->GetInstanceSet ();
        }
    }
}

void
Composer::ElaborateComposition ()
{
  // At this point, we have instances, push ports, and pull ports.
  // Enumerate getters first because they can be called by actions and reactions.
  enumerateGetters ();
  elaborateGetters ();
  enumerateActions ();
  elaborateActions ();
  enumerateReactions ();
  elaborateReactions ();
  // At this point, we have instances, push ports, pull ports, actions, reactions, and activations.
  // The edges from actions and reactions to activations are present.
  // The edges from activations to push ports are present.
  elaborateBindings ();
}

void
Composer::AnalyzeComposition ()
{
  checkStructure ();
  computeInstanceSets ();
}

static void collect (std::set<Node*>& nodes, Node* n)
{
  if (nodes.count (n) == 0)
    {
      nodes.insert (n);
      for (size_t i = 0; i != n->OutgoingCount (); ++i)
        {
          collect (nodes, n->OutgoingNode (i));
        }
    }
}

void
Composer::DumpGraphviz () const
{
  // Collect all the nodes.
  // Necessary if graph is cyclic.
  std::set<Node*> nodes;
  for (InstancesType::const_iterator ipos = instances.begin (),
       ilimit = instances.end ();
       ipos != ilimit;
       ++ipos)
    {
      Instance* instance = ipos->second;
      for (ActionsType::const_iterator apos = instance->actions.begin (),
           alimit = instance->actions.end ();
           apos != alimit;
           ++apos)
        {
          collect (nodes, *apos);
        }
    }

  std::cout << "digraph {\n";
  for (std::set<Node*>::const_iterator pos = nodes.begin (),
       limit = nodes.end ();
       pos != limit;
       ++pos)
    {
      Node* node = *pos;
      std::cout << '"' << node->name << '"' << " -> " << '{';
      for (size_t i = 0; i != node->OutgoingCount (); ++i)
        {
          std::cout << ' ' << '"' << node->OutgoingNode (i)->name << '"';
        }
      std::cout << "}\n";
    }
  std::cout << "}\n";
}
}
