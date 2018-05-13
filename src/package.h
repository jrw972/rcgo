#ifndef SRC_PACKAGE_H_
#define SRC_PACKAGE_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cassert>
#include <map>
#include <string>
#include <utility>

#include "src/symbol.h"
#include "src/symbol_table.h"

namespace rcgo {

struct Package : public SymbolTable {  // NOT_COVERED
  explicit Package(const std::string& a_import_path)
      : import_path(a_import_path), m_state(kNew) {}

  bool IsNew() const { return m_state == kNew; }
  void Changed() { assert(IsNew()); m_state = kChanged; }
  bool IsChanged() const { return m_state == kChanged; }
  void NotChanged() { assert(IsNew()); m_state = kNotChanged; }
  bool IsNotChanged() const { return m_state == kNotChanged; }
  void Error() { assert(IsNew() || IsError()); m_state = kError; }
  bool IsError() const { return m_state == kError; }

  std::string const import_path;

  void name(const std::string& name) { m_name = name; }
  const std::string& name() const { return m_name; }

 private:
  enum State {
    kNew,
    kChanged,
    kNotChanged,
    kError
  };
  State m_state;
  std::string m_name;
};

struct PackageCache {
  ~PackageCache() {
    for (PackageMapType::value_type p : m_package_map) delete p.second;
  }
  void Insert(Package* package_archive) {
    m_package_map.insert(
        std::make_pair(package_archive->import_path, package_archive));
  }
  Package* Find(const std::string& a_path) {
    PackageMapType::const_iterator pos = m_package_map.find(a_path);
    if (pos != m_package_map.end()) return pos->second;
    return NULL;
  }

 private:
  typedef std::map<std::string, Package*> PackageMapType;
  PackageMapType m_package_map;
};

}  // namespace rcgo

#endif  // SRC_PACKAGE_H_
