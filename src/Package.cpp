/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Package.hpp"

#include "Ast.hpp"

namespace rcgo {

  Package::Package (const std::string& a_import_path)
    : import_path (a_import_path)
    , m_state (New)
  { }

  bool Package::is_new () const
  {
    return m_state == New;
  }

  void Package::changed ()
  {
    assert (is_new ());
    m_state = Changed;
  }

  bool Package::is_changed () const
  {
    return m_state == Changed;
  }

  void Package::not_changed ()
  {
    assert (is_new ());
    m_state = NotChanged;
  }

  bool Package::is_not_changed () const
  {
    return m_state == NotChanged;
  }

  void Package::error ()
  {
    assert (is_new () || is_error ());
    m_state = Error;
  }

  bool Package::is_error () const
  {
    return m_state == Error;
  }

  void Package::name (const std::string& name)
  {
    m_name = name;
  }

  const std::string& Package::name () const
  {
    return m_name;
  }

  PackageCache::~PackageCache ()
  {
    for (PackageMapType::const_iterator pos = m_package_map.begin (),
           limit = m_package_map.end ();
         pos != limit;
         ++pos)
      {
        delete pos->second;
      }
  }

  void PackageCache::insert (Package* package_archive)
  {
    // Always insert into the map.
    m_package_map.insert (std::make_pair (package_archive->import_path, package_archive));
  }

  Package* PackageCache::find (const std::string& a_path)
  {
    PackageMapType::const_iterator pos = m_package_map.find (a_path);
    if (pos != m_package_map.end ())
      {
        return pos->second;
      }
    return NULL;
  }

}
