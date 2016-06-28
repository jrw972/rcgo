#ifndef RC_SRC_COMPOSITION_HPP
#define RC_SRC_COMPOSITION_HPP

#include "types.hpp"
#include "type.hpp"
#include "runtime_types.hpp"

namespace composition
{
typedef std::vector<Node*> NodesType;
typedef std::vector<Action*> ActionsType;
typedef std::vector<Getter*> GettersType;
typedef std::vector<Reaction*> ReactionsType;

struct Instance
{
  Instance (Instance* parent,
            size_t address,
            const type::NamedType* type,
            decl::Initializer* initializer,
            const runtime::Operation* operation,
            const std::string& name);

  Instance* const parent;
  size_t const address;
  const type::NamedType* const type;
  runtime::component_t* component;
  const decl::Initializer* const initializer;
  const runtime::Operation* const operation;
  ActionsType actions;
  std::string const name;

  bool is_top_level () const;

  size_t offset () const;
  // When this instance changes, preconditions in these instances may change.
  typedef std::set<Instance*> LinkedInstancesType;
  LinkedInstancesType linked_instances;
};

struct InstanceSet : public std::map<Instance*, ReceiverAccess>
{
  void insert (const value_type& v);
  bool is_compatible (const InstanceSet& other) const;
  void add_union (const InstanceSet& other);
};

struct Node
{
  enum State
  {
    Unmarked,
    Temporary,
    Marked
  };

  Node (const std::string& name);
  virtual ~Node();
  // Return the number of outgoing edges.
  virtual size_t outgoing_count () const = 0;
  // Return the ith outgoing node.
  virtual Node* outgoing_node (size_t i) const = 0;
  virtual const InstanceSet& instance_set () = 0;
  std::string const name;
  State state;
protected:
  bool instance_set_computed_;
  InstanceSet instance_set_;
};

struct Action : public Node
{
  Action (Instance* instance,
          decl::Action* action,
          long iota = 0);
  Instance* const instance;
  decl::Action* const action;
  long const iota;
  virtual size_t outgoing_count () const;
  virtual Node* outgoing_node (size_t i) const;
  const InstanceSet& instance_set ();
  const InstanceSet& instance_set () const
  {
    return instance_set_;
  }
  NodesType nodes;
  NodesType precondition_nodes;
private:
  static std::string getname (Instance* instance,
                              decl::Action* action,
                              long iota);
};

struct ReactionKey
{
  ReactionKey (Instance* instance,
               const decl::Reaction* reaction,
               long iota = 0);
  bool operator< (const ReactionKey& other) const;
  Instance* instance;
  const decl::Reaction* reaction;
  long iota;
};

struct Reaction : public Node
{
  Reaction (Instance* instance,
            decl::Reaction* reaction,
            long iota = 0);
  virtual size_t outgoing_count () const;
  virtual Node* outgoing_node (size_t i) const;
  const InstanceSet& instance_set ();
  Instance* const instance;
  decl::Reaction* const reaction;
  long const iota;
  NodesType nodes;
  std::vector<PushPort*> push_ports;
private:
  static std::string getname (Instance* instance,
                              decl::Reaction* reaction,
                              long iota);
};

struct GetterKey
{
  GetterKey (Instance* instance, const decl::Callable* getter);
  bool operator< (const GetterKey& other) const;
  Instance* instance;
  const decl::Callable* getter;
};

struct Getter : public Node
{
  Getter (Instance* instance, decl::Getter* getter);
  virtual size_t outgoing_count () const;
  virtual Node* outgoing_node (size_t i) const;
  const InstanceSet& instance_set ();
  Instance* const instance;
  decl::Getter* const getter;
  NodesType nodes;
};

struct Activation : public Node
{
  Activation (Instance* instance, ReceiverAccess mutable_phase_access);
  virtual size_t outgoing_count () const;
  virtual Node* outgoing_node (size_t i) const;
  const InstanceSet& instance_set ();
  Instance* const instance;
  ReceiverAccess const mutable_phase_access;
  NodesType nodes;
private:
  static std::string getname (Activation* activation);
};

struct PushPort : public Node
{
  PushPort (size_t address,
            Instance* instance,
            type::Field* field,
            const std::string& name);
  virtual size_t outgoing_count () const;
  virtual Node* outgoing_node (size_t i) const;
  const InstanceSet& instance_set ();
  size_t const address;
  Instance* const instance;
  type::Field* const field;
  ReactionsType reactions;
};

struct PullPort : public Node
{
  PullPort (size_t address,
            Instance* instance,
            type::Field* field,
            const std::string& name);
  virtual size_t outgoing_count () const;
  virtual Node* outgoing_node (size_t i) const;
  virtual const InstanceSet& instance_set ();
  size_t const address;
  Instance* const instance;
  type::Field* const field;
  GettersType getters;
};

// Instantiates components and bindings and checks the resulting composition.
class Composer
{
public:
  void enumerate_instances (ast::Node* node);
  void elaborate ();
  void analyze ();

  typedef std::map<size_t, Instance*> InstancesType;
  typedef std::map<size_t, PushPort*> PushPortsType;
  typedef std::map<size_t, PullPort*> PullPortsType;
  typedef std::map<ReactionKey, Reaction*> ReactionsType;
  typedef std::map<GetterKey, Getter*> GettersType;
  InstancesType::const_iterator instances_begin () const;
  InstancesType::const_iterator instances_end () const;
  PushPortsType::const_iterator push_ports_begin () const;
  PushPortsType::const_iterator push_ports_end () const;
  PullPortsType::const_iterator pull_ports_begin () const;
  PullPortsType::const_iterator pull_ports_end () const;
  void dump_graphviz () const;
private:
  InstancesType instances_;
  PushPortsType push_ports_;
  PullPortsType pull_ports_;
  ReactionsType reactions_;
  GettersType getters_;
  struct ElaborationVisitor;
  Instance* instantiate_contained_instances (const type::Type * type,
      Instance* parent,
      decl::Initializer* initializer,
      size_t address,
      unsigned int line,
      ast::Instance* node,
      const std::string& name,
      const type::NamedType* named_type = NULL,
      type::Field* field = NULL);
  void add_instance (Instance* instance);
  void add_push_port (size_t address,
                      Instance* output_instance,
                      type::Field* output_field,
                      const std::string& name);
  void add_pull_port (size_t address,
                      Instance* input_instance,
                      type::Field* input_field,
                      const std::string& name);
  void enumerate_actions ();
  void elaborate_actions ();
  void enumerate_reactions ();
  void elaborate_reactions ();
  void enumerate_getters ();
  void elaborate_getters ();
  void elaborate_bindings ();
  void compute_dependent_instances ();
  void check_structure ();
  void compute_instance_sets ();
};
}

#endif // RC_SRC_COMPOSITION_HPP
