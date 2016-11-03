#include <cmath>

#include "type.hpp"

namespace semantic
{

using namespace type;

UntypedComplex UntypedComplex::make (double r, double i)
{
  UntypedComplex retval;
  retval.real = r;
  retval.imag = i;
  return retval;
}

bool UntypedComplex::operator== (const UntypedComplex& other) const
{
  return this->real == other.real && this->imag == other.imag;
}

UntypedComplex& UntypedComplex::operator= (const double& x)
{
  this->real = x;
  this->imag = 0;
  return *this;
}

UntypedComplex& UntypedComplex::operator= (const runtime::Complex64& x)
{
  this->real = x.real;
  this->imag = x.imag;
  return *this;
}

UntypedComplex& UntypedComplex::operator= (const runtime::Complex128& x)
{
  this->real = x.real;
  this->imag = x.imag;
  return *this;
}

UntypedComplex::operator double() const
{
  return this->real;
}

UntypedComplex operator* (const UntypedComplex& x, const UntypedComplex& y)
{
  UntypedComplex uc;
  uc.real = x.real * y.real - x.imag * y.imag;
  uc.imag = x.real * y.imag + x.imag * y.real;
  return uc;
}
UntypedComplex operator/ (const UntypedComplex& x, const UntypedComplex& y)
{
  const double xmag = sqrt (x.real * x.real + x.imag * x.imag);
  const double xtheta = atan (x.imag / x.real);
  const double ymag = sqrt (y.real * y.real + y.imag * y.imag);
  const double ytheta = atan (y.imag / y.real);
  const double mag = xmag / ymag;
  const double theta = xtheta - ytheta;
  UntypedComplex uc;
  uc.real = mag * cos (theta);
  uc.imag = mag * sin (theta);
  return uc;
}
UntypedComplex operator+ (const UntypedComplex& x, const UntypedComplex& y)
{
  UntypedComplex uc;
  uc.real = x.real + y.real;
  uc.imag = x.imag + y.imag;
  return uc;
}
UntypedComplex operator- (const UntypedComplex& x, const UntypedComplex& y)
{
  UntypedComplex uc;
  uc.real = x.real - y.real;
  uc.imag = x.imag - y.imag;
  return uc;
}
UntypedComplex operator- (const UntypedComplex& x)
{
  UntypedComplex uc;
  uc.real = -x.real;
  uc.imag = -x.imag;
  return uc;
}

}
