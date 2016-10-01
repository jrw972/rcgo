#include "composition.hpp"

#include <error.h>

#include <set>
#include <sstream>

#include "node.hpp"
#include "node_visitor.hpp"
#include "node_cast.hpp"
#include "bind.hpp"
#include "action.hpp"
#include "callable.hpp"
#include "semantic.hpp"
#include "evaluate_static.hpp"
#include "stack.hpp"
#include "runtime.hpp"
#include "parameter_list.hpp"
#include "executor_base.hpp"
#include "operation.hpp"

namespace composition
{
using namespace type;
using namespace ast;
using namespace decl;
using namespace runtime;

Instance::Instance (Instance* p,
                    size_t a,
                    const type::NamedType* t,
                    decl::Initializer* i,
                    const Operation* o,
                    const std::string& aName)
  : parent (p)
  , address (a)
  , type (t)
  , component (NULL)
  , initializer (i)
  , operation (o)
  , name (aName)
{ }

size_t
Instance::offset () const
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
InstanceSet::insert (const value_type& v)
{
  std::pair<iterator, bool> p = std::map<Instance*, ReceiverAccess>::insert (v);
  p.first->second = std::max (p.first->second, v.second);
}

bool
InstanceSet::is_compatible (const InstanceSet& other) const
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
InstanceSet::add_union (const InstanceSet& other)
{
  for (const_iterator pos = other.begin (), limit = other.end ();
       pos != limit;
       ++pos)
    {
      insert (*pos);
    }
}

Node::Node (const std::string& n)
  : name (n)
  , state (Unmarked)
  , instance_set_computed_ (false)
{ }

Action::Action (Instance* i,
                decl::Action* a,
                long p)
  : Node (getname (i, a, p))
  , instance (i)
  , action (a)
  , iota (p)
{ }

size_t
Action::outgoing_count () const
{
  return nodes.size ();
}

Node*
Action::outgoing_node (size_t i) const
{
  return nodes.at (i);
}

const InstanceSet&
Action::instance_set ()
{
  if (instance_set_computed_)
    {
      return instance_set_;
    }

  for (NodesType::const_iterator pos = nodes.begin (),
       limit = nodes.end ();
       pos != limit;
       ++pos)
    {
      instance_set_.add_union ((*pos)->instance_set ());
    }

  // Is the instance read in the precondition?
  instance_set_.insert (std::make_pair (instance, action->precondition_access));
  // Is the instance read in the immutable phase?
  instance_set_.insert (std::make_pair (instance, action->immutable_phase_access));

  instance_set_computed_ = true;
  return instance_set_;
}

std::string
Action::getname (Instance* i, decl::Action* a, long p)
{
  std::stringstream str;
  str << i->name << '.' << a->name;
  if (a->has_dimension ())
    {
      str << '[' << p << ']';
    }
  return str.str ();
}

ReactionKey::ReactionKey (Instance* i, const decl::Reaction* a, long p)
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
                    decl::Reaction* a,
                    long p)
  : Node (getname (i, a, p))
  , instance (i)
  , reaction (a)
  , iota (p)
{ }

size_t
Reaction::outgoing_count () const
{
  return nodes.size ();
}

Node*
Reaction::outgoing_node (size_t i) const
{
  return nodes.at (i);
}

const InstanceSet&
Reaction::instance_set ()
{
  if (instance_set_computed_)
    {
      return instance_set_;
    }

  for (NodesType::const_iterator pos = nodes.begin (),
       limit = nodes.end ();
       pos != limit;
       ++pos)
    {
      instance_set_.add_union ((*pos)->instance_set ());
    }

  // Is the instance read in the immutable phase?
  instance_set_.insert (std::make_pair (instance, reaction->immutable_phase_access));

  instance_set_computed_ = true;
  return instance_set_;
}

std::string
Reaction::getname (Instance* i, decl::Reaction* a, long p)
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
Getter::outgoing_count () const
{
  return nodes.size ();
}

