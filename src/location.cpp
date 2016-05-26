#include "location.hpp"

namespace util
{
std::string Location::static_file;

Location::Location (unsigned int a_line)
  : file (static_file)
  , line (a_line)
{ }

bool Location::operator== (const Location& other) const
{
  return this->file == other.file && this->line == other.line;
}
}
