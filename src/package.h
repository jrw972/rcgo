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
#include "src/block.h"
#include "yaml-cpp/yaml.h"

namespace rcgo {

struct Version {
  enum Kind {
    kSemantic,
    kUnversioned
  };

  explicit Version(Kind a_kind)
      : kind(a_kind) {}

  Kind const kind;

  bool operator> (const Version& other) const {
    return this->kind > other.kind;
  }
};

struct Package : public symbol::Table {
  // TODO(jrw972): Extract the version from the path.
  Package(const std::string& a_path, const YAML::Node& a_config)
      : symbol::Table(this), path(a_path), config(a_config),
      version(Version::kUnversioned) {}

  std::string const path;
  YAML::Node const config;
  Version const version;
  typedef std::map<std::string, Package*> ImportsType;
  ImportsType imports;

  void name(const std::string& name) { m_name = name; }
  const std::string& name() const { return m_name; }

 private:
  std::string m_name;
};

}  // namespace rcgo

#endif  // SRC_PACKAGE_H_
