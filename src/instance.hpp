#ifndef instance_hpp
#define instance_hpp

#include "types.hpp"
#include "type.hpp"
#include <set>

class instance_t
{
public:
  instance_t (instance_t* parent,
              size_t address,
              const named_type_t* type,
              method_t* method)
    : parent_ (parent)
    , address_ (address)
    , type_ (type)
    , method_ (method)
    , record (NULL)
  { }

  instance_t* parent () const { return parent_; }
  size_t address () const { return address_; }
  const named_type_t* type () const { return type_; }
  method_t* method () const { return method_; }

  bool is_top_level () const { return parent_ == NULL; }
  size_t offset () const
  {
    if (parent_ == NULL)
      {
        return 0;
      }
    else
      {
        return address_ - parent_->address_;
      }
  }

private:
  instance_t* parent_;
  size_t address_;
  const named_type_t *type_;
  method_t* method_;

public:
  // Pointer to the run-time instance.
  instance_record_t* record;

  struct ConcreteAction
  {
    const action_t* action;
    instance_set_t set;
    size_t iota;

    ConcreteAction () { }

    ConcreteAction (const action_t* a, const instance_set_t& s, size_t i = 0)
      : action (a)
      , set (s)
      , iota (i)
    { }
  };

  typedef std::vector<ConcreteAction> InstanceSetsType;
  InstanceSetsType instance_sets;
};

struct instance_table_t
{
  typedef std::map<size_t, instance_t*> InstancesType;
  InstancesType instances;
  struct InputType
  {
    instance_t* instance;
    reaction_t* reaction;
    int64_t parameter;

    InputType () { }
    InputType (instance_t* i, reaction_t* r, int64_t p) : instance (i), reaction (r), parameter (p) { }

    bool operator< (const InputType& other) const
    {
      if (this->instance != other.instance) return this->instance < other.instance;
      if (this->reaction != other.reaction) return this->reaction < other.reaction;
      return this->parameter < other.parameter;
    }
  };
  typedef std::set<InputType> InputsType;
  struct PortValueType
  {
    PortValueType () { }
    PortValueType (instance_t* oi) : output_instance (oi) { }
    instance_t* output_instance;
    InputsType inputs;
  };
  typedef std::map<size_t, PortValueType> PortsType;
  PortsType ports;
  typedef std::map<InputType, std::set<size_t> > ReversePortsType;
  ReversePortsType reverse_ports;
};

instance_table_t *instance_table_make (void);

instance_t *instance_table_insert (instance_table_t * table,
                                   instance_t* parent,
				   const named_type_t * type,
                                   method_t* method,
                                   size_t address);

void instance_table_insert_port (instance_table_t* table,
                                 size_t address,
                                 instance_t* output_instance);

void instance_table_enumerate_bindings (instance_table_t * table);

void instance_table_analyze_composition (const instance_table_t * table);

void instance_table_dump (const instance_table_t * table);

#endif /* instance_hpp */
