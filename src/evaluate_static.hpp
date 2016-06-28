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

struct static_Value
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

  static_Value ()
    : kind (VALUE)
    , value (0)
  { }

  static static_Value make_stack_offset (ptrdiff_t offset)
  {
    static_Value v;
    v.kind = STACK_ADDRESS;
    v.offset = offset;
    return v;
  }

  static static_Value make_value (size_t value)
  {
    static_Value v;
    v.kind = VALUE;
    v.value = value;
    return v;
  }

  static static_Value implicit_dereference (static_Value in, const static_memory_t& memory)
  {
    static_Value out;
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

  static static_Value dereference (static_Value in)
  {
    static_Value out;
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

  static static_Value select (static_Value in, ptrdiff_t offset)
  {
    static_Value out;
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

  static static_Value index (static_Value in, const type::Array* type, static_Value idx)
  {
    static_Value out;
    switch (in.kind)
      {
      case STACK_ADDRESS:
        out = in;
        UNIMPLEMENTED;
        //out.offset += type->unit_size () * idx.value;
        break;
      case ABSOLUTE_ADDRESS:
        out = in;
        UNIMPLEMENTED;
        //out.address += type->unit_size () * idx.value;
        break;
      case VALUE:
        NOT_REACHED;
      }
    return out;
  }

  static static_Value address_of (static_Value in)
  {
    static_Value out;
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
            const static_Value& v);

static_Value
EvaluateStatic (const ast::Node* node, const static_memory_t& memory);

}

#endif // RC_SRC_EVALUATE_STATIC_HPP
