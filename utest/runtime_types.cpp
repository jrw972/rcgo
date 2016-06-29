#include "tap.hpp"
#include "runtime_types.hpp"
#include "mock_execution.hpp"

#include <cstring>
#include <utility>
#include <sstream>

using namespace std::rel_ops;
using namespace runtime;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    Complex64 c64 = Complex64::make (3.0, 4.0);
    tap.tassert ("Complex64::make",
                 c64.real == 3.0 && c64.imag == 4.0);
    tap.tassert ("Complex64::operator==",
                 c64 == c64);
    double d = c64;
    tap.tassert ("Complex64::operator double()",
                 d == 3.0);
    c64 = 2.0;
    tap.tassert ("Complex64::operator=(double)",
                 c64.real == 2.0 && c64.imag == 0.0);
    ++c64;
    tap.tassert ("Complex64::operator++",
                 c64.real == 3.0 && c64.imag == 0.0);
    --c64;
    tap.tassert ("Complex64::operator--",
                 c64.real == 2.0 && c64.imag == 0.0);
    c64 += Complex64::make (1.0, 1.0);
    tap.tassert ("Complex64::operator+=",
                 c64.real == 3.0 && c64.imag == 1.0);
    c64 = Complex64::make (1.0, 2.0) * Complex64::make (3.0, 4.0);
    tap.tassert ("Complex64::operator*",
                 c64.real == -5.0 && c64.imag == 10.0);
    c64 = Complex64::make (1.0, 2.0) / Complex64::make (3.0, 4.0);
    tap.tassert ("Complex64::operator/",
                 abs (c64.real - .44) < .01 && abs (c64.imag - .08) < .01);
    c64 = Complex64::make (1.0, 2.0) + Complex64::make (3.0, 4.0);
    tap.tassert ("Complex64::operator+",
                 c64.real == 4.0 && c64.imag == 6.0);
    c64 = Complex64::make (1.0, 2.0) - Complex64::make (3.0, 4.0);
    tap.tassert ("Complex64::operator-",
                 c64.real == -2.0 && c64.imag == -2.0);
    c64 = -Complex64::make (1.0, 2.0);
    tap.tassert ("Complex64::operator-",
                 c64.real == -1.0 && c64.imag == -2.0);
   }

  {
    Complex128 c128 = Complex128::make (3.0, 4.0);
    tap.tassert ("Complex128::make",
                 c128.real == 3.0 && c128.imag == 4.0);
    tap.tassert ("Complex128::operator==",
                 c128 == c128);
    double d = c128;
    tap.tassert ("Complex128::operator double()",
                 d == 3.0);
    c128 = 2.0;
    tap.tassert ("Complex128::operator=(double)",
                 c128.real == 2.0 && c128.imag == 0.0);
    ++c128;
    tap.tassert ("Complex128::operator++",
                 c128.real == 3.0 && c128.imag == 0.0);
    --c128;
    tap.tassert ("Complex128::operator--",
                 c128.real == 2.0 && c128.imag == 0.0);
    c128 += Complex128::make (1.0, 1.0);
    tap.tassert ("Complex128::operator+=",
                 c128.real == 3.0 && c128.imag == 1.0);
    c128 = Complex128::make (1.0, 2.0) * Complex128::make (3.0, 4.0);
    tap.tassert ("Complex128::operator*",
                 c128.real == -5.0 && c128.imag == 10.0);
    c128 = Complex128::make (1.0, 2.0) / Complex128::make (3.0, 4.0);
    tap.tassert ("Complex128::operator/",
                 abs (c128.real - .44) < .01 && abs (c128.imag - .08) < .01);
    c128 = Complex128::make (1.0, 2.0) + Complex128::make (3.0, 4.0);
    tap.tassert ("Complex128::operator+",
                 c128.real == 4.0 && c128.imag == 6.0);
    c128 = Complex128::make (1.0, 2.0) - Complex128::make (3.0, 4.0);
    tap.tassert ("Complex128::operator-",
                 c128.real == -2.0 && c128.imag == -2.0);
    c128 = -Complex128::make (1.0, 2.0);
    tap.tassert ("Complex128::operator-",
                 c128.real == -1.0 && c128.imag == -2.0);
   }

  {
    String s, t;
    s = String::make ("hello", 5);
    tap.tassert ("String::operator==",
                 s == s);
    t = String::make (s.ptr, 4);
    tap.tassert ("String::operator==",
                 s != t);
    void* o = malloc (5);
    memcpy (o, "hello", 5);
    t = String::make (o, 5);
    tap.tassert ("String::operator==",
                 s == t);
  }

  {
    String s, t;
    s = String::make ("alpha", 5);
    t = String::make ("beta", 4);
    tap.tassert ("String::operator<",
                 (s < t) == true);
    tap.tassert ("String::operator<",
                 (t < s) == false);
    t = String::make ("alpha", 5);
    tap.tassert ("String::operator<",
                 (s < t) == false);
    t = String::make ("alphaalpha", 10);
    tap.tassert ("String::operator<",
                 (s < t) == true);
  }

  {
    // Coverage.
    std::stringstream ss;
    String str = String::make ("hello", 5);
    Slice sli;
    ss << str;
    ss << sli;
  }

  {
    MockExecutor exec;
    FileDescriptor* fd = exec.allocate_file_descriptor (87);
    tap.tassert ("FileDescriptor::FileDescriptor", fd->fd () == 87);
  }

  tap.print_plan ();

  return 0;
}
