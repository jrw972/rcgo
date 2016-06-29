#include "runtime_types.hpp"

#include <cstring>
#include <cmath>

namespace runtime
{

  Complex64 Complex64::make (float r, float i)
  {
    Complex64 c64;
    c64.real = r;
    c64.imag = i;
    return c64;
  }

bool Complex64::operator== (const Complex64& other) const
{
  return this->real == other.real && this->imag == other.imag;
}

Complex64::operator double() const
{
  return real;
}

Complex64& Complex64::operator= (double x)
{
  this->real = x;
  this->imag = 0;
  return *this;
}

Complex64& Complex64::operator++ ()
{
  this->real += 1;
  return *this;
}

Complex64& Complex64::operator-- ()
{
  this->real -= 1;
  return *this;
}

Complex64& Complex64::operator+= (const Complex64& x)
{
  this->real += x.real;
  this->imag += x.imag;
  return *this;
}

  Complex64 operator* (const Complex64& x, const Complex64& y)
{
  Complex64 c64;
  c64.real = x.real * y.real - x.imag * y.imag;
  c64.imag = x.real * y.imag + x.imag * y.real;
  return c64;
}
Complex64 operator/ (const Complex64& x, const Complex64& y)
{
  const float xmag = sqrt (x.real * x.real + x.imag * x.imag);
  const float xtheta = atan (x.imag / x.real);
  const float ymag = sqrt (y.real * y.real + y.imag * y.imag);
  const float ytheta = atan (y.imag / y.real);
  const float mag = xmag / ymag;
  const float theta = xtheta - ytheta;
  Complex64 c64;
  c64.real = mag * cos (theta);
  c64.imag = mag * sin (theta);
  return c64;
}

Complex64 operator+ (const Complex64& x, const Complex64& y)
{
  Complex64 c64;
  c64.real = x.real + y.real;
  c64.imag = x.imag + y.imag;
  return c64;
}
Complex64 operator- (const Complex64& x, const Complex64& y)
{
  Complex64 c64;
  c64.real = x.real - y.real;
  c64.imag = x.imag - y.imag;
  return c64;
}
Complex64 operator- (const Complex64& x)
{
  Complex64 c64;
  c64.real = -x.real;
  c64.imag = -x.imag;
  return c64;
}

  Complex128 Complex128::make (double r, double i)
  {
    Complex128 c128;
    c128.real = r;
    c128.imag = i;
    return c128;
  }

bool Complex128::operator== (const Complex128& other) const
{
  return this->real == other.real && this->imag == other.imag;
}

Complex128::operator double() const
{
  return real;
}

Complex128& Complex128::operator= (double x)
{
  this->real = x;
  this->imag = 0;
  return *this;
}

Complex128& Complex128::operator++ ()
{
  this->real += 1;
  return *this;
}

Complex128& Complex128::operator-- ()
{
  this->real -= 1;
  return *this;
}

Complex128& Complex128::operator+= (const Complex128& x)
{
  this->real += x.real;
  this->imag += x.imag;
  return *this;
}

Complex128 operator* (const Complex128& x, const Complex128& y)
{
  Complex128 c128;
  c128.real = x.real * y.real - x.imag * y.imag;
  c128.imag = x.real * y.imag + x.imag * y.real;
  return c128;
}
Complex128 operator/ (const Complex128& x, const Complex128& y)
{
  const double xmag = sqrt (x.real * x.real + x.imag * x.imag);
  const double xtheta = atan (x.imag / x.real);
  const double ymag = sqrt (y.real * y.real + y.imag * y.imag);
  const double ytheta = atan (y.imag / y.real);
  const double mag = xmag / ymag;
  const double theta = xtheta - ytheta;
  Complex128 c128;
  c128.real = mag * cos (theta);
  c128.imag = mag * sin (theta);
  return c128;
}
Complex128 operator+ (const Complex128& x, const Complex128& y)
{
  Complex128 c128;
  c128.real = x.real + y.real;
  c128.imag = x.imag + y.imag;
  return c128;
}
Complex128 operator- (const Complex128& x, const Complex128& y)
{
  Complex128 c128;
  c128.real = x.real - y.real;
  c128.imag = x.imag - y.imag;
  return c128;
}
Complex128 operator- (const Complex128& x)
{
  Complex128 c128;
  c128.real = -x.real;
  c128.imag = -x.imag;
  return c128;
}

  String String::make (const void* ptr, size_t length)
  {
    String s;
    s.ptr = ptr;
    s.length = length;
    return s;
  }

bool String::operator== (const String& other) const
{
  if (this->ptr == other.ptr &&
      this->length == other.length)
    {
      return true;
    }

  if (this->length != other.length)
    {
      return false;
    }

  return memcmp (this->ptr, other.ptr, this->length) == 0;
}

bool String::operator< (const String& other) const
{
  int x = memcmp (this->ptr, other.ptr, std::min (this->length, other.length));
  if (x < 0)
    {
      return true;
    }
  else if (x > 0)
    {
      return false;
    }
  else
    {
      return this->length < other.length;
    }
}

std::ostream& operator<< (std::ostream& out, const String& s)
{
  return out << std::string (static_cast<const char*> (s.ptr), s.length);
}

std::ostream& operator<< (std::ostream& out, const Slice& s)
{
  out << '{' << s.ptr << ',' << s.length << ',' << s.capacity << '}';
  return out;
}

int FileDescriptor::fd () const
{
  return fd_;
}

FileDescriptor::FileDescriptor (int fd)
  : fd_ (fd)
{ }

};
