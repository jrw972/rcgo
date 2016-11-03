#include "location.hpp"

namespace util
{
std::string Location::static_file;

Location::Location (unsigned int a_line)
  : file_ (static_file)
  , line_ (a_line)
{ }

Location::Location (const std::string& a_file)
  : file_ (a_file)
  , line_ (0)
{ }

bool Location::operator== (const Location& other) const
{
  return this->file_ == other.file_ && this->line_ == other.line_;
}

const Location builtin ("<builtin>");
}
