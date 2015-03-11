#ifndef instance_table_hpp
#define instance_table_hpp

#include "instance.hpp"
#include <set>

struct instance_table_t
{
  typedef std::map<size_t, instance_t*> InstancesType;
  InstancesType instances;

  void insert (instance_t* instance)
  {
    instances[instance->address ()] = instance;
  }

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
    PortValueType (size_t a, instance_t* oi, field_t* of) : address (a), output_instance (oi), output_field (of) { }
    size_t address;
    instance_t* output_instance;
    field_t* output_field;
    InputsType inputs;
  };
  typedef std::map<size_t, PortValueType> PortsType;
  PortsType ports;

  typedef std::map<InputType, std::set<size_t> > ReversePortsType;
  ReversePortsType reverse_ports;

  typedef std::vector<instance_t::ConcreteAction> ActionsType;
  ActionsType actions () const;
};

std::ostream&
operator<< (std::ostream&,
            const instance_table_t::PortValueType&);

void instance_table_insert_port (instance_table_t& table,
                                 size_t address,
                                 instance_t* output_instance,
                                 field_t* output_field);

void instance_table_enumerate_bindings (instance_table_t& table);

void instance_table_analyze_composition (const instance_table_t& table);

void instance_table_dump (const instance_table_t& table);

template <typename XIterator, typename YIterator>
bool
independent (XIterator x_begin, XIterator x_end,
             YIterator y_begin, YIterator y_end)
{
  while (x_begin != x_end && y_begin != y_end)
    {
      if (x_begin->first < y_begin->first)
        {
          ++x_begin;
        }
      else if (y_begin->first < x_begin->first)
        {
          ++y_begin;
        }
      else
        {
          if (x_begin->second == TRIGGER_WRITE || y_begin->second == TRIGGER_WRITE)
            {
              return false;
            }
          else
            {
              ++x_begin;
              ++y_begin;
            }
        }
    }

  return true;
}

#endif /* instance_table_hpp */
