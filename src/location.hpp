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
  static std::string static_file;
  std::string const file;
  unsigned int const line;

  Location (unsigned int a_line = 0);
  Location (const std::string& file);
  bool operator== (const Location& other) const;
};

}

#endif // RC_SRC_LOCATION_HPP
