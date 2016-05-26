#ifndef RC_UTEST_SYMBOL_HELPERS_HPP
#define RC_UTEST_SYMBOL_HELPERS_HPP

#include "symbol_visitor.hpp"

template <typename T>
struct TestSymbolVisitor : public decl::SymbolVisitor
{
  T* sym;

  TestSymbolVisitor () : sym (NULL) { }

  void visit (T& sym)
  {
    this->sym = &sym;
  }

};

template <typename T>
struct TestConstSymbolVisitor : public decl::ConstSymbolVisitor
{
  const T* sym;

  TestConstSymbolVisitor () : sym (NULL) { }

  void visit (const T& sym)
  {
    this->sym = &sym;
  }

};

#endif // RC_UTEST_SYMBOL_HELPERS_HPP
