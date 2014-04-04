#ifndef RCGO_SRC_PACKAGE_HPP
#define RCGO_SRC_PACKAGE_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.hpp"

#include "SymbolTable.hpp"
#include "Symbol.hpp"

namespace rcgo {

  struct Package : public SymbolTable { // NOT_COVERED
    Package (const std::string& a_import_path);

    bool is_new () const;
    void changed ();
    bool is_changed () const;
    void not_changed ();
    bool is_not_changed () const;
    void error ();
    bool is_error () const;

    std::string const import_path;

    void name (const std::string& a_name);
    const std::string& name () const;

  private:
    enum State {
      New,
      Changed,
      NotChanged,
      Error
    };
    State m_state;
    std::string m_name;
  };

  struct PackageCache {
    ~PackageCache ();
    void insert (Package* package);
    Package* find (const std::string& a_path);

  private:
    typedef std::map<std::string, Package*> PackageMapType;
    PackageMapType m_package_map;
  };
}

#endif // RCGO_SRC_PACKAGE_HPP
