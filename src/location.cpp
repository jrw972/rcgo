#include "location.hpp"

namespace util
{
std::string Location::static_file;

Location::Location (unsigned int a_line)
  : file (static_file)
  , line (a_line)
{ }

}
