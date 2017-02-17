#ifndef RC_SRC_LOCATION_HPP
#define RC_SRC_LOCATION_HPP

#include "types.hpp"

namespace source
{

/*
 * Describes a location in a file.
 *
 * The StaticFile member serves as a default arguement.
 */

struct Location
{
  Location (const SourceFile* a_file, unsigned int a_line);
  bool operator== (const Location& other) const;
  const SourceFile* file () const
  {
    return file_;
  }
  unsigned int line () const
  {
    return line_;
  }

  static const Location builtin;

private:
  const SourceFile* file_;
  unsigned int line_;
};

typedef std::vector<Location> LocationList;
}

#endif // RC_SRC_LOCATION_HPP
