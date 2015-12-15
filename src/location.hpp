#ifndef rc_src_location_hpp
#define rc_src_location_hpp

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

#endif // rc_src_location_hpp
