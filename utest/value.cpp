#include "tap.hpp"
#include "value.hpp"
#include "type.hpp"

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

  tap.print_plan ();

  return 0;
}