Node*
Getter::outgoing_node (size_t i) const
{
  return nodes.at (i);
}

const InstanceSet&
Getter::instance_set ()
{
  if (instance_set_computed_)
    {
      return instance_set_;
    }

  for (NodesType::const_iterator pos = nodes.begin (),
       limit = nodes.end ();
       pos != limit;
       ++pos)
    {
      instance_set_.add_union ((*pos)->instance_set ());
    }

  // Is the instance read in the immutable phase?
  instance_set_.insert (std::make_pair (instance, getter->immutable_phase_access));

  instance_set_computed_ = true;
  return instance_set_;
}

Activation::Activation (Instance* i, ReceiverAccess mpa)
  : Node (getname (this))
  , instance (i)
  , mutable_phase_access (mpa)
{ }

size_t
Activation::outgoing_count () const
{
  return nodes.size ();
}

Node*
Activation::outgoing_node (size_t i) const
{
  return nodes.at (i);
}

const InstanceSet&
Activation::instance_set ()
{
  if (instance_set_computed_)
    {
      return instance_set_;
    }

  for (NodesType::const_iterator pos = nodes.begin (),
       limit = nodes.end ();
       pos != limit;
       ++pos)
    {
      const InstanceSet& s = (*pos)->instance_set ();
      if (instance_set_.is_compatible (s))
        {
          instance_set_.add_union (s);
        }
      else
        {
          error (-1, 0, "system is non-deterministic (E137)");
        }
    }

  instance_set_.insert (std::make_pair (instance, mutable_phase_access));

  instance_set_computed_ = true;
  return instance_set_;
}

std::string
Activation::getname (Activation* a)
{
  std::stringstream str;
  str << a;
  return str.str ();
}

PushPort::PushPort (size_t a, Instance* oi, Field* of, const std::string& name)
  : Node (name)
  , address (a)
  , instance (oi)
  , field (of)
{ }

size_t
PushPort::outgoing_count () const
{
  return reactions.size ();
}

Node*
PushPort::outgoing_node (size_t i) const
{
  return reactions.at (i);
}

const InstanceSet&
PushPort::instance_set ()
{
  if (instance_set_computed_)
    {
      return instance_set_;
    }

  for (ReactionsType::const_iterator pos = reactions.begin (),
       limit = reactions.end ();
       pos != limit;
       ++pos)
    {
      const InstanceSet& s = (*pos)->instance_set ();
      if (instance_set_.is_compatible (s))
        {
          instance_set_.add_union (s);
        }
      else
        {
          error (-1, 0, "system is non-deterministic (E138)");
        }
    }

  instance_set_computed_ = true;
  return instance_set_;
}

PullPort::PullPort (size_t a, Instance* oi, Field* of, const std::string& name)
  : Node (name)
  , address (a)
  , instance (oi)
  , field (of)
{ }

size_t
PullPort::outgoing_count () const
{
  return getters.size ();
}

Node*
PullPort::outgoing_node (size_t i) const
{
  return getters.at (i);
}

const InstanceSet&
PullPort::instance_set ()
{
  if (instance_set_computed_)
    {
      return instance_set_;
    }

  for (GettersType::const_iterator pos = getters.begin (),
       limit = getters.end ();
       pos != limit;
       ++pos)
    {
      const InstanceSet& s = (*pos)->instance_set ();
      if (instance_set_.is_compatible (s))
        {
          instance_set_.add_union (s);
        }
      else
        {
          error (-1, 0, "system is non-deterministic (E120)");
        }
    }

  instance_set_computed_ = true;
  return instance_set_;
}

void
Composer::add_instance (Instance* instance)
{
  instances_[instance->address] = instance;
}

void
Composer::add_push_port (size_t address,
                         Instance* output_instance,
                         Field* output_field,
                         const std::string& name)
{
  push_ports_[address] = new PushPort (address, output_instance, output_field, name);
}

