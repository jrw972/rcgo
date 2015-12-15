#ifndef RC_SRC_COMPOSITION_HPP
#define RC_SRC_COMPOSITION_HPP

#include "types.hpp"
#include "type.hpp"

namespace composition
{
struct Instance;
struct Node;
struct Action;
struct Reaction;
struct Activation;
struct PushPort;
struct PullPort;
struct Getter;

typedef std::vector<Node*> NodesType;
typedef std::vector<Action*> ActionsType;
typedef std::vector<Getter*> GettersType;
typedef std::vector<Reaction*> ReactionsType;

struct Instance
{
  Instance (Instance* p,
            size_t a,
            const type::NamedType* t,
            decl::Initializer* i,
            ast::ast_instance_t* n,
            const std::string& aName);

  Instance* const parent;
  size_t const address;
  const type::NamedType* const type;
  component_t* component;
  const decl::Initializer* const initializer;
  const ast::ast_instance_t* const node;
  ActionsType actions;
  std::string const name;

  bool IsTopLevel () const
  {
    return parent == NULL;
  }

  size_t Offset () const;
};

struct InstanceSet : public std::map<Instance*, ReceiverAccess>
{
  void Insert (const value_type& v);
  bool Compatible (const InstanceSet& other) const;
  void Union (const InstanceSet& other);
};

struct Node
{
  enum State
  {
    Unmarked,
    Temporary,
    Marked
  };

  Node (const std::string& n) : name (n), state (Unmarked), instance_set_computed (false) { }
  virtual ~Node() { }
  // Return the number of outgoing edges.
  virtual size_t OutgoingCount () const = 0;
  // Return the ith outgoing node.
  virtual Node* OutgoingNode (size_t i) const = 0;
  virtual const InstanceSet& GetInstanceSet () = 0;
  std::string const name;
  State state;
protected:
  bool instance_set_computed;
  InstanceSet instance_set;
};

struct Action : public Node
{
  Action (Instance* i, decl::Action* a, type::Int::ValueType p = 0);
  Instance* const instance;
  decl::Action* const action;
  type::Int::ValueType const iota;
  virtual size_t OutgoingCount () const;
  virtual Node* OutgoingNode (size_t i) const;
  const InstanceSet& GetInstanceSet ();
  const InstanceSet& GetInstanceSet () const
  {
    return instance_set;
  }
  NodesType nodes;
private:
  static std::string getname (Instance* i, decl::Action* a, type::Int::ValueType p);
};

struct ReactionKey
{
  ReactionKey (Instance* i, const decl::reaction_t* a, type::Int::ValueType p = 0);
  bool operator< (const ReactionKey& other) const;
  Instance* instance;
  const decl::reaction_t* reaction;
  type::Int::ValueType iota;
};

struct Reaction : public Node
{
  Reaction (Instance* i, decl::reaction_t* a, type::Int::ValueType p = 0);
  virtual size_t OutgoingCount () const;
  virtual Node* OutgoingNode (size_t i) const;
  const InstanceSet& GetInstanceSet ();
  Instance* const instance;
  decl::reaction_t* const reaction;
  type::Int::ValueType const iota;
  NodesType nodes;
  std::vector<PushPort*> push_ports;
private:
  static std::string getname (Instance* i, decl::reaction_t* a, type::Int::ValueType p);
};

struct GetterKey
{
  GetterKey (Instance* i, const decl::Callable* c);
  bool operator< (const GetterKey& other) const;
  Instance* instance;
  const decl::Callable* getter;
};

struct Getter : public Node
{
  Getter (Instance* i, decl::Getter* g);
  virtual size_t OutgoingCount () const;
  virtual Node* OutgoingNode (size_t i) const;
  const InstanceSet& GetInstanceSet ();
  Instance* const instance;
  decl::Getter* const getter;
  NodesType nodes;
};

struct Activation : public Node
{
  Activation (Instance* i, const ast::ast_activate_statement_t* as);
  virtual size_t OutgoingCount () const;
  virtual Node* OutgoingNode (size_t i) const;
  const InstanceSet& GetInstanceSet ();
  Instance* const instance;
  const ast::ast_activate_statement_t* const activate_statement;
  NodesType nodes;
private:
  static std::string getname (Activation* a);
};

struct PushPort : public Node
{
  PushPort (size_t a, Instance* oi, type::field_t* of, const std::string& name);
  virtual size_t OutgoingCount () const;
  virtual Node* OutgoingNode (size_t i) const;
  const InstanceSet& GetInstanceSet ();
  size_t const address;
  Instance* const instance;
  type::field_t* const field;
  ReactionsType reactions;
};

struct PullPort : public Node
{
  PullPort (size_t a, Instance* oi, type::field_t* of, const std::string& name);
  virtual size_t OutgoingCount () const;
  virtual Node* OutgoingNode (size_t i) const;
  virtual const InstanceSet& GetInstanceSet ();
  size_t const address;
  Instance* const instance;
  type::field_t* const field;
  GettersType getters;
};

/*
  Instantiates components and bindings and checks the resulting composition.
*/
class Composer
{
public:
  void AddInstance (Instance* instance);
  void AddPushPort (size_t address,
                    Instance* output_instance,
                    type::field_t* output_field,
                    const std::string& name);
  void AddPullPort (size_t address,
                    Instance* input_instance,
                    type::field_t* input_field,
                    const std::string& name);

  void ElaborateComposition ();
  void AnalyzeComposition ();

  typedef std::map<size_t, Instance*> InstancesType;
  typedef std::map<size_t, PushPort*> PushPortsType;
  typedef std::map<size_t, PullPort*> PullPortsType;
  typedef std::map<ReactionKey, Reaction*> ReactionsType;
  typedef std::map<GetterKey, Getter*> GettersType;
  InstancesType::const_iterator InstancesBegin () const
  {
    return instances.begin ();
  }
  InstancesType::const_iterator InstancesEnd () const
  {
    return instances.end ();
  }
  PushPortsType::const_iterator PushPortsBegin () const
  {
    return push_ports.begin ();
  }
  PushPortsType::const_iterator PushPortsEnd () const
  {
    return push_ports.end ();
  }
  PullPortsType::const_iterator PullPortsBegin () const
  {
    return pull_ports.begin ();
  }
  PullPortsType::const_iterator PullPortsEnd () const
  {
    return pull_ports.end ();
  }
  void DumpGraphviz () const;
private:
  InstancesType instances;
  PushPortsType push_ports;
  PullPortsType pull_ports;
  ReactionsType reactions;
  GettersType getters;
  struct ElaborationVisitor;
  void enumerateActions ();
  void elaborateActions ();
  void enumerateReactions ();
  void elaborateReactions ();
  void enumerateGetters ();
  void elaborateGetters ();
  void elaborateBindings ();
  void checkStructure ();
  void computeInstanceSets ();
};
}

#endif // RC_SRC_COMPOSITION_HPP
