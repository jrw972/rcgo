#ifndef RC_SRC_PARAMETER_LIST_HPP
#define RC_SRC_PARAMETER_LIST_HPP

#include "types.hpp"
#include "location.hpp"

namespace decl
{

struct ParameterList
{
  typedef std::vector<ParameterSymbol*> ParametersType;
  typedef ParametersType::const_iterator const_iterator;
  typedef ParametersType::const_reverse_iterator const_reverse_iterator;

  ParameterList (const util::Location& loc);
  size_t size_on_stack () const;
  size_t size () const;
  bool empty () const;
  ParameterSymbol* at (size_t idx) const;
  const_iterator begin () const;
  const_iterator end () const;
  const_reverse_iterator rbegin () const;
  const_reverse_iterator rend () const;
  ParameterSymbol* find (const std::string& name) const;
  ParameterList* append (ParameterSymbol* p);
  ParameterList* set_variadic (bool v);
  bool is_variadic () const;
  bool is_foreign_safe () const;

  void print (std::ostream& out = std::cout) const;
  std::string to_string () const;

  util::Location const location;

private:
  ParametersType parameters_;
  size_t size_on_stack_;
  bool variadic_;
};

std::ostream& operator<< (std::ostream& out,
                          const ParameterList& list);

}

#endif // RC_SRC_PARAMETER_LIST_HPP
