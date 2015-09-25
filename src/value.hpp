#ifndef value_hpp
#define value_hpp

#include "type.hpp"

struct value_t {
  value_t () : present (false) { }
  value_t (const bool_type_t*  t, bool_type_t::ValueType v)  : present (true), bool_value_ (v) { }
  value_t (const int_type_t*   t, int_type_t::ValueType  v)  : present (true), int_value_ (v)  { }
  value_t (const int8_type_t*  t, int8_type_t::ValueType v)  : present (true), int8_value_ (v) { }
  value_t (const int64_type_t*  t, int64_type_t::ValueType v)  : present (true), int64_value_ (v) { }
  value_t (const uint_type_t*  t, uint_type_t::ValueType v)  : present (true), uint_value_ (v) { }
  value_t (const uint8_type_t* t, uint8_type_t::ValueType v) : present (true), uint8_value_ (v) { }
  value_t (const uint32_type_t* t, uint32_type_t::ValueType v) : present (true), uint32_value_ (v) { }
  value_t (const uint64_type_t* t, uint64_type_t::ValueType v) : present (true), uint64_value_ (v) { }
  value_t (const uint128_type_t* t, uint128_type_t::ValueType v) : present (true), uint128_value_ (v) { }
  value_t (const named_type_t* t, enum_type_t::ValueType v) : present (true), enum_value_ (v) { }
  value_t (const float64_type_t* t, float64_type_t::ValueType v) : present (true), float64_value_ (v) { }
  value_t (const std::string& s) : present (true), string_value_ (s) { }
  value_t (Callable* c) : present (true), callable_value_ (c) { }
  value_t (reaction_t* r) : present (true), reaction_value_ (r) { }
  value_t (const slice_type_t* t, slice_type_t::ValueType v) : present (true), slice_value_ (v) { }

  static value_t make_reference (pointer_type_t::ValueType v) {
    value_t r;
    r.present = true;
    r.reference_value_ = v;
    return r;
  }

  bool present;

  const bool_type_t::ValueType& ref (const bool_type_t&) const
  { assert (present); return bool_value_; }
  bool_type_t::ValueType& ref (const bool_type_t&)
  { assert (present); return bool_value_; }
  const int_type_t::ValueType& ref (const int_type_t&) const
  { assert (present); return int_value_; }
  int_type_t::ValueType& ref (const int_type_t&)
  { assert (present); return int_value_; }
  const int8_type_t::ValueType& ref (const int8_type_t&) const
  { assert (present); return int8_value_; }
  int8_type_t::ValueType& ref (const int8_type_t&)
  { assert (present); return int8_value_; }
  const uint_type_t::ValueType& ref (const uint_type_t&) const
  { assert (present); return uint_value_; }
  uint_type_t::ValueType& ref (const uint_type_t&)
  { assert (present); return uint_value_; }
  const uint8_type_t::ValueType& ref (const uint8_type_t&) const
  { assert (present); return uint8_value_; }
  uint8_type_t::ValueType& ref (const uint8_type_t&)
  { assert (present); return uint8_value_; }
  const uint32_type_t::ValueType& ref (const uint32_type_t&) const
  { assert (present); return uint32_value_; }
  uint32_type_t::ValueType& ref (const uint32_type_t&)
  { assert (present); return uint32_value_; }
  const uint64_type_t::ValueType& ref (const uint64_type_t&) const
  { assert (present); return uint64_value_; }
  uint64_type_t::ValueType& ref (const uint64_type_t&)
  { assert (present); return uint64_value_; }
  const uint128_type_t::ValueType& ref (const uint128_type_t&) const
  { assert (present); return uint128_value_; }
  uint128_type_t::ValueType& ref (const uint128_type_t&)
  { assert (present); return uint128_value_; }
  const float64_type_t::ValueType& ref (const float64_type_t&) const
  { assert (present); return float64_value_; }
  float64_type_t::ValueType& ref (const float64_type_t&)
  { assert (present); return float64_value_; }
  const string_type_t::ValueType& ref (const string_type_t&) const
  { assert (present); return string_value_; }
  string_type_t::ValueType& ref (const string_type_t&)
  { assert (present); return string_value_; }
  const enum_type_t::ValueType& ref (const enum_type_t&) const
  { assert (present); return enum_value_; }
  slice_type_t::ValueType& ref (const slice_type_t&)
  { assert (present); return slice_value_; }
  const slice_type_t::ValueType& ref (const slice_type_t&) const
  { assert (present); return slice_value_; }

  Callable* callable_value () const { return callable_value_; }
  reaction_t* reaction_value () const { return reaction_value_; }
  pointer_type_t::ValueType reference_value () const { return reference_value_; }
  slice_type_t::ValueType slice_value () const { return slice_value_; }

  void print (std::ostream& out, const type_t* type) const;

private:
  union {
    bool_type_t::ValueType bool_value_;
    int_type_t::ValueType int_value_;
    int8_type_t::ValueType int8_value_;
    int64_type_t::ValueType int64_value_;
    uint_type_t::ValueType uint_value_;
    uint8_type_t::ValueType uint8_value_;
    uint32_type_t::ValueType uint32_value_;
    uint64_type_t::ValueType uint64_value_;
    uint128_type_t::ValueType uint128_value_;
    float64_type_t::ValueType float64_value_;
    Callable* callable_value_;
    reaction_t* reaction_value_;
    enum_type_t::ValueType enum_value_;
    pointer_type_t::ValueType reference_value_;
    slice_type_t::ValueType slice_value_;
  };
  string_type_t::ValueType string_value_;
};

#endif /* value_hpp */
