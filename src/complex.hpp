#ifndef RC_SRC_COMPLEX_HPP
#define RC_SRC_COMPLEX_HPP

#include <cstring>

namespace type
{

struct C64
{
  float real;
  float imag;

  bool operator== (const C64& other) const
  {
    return this->real == other.real && this->imag == other.imag;
  }

  operator double() const
  {
    return real;
  }

  C64& operator= (double x)
  {
    this->real = x;
    this->imag = 0;
    return *this;
  }

  C64& operator++ ()
  {
    this->real += 1;
    return *this;
  }

  C64& operator-- ()
  {
    this->real -= 1;
    return *this;
  }

  C64& operator+= (const C64& x)
  {
    this->real += x.real;
    this->imag += x.imag;
    return *this;
  }
};

struct C128
{
  double real;
  double imag;

  bool operator== (const C128& other) const
  {
    return this->real == other.real && this->imag == other.imag;
  }

  operator double() const
  {
    return real;
  }

  C128& operator= (double x)
  {
    this->real = x;
    this->imag = 0;
    return *this;
  }

  C128& operator++ ()
  {
    this->real += 1;
    return *this;
  }

  C128& operator-- ()
  {
    this->real -= 1;
    return *this;
  }

  C128& operator+= (const C128& x)
  {
    this->real += x.real;
    this->imag += x.imag;
    return *this;
  }
};

struct CU
{
  double real;
  double imag;
  static CU make (double r, double i)
  {
    CU retval;
    retval.real = r;
    retval.imag = i;
    return retval;
  }

  bool operator== (const CU& other) const
  {
    return this->real == other.real && this->imag == other.imag;
  }

  CU& operator= (const double& x)
  {
    this->real = x;
    this->imag = 0;
    return *this;
  }

  CU& operator= (const C64& x)
  {
    this->real = x.real;
    this->imag = x.imag;
    return *this;
  }

  CU& operator= (const C128& x)
  {
    this->real = x.real;
    this->imag = x.imag;
    return *this;
  }

  operator double() const
  {
    return this->real;
  }
};

struct StringRep
{
  void* ptr;
  size_t length;

  bool operator== (const StringRep& other) const
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

  bool operator< (const StringRep& other) const
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
};

inline C64 operator* (const C64&, const C64&)
{
  UNIMPLEMENTED;
}
inline C64 operator/ (const C64&, const C64&)
{
  UNIMPLEMENTED;
}


inline C128 operator* (const C128&, const C128&)
{
  UNIMPLEMENTED;
}
inline C128 operator/ (const C128&, const C128&)
{
  UNIMPLEMENTED;
}

inline CU operator* (const CU&, const CU&)
{
  UNIMPLEMENTED;
}
inline CU operator/ (const CU&, const CU&)
{
  UNIMPLEMENTED;
}

inline C64 operator+ (const C64&, const C64&)
{
  UNIMPLEMENTED;
}
inline C64 operator- (const C64&, const C64&)
{
  UNIMPLEMENTED;
}
inline C64 operator- (const C64&)
{
  UNIMPLEMENTED;
}

inline C128 operator+ (const C128&, const C128&)
{
  UNIMPLEMENTED;
}
inline C128 operator- (const C128&, const C128&)
{
  UNIMPLEMENTED;
}
inline C128 operator- (const C128&)
{
  UNIMPLEMENTED;
}

inline CU operator+ (const CU&, const CU&)
{
  UNIMPLEMENTED;
}
inline CU operator- (const CU&, const CU&)
{
  UNIMPLEMENTED;
}
inline CU operator- (const CU&)
{
  UNIMPLEMENTED;
}

inline std::ostream& operator<< (std::ostream& out, const StringRep& s)
{
  return out << std::string (static_cast<const char*> (s.ptr), s.length);
}

}

#endif // RC_SRC_COMPLEX_HPP
