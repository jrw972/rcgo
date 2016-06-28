#include "runtime_types.hpp"

#include <cstring>

namespace runtime
{

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

Complex64 operator* (const Complex64&, const Complex64&)
{
  UNIMPLEMENTED;
}
Complex64 operator/ (const Complex64&, const Complex64&)
{
  UNIMPLEMENTED;
}

Complex64 operator+ (const Complex64&, const Complex64&)
{
  UNIMPLEMENTED;
}
Complex64 operator- (const Complex64&, const Complex64&)
{
  UNIMPLEMENTED;
}
Complex64 operator- (const Complex64&)
{
  UNIMPLEMENTED;
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

Complex128 operator* (const Complex128&, const Complex128&)
{
  UNIMPLEMENTED;
}
Complex128 operator/ (const Complex128&, const Complex128&)
{
  UNIMPLEMENTED;
}
Complex128 operator+ (const Complex128&, const Complex128&)
{
  UNIMPLEMENTED;
}
Complex128 operator- (const Complex128&, const Complex128&)
{
  UNIMPLEMENTED;
}
Complex128 operator- (const Complex128&)
{
  UNIMPLEMENTED;
}

std::ostream& operator<< (std::ostream& out, const String& s)
{
  return out << std::string (static_cast<const char*> (s.ptr), s.length);
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
