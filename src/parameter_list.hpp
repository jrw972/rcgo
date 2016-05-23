#ifndef RC_SRC_PARAMETER_LIST_HPP
#define RC_SRC_PARAMETER_LIST_HPP

#include "types.hpp"

namespace decl
{

struct ParameterList
{
  ParameterList () : size_ (0), variadic_ (false) { }
  typedef std::vector<decl::ParameterSymbol*> ParametersType;
  typedef ParametersType::const_iterator const_iterator;
  typedef ParametersType::const_reverse_iterator const_reverse_iterator;
  std::string to_string () const;
  size_t allocation_size () const
  {
    return size_;
  }
  size_t size () const
  {
    return parameters_.size ();
  }
  decl::ParameterSymbol* at (size_t idx) const
  {
    return parameters_.at (idx);
  }
  const_iterator begin () const
  {
    return parameters_.begin ();
  }
  const_iterator end () const
  {
    return parameters_.end ();
  }
  const_reverse_iterator rbegin () const
  {
    return parameters_.rbegin ();
  }
  const_reverse_iterator rend () const
  {
    return parameters_.rend ();
  }
  decl::ParameterSymbol* find (const std::string& name) const;
  ParameterList* append (decl::ParameterSymbol* p);
  ParameterList* append (const util::Location& loc,
                         const std::string& name,
                         const type::Type* type,
                         Mutability intrinsic_mutability,
                         Mutability dereference_mutability);
  ParameterList* append (const util::Location& loc,
                         const std::string& name,
                         const type::Type* type,
                         Mutability dereference_mutability);
  ParameterList* set_variadic (bool v)
  {
    variadic_ = v;
    return this;
  }
  bool is_variadic () const
  {
    return variadic_;
  }

  void check_foreign_safe () const;
private:
  ParametersType parameters_;
  size_t size_;
  bool variadic_;
};

inline std::ostream& operator<< (std::ostream& out, const ParameterList& s)
{
  return out << s.to_string ();
}

}

#endif // RC_SRC_PARAMETER_LIST_HPP
