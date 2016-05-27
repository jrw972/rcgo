#include "parameter_list.hpp"

#include "symbol.hpp"
#include "arch.hpp"

namespace decl
{

ParameterList::ParameterList (const util::Location& a_location)
  : location (a_location)
  , size_on_stack_ (0)
  , variadic_ (false)
{ }

size_t ParameterList::size_on_stack () const
{
  return size_on_stack_;
}
size_t ParameterList::size () const
{
  return parameters_.size ();
}
ParameterSymbol* ParameterList::at (size_t idx) const
{
  return parameters_.at (idx);
}
ParameterList::const_iterator ParameterList::begin () const
{
  return parameters_.begin ();
}
ParameterList::const_iterator ParameterList::end () const
{
  return parameters_.end ();
}
ParameterList::const_reverse_iterator ParameterList::rbegin () const
{
  return parameters_.rbegin ();
}
ParameterList::const_reverse_iterator ParameterList::rend () const
{
  return parameters_.rend ();
}

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

void
ParameterList::print (std::ostream& out) const
{
  out << '(';
  bool flag = false;
  for (ParametersType::const_iterator ptr = parameters_.begin (),
       limit = parameters_.end ();
       ptr != limit;
       ++ptr)
    {
      if (flag)
        {
          out << ", ";
        }

      out << (*ptr)->type->to_string ();
      flag = true;
    }
  out << ')';
}

std::string
ParameterList::to_string () const
{
  std::stringstream str;
  print (str);
  return str.str ();
}

ParameterList*
ParameterList::append (ParameterSymbol* p)
{
  parameters_.push_back (p);
  size_on_stack_ += util::align_up (p->type->size (), arch::stack_alignment ());
  return this;
}

ParameterList* ParameterList::set_variadic (bool v)
{
  variadic_ = v;
  return this;
}
bool ParameterList::is_variadic () const
{
  return variadic_;
}

bool
ParameterList::is_foreign_safe () const
{
  for (const_iterator pos = begin (), limit = end ();
       pos != limit;
       ++pos)
    {
      if (!(*pos)->is_foreign_safe ())
        {
          return false;
        }
    }
  return true;
}

std::ostream& operator<< (std::ostream& out,
                          const ParameterList& list)
{
  list.print (out);
  return out;
}

}
