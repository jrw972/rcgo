#ifndef RC_SRC_PARAMETER_LIST_HPP
#define RC_SRC_PARAMETER_LIST_HPP

#include "types.hpp"
#include "location.hpp"

namespace decl
{

struct ParameterList
{
  typedef std::vector<Parameter*> ParametersType;
  typedef ParametersType::const_iterator const_iterator;
  typedef ParametersType::const_reverse_iterator const_reverse_iterator;

  ParameterList (const source::Location& loc);
  size_t size () const;
  bool empty () const;
  Parameter* at (size_t idx) const;
  const_iterator begin () const;
  const_iterator end () const;
  const_reverse_iterator rbegin () const;
  const_reverse_iterator rend () const;
  Parameter* find (const std::string& name) const;
  ParameterList* append (Parameter* p);
  ParameterList* set_variadic (bool v);
  bool is_variadic () const;

  void print (std::ostream& out = std::cout) const;
  std::string to_string () const;

  source::Location const location;

private:
  ParametersType parameters_;
  bool variadic_;
};

std::ostream& operator<< (std::ostream& out,
                          const ParameterList& list);

}

#endif // RC_SRC_PARAMETER_LIST_HPP
