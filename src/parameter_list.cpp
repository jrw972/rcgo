#include "parameter_list.hpp"

#include "symbol.hpp"
#include "arch.hpp"

namespace decl
{

ParameterSymbol*
ParameterList::find (const std::string& name) const
{
  for (ParametersType::const_iterator ptr = parameters_.begin (),
       limit = parameters_.end ();
       ptr != limit;
       ++ptr)
    {
      if ((*ptr)->identifier == name)
        {
          return *ptr;
        }
    }
  return NULL;
}

std::string
ParameterList::to_string () const
{
  std::stringstream str;
  str << '(';
  bool flag = false;
  for (ParametersType::const_iterator ptr = parameters_.begin (),
       limit = parameters_.end ();
       ptr != limit;
       ++ptr)
    {
      if (flag)
        {
          str << ", ";
        }

      str << (*ptr)->type->to_string ();
      flag = true;
    }
  str << ')';
  return str.str ();
}

ParameterList*
ParameterList::append (ParameterSymbol* p)
{
  parameters_.push_back (p);
  size_ += util::align_up (p->type->Size (), arch::stack_alignment ());
  return this;
}

ParameterList* ParameterList::append (const util::Location& loc,
                                      const std::string& name,
                                      const type::Type* type,
                                      Mutability intrinsic_mutability,
                                      Mutability dereference_mutability)
{
  return append (ParameterSymbol::make (loc, name, type, intrinsic_mutability, dereference_mutability));
}

ParameterList* ParameterList::append (const util::Location& loc,
                                      const std::string& name,
                                      const type::Type* type,
                                      Mutability dereference_mutability)
{
  return append (ParameterSymbol::makeReturn (loc, name, type, dereference_mutability));
}

void
ParameterList::check_foreign_safe () const
{
  for (const_iterator pos = begin (), limit = end ();
       pos != limit;
       ++pos)
    {
      (*pos)->check_foreign_safe ();
    }
}

}
