#ifndef RC_SRC_LOCATION_HPP
#define RC_SRC_LOCATION_HPP

#include <string>

namespace util
{

/*
 * Describes a location in a file.
 *
 * The StaticFile member serves as a default arguement.
 */

struct Location
{
  static std::string StaticFile;
  std::string const File;
  unsigned int const Line;

  Location ()
    : File (StaticFile)
    , Line (0)
  { }

  Location (unsigned int aLine)
    : File (StaticFile)
    , Line (aLine)
  { }
};

}

#endif // RC_SRC_LOCATION_HPP
