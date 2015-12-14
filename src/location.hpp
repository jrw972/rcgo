#ifndef Location_hpp
#define Location_hpp

#include <string>

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

#endif /* Location_hpp */
