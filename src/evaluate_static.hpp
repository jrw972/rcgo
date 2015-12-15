#ifndef RC_SRC_EVALUATE_STATIC_HPP
#define RC_SRC_EVALUATE_STATIC_HPP

#include <cstddef>
#include <map>

#include "type.hpp"

namespace semantic
{

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
  union
  {
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
        UNIMPLEMENTED;
      case VALUE:
        NOT_REACHED;
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
        NOT_REACHED;
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
        NOT_REACHED;
      }
    return out;
  }

  static static_value_t index (static_value_t in, const type::Array* type, static_value_t idx)
  {
    static_value_t out;
    switch (in.kind)
      {
      case STACK_ADDRESS:
        out = in;
        out.offset += type->UnitSize () * idx.value;
        break;
      case ABSOLUTE_ADDRESS:
        out = in;
        out.address += type->UnitSize () * idx.value;
        break;
      case VALUE:
        NOT_REACHED;
      }
    return out;
  }

  static static_value_t address_of (static_value_t in)
  {
    static_value_t out;
    out.kind = VALUE;

    switch (in.kind)
      {
      case STACK_ADDRESS:
        UNIMPLEMENTED;
      case ABSOLUTE_ADDRESS:
        out.value = in.address;
        break;
        UNIMPLEMENTED;
      case VALUE:
        UNIMPLEMENTED;
      }

    return out;
  }
};

std::ostream&
operator<< (std::ostream& o,
            const static_value_t& v);

static_value_t
EvaluateStatic (const ast::Node* node, const static_memory_t& memory);

}

#endif // RC_SRC_EVALUATE_STATIC_HPP
