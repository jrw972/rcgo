// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/compile.h"

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "src/byte_stream.h"
#include "src/check_types.h"
#include "src/define_symbols.h"
#include "src/get_and_check_package_name.h"
#include "src/get_import_locations.h"
#include "src/parser.h"
#include "src/populate_file_block.h"
#include "src/populate_package_block.h"
#include "src/populate_universe_block.h"
#include "src/type.h"

namespace rcgo {

typedef std::map<Path, Package*> PackageCache;

Package* CollectDependencies(
    const Path& path, Paths* paths, PackageCache* package_cache,
    ErrorList* error_list) {
  PackageCache& pc = *package_cache;

  // Check for recursive dependency.
  if (std::find(paths->begin(), paths->end(), path) !=
      paths->end()) {
    error_list->push_back(RecursiveImport(*paths, path));
    return NULL;
  }

  // Check the cache.
  PackageCache::const_iterator pos = pc.find(path);
  if (pos != pc.end()) {
    return pos->second;
  }

  paths->push_back(path);

  Package* package = NULL;

  try {
    // Read in the config.
    YAML::Node config = YAML::LoadFile(path + "/config.yaml");
    package = new Package(path, config);
    pc[path] = package;

    if (config["imports"]) {
      for (const auto& item : config["imports"]) {
        Package* pkg =
            CollectDependencies(item.second.as<std::string>(), paths,
                                package_cache, error_list);
        if (pkg) {
          package->imports[item.first.as<std::string>()] = pkg;
        }
      }
    }
  } catch(const YAML::ParserException& exception) {
    std::cout << "TODO: ParserException" << std::endl;
  } catch(const YAML::BadFile& exception) {
    std::cout << "TODO: BadFile " << path << std::endl;
  }

  paths->pop_back();
  return package;
}

void FindHighestVersion(Package* package, PackageCache* version_cache) {
  for (const auto& p : package->imports) {
    const std::string& path = p.first;
    Package* pkg = p.second;
    auto x = version_cache->insert(std::make_pair(path, pkg));
    if (pkg->version > x.first->second->version) {
      // TODO(jrw972):  Fix memory leak.
      x.first->second = pkg;
    }
    FindHighestVersion(pkg, version_cache);
  }
}

void ReplaceWithHighestVersion(
    Package* package, const PackageCache& version_cache) {
  for (auto& p : package->imports) {
    const std::string& path = p.first;
    Package* pkg = p.second;
    p.second = version_cache.find(path)->second;
    ReplaceWithHighestVersion(pkg, version_cache);
  }
}

void AnalyzeDependencies(Package* package) {
  // Find the highest version of each package.
  PackageCache version_cache;
  FindHighestVersion(package, &version_cache);
  // Substitute the highest version for each package.
  ReplaceWithHighestVersion(package, version_cache);
}

void CompilePackage(
    Package* package, ErrorList* error_list, std::ostream& out) {
  // Compile dependencies.
  for (auto p : package->imports) {
    CompilePackage(p.second, error_list, out);
  }

  // Find the directory.
  DIR* package_source_directory = opendir(package->path.c_str());
  if (package_source_directory == NULL) {
    error_list->push_back(
        Perror(std::string("Could not open ") + package->path, errno));
    return;
  }

  // Get the files.
  Paths package_source_file_paths =
      GetPackageSourceFilePaths(package->path, package_source_directory,
                                error_list);
  closedir(package_source_directory);

  if (package_source_file_paths.empty()) {
    error_list->push_back(NoFiles(package->path));
    return;
  }

  // Parse the files.
  ast::SourceFiles source_files;
  for (const std::string& path : package_source_file_paths) {
    FileByteStream byte_stream(path.c_str());
    Parser parser(&byte_stream, error_list, out);
    source_files.push_back(parser.SourceFile());
  }

  // Set the package name.
  package->name(GetAndCheckPackageName(source_files, error_list));

  {
    type::Factory type_factory;
    symbol::Table universe_symbol_table(nullptr);
    // Populate the universe block.
    MutableBlock universe_block(&universe_symbol_table, nullptr);
    PopulateUniverseBlock(&type_factory, &universe_block);
    // Populate the package block.
    MutableBlock package_block(package, &universe_block);
    PopulatePackageBlock(source_files, &package_block, error_list);

    for (ast::Node* source_file : source_files) {
      // Populate the file block.
      symbol::Table file_block_st(package_block.package());
      MutableBlock file_block(&file_block_st, &package_block);
      PopulateFileBlock(source_file, &file_block, package, error_list);
      DefineSymbols(source_file, &file_block, &type_factory, error_list);
      CheckTypes(source_file, &file_block, &type_factory, error_list);
      // TODO(jrw972):  control flow analysis and other semantic checks
      // TODO(jrw972):  code generation
    }
  }

  // Clean up.
  for (ast::Node* source_file : source_files) delete source_file;
}

Package* Compile(
    const Path& path, Paths* path_list, ErrorList* error_list,
    std::ostream& out) {

  // TODO(jrw972): Delete the packages in the package cache.
  PackageCache package_cache;
  Package* package =
      CollectDependencies(path, path_list, &package_cache, error_list);
  if (!error_list->empty()) {
    return nullptr;
  }

  AnalyzeDependencies(package);
  if (!error_list->empty()) {
    return nullptr;
  }

  CompilePackage(package, error_list, out);
  if (!error_list->empty()) {
    return NULL;
  }

  return package;
}

Error RecursiveImport(const Paths& path_list, const Path& path) {
  Error error;
  // The last import_path is the problem.
  error.message << "error: recursive import of \""
                << path << "\" detected.  Import chain to follow:"
                << std::endl;

  for (const auto& p : path_list) {
    error.message << p << ((p == path) ? " *" : "") << std::endl;
  }
  error.message << path << " *" << std::endl;
  return error;
}

Error NoFiles(std::string const& package_source_directory_path) {
  Error error;
  error.message << "error: " + package_source_directory_path + " contains no files" << std::endl;
  return error;
}

}  // namespace rcgo
