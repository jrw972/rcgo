#ifndef RC_SRC_VALUE_HPP
#define RC_SRC_VALUE_HPP

#include "types.hpp"

// Borrow some types from runtime.
#include "runtime_types.hpp"

namespace semantic
{

struct UntypedComplex
{
  double real;
  double imag;
  static UntypedComplex make (double r, double i);
  bool operator== (const UntypedComplex& other) const;
  UntypedComplex& operator= (const double& x);
  UntypedComplex& operator= (const runtime::Complex64& x);
  UntypedComplex& operator= (const runtime::Complex128& x);
  operator double() const;
};

UntypedComplex operator* (const UntypedComplex&, const UntypedComplex&);
UntypedComplex operator/ (const UntypedComplex&, const UntypedComplex&);
UntypedComplex operator+ (const UntypedComplex&, const UntypedComplex&);
UntypedComplex operator- (const UntypedComplex&, const UntypedComplex&);
UntypedComplex operator- (const UntypedComplex&);

struct Value
{
  Value ();
  bool representable (const type::Type* from, const type::Type* to) const;
  void convert (const type::Type* from, const type::Type* to);
  long to_int (const type::Type* type) const;
  void print (const type::Type* type, std::ostream& out = std::cout) const;

  bool present;
  union
  {
    bool bool_value;

    uint8_t uint8_value;
    uint16_t uint16_value;
    uint32_t uint32_value;
    uint64_t uint64_value;

    int8_t int8_value;
    int16_t int16_value;
    int32_t int32_value;
    int64_t int64_value;

    float float32_value;
    double float64_value;

    runtime::Complex64 complex64_value;
    runtime::Complex128 complex128_value;

    unsigned long uint_value;
    long int_value;
    size_t uintptr_value;

    void* pointer_value;
    runtime::Slice slice_value;
    runtime::String string_value;

    bool untyped_boolean_value;
    int32_t untyped_rune_value;
    long long untyped_integer_value;
    double untyped_float_value;
    UntypedComplex untyped_complex_value;
    runtime::String untyped_string_value;
  };
};

typedef std::vector<Value> ValueList;

struct ValuePrinter
{
  const type::Type* type;
  const Value& value;
  ValuePrinter (const type::Type* t, const Value& v);
};

std::ostream& operator<< (std::ostream& out, const ValuePrinter& vp);

void equal (Value& out, const type::Type* type,
            const Value& left, const Value& right);
void not_equal (Value& out, const type::Type* type,
                const Value& left, const Value& right);
void less_than (Value& out, const type::Type* type,
                const Value& left, const Value& right);
void less_equal (Value& out, const type::Type* type,
                 const Value& left, const Value& right);
void more_than (Value& out, const type::Type* type,
                const Value& left, const Value& right);
void more_equal (Value& out, const type::Type* type,
                 const Value& left, const Value& right);

void multiply (Value& out, const type::Type* type,
               const Value& left, const Value& right);
void divide (Value& out, const type::Type* type,
             const Value& left, const Value& right);
void modulus (Value& out, const type::Type* type,
              const Value& left, const Value& right);
void add (Value& out, const type::Type* type,
          const Value& left, const Value& right);
void subtract (Value& out, const type::Type* type,
               const Value& left, const Value& right);

void bit_and (Value& out, const type::Type* type,
              const Value& left, const Value& right);
void bit_and_not (Value& out, const type::Type* type,
                  const Value& left, const Value& right);
void bit_or (Value& out, const type::Type* type,
             const Value& left, const Value& right);
void bit_xor (Value& out, const type::Type* type,
              const Value& left, const Value& right);

}

#endif // RC_SRC_VALUE_HPP
