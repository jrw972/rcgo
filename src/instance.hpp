#ifndef instance_hpp
#define instance_hpp

#include "types.hpp"
#include "Type.hpp"
#include <set>

class instance_t
{
public:
  instance_t (instance_t* parent,
              size_t address,
              const Type::NamedType* type,
              Initializer* initializer,
              unsigned int line)
    : parent_ (parent)
    , address_ (address)
    , type_ (type)
    , ptr_ (NULL)
    , initializer_ (initializer)
    , line_ (line)
  { }

  instance_t* parent () const
  {
    return parent_;
  }
  size_t address () const
  {
    return address_;
  }
  const Type::NamedType* type () const
  {
    return type_;
  }

  void ptr (component_t* p)
  {
    ptr_ = p;
  }
  component_t* ptr () const
  {
    return ptr_;
  }

  Initializer* initializer () const
  {
    return initializer_;
  }

  unsigned int line () const
  {
    return line_;
  }

  bool is_top_level () const
  {
    return parent_ == NULL;
  }
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
  const Type::NamedType *type_;
  component_t* ptr_;
  Initializer* initializer_;
  unsigned int line_;

public:
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

std::ostream&
operator<< (std::ostream& o,
            const instance_t& i);

std::ostream&
operator<< (std::ostream& o,
            const instance_t::ConcreteAction& ca);

#endif /* instance_hpp */
