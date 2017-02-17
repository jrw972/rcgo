#ifndef RC_SRC_PACKAGE_SET_HPP
#define RC_SRC_PACKAGE_SET_HPP

#include "types.hpp"
#include <map>

namespace source
{

struct PackageSet
{
  Package* load (const std::string& rcgo_root, const std::string& import_path);
  bool empty () const;

  typedef std::vector<Package*> ListType;
  typedef ListType::iterator packages_iterator;
  packages_iterator packages_begin ();
  packages_iterator packages_end ();
  typedef ListType::const_iterator const_packages_iterator;
  const_packages_iterator packages_begin () const;
  const_packages_iterator packages_end () const;

  void determine_package_names (util::ErrorReporter& er);
  void process_symbols (util::ErrorReporter& er) const;

private:
  typedef std::map<std::string, Package*> MapType;
  MapType map_;
  ListType list_;
};

}

#endif // RC_SRC_PACKAGE_SET_HPP
