#ifndef RC_SRC_LOCATION_HPP
#define RC_SRC_LOCATION_HPP

#include <string>
#include <vector>

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

  Location (unsigned int a_line = 0);
  Location (const std::string& file);
  bool operator== (const Location& other) const;
  const std::string& file () const
  {
    return file_;
  }
  unsigned int line () const
  {
    return line_;
  }

private:
  std::string file_;
  unsigned int line_;

};

typedef std::vector<Location> LocationList;
extern const Location builtin;

}

#endif // RC_SRC_LOCATION_HPP