void
Composer::add_pull_port (size_t address,
                         Instance* input_instance,
                         Field* input_field,
                         const std::string& name)
{
  pull_ports_[address] = new PullPort (address, input_instance, input_field, name);
}

struct Executor : public ExecutorBase
{
  Executor () : ExecutorBase (1024, NULL, 0) { }
  virtual runtime::Heap* heap () const
  {
    UNIMPLEMENTED;
  }
  virtual void heap (runtime::Heap* heap)
  {
    UNIMPLEMENTED;
  }
  virtual void push ()
  {
    UNIMPLEMENTED;
  }
};

void
Composer::elaborate_bindings ()
{
  // For each instance.
  for (Composer::InstancesType::const_iterator instance_pos = this->instances_.begin (),
       instance_limit = this->instances_.end ();
       instance_pos != instance_limit;
       ++instance_pos)
    {
      Instance *instance = instance_pos->second;
      const NamedType *type = instance->type;
      // Enumerate the bindings.
      for (NamedType::BindsType::const_iterator bind_pos = type->binds_begin (),
           bind_limit = type->binds_end ();
           bind_pos != bind_limit;
           ++bind_pos)
        {
          struct visitor : public ast::DefaultNodeVisitor
          {
            Composer& table;
            Executor exec;

            visitor (Composer& t, size_t receiver_address, const decl::Bind* b) : table (t)
            {
              // Build a stack frame.
              exec.stack ().push_pointer(reinterpret_cast<void*> (receiver_address));
              exec.stack ().push_pointer (NULL);
              exec.stack ().setup (b->memory_model.locals_size_on_stack ());
            }

            void default_action (const Node& node)
            {
              NOT_REACHED;
            }

            void visit (ast::If& node)
            {
              node.condition->operation->execute (exec);
              assert (node.condition->eval.expression_kind != semantic::UnknownExpressionKind);
              if (node.condition->eval.expression_kind == semantic::VariableExpressionKind)
                {
                  UNIMPLEMENTED;
                  // void* ptr = exec.stack ().pop_pointer ();
                  //exec.stack ().load (ptr, node.condition->eval.type->size ());
                }
              bool c;
              exec.stack ().pop (c);
              if (c)
                {
                  node.true_body->accept (*this);
                }
              else
                {
                  node.false_body->accept (*this);
                }
            }

            void visit (StatementList& node)
            {
              node.visit_children (*this);
            }

            void visit (ast::ForIota& node)
            {
              for (long idx = 0, limit = node.limit_value;
                   idx != limit;
                   ++idx)
                {
                  long* i = static_cast<long*> (exec.stack ().get_address (node.symbol->offset ()));
                  *i = idx;
                  node.body->accept (*this);
                }
            }

            void visit (ast::BindDecl& node)
            {
              node.body->accept (*this);
            }

            void bind (ast::Node* left, ast::Node* right, long param = 0)
            {
              left->operation->execute (exec);
              void* port = exec.stack ().pop_pointer ();
              ast::Node* sb = node_cast<ast::Select> (right)->base;
              sb->operation->execute (exec);
              void* reaction_component = exec.stack ().pop_pointer ();
              if (sb->eval.type->underlying_type ()->to_pointer ())
                {
                  exec.stack ().load (reaction_component, arch::size (sb->eval.type));
                  reaction_component = exec.stack ().pop_pointer ();
                }
              const decl::Reaction* reaction = static_cast<const decl::Reaction*> (right->callable);
              PushPortsType::const_iterator pp_pos = table.push_ports_.find (reinterpret_cast<size_t> (port));
              assert (pp_pos != table.push_ports_.end ());
              PushPort* pp = pp_pos->second;
              InstancesType::const_iterator i_pos = table.instances_.find (reinterpret_cast<size_t> (reaction_component));
              assert (i_pos != table.instances_.end ());
              Instance* i = i_pos->second;
              ReactionsType::const_iterator r_pos = table.reactions_.find (ReactionsType::key_type (i, reaction, param));
              assert (r_pos != table.reactions_.end ());
              Reaction* r = r_pos->second;
              pp->reactions.push_back (r);
              r->push_ports.push_back (pp);
            }

            void visit (BindPushPort& node)
            {
              bind (node.left, node.right);
            }

            void visit (BindPushPortParameter& node)
            {
              node.param->operation->execute (exec);
              assert (node.param->eval.expression_kind != semantic::UnknownExpressionKind);
              if (node.param->eval.expression_kind == semantic::VariableExpressionKind)
                {
                  void* ptr = exec.stack ().pop_pointer ();
                  exec.stack ().load (ptr, arch::size (node.param->eval.type));
                }
              long idx;
              exec.stack ().pop (idx);
              bind (node.left, node.right, idx);
            }

            void visit (BindPullPort& node)
            {
              node.left->operation->execute (exec);
              void* port = exec.stack ().pop_pointer ();
              ast::Node* sb = node_cast<ast::Select> (node.right)->base;
              sb->operation->execute (exec);
              void* getter_component = exec.stack ().pop_pointer ();
              const decl::Getter* getter = static_cast<const decl::Getter*> (node.right->callable);

              PullPortsType::const_iterator pp_pos = table.pull_ports_.find (reinterpret_cast<size_t> (port));
              assert (pp_pos != table.pull_ports_.end ());
              PullPort* pp = pp_pos->second;
              InstancesType::const_iterator i_pos = table.instances_.find (reinterpret_cast<size_t> (getter_component));
              assert (i_pos != table.instances_.end ());
              Instance* i = i_pos->second;
              GettersType::const_iterator g_pos = table.getters_.find (GettersType::key_type (i, getter));
              assert (g_pos != table.getters_.end ());
              Getter* g = g_pos->second;
              pp->getters.push_back (g);
            }
          };
          visitor v (*this, instance_pos->first, *bind_pos);
          (*bind_pos)->node->accept (v);
        }
    }
}

