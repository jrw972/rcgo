#ifndef RC_SRC_RUNTIME_TYPES_HPP
#define RC_SRC_RUNTIME_TYPES_HPP

#include "types.hpp"

namespace runtime
{

struct Complex64
{
  float real;
  float imag;

  static Complex64 make (float r, float i);
  bool operator== (const Complex64& other) const;
  // TODO:  Remove this method.  Requires refactoring operation.
  operator double() const;
  Complex64& operator= (double x);
  Complex64& operator++ ();
  Complex64& operator-- ();
  Complex64& operator+= (const Complex64& x);
};

Complex64 operator* (const Complex64&, const Complex64&);
Complex64 operator/ (const Complex64&, const Complex64&);
Complex64 operator+ (const Complex64&, const Complex64&);
Complex64 operator- (const Complex64&, const Complex64&);
Complex64 operator- (const Complex64&);

struct Complex128
{
  double real;
  double imag;

  static Complex128 make (double r, double i);
  bool operator== (const Complex128& other) const;
  // TODO:  Remove this method.  Requires refactoring operation.
  operator double() const;
  Complex128& operator= (double x);
  Complex128& operator++ ();
  Complex128& operator-- ();
  Complex128& operator+= (const Complex128& x);
};

Complex128 operator* (const Complex128&, const Complex128&);
Complex128 operator/ (const Complex128&, const Complex128&);
Complex128 operator+ (const Complex128&, const Complex128&);
Complex128 operator- (const Complex128&, const Complex128&);
Complex128 operator- (const Complex128&);

struct String
{
  const void* ptr;
  size_t length;

  static String make (const void* ptr, size_t length);
  bool operator== (const String& other) const;
  bool operator< (const String& other) const;
};

  std::ostream& operator<< (std::ostream& out, const String& s);

struct Slice
{
  void* ptr;
  size_t length;
  size_t capacity;
};

std::ostream& operator<< (std::ostream& out, const Slice& s);

struct FileDescriptor
{
  int fd () const;

private:
  FileDescriptor (int fd);
  int const fd_;
  friend class ExecutorBase;
};

// A cmponent_t* contains the address of a component instance.
struct component_t;

struct PushPort
{
  component_t* instance;
  const decl::Reaction* reaction;
  long parameter;
  PushPort* next;
};

struct PullPort
{
  component_t* instance;
  decl::Getter* getter;
};

}

#endif //RC_SRC_RUNTIME_TYPES_HPP
