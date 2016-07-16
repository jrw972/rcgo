#include "tap.hpp"
#include "value.hpp"
#include "type.hpp"

#include <sstream>

using namespace semantic;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    UntypedComplex uc = UntypedComplex::make (3.0, 4.0);
    tap.tassert ("UntypedComplex::make",
                 uc.real == 3.0 && uc.imag == 4.0);
    tap.tassert ("UntypedComplex::operator==",
                 uc == uc);
    double d = uc;
    tap.tassert ("UntypedComplex::operator double()",
                 d == 3.0);
    uc = 2.0;
    tap.tassert ("UntypedComplex::operator=(double)",
                 uc.real == 2.0 && uc.imag == 0.0);

    uc = runtime::Complex64::make (8.0, 9.0);
    tap.tassert ("UntypedComplex::operator=(Complex64)",
                 uc.real == 8.0 && uc.imag == 9.0);

    uc = runtime::Complex128::make (8.0, 9.0);
    tap.tassert ("UntypedComplex::operator=(Complex128)",
                 uc.real == 8.0 && uc.imag == 9.0);

    uc = UntypedComplex::make (1.0, 2.0) * UntypedComplex::make (3.0, 4.0);
    tap.tassert ("UntypedComplex::operator*",
                 uc.real == -5.0 && uc.imag == 10.0);
    uc = UntypedComplex::make (1.0, 2.0) / UntypedComplex::make (3.0, 4.0);
    tap.tassert ("UntypedComplex::operator/",
                 abs (uc.real - .44) < .01 && abs (uc.imag - .08) < .01);
    uc = UntypedComplex::make (1.0, 2.0) + UntypedComplex::make (3.0, 4.0);
    tap.tassert ("UntypedComplex::operator+",
                 uc.real == 4.0 && uc.imag == 6.0);
    uc = UntypedComplex::make (1.0, 2.0) - UntypedComplex::make (3.0, 4.0);
    tap.tassert ("UntypedComplex::operator-",
                 uc.real == -2.0 && uc.imag == -2.0);
    uc = -UntypedComplex::make (1.0, 2.0);
    tap.tassert ("UntypedComplex::operator-",
                 uc.real == -1.0 && uc.imag == -2.0);
  }

  {
    Value v;
    tap.tassert ("Value::Value", v.present == false);
  }

  {
    Value v;
    v.present = true;

    tap.tassert ("Value::representable nil -> pointer",
                 v.representable (type::UntypedNil::instance (), type::named_int.get_pointer ()) == true);
    tap.tassert ("Value::representable nil -> slice",
                 v.representable (type::UntypedNil::instance (), type::named_int.get_slice ()) == true);
    tap.tassert ("Value::representable nil -> int",
                 v.representable (type::UntypedNil::instance (), &type::named_int) == false);

    tap.tassert ("Value::representable boolean -> bool",
                 v.representable (type::UntypedBoolean::instance (), &type::named_bool) == true);
    tap.tassert ("Value::representable boolean -> int",
                 v.representable (type::UntypedBoolean::instance (), &type::named_int) == false);

    v.untyped_rune_value = 555;
    tap.tassert ("Value::representable rune -> int32",
                 v.representable (type::UntypedRune::instance (), &type::named_int32) == true);
    tap.tassert ("Value::representable rune -> int8",
                 v.representable (type::UntypedRune::instance (), &type::named_int8) == false);

    v.untyped_integer_value = 555;
    tap.tassert ("Value::representable integer -> int32",
                 v.representable (type::UntypedInteger::instance (), &type::named_int32) == true);
    tap.tassert ("Value::representable integer -> int8",
                 v.representable (type::UntypedInteger::instance (), &type::named_int8) == false);

    v.untyped_float_value = 3.5;
    tap.tassert ("Value::representable float -> float32",
                 v.representable (type::UntypedFloat::instance (), &type::named_float32) == true);
    tap.tassert ("Value::representable float -> int32",
                 v.representable (type::UntypedFloat::instance (), &type::named_int32) == false);

    v.untyped_complex_value = UntypedComplex::make (1.0, 2.0);
    tap.tassert ("Value::representable complex -> complex64",
                 v.representable (type::UntypedComplex::instance (), &type::named_complex64) == true);
    tap.tassert ("Value::representable complex -> int32",
                 v.representable (type::UntypedComplex::instance (), &type::named_int32) == false);
    tap.tassert ("Value::representable complex -> string",
                 v.representable (type::UntypedComplex::instance (), &type::named_string) == false);
    v.untyped_complex_value = UntypedComplex::make (1.0, 0.0);
    tap.tassert ("Value::representable complex -> uint8",
                 v.representable (type::UntypedComplex::instance (), &type::named_uint8) == true);
    tap.tassert ("Value::representable complex -> uint16",
                 v.representable (type::UntypedComplex::instance (), &type::named_uint16) == true);
    tap.tassert ("Value::representable complex -> uint32",
                 v.representable (type::UntypedComplex::instance (), &type::named_uint32) == true);
    tap.tassert ("Value::representable complex -> uint64",
                 v.representable (type::UntypedComplex::instance (), &type::named_uint64) == true);
    tap.tassert ("Value::representable complex -> int8",
                 v.representable (type::UntypedComplex::instance (), &type::named_int8) == true);
    tap.tassert ("Value::representable complex -> int16",
                 v.representable (type::UntypedComplex::instance (), &type::named_int16) == true);
    tap.tassert ("Value::representable complex -> int32",
                 v.representable (type::UntypedComplex::instance (), &type::named_int32) == true);
    tap.tassert ("Value::representable complex -> int64",
                 v.representable (type::UntypedComplex::instance (), &type::named_int64) == true);
    tap.tassert ("Value::representable complex -> float32",
                 v.representable (type::UntypedComplex::instance (), &type::named_float32) == true);
    tap.tassert ("Value::representable complex -> float64",
                 v.representable (type::UntypedComplex::instance (), &type::named_float64) == true);
    tap.tassert ("Value::representable complex -> complex64",
                 v.representable (type::UntypedComplex::instance (), &type::named_complex64) == true);
    tap.tassert ("Value::representable complex -> complex128",
                 v.representable (type::UntypedComplex::instance (), &type::named_complex128) == true);
    tap.tassert ("Value::representable complex -> uint",
                 v.representable (type::UntypedComplex::instance (), &type::named_uint) == true);
    tap.tassert ("Value::representable complex -> int",
                 v.representable (type::UntypedComplex::instance (), &type::named_int) == true);
    tap.tassert ("Value::representable complex -> uintptr",
                 v.representable (type::UntypedComplex::instance (), &type::named_uintptr) == true);

    tap.tassert ("Value::representable string -> string",
                 v.representable (type::UntypedString::instance (), &type::named_string) == true);
    tap.tassert ("Value::representable string -> int32",
                 v.representable (type::UntypedString::instance (), &type::named_int32) == false);
  }

  {
    Value v;
    v.present = true;

    v.untyped_boolean_value = true;
    v.convert (type::UntypedBoolean::instance (), &type::named_bool);
    tap.tassert ("Value::convert boolean -> bool", v.bool_value == true);

    v.untyped_rune_value = 120;
    v.convert (type::UntypedRune::instance (), &type::named_uint8);
    tap.tassert ("Value::convert rune -> uint8", v.uint8_value == 120);

    v.untyped_integer_value = 120;
    v.convert (type::UntypedInteger::instance (), &type::named_uint16);
    tap.tassert ("Value::convert integer -> uint16", v.uint16_value == 120);

    v.untyped_float_value = 120;
    v.convert (type::UntypedFloat::instance (), &type::named_uint32);
    tap.tassert ("Value::convert float -> uint32", v.uint32_value == 120);

    v.uint64_value = 120;
    v.convert (&type::named_uint8, &type::named_uint64);
    tap.tassert ("Value::convert uint8 -> uint64", v.uint64_value == 120);

    v.uint16_value = 120;
    v.convert (&type::named_uint16, &type::named_int8);
    tap.tassert ("Value::convert uint16 -> int8", v.int8_value == 120);

    v.uint32_value = 120;
    v.convert (&type::named_uint32, &type::named_int16);
    tap.tassert ("Value::convert uint32 -> int16", v.int16_value == 120);

    v.uint64_value = 120;
    v.convert (&type::named_uint64, &type::named_int32);
    tap.tassert ("Value::convert uint64 -> int32", v.int32_value == 120);

    v.int64_value = 120;
    v.convert (&type::named_int8, &type::named_int64);
    tap.tassert ("Value::convert int8 -> int64", v.int64_value == 120);

    v.int16_value = 120;
    v.convert (&type::named_int16, &type::named_uint);
    tap.tassert ("Value::convert int16 -> uint", v.uint_value == 120);

    v.int32_value = 120;
    v.convert (&type::named_int32, &type::named_int);
    tap.tassert ("Value::convert int32 -> int", v.int_value == 120);

    v.int64_value = 120;
    v.convert (&type::named_int64, &type::named_uintptr);
    tap.tassert ("Value::convert int64 -> uintptr", v.uintptr_value == 120);

    v.uint_value = 120;
    v.convert (&type::named_uint, &type::named_float32);
    tap.tassert ("Value::convert uint -> float32", v.float32_value == 120);

    v.int_value = 120;
    v.convert (&type::named_int, &type::named_float64);
    tap.tassert ("Value::convert int -> float64", v.float64_value == 120);

    v.uintptr_value = 120;
    v.convert (&type::named_uintptr, &type::named_complex64);
    tap.tassert ("Value::convert uintptr -> complex64", v.complex64_value == 120);

    v.float32_value = 120;
    v.convert (&type::named_float32, &type::named_complex128);
    tap.tassert ("Value::convert float32 -> complex128", v.complex128_value == 120);

    v.float64_value = 120;
    v.convert (&type::named_float64, type::UntypedInteger::instance ());
    tap.tassert ("Value::convert float64 -> integer", v.untyped_integer_value == 120);

    v.complex64_value = 120;
    v.convert (&type::named_complex64, type::UntypedFloat::instance ());
    tap.tassert ("Value::convert complex64 -> float", v.untyped_float_value == 120);

    v.complex128_value = 120;
    v.convert (&type::named_complex128, &type::named_int8);
    tap.tassert ("Value::convert complex128 -> int8", v.int8_value == 120);

    v.untyped_string_value = runtime::String::make ("hello");
    v.convert (type::UntypedString::instance (), &type::named_string);
    tap.tassert ("Value::convert string -> string", v.string_value == "hello");

    v.convert (type::UntypedNil::instance (), type::named_int8.get_pointer ());
    tap.tassert ("Value::convert nil -> pointer", v.pointer_value == NULL);

    v.convert (type::UntypedNil::instance (), type::named_int8.get_slice ());
    tap.tassert ("Value::convert nil -> slice", v.slice_value.is_nil ());
  }

  {
    Value v;
    v.present = true;

    v.uint8_value = 8;
    tap.tassert ("Value::to_int", v.to_int (&type::named_uint8) == 8);
    v.uint16_value = 16;
    tap.tassert ("Value::to_int", v.to_int (&type::named_uint16) == 16);
    v.uint32_value = 32;
    tap.tassert ("Value::to_int", v.to_int (&type::named_uint32) == 32);
    v.uint64_value = 64;
    tap.tassert ("Value::to_int", v.to_int (&type::named_uint64) == 64);
    v.int8_value = 8;
    tap.tassert ("Value::to_int", v.to_int (&type::named_int8) == 8);
    v.int16_value = 16;
    tap.tassert ("Value::to_int", v.to_int (&type::named_int16) == 16);
    v.int32_value = 32;
    tap.tassert ("Value::to_int", v.to_int (&type::named_int32) == 32);
    v.int64_value = 64;
    tap.tassert ("Value::to_int", v.to_int (&type::named_int64) == 64);
    v.uint_value = 65;
    tap.tassert ("Value::to_int", v.to_int (&type::named_uint) == 65);
    v.int_value = 66;
    tap.tassert ("Value::to_int", v.to_int (&type::named_int) == 66);
    v.uintptr_value = 67;
    tap.tassert ("Value::to_int", v.to_int (&type::named_uintptr) == 67);
  }

  {
    Value out;
    Value left;
    left.present = true;
    Value right;
    right.present = true;

    equal (out, type::UntypedNil::instance (), left, right);
    tap.tassert ("nil equal", out.untyped_boolean_value == true);
    not_equal (out, type::UntypedNil::instance (), left, right);
    tap.tassert ("nil not_equal", out.untyped_boolean_value == false);

    left.untyped_boolean_value = true;
    right.untyped_boolean_value = true;
    equal (out, type::UntypedBoolean::instance (), left, right);
    tap.tassert ("boolean equal", out.untyped_boolean_value == true);
    not_equal (out, type::UntypedBoolean::instance (), left, right);
    tap.tassert ("boolean not_equal", out.untyped_boolean_value == false);

    left.untyped_rune_value = 113;
    right.untyped_rune_value = 89;
    equal (out, type::UntypedRune::instance (), left, right);
    tap.tassert ("rune equal", out.untyped_boolean_value == false);
    not_equal (out, type::UntypedRune::instance (), left, right);
    tap.tassert ("rune not_equal", out.untyped_boolean_value == true);
    less_than (out, type::UntypedRune::instance (), left, right);
    tap.tassert ("rune less_than", out.untyped_boolean_value == false);
    less_equal (out, type::UntypedRune::instance (), left, right);
    tap.tassert ("rune less_equal", out.untyped_boolean_value == false);
    more_than (out, type::UntypedRune::instance (), left, right);
    tap.tassert ("rune more_than", out.untyped_boolean_value == true);
    more_equal (out, type::UntypedRune::instance (), left, right);
    tap.tassert ("rune more_equal", out.untyped_boolean_value == true);
    multiply (out, type::UntypedRune::instance (), left, right);
    tap.tassert ("rune multiply", out.untyped_rune_value == 113 * 89);
    divide (out, type::UntypedRune::instance (), left, right);
    tap.tassert ("rune divide", out.untyped_rune_value == 113 / 89);
    modulus (out, type::UntypedRune::instance (), left, right);
    tap.tassert ("rune modulus", out.untyped_rune_value == 113 % 89);
    add (out, type::UntypedRune::instance (), left, right);
    tap.tassert ("rune add", out.untyped_rune_value == 113 + 89);
    subtract (out, type::UntypedRune::instance (), left, right);
    tap.tassert ("rune subtract", out.untyped_rune_value == 113 - 89);
    bit_and (out, type::UntypedRune::instance (), left, right);
    tap.tassert ("rune bit_and", out.untyped_rune_value == (113 & 89));
    bit_and_not (out, type::UntypedRune::instance (), left, right);
    tap.tassert ("rune bit_and_not", out.untyped_rune_value == (113 & (~89)));
    bit_xor (out, type::UntypedRune::instance (), left, right);
    tap.tassert ("rune bit_xor", out.untyped_rune_value == (113 ^ 89));
    bit_or (out, type::UntypedRune::instance (), left, right);
    tap.tassert ("rune bit_or", out.untyped_rune_value == (113 | 89));

    left.untyped_integer_value = 113;
    right.untyped_integer_value = 89;
    equal (out, type::UntypedInteger::instance (), left, right);
    tap.tassert ("integer equal", out.untyped_boolean_value == false);
    not_equal (out, type::UntypedInteger::instance (), left, right);
    tap.tassert ("integer not_equal", out.untyped_boolean_value == true);
    less_than (out, type::UntypedInteger::instance (), left, right);
    tap.tassert ("integer less_than", out.untyped_boolean_value == false);
    less_equal (out, type::UntypedInteger::instance (), left, right);
    tap.tassert ("integer less_equal", out.untyped_boolean_value == false);
    more_than (out, type::UntypedInteger::instance (), left, right);
    tap.tassert ("integer more_than", out.untyped_boolean_value == true);
    more_equal (out, type::UntypedInteger::instance (), left, right);
    tap.tassert ("integer more_equal", out.untyped_boolean_value == true);
    multiply (out, type::UntypedInteger::instance (), left, right);
    tap.tassert ("integer multiply", out.untyped_integer_value == 113 * 89);
    divide (out, type::UntypedInteger::instance (), left, right);
    tap.tassert ("integer divide", out.untyped_integer_value == 113 / 89);
    modulus (out, type::UntypedInteger::instance (), left, right);
    tap.tassert ("integer modulus", out.untyped_integer_value == 113 % 89);
    add (out, type::UntypedInteger::instance (), left, right);
    tap.tassert ("integer add", out.untyped_integer_value == 113 + 89);
    subtract (out, type::UntypedInteger::instance (), left, right);
    tap.tassert ("integer subtract", out.untyped_integer_value == 113 - 89);
    bit_and (out, type::UntypedInteger::instance (), left, right);
    tap.tassert ("integer bit_and", out.untyped_integer_value == (113 & 89));
    bit_and_not (out, type::UntypedInteger::instance (), left, right);
    tap.tassert ("integer bit_and_not", out.untyped_integer_value == (113 & (~89)));
    bit_xor (out, type::UntypedInteger::instance (), left, right);
    tap.tassert ("integer bit_xor", out.untyped_integer_value == (113 ^ 89));
    bit_or (out, type::UntypedInteger::instance (), left, right);
    tap.tassert ("integer bit_or", out.untyped_integer_value == (113 | 89));

    left.untyped_float_value = 114;
    right.untyped_float_value = 57;
    equal (out, type::UntypedFloat::instance (), left, right);
    tap.tassert ("float equal", out.untyped_boolean_value == false);
    not_equal (out, type::UntypedFloat::instance (), left, right);
    tap.tassert ("float not_equal", out.untyped_boolean_value == true);
    less_than (out, type::UntypedFloat::instance (), left, right);
    tap.tassert ("float less_than", out.untyped_boolean_value == false);
    less_equal (out, type::UntypedFloat::instance (), left, right);
    tap.tassert ("float less_equal", out.untyped_boolean_value == false);
    more_than (out, type::UntypedFloat::instance (), left, right);
    tap.tassert ("float more_than", out.untyped_boolean_value == true);
    more_equal (out, type::UntypedFloat::instance (), left, right);
    tap.tassert ("float more_equal", out.untyped_boolean_value == true);
    multiply (out, type::UntypedFloat::instance (), left, right);
    tap.tassert ("float multiply", out.untyped_float_value == 114 * 57);
    divide (out, type::UntypedFloat::instance (), left, right);
    tap.tassert ("float divide", out.untyped_float_value == 114 / 57);
    add (out, type::UntypedFloat::instance (), left, right);
    tap.tassert ("float add", out.untyped_float_value == 114 + 57);
    subtract (out, type::UntypedFloat::instance (), left, right);
    tap.tassert ("float subtract", out.untyped_float_value == 114 - 57);

    left.untyped_complex_value = 114;
    right.untyped_complex_value = 57;
    equal (out, type::UntypedComplex::instance (), left, right);
    tap.tassert ("complex equal", out.untyped_boolean_value == false);
    not_equal (out, type::UntypedComplex::instance (), left, right);
    tap.tassert ("complex not_equal", out.untyped_boolean_value == true);
    multiply (out, type::UntypedComplex::instance (), left, right);
    tap.tassert ("complex multiply", out.untyped_complex_value == 114 * 57);
    divide (out, type::UntypedComplex::instance (), left, right);
    tap.tassert ("complex divide", out.untyped_complex_value == 114 / 57);
    add (out, type::UntypedComplex::instance (), left, right);
    tap.tassert ("complex add", out.untyped_complex_value == 114 + 57);
    subtract (out, type::UntypedComplex::instance (), left, right);
    tap.tassert ("complex subtract", out.untyped_complex_value == 114 - 57);

    left.untyped_string_value = runtime::String::make ("aardvark");
    right.untyped_string_value = runtime::String::make ("zebra");
    equal (out, type::UntypedString::instance (), left, right);
    tap.tassert ("string equal", out.untyped_boolean_value == false);
    not_equal (out, type::UntypedString::instance (), left, right);
    tap.tassert ("string not_equal", out.untyped_boolean_value == true);
    less_than (out, type::UntypedString::instance (), left, right);
    tap.tassert ("string less_than", out.untyped_boolean_value == true);
    less_equal (out, type::UntypedString::instance (), left, right);
    tap.tassert ("string less_equal", out.untyped_boolean_value == true);
    more_than (out, type::UntypedString::instance (), left, right);
    tap.tassert ("string more_than", out.untyped_boolean_value == false);
    more_equal (out, type::UntypedString::instance (), left, right);
    tap.tassert ("string more_equal", out.untyped_boolean_value == false);

    left.bool_value = true;
    right.bool_value = true;
    equal (out, &type::named_bool, left, right);
    tap.tassert ("bool equal", out.untyped_boolean_value == true);
    not_equal (out, &type::named_bool, left, right);
    tap.tassert ("bool not_equal", out.untyped_boolean_value == false);

    left.uint8_value = 113;
    right.uint8_value = 2;
    equal (out, &type::named_uint8, left, right);
    tap.tassert ("uint8 equal", out.untyped_boolean_value == false);
    not_equal (out, &type::named_uint8, left, right);
    tap.tassert ("uint8 not_equal", out.untyped_boolean_value == true);
    less_than (out, &type::named_uint8, left, right);
    tap.tassert ("uint8 less_than", out.untyped_boolean_value == false);
    less_equal (out, &type::named_uint8, left, right);
    tap.tassert ("uint8 less_equal", out.untyped_boolean_value == false);
    more_than (out, &type::named_uint8, left, right);
    tap.tassert ("uint8 more_than", out.untyped_boolean_value == true);
    more_equal (out, &type::named_uint8, left, right);
    tap.tassert ("uint8 more_equal", out.untyped_boolean_value == true);
    multiply (out, &type::named_uint8, left, right);
    tap.tassert ("uint8 multiply", out.uint8_value == 113 * 2);
    divide (out, &type::named_uint8, left, right);
    tap.tassert ("uint8 divide", out.uint8_value == 113 / 2);
    modulus (out, &type::named_uint8, left, right);
    tap.tassert ("uint8 modulus", out.uint8_value == 113 % 2);
    add (out, &type::named_uint8, left, right);
    tap.tassert ("uint8 add", out.uint8_value == 113 + 2);
    subtract (out, &type::named_uint8, left, right);
    tap.tassert ("uint8 subtract", out.uint8_value == 113 - 2);
    bit_and (out, &type::named_uint8, left, right);
    tap.tassert ("uint8 bit_and", out.uint8_value == (113 & 2));
    bit_and_not (out, &type::named_uint8, left, right);
    tap.tassert ("uint8 bit_and_not", out.uint8_value == (113 & (~2)));
    bit_xor (out, &type::named_uint8, left, right);
    tap.tassert ("uint8 bit_xor", out.uint8_value == (113 ^ 2));
    bit_or (out, &type::named_uint8, left, right);
    tap.tassert ("uint8 bit_or", out.uint8_value == (113 | 2));

    left.uint16_value = 113;
    right.uint16_value = 2;
    equal (out, &type::named_uint16, left, right);
    tap.tassert ("uint16 equal", out.untyped_boolean_value == false);
    not_equal (out, &type::named_uint16, left, right);
    tap.tassert ("uint16 not_equal", out.untyped_boolean_value == true);
    less_than (out, &type::named_uint16, left, right);
    tap.tassert ("uint16 less_than", out.untyped_boolean_value == false);
    less_equal (out, &type::named_uint16, left, right);
    tap.tassert ("uint16 less_equal", out.untyped_boolean_value == false);
    more_than (out, &type::named_uint16, left, right);
    tap.tassert ("uint16 more_than", out.untyped_boolean_value == true);
    more_equal (out, &type::named_uint16, left, right);
    tap.tassert ("uint16 more_equal", out.untyped_boolean_value == true);
    multiply (out, &type::named_uint16, left, right);
    tap.tassert ("uint16 multiply", out.uint16_value == 113 * 2);
    divide (out, &type::named_uint16, left, right);
    tap.tassert ("uint16 divide", out.uint16_value == 113 / 2);
    modulus (out, &type::named_uint16, left, right);
    tap.tassert ("uint16 modulus", out.uint16_value == 113 % 2);
    add (out, &type::named_uint16, left, right);
    tap.tassert ("uint16 add", out.uint16_value == 113 + 2);
    subtract (out, &type::named_uint16, left, right);
    tap.tassert ("uint16 subtract", out.uint16_value == 113 - 2);
    bit_and (out, &type::named_uint16, left, right);
    tap.tassert ("uint16 bit_and", out.uint16_value == (113 & 2));
    bit_and_not (out, &type::named_uint16, left, right);
    tap.tassert ("uint16 bit_and_not", out.uint16_value == (113 & (~2)));
    bit_xor (out, &type::named_uint16, left, right);
    tap.tassert ("uint16 bit_xor", out.uint16_value == (113 ^ 2));
    bit_or (out, &type::named_uint16, left, right);
    tap.tassert ("uint16 bit_or", out.uint16_value == (113 | 2));

    left.uint32_value = 113;
    right.uint32_value = 2;
    equal (out, &type::named_uint32, left, right);
    tap.tassert ("uint32 equal", out.untyped_boolean_value == false);
    not_equal (out, &type::named_uint32, left, right);
    tap.tassert ("uint32 not_equal", out.untyped_boolean_value == true);
    less_than (out, &type::named_uint32, left, right);
    tap.tassert ("uint32 less_than", out.untyped_boolean_value == false);
    less_equal (out, &type::named_uint32, left, right);
    tap.tassert ("uint32 less_equal", out.untyped_boolean_value == false);
    more_than (out, &type::named_uint32, left, right);
    tap.tassert ("uint32 more_than", out.untyped_boolean_value == true);
    more_equal (out, &type::named_uint32, left, right);
    tap.tassert ("uint32 more_equal", out.untyped_boolean_value == true);
    multiply (out, &type::named_uint32, left, right);
    tap.tassert ("uint32 multiply", out.uint32_value == 113 * 2);
    divide (out, &type::named_uint32, left, right);
    tap.tassert ("uint32 divide", out.uint32_value == 113 / 2);
    modulus (out, &type::named_uint32, left, right);
    tap.tassert ("uint32 modulus", out.uint32_value == 113 % 2);
    add (out, &type::named_uint32, left, right);
    tap.tassert ("uint32 add", out.uint32_value == 113 + 2);
    subtract (out, &type::named_uint32, left, right);
    tap.tassert ("uint32 subtract", out.uint32_value == 113 - 2);
    bit_and (out, &type::named_uint32, left, right);
    tap.tassert ("uint32 bit_and", out.uint32_value == (113 & 2));
    bit_and_not (out, &type::named_uint32, left, right);
    tap.tassert ("uint32 bit_and_not", out.uint32_value == (113 & (~2)));
    bit_xor (out, &type::named_uint32, left, right);
    tap.tassert ("uint32 bit_xor", out.uint32_value == (113 ^ 2));
    bit_or (out, &type::named_uint32, left, right);
    tap.tassert ("uint32 bit_or", out.uint32_value == (113 | 2));

    left.uint64_value = 113;
    right.uint64_value = 2;
    equal (out, &type::named_uint64, left, right);
    tap.tassert ("uint64 equal", out.untyped_boolean_value == false);
    not_equal (out, &type::named_uint64, left, right);
    tap.tassert ("uint64 not_equal", out.untyped_boolean_value == true);
    less_than (out, &type::named_uint64, left, right);
    tap.tassert ("uint64 less_than", out.untyped_boolean_value == false);
    less_equal (out, &type::named_uint64, left, right);
    tap.tassert ("uint64 less_equal", out.untyped_boolean_value == false);
    more_than (out, &type::named_uint64, left, right);
    tap.tassert ("uint64 more_than", out.untyped_boolean_value == true);
    more_equal (out, &type::named_uint64, left, right);
    tap.tassert ("uint64 more_equal", out.untyped_boolean_value == true);
    multiply (out, &type::named_uint64, left, right);
    tap.tassert ("uint64 multiply", out.uint64_value == 113 * 2);
    divide (out, &type::named_uint64, left, right);
    tap.tassert ("uint64 divide", out.uint64_value == 113 / 2);
    modulus (out, &type::named_uint64, left, right);
    tap.tassert ("uint64 modulus", out.uint64_value == 113 % 2);
    add (out, &type::named_uint64, left, right);
    tap.tassert ("uint64 add", out.uint64_value == 113 + 2);
    subtract (out, &type::named_uint64, left, right);
    tap.tassert ("uint64 subtract", out.uint64_value == 113 - 2);
    bit_and (out, &type::named_uint64, left, right);
    tap.tassert ("uint64 bit_and", out.uint64_value == (113 & 2));
    bit_and_not (out, &type::named_uint64, left, right);
    tap.tassert ("uint64 bit_and_not", out.uint64_value == (113 & (~2)));
    bit_xor (out, &type::named_uint64, left, right);
    tap.tassert ("uint64 bit_xor", out.uint64_value == (113 ^ 2));
    bit_or (out, &type::named_uint64, left, right);
    tap.tassert ("uint64 bit_or", out.uint64_value == (113 | 2));

    left.int8_value = 113;
    right.int8_value = 2;
    equal (out, &type::named_int8, left, right);
    tap.tassert ("int8 equal", out.untyped_boolean_value == false);
    not_equal (out, &type::named_int8, left, right);
    tap.tassert ("int8 not_equal", out.untyped_boolean_value == true);
    less_than (out, &type::named_int8, left, right);
    tap.tassert ("int8 less_than", out.untyped_boolean_value == false);
    less_equal (out, &type::named_int8, left, right);
    tap.tassert ("int8 less_equal", out.untyped_boolean_value == false);
    more_than (out, &type::named_int8, left, right);
    tap.tassert ("int8 more_than", out.untyped_boolean_value == true);
    more_equal (out, &type::named_int8, left, right);
    tap.tassert ("int8 more_equal", out.untyped_boolean_value == true);
    multiply (out, &type::named_int8, left, right);
    tap.tassert ("int8 multiply", out.int8_value == -30);
    divide (out, &type::named_int8, left, right);
    tap.tassert ("int8 divide", out.int8_value == 113 / 2);
    modulus (out, &type::named_int8, left, right);
    tap.tassert ("int8 modulus", out.int8_value == 113 % 2);
    add (out, &type::named_int8, left, right);
    tap.tassert ("int8 add", out.int8_value == 113 + 2);
    subtract (out, &type::named_int8, left, right);
    tap.tassert ("int8 subtract", out.int8_value == 113 - 2);
    bit_and (out, &type::named_int8, left, right);
    tap.tassert ("int8 bit_and", out.int8_value == (113 & 2));
    bit_and_not (out, &type::named_int8, left, right);
    tap.tassert ("int8 bit_and_not", out.int8_value == (113 & (~2)));
    bit_xor (out, &type::named_int8, left, right);
    tap.tassert ("int8 bit_xor", out.int8_value == (113 ^ 2));
    bit_or (out, &type::named_int8, left, right);
    tap.tassert ("int8 bit_or", out.int8_value == (113 | 2));

    left.int16_value = 113;
    right.int16_value = 2;
    equal (out, &type::named_int16, left, right);
    tap.tassert ("int16 equal", out.untyped_boolean_value == false);
    not_equal (out, &type::named_int16, left, right);
    tap.tassert ("int16 not_equal", out.untyped_boolean_value == true);
    less_than (out, &type::named_int16, left, right);
    tap.tassert ("int16 less_than", out.untyped_boolean_value == false);
    less_equal (out, &type::named_int16, left, right);
    tap.tassert ("int16 less_equal", out.untyped_boolean_value == false);
    more_than (out, &type::named_int16, left, right);
    tap.tassert ("int16 more_than", out.untyped_boolean_value == true);
    more_equal (out, &type::named_int16, left, right);
    tap.tassert ("int16 more_equal", out.untyped_boolean_value == true);
    multiply (out, &type::named_int16, left, right);
    tap.tassert ("int16 multiply", out.int16_value == 113 * 2);
    divide (out, &type::named_int16, left, right);
    tap.tassert ("int16 divide", out.int16_value == 113 / 2);
    modulus (out, &type::named_int16, left, right);
    tap.tassert ("int16 modulus", out.int16_value == 113 % 2);
    add (out, &type::named_int16, left, right);
    tap.tassert ("int16 add", out.int16_value == 113 + 2);
    subtract (out, &type::named_int16, left, right);
    tap.tassert ("int16 subtract", out.int16_value == 113 - 2);
    bit_and (out, &type::named_int16, left, right);
    tap.tassert ("int16 bit_and", out.int16_value == (113 & 2));
    bit_and_not (out, &type::named_int16, left, right);
    tap.tassert ("int16 bit_and_not", out.int16_value == (113 & (~2)));
    bit_xor (out, &type::named_int16, left, right);
    tap.tassert ("int16 bit_xor", out.int16_value == (113 ^ 2));
    bit_or (out, &type::named_int16, left, right);
    tap.tassert ("int16 bit_or", out.int16_value == (113 | 2));

    left.int32_value = 113;
    right.int32_value = 2;
    equal (out, &type::named_int32, left, right);
    tap.tassert ("int32 equal", out.untyped_boolean_value == false);
    not_equal (out, &type::named_int32, left, right);
    tap.tassert ("int32 not_equal", out.untyped_boolean_value == true);
    less_than (out, &type::named_int32, left, right);
    tap.tassert ("int32 less_than", out.untyped_boolean_value == false);
    less_equal (out, &type::named_int32, left, right);
    tap.tassert ("int32 less_equal", out.untyped_boolean_value == false);
    more_than (out, &type::named_int32, left, right);
    tap.tassert ("int32 more_than", out.untyped_boolean_value == true);
    more_equal (out, &type::named_int32, left, right);
    tap.tassert ("int32 more_equal", out.untyped_boolean_value == true);
    multiply (out, &type::named_int32, left, right);
    tap.tassert ("int32 multiply", out.int32_value == 113 * 2);
    divide (out, &type::named_int32, left, right);
    tap.tassert ("int32 divide", out.int32_value == 113 / 2);
    modulus (out, &type::named_int32, left, right);
    tap.tassert ("int32 modulus", out.int32_value == 113 % 2);
    add (out, &type::named_int32, left, right);
    tap.tassert ("int32 add", out.int32_value == 113 + 2);
    subtract (out, &type::named_int32, left, right);
    tap.tassert ("int32 subtract", out.int32_value == 113 - 2);
    bit_and (out, &type::named_int32, left, right);
    tap.tassert ("int32 bit_and", out.int32_value == (113 & 2));
    bit_and_not (out, &type::named_int32, left, right);
    tap.tassert ("int32 bit_and_not", out.int32_value == (113 & (~2)));
    bit_xor (out, &type::named_int32, left, right);
    tap.tassert ("int32 bit_xor", out.int32_value == (113 ^ 2));
    bit_or (out, &type::named_int32, left, right);
    tap.tassert ("int32 bit_or", out.int32_value == (113 | 2));

    left.int64_value = 113;
    right.int64_value = 2;
    equal (out, &type::named_int64, left, right);
    tap.tassert ("int64 equal", out.untyped_boolean_value == false);
    not_equal (out, &type::named_int64, left, right);
    tap.tassert ("int64 not_equal", out.untyped_boolean_value == true);
    less_than (out, &type::named_int64, left, right);
    tap.tassert ("int64 less_than", out.untyped_boolean_value == false);
    less_equal (out, &type::named_int64, left, right);
    tap.tassert ("int64 less_equal", out.untyped_boolean_value == false);
    more_than (out, &type::named_int64, left, right);
    tap.tassert ("int64 more_than", out.untyped_boolean_value == true);
    more_equal (out, &type::named_int64, left, right);
    tap.tassert ("int64 more_equal", out.untyped_boolean_value == true);
    multiply (out, &type::named_int64, left, right);
    tap.tassert ("int64 multiply", out.int64_value == 113 * 2);
    divide (out, &type::named_int64, left, right);
    tap.tassert ("int64 divide", out.int64_value == 113 / 2);
    modulus (out, &type::named_int64, left, right);
    tap.tassert ("int64 modulus", out.int64_value == 113 % 2);
    add (out, &type::named_int64, left, right);
    tap.tassert ("int64 add", out.int64_value == 113 + 2);
    subtract (out, &type::named_int64, left, right);
    tap.tassert ("int64 subtract", out.int64_value == 113 - 2);
    bit_and (out, &type::named_int64, left, right);
    tap.tassert ("int64 bit_and", out.int64_value == (113 & 2));
    bit_and_not (out, &type::named_int64, left, right);
    tap.tassert ("int64 bit_and_not", out.int64_value == (113 & (~2)));
    bit_xor (out, &type::named_int64, left, right);
    tap.tassert ("int64 bit_xor", out.int64_value == (113 ^ 2));
    bit_or (out, &type::named_int64, left, right);
    tap.tassert ("int64 bit_or", out.int64_value == (113 | 2));

    left.float32_value = 113;
    right.float32_value = 2;
    equal (out, &type::named_float32, left, right);
    tap.tassert ("float32 equal", out.untyped_boolean_value == false);
    not_equal (out, &type::named_float32, left, right);
    tap.tassert ("float32 not_equal", out.untyped_boolean_value == true);
    less_than (out, &type::named_float32, left, right);
    tap.tassert ("float32 less_than", out.untyped_boolean_value == false);
    less_equal (out, &type::named_float32, left, right);
    tap.tassert ("float32 less_equal", out.untyped_boolean_value == false);
    more_than (out, &type::named_float32, left, right);
    tap.tassert ("float32 more_than", out.untyped_boolean_value == true);
    more_equal (out, &type::named_float32, left, right);
    tap.tassert ("float32 more_equal", out.untyped_boolean_value == true);
    multiply (out, &type::named_float32, left, right);
    tap.tassert ("float32 multiply", out.float32_value == 113 * 2);
    divide (out, &type::named_float32, left, right);
    tap.tassert ("float32 divide", out.float32_value == 56.5);
    add (out, &type::named_float32, left, right);
    tap.tassert ("float32 add", out.float32_value == 113 + 2);
    subtract (out, &type::named_float32, left, right);
    tap.tassert ("float32 subtract", out.float32_value == 113 - 2);

    left.float64_value = 113;
    right.float64_value = 2;
    equal (out, &type::named_float64, left, right);
    tap.tassert ("float64 equal", out.untyped_boolean_value == false);
    not_equal (out, &type::named_float64, left, right);
    tap.tassert ("float64 not_equal", out.untyped_boolean_value == true);
    less_than (out, &type::named_float64, left, right);
    tap.tassert ("float64 less_than", out.untyped_boolean_value == false);
    less_equal (out, &type::named_float64, left, right);
    tap.tassert ("float64 less_equal", out.untyped_boolean_value == false);
    more_than (out, &type::named_float64, left, right);
    tap.tassert ("float64 more_than", out.untyped_boolean_value == true);
    more_equal (out, &type::named_float64, left, right);
    tap.tassert ("float64 more_equal", out.untyped_boolean_value == true);
    multiply (out, &type::named_float64, left, right);
    tap.tassert ("float64 multiply", out.float64_value == 113 * 2);
    divide (out, &type::named_float64, left, right);
    tap.tassert ("float64 divide", out.float64_value == 56.5);
    add (out, &type::named_float64, left, right);
    tap.tassert ("float64 add", out.float64_value == 113 + 2);
    subtract (out, &type::named_float64, left, right);
    tap.tassert ("float64 subtract", out.float64_value == 113 - 2);

    left.complex64_value = 113;
    right.complex64_value = 2;
    equal (out, &type::named_complex64, left, right);
    tap.tassert ("complex64 equal", out.untyped_boolean_value == false);
    not_equal (out, &type::named_complex64, left, right);
    tap.tassert ("complex64 not_equal", out.untyped_boolean_value == true);
    multiply (out, &type::named_complex64, left, right);
    tap.tassert ("complex64 multiply", out.complex64_value == 113 * 2);
    divide (out, &type::named_complex64, left, right);
    tap.tassert ("complex64 divide", out.complex64_value == 56.5);
    add (out, &type::named_complex64, left, right);
    tap.tassert ("complex64 add", out.complex64_value == 113 + 2);
    subtract (out, &type::named_complex64, left, right);
    tap.tassert ("complex64 subtract", out.complex64_value == 113 - 2);

    left.complex128_value = 113;
    right.complex128_value = 2;
    equal (out, &type::named_complex128, left, right);
    tap.tassert ("complex128 equal", out.untyped_boolean_value == false);
    not_equal (out, &type::named_complex128, left, right);
    tap.tassert ("complex128 not_equal", out.untyped_boolean_value == true);
    multiply (out, &type::named_complex128, left, right);
    tap.tassert ("complex128 multiply", out.complex128_value == 113 * 2);
    divide (out, &type::named_complex128, left, right);
    tap.tassert ("complex128 divide", out.complex128_value == 56.5);
    add (out, &type::named_complex128, left, right);
    tap.tassert ("complex128 add", out.complex128_value == 113 + 2);
    subtract (out, &type::named_complex128, left, right);
    tap.tassert ("complex128 subtract", out.complex128_value == 113 - 2);

    left.uint_value = 113;
    right.uint_value = 2;
    equal (out, &type::named_uint, left, right);
    tap.tassert ("uint equal", out.untyped_boolean_value == false);
    not_equal (out, &type::named_uint, left, right);
    tap.tassert ("uint not_equal", out.untyped_boolean_value == true);
    less_than (out, &type::named_uint, left, right);
    tap.tassert ("uint less_than", out.untyped_boolean_value == false);
    less_equal (out, &type::named_uint, left, right);
    tap.tassert ("uint less_equal", out.untyped_boolean_value == false);
    more_than (out, &type::named_uint, left, right);
    tap.tassert ("uint more_than", out.untyped_boolean_value == true);
    more_equal (out, &type::named_uint, left, right);
    tap.tassert ("uint more_equal", out.untyped_boolean_value == true);
    multiply (out, &type::named_uint, left, right);
    tap.tassert ("uint multiply", out.uint_value == 113 * 2);
    divide (out, &type::named_uint, left, right);
    tap.tassert ("uint divide", out.uint_value == 113 / 2);
    modulus (out, &type::named_uint, left, right);
    tap.tassert ("uint modulus", out.uint_value == 113 % 2);
    add (out, &type::named_uint, left, right);
    tap.tassert ("uint add", out.uint_value == 113 + 2);
    subtract (out, &type::named_uint, left, right);
    tap.tassert ("uint subtract", out.uint_value == 113 - 2);
    bit_and (out, &type::named_uint, left, right);
    tap.tassert ("uint bit_and", out.uint_value == (113 & 2));
    bit_and_not (out, &type::named_uint, left, right);
    tap.tassert ("uint bit_and_not", out.uint_value == (113 & (~2)));
    bit_xor (out, &type::named_uint, left, right);
    tap.tassert ("uint bit_xor", out.uint_value == (113 ^ 2));
    bit_or (out, &type::named_uint, left, right);
    tap.tassert ("uint bit_or", out.uint_value == (113 | 2));

    left.int_value = 113;
    right.int_value = 2;
    equal (out, &type::named_int, left, right);
    tap.tassert ("int equal", out.untyped_boolean_value == false);
    not_equal (out, &type::named_int, left, right);
    tap.tassert ("int not_equal", out.untyped_boolean_value == true);
    less_than (out, &type::named_int, left, right);
    tap.tassert ("int less_than", out.untyped_boolean_value == false);
    less_equal (out, &type::named_int, left, right);
    tap.tassert ("int less_equal", out.untyped_boolean_value == false);
    more_than (out, &type::named_int, left, right);
    tap.tassert ("int more_than", out.untyped_boolean_value == true);
    more_equal (out, &type::named_int, left, right);
    tap.tassert ("int more_equal", out.untyped_boolean_value == true);
    multiply (out, &type::named_int, left, right);
    tap.tassert ("int multiply", out.int_value == 113 * 2);
    divide (out, &type::named_int, left, right);
    tap.tassert ("int divide", out.int_value == 113 / 2);
    modulus (out, &type::named_int, left, right);
    tap.tassert ("int modulus", out.int_value == 113 % 2);
    add (out, &type::named_int, left, right);
    tap.tassert ("int add", out.int_value == 113 + 2);
    subtract (out, &type::named_int, left, right);
    tap.tassert ("int subtract", out.int_value == 113 - 2);
    bit_and (out, &type::named_int, left, right);
    tap.tassert ("int bit_and", out.int_value == (113 & 2));
    bit_and_not (out, &type::named_int, left, right);
    tap.tassert ("int bit_and_not", out.int_value == (113 & (~2)));
    bit_xor (out, &type::named_int, left, right);
    tap.tassert ("int bit_xor", out.int_value == (113 ^ 2));
    bit_or (out, &type::named_int, left, right);
    tap.tassert ("int bit_or", out.int_value == (113 | 2));

    left.uintptr_value = 113;
    right.uintptr_value = 2;
    equal (out, &type::named_uintptr, left, right);
    tap.tassert ("uintptr equal", out.untyped_boolean_value == false);
    not_equal (out, &type::named_uintptr, left, right);
    tap.tassert ("uintptr not_equal", out.untyped_boolean_value == true);
    less_than (out, &type::named_uintptr, left, right);
    tap.tassert ("uintptr less_than", out.untyped_boolean_value == false);
    less_equal (out, &type::named_uintptr, left, right);
    tap.tassert ("uintptr less_equal", out.untyped_boolean_value == false);
    more_than (out, &type::named_uintptr, left, right);
    tap.tassert ("uintptr more_than", out.untyped_boolean_value == true);
    more_equal (out, &type::named_uintptr, left, right);
    tap.tassert ("uintptr more_equal", out.untyped_boolean_value == true);
    multiply (out, &type::named_uintptr, left, right);
    tap.tassert ("uintptr multiply", out.uintptr_value == 113 * 2);
    divide (out, &type::named_uintptr, left, right);
    tap.tassert ("uintptr divide", out.uintptr_value == 113 / 2);
    modulus (out, &type::named_uintptr, left, right);
    tap.tassert ("uintptr modulus", out.uintptr_value == 113 % 2);
    add (out, &type::named_uintptr, left, right);
    tap.tassert ("uintptr add", out.uintptr_value == 113 + 2);
    subtract (out, &type::named_uintptr, left, right);
    tap.tassert ("uintptr subtract", out.uintptr_value == 113 - 2);
    bit_and (out, &type::named_uintptr, left, right);
    tap.tassert ("uintptr bit_and", out.uintptr_value == (113 & 2));
    bit_and_not (out, &type::named_uintptr, left, right);
    tap.tassert ("uintptr bit_and_not", out.uintptr_value == (113 & (~2)));
    bit_xor (out, &type::named_uintptr, left, right);
    tap.tassert ("uintptr bit_xor", out.uintptr_value == (113 ^ 2));
    bit_or (out, &type::named_uintptr, left, right);
    tap.tassert ("uintptr bit_or", out.uintptr_value == (113 | 2));

    left.string_value = runtime::String::make ("aardvark");
    right.string_value = runtime::String::make ("zebra");
    equal (out, &type::named_string, left, right);
    tap.tassert ("string equal", out.untyped_boolean_value == false);
    not_equal (out, &type::named_string, left, right);
    tap.tassert ("string not_equal", out.untyped_boolean_value == true);
    less_than (out, &type::named_string, left, right);
    tap.tassert ("string less_than", out.untyped_boolean_value == true);
    less_equal (out, &type::named_string, left, right);
    tap.tassert ("string less_equal", out.untyped_boolean_value == true);
    more_than (out, &type::named_string, left, right);
    tap.tassert ("string more_than", out.untyped_boolean_value == false);
    more_equal (out, &type::named_string, left, right);
    tap.tassert ("string more_equal", out.untyped_boolean_value == false);

    left.pointer_value = NULL;
    right.pointer_value = NULL;
    equal (out, type::named_int.get_pointer (), left, right);
    tap.tassert ("nil equal", out.untyped_boolean_value == true);
    not_equal (out, type::named_int.get_pointer (), left, right);
    tap.tassert ("nil not_equal", out.untyped_boolean_value == false);
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.print (type::UntypedNil::instance (), str);
    tap.tassert ("nil print", str.str () == "<nil>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.untyped_boolean_value = true;
    v.print (type::UntypedBoolean::instance (), str);
    tap.tassert ("boolean print", str.str () == "<true>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.untyped_rune_value = 89;
    v.print (type::UntypedRune::instance (), str);
    tap.tassert ("rune print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.untyped_integer_value = 89;
    v.print (type::UntypedInteger::instance (), str);
    tap.tassert ("integer print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.untyped_float_value = 89;
    v.print (type::UntypedFloat::instance (), str);
    tap.tassert ("float print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.untyped_complex_value = 89;
    v.print (type::UntypedComplex::instance (), str);
    tap.tassert ("complex print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.untyped_string_value = runtime::String::make ("hello");
    v.print (type::UntypedString::instance (), str);
    tap.tassert ("string print", str.str () == "<hello>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.bool_value = true;
    v.print (&type::named_bool, str);
    tap.tassert ("bool print", str.str () == "<true>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.uint8_value = 89;
    v.print (&type::named_uint8, str);
    tap.tassert ("uint8 print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.uint16_value = 89;
    v.print (&type::named_uint16, str);
    tap.tassert ("uint16 print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.uint32_value = 89;
    v.print (&type::named_uint32, str);
    tap.tassert ("uint32 print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.uint64_value = 89;
    v.print (&type::named_uint64, str);
    tap.tassert ("uint64 print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.int8_value = 89;
    v.print (&type::named_int8, str);
    tap.tassert ("int8 print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.int16_value = 89;
    v.print (&type::named_int16, str);
    tap.tassert ("int16 print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.int32_value = 89;
    v.print (&type::named_int32, str);
    tap.tassert ("int32 print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.int64_value = 89;
    v.print (&type::named_int64, str);
    tap.tassert ("int64 print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.float32_value = 89;
    v.print (&type::named_float32, str);
    tap.tassert ("float32 print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.float64_value = 89;
    v.print (&type::named_float64, str);
    tap.tassert ("float64 print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.complex64_value = 89;
    v.print (&type::named_complex64, str);
    tap.tassert ("complex64 print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.complex128_value = 89;
    v.print (&type::named_complex128, str);
    tap.tassert ("complex128 print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.uint_value = 89;
    v.print (&type::named_uint, str);
    tap.tassert ("uint print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.int_value = 89;
    v.print (&type::named_int, str);
    tap.tassert ("int print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.uintptr_value = 89;
    v.print (&type::named_uintptr, str);
    tap.tassert ("uintptr print", str.str () == "<89>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.string_value = runtime::String::make ("hello");
    v.print (&type::named_string, str);
    tap.tassert ("string print", str.str () == "<hello>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.pointer_value = NULL;
    v.print (type::named_string.get_pointer (), str);
    tap.tassert ("pointer print", str.str () == "<0>");
  }

  {
    std::stringstream str;
    Value v;
    v.present = true;
    v.pointer_value = NULL;
    str << ValuePrinter (type::named_string.get_pointer (), v);
    tap.tassert ("operator<< ValuePrinter", str.str () == "<0>");
  }

  tap.print_plan ();

  return 0;
}