void Composer::compute_dependent_instances ()
{
  // Determine which components influence other components via precondition.
  for (InstancesType::const_iterator ipos = instances_.begin (),
       ilimit = instances_.end ();
       ipos != ilimit;
       ++ipos)
    {
      Instance* instance = ipos->second;
      for (ActionsType::const_iterator apos = instance->actions.begin (),
           alimit = instance->actions.end ();
           apos != alimit;
           ++apos)
        {
          Action* action = *apos;
          for (NodesType::const_iterator npos = action->precondition_nodes.begin (),
               nlimit = action->precondition_nodes.end ();
               npos != nlimit;
               ++npos)
            {
              Node* node = *npos;
              for (InstanceSet::const_iterator pos = node->instance_set ().begin (),
                   limit = node->instance_set ().end ();
                   pos != limit;
                   ++pos)
                {
                  pos->first->linked_instances.insert (instance);
                }
            }
        }
    }
}

static void tarjan (Node* n)
{
  switch (n->state)
    {
    case Node::Unmarked:
      n->state = Node::Temporary;
      for (size_t i = 0; i != n->outgoing_count (); ++i)
        {
          Node* m = n->outgoing_node (i);
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
Composer::enumerate_actions ()
{
  for (InstancesType::const_iterator pos = instances_.begin (),
       limit = instances_.end ();
       pos != limit;
       ++pos)
    {
      Instance* instance = pos->second;
      const NamedType* type = instance->type;
      for (NamedType::ActionsType::const_iterator pos = type->actions_begin (),
           limit = type->actions_end ();
           pos != limit;
           ++pos)
        {
          decl::Action* action = *pos;
          if (action->has_dimension ())
            {
              for (long idx = 0; idx != action->dimension; ++idx)
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
struct Composer::ElaborationVisitor : public ast::DefaultNodeVisitor
{
  Executor exec;
  Composer& table;
  Action* action;
  bool in_precondition;
  Reaction* reaction;
  Getter* getter;
  Activation* activation;

  ElaborationVisitor (Composer& t, Action* a, bool in_pre = false)
    : table (t)
    , action (a)
    , in_precondition (in_pre)
    , reaction (NULL)
    , getter (NULL)
    , activation (NULL)
  {
    if (action->action->has_dimension ())
      {
        exec.stack ().push (action->iota);
      }
    exec.stack ().push_pointer (reinterpret_cast<void*> (action->instance->address));
    exec.stack ().push_pointer (NULL);
    exec.stack ().setup (action->action->memory_model.locals_size_on_stack ());
  }

  ElaborationVisitor (Composer& t, Reaction* r)
    : table (t)
    , action (NULL)
    , reaction (r)
    , getter (NULL)
    , activation (NULL)
  {
    if (reaction->reaction->has_dimension ())
      {
        exec.stack ().push (reaction->iota);
      }
    exec.stack ().push_pointer (reinterpret_cast<void*> (reaction->instance->address));
    exec.stack ().reserve (arch::size_on_stack (reaction->reaction->parameter_list ()) - sizeof (void*));
    exec.stack ().push_pointer (NULL);
    exec.stack ().setup (reaction->reaction->memory_model.locals_size_on_stack ());
  }

  ElaborationVisitor (Composer& t, Getter* g)
    : table (t)
    , action (NULL)
    , reaction (NULL)
    , getter (g)
    , activation (NULL)
  {
    exec.stack ().push_pointer (reinterpret_cast<void*> (getter->instance->address));
    exec.stack ().reserve (arch::size_on_stack (getter->getter->parameter_list ()) - sizeof (void*));
    exec.stack ().setup (getter->getter->memory_model.locals_size_on_stack ());
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
        NOT_REACHED;
      }
  }

  void addCall (Node* n)
  {
    assert (n != NULL);
    if (action != NULL)
      {
        action->nodes.push_back (n);
        if (in_precondition)
          {
            action->precondition_nodes.push_back (n);
          }
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
        NOT_REACHED;
      }
  }

  void default_action (ast::Node& node)
  {
    AST_NOT_REACHED (node);
  }

  void visit (Conversion& node)
  {
    node.argument->accept (*this);
  }

  void visit (StatementList& node)
  {
    node.visit_children (*this);
  }

  void visit (ExpressionStatement& node)
  {
    node.visit_children (*this);
  }

  void visit (ast::Return& node)
  {
    node.visit_children (*this);
  }

  void visit (ast::Activate& node)
  {
    if (action != NULL)
      {
        activation = new Activation (action->instance, node.mutable_phase_access);
        action->nodes.push_back (activation);
      }
    else if (reaction != NULL)
      {
        activation = new Activation (reaction->instance, node.mutable_phase_access);
        reaction->nodes.push_back (activation);
      }
    else
      {
        NOT_REACHED;
      }
    node.arguments->accept (*this);
  }

  void visit (ExpressionList& node)
  {
    node.visit_children (*this);
  }

  void visit (ast::PushPortCall& node)
  {
    size_t port = activation->instance->address + arch::offset (node.field);
    // Find what is bound to this port.
    Composer::PushPortsType::const_iterator port_pos = table.push_ports_.find (port);
    assert (port_pos != table.push_ports_.end ());
    activation->nodes.push_back (port_pos->second);
    node.arguments->accept (*this);
  }

  void visit (ast::IndexedPushPortCall& node)
  {
    node.index->operation->execute (exec);
    assert (node.index->eval.expression_kind != semantic::UnknownExpressionKind);
    if (node.index->eval.expression_kind == semantic::VariableExpressionKind)
      {
        void* ptr = exec.stack ().pop_pointer ();
        exec.stack ().load (ptr, arch::size (node.index->eval.type));
      }

    long idx;
    exec.stack ().pop (idx);
    if (idx < 0)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "port index is negative (E100)");
      }
    if (idx >= node.array_type->dimension)
      {
        error_at_line (-1, 0, node.location.file.c_str (), node.location.line,
                       "port index is negative (E75)");
      }

    size_t port = activation->instance->address + arch::offset (node.field) + idx * arch::unit_size (node.array_type);

    // Find what is bound to this port.
    Composer::PushPortsType::const_iterator port_pos = table.push_ports_.find (port);
    assert (port_pos != table.push_ports_.end ());
    activation->nodes.push_back (port_pos->second);
    node.arguments->accept (*this);
  }

  void visit (Call& node)
  {
    assert (node.expression->eval.expression_kind != semantic::UnknownExpressionKind);
    if (node.expression->eval.expression_kind != semantic::TypeExpressionKind)
      {
        // Are we calling a getter or pull port.
        const type::Getter* method = node.getter_type;
        if (method != NULL)
          {
            ast::Node* sb = node_cast<ast::Select> (node.expression)->base;
            sb->operation->execute (exec);
            assert (sb->eval.expression_kind != semantic::UnknownExpressionKind);
            if (sb->eval.expression_kind == semantic::VariableExpressionKind &&
                sb->eval.type->underlying_type ()->to_pointer ())
              {
                void* ptr = exec.stack ().pop_pointer ();
                exec.stack ().load (ptr, arch::size (sb->eval.type));
              }
            size_t inst_addr = reinterpret_cast<size_t> (exec.stack ().pop_pointer ());
            InstancesType::const_iterator i_pos = table.instances_.find (inst_addr);
            assert (i_pos != table.instances_.end ());
            Instance* i = i_pos->second;
            const Callable* g = node.callable;
            assert (g != NULL);
            GettersType::const_iterator g_pos = table.getters_.find (GetterKey (i, g));
            assert (g_pos != table.getters_.end ());
            addCall (g_pos->second);
          }

        const type::PullPort* function = node.pull_port_type;
        if (function != NULL)
          {
            size_t port = get_instance ()->address + arch::offset (node.field);
            Composer::PullPortsType::const_iterator port_pos = table.pull_ports_.find (port);
            assert (port_pos != table.pull_ports_.end ());
            addCall (port_pos->second);
          }
      }

    node.visit_children (*this);
  }

  void visit (IdentifierExpression& node)
  {
    // Do nothing.
  }

  void visit (ast::Literal& node)
  {
    // Do nothing.
  }

  void visit (ast::BinaryArithmetic& node)
  {
    node.visit_children (*this);
  }

  void visit (ast::UnaryArithmetic& node)
  {
    node.visit_children (*this);
  }

  void visit (AddressOf& node)
  {
    node.visit_children (*this);
  }

  void visit (ast::Select& node)
  {
    node.base->accept (*this);
  }

  void visit (Dereference& node)
  {
    node.visit_children (*this);
  }

  void visit (Var& node)
  {
    node.expressions->accept (*this);
  }

  void visit (ast::Assign& node)
  {
    node.visit_children (*this);
  }

  void visit (TypeExpression& node)
  {
  }

  void visit (ast::Change& node)
  {
    node.argument->accept (*this);
    node.body->accept (*this);
  }

  void visit (EmptyStatement& node)
  {
  }

  void visit (Index& node)
  {
    node.visit_children (*this);
  }

  void visit (ast::If& node)
  {
    node.visit_children (*this);
  }
};

void
Composer::elaborate_actions ()
{
  for (InstancesType::const_iterator ipos = instances_.begin (),
       ilimit = instances_.end ();
       ipos != ilimit;
       ++ipos)
    {
      for (ActionsType::const_iterator pos = ipos->second->actions.begin (),
           limit = ipos->second->actions.end ();
           pos != limit;
           ++pos)
        {
          Action* action = *pos;
          {
            ElaborationVisitor v (*this, action, true);
            action->action->precondition->accept (v);
          }
          {
            ElaborationVisitor v (*this, action);
            action->action->body->accept (v);
          }
        }
    }
}

void
Composer::enumerate_reactions ()
{
  for (InstancesType::const_iterator pos = instances_.begin (),
       limit = instances_.end ();
       pos != limit;
       ++pos)
    {
      Instance* instance = pos->second;
      const NamedType* type = instance->type;
      for (NamedType::ReactionsType::const_iterator pos = type->reactions_begin (),
           limit = type->reactions_end ();
           pos != limit;
           ++pos)
        {
          decl::Reaction* reaction = *pos;
          if (reaction->has_dimension ())
            {
              for (long idx = 0; idx != reaction->dimension; ++idx)
                {
                  reactions_.insert (std::make_pair (ReactionKey (instance, reaction, idx), new Reaction (instance, reaction, idx)));
                }
            }
          else
            {
              reactions_.insert (std::make_pair (ReactionKey (instance, reaction), new Reaction (instance, reaction)));
            }
        }
    }
}

void
Composer::elaborate_reactions ()
{
  for (ReactionsType::const_iterator pos = reactions_.begin (),
       limit = reactions_.end ();
       pos != limit;
       ++pos)
    {
      Reaction* reaction = pos->second;
      ElaborationVisitor v (*this, reaction);
      reaction->reaction->body->accept (v);
    }
}

void
Composer::enumerate_getters ()
{
  for (InstancesType::const_iterator pos = instances_.begin (),
       limit = instances_.end ();
       pos != limit;
       ++pos)
    {
      Instance* instance = pos->second;
      const NamedType* type = instance->type;
      for (NamedType::GettersType::const_iterator pos = type->getters_begin (),
           limit = type->getters_end ();
           pos != limit;
           ++pos)
        {
          decl::Getter* getter = *pos;
          getters_.insert (std::make_pair (GetterKey (instance, getter), new Getter (instance, getter)));
        }
    }
}

void
Composer::elaborate_getters ()
{
  for (GettersType::const_iterator pos = getters_.begin (),
       limit = getters_.end ();
       pos != limit;
       ++pos)
    {
      Getter* getter = pos->second;
      ElaborationVisitor v (*this, getter);
      getter->getter->node->body->accept (v);
    }
}

void
Composer::check_structure ()
{
  // Check that no reaction is bound more than once.
  for (ReactionsType::const_iterator pos = reactions_.begin (),
       limit = reactions_.end ();
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
  for (Composer::PullPortsType::const_iterator pos = pull_ports_.begin (),
       limit = pull_ports_.end ();
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
  for (InstancesType::const_iterator ipos = instances_.begin (),
       ilimit = instances_.end ();
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
Composer::compute_instance_sets ()
{
  for (InstancesType::const_iterator ipos = instances_.begin (),
       ilimit = instances_.end ();
       ipos != ilimit;
       ++ipos)
    {
      for (ActionsType::const_iterator pos = ipos->second->actions.begin (),
           limit = ipos->second->actions.end ();
           pos != limit;
           ++pos)
        {
          (*pos)->instance_set ();
        }
    }
}

void
Composer::elaborate ()
{
  // At this point, we have instances, push ports, and pull ports.
  // Enumerate getters first because they can be called by actions and reactions.
  enumerate_getters ();
  elaborate_getters ();
  enumerate_actions ();
  elaborate_actions ();
  enumerate_reactions ();
  elaborate_reactions ();
  // At this point, we have instances, push ports, pull ports, actions, reactions, and activations.
  // The edges from actions and reactions to activations are present.
  // The edges from activations to push ports are present.
  elaborate_bindings ();
  compute_dependent_instances ();
}

void
Composer::analyze ()
{
  check_structure ();
  compute_instance_sets ();
}

static void collect (std::set<Node*>& nodes, Node* n)
{
  if (nodes.count (n) == 0)
    {
      nodes.insert (n);
      for (size_t i = 0; i != n->outgoing_count (); ++i)
        {
          collect (nodes, n->outgoing_node (i));
        }
    }
}

void
Composer::dump_graphviz () const
{
  // Collect all the nodes.
  // Necessary if graph is cyclic.
  std::set<Node*> nodes;
  for (InstancesType::const_iterator ipos = instances_.begin (),
       ilimit = instances_.end ();
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
      for (size_t i = 0; i != node->outgoing_count (); ++i)
        {
          std::cout << ' ' << '"' << node->outgoing_node (i)->name << '"';
        }
      std::cout << "}\n";
    }
  std::cout << "}\n";
}

// Find instances and ports.
void
Composer::enumerate_instances (ast::Node * node)
{
  struct visitor : public ast::DefaultNodeVisitor
  {
    composition::Composer& instance_table;
    size_t address;

    visitor (composition::Composer& it) : instance_table (it), address (0) { }

    void visit (ast::InstanceDecl& node)
    {
      const NamedType *type = node.symbol->type ();
      decl::Initializer* initializer = node.symbol->initializer ();
      node.symbol->instance = instance_table.instantiate_contained_instances (type, NULL, initializer, address, node.location.line, &node, node.identifier->identifier);
      address += arch::size (type);
    }

    void visit (SourceFile& node)
    {
      node.top_level_decl_list->accept (*this);
    }

    void visit (TopLevelDeclList& node)
    {
      node.visit_children (*this);
    }
  };

  visitor v (*this);
  node->accept (v);
}

Instance*
Composer::instantiate_contained_instances (const type::Type * type,
    composition::Instance* parent,
    decl::Initializer* initializer,
    size_t address,
    unsigned int line,
    ast::InstanceDecl* node,
    const std::string& name,
    const NamedType* named_type,
    Field* field)
{
  switch (type->kind ())
    {
    case Named_Kind:
      return instantiate_contained_instances (type->underlying_type (), parent, initializer, address, line, node, name, type->to_named_type ());

    case Component_Kind:
    {
      assert (named_type != NULL);

      parent = new composition::Instance (parent, address, named_type, initializer, (node != NULL) ? node->operation : NULL, name);
      this->add_instance (parent);

      // Recur changing instance.
      const Struct* s = type->to_struct ();

      for (Struct::const_iterator pos = s->begin (),
           limit = s->end ();
           pos != limit;
           ++pos)
        {
          // Recur changing address (and field).
          instantiate_contained_instances ((*pos)->type, parent, NULL, address + arch::offset (*pos), line, NULL, name + "." + (*pos)->name, NULL, *pos);
        }

      return parent;
    }
    break;

    case Array_Kind:
    {
      const type::Array* a = type->to_array ();
      for (long idx = 0; idx != a->dimension; ++idx)
        {
          // Recur changing address.
          std::stringstream newname;
          newname << name << '[' << idx << ']';
          instantiate_contained_instances (a->base_type, parent, NULL, address + idx * arch::unit_size (a), line, NULL, newname.str (), NULL, field);
        }
    }
    break;

    case Push_Port_Kind:
    {
      this->add_push_port (address, parent, field, name);
    }
    break;

    case Pull_Port_Kind:
      this->add_pull_port (address, parent, field, name);
      break;

    default:
      break;
    }

  return NULL;
}

bool Instance::is_top_level () const
{
  return parent == NULL;
}

Node::~Node() { }

Composer::InstancesType::const_iterator Composer::instances_begin () const
{
  return instances_.begin ();
}
Composer::InstancesType::const_iterator Composer::instances_end () const
{
  return instances_.end ();
}
Composer::PushPortsType::const_iterator Composer::push_ports_begin () const
{
  return push_ports_.begin ();
}
Composer::PushPortsType::const_iterator Composer::push_ports_end () const
{
  return push_ports_.end ();
}
Composer::PullPortsType::const_iterator Composer::pull_ports_begin () const
{
  return pull_ports_.begin ();
}
Composer::PullPortsType::const_iterator Composer::pull_ports_end () const
{
  return pull_ports_.end ();
}

}
