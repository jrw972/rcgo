#ifndef RC_SRC_SOURCE_FILE_HPP
#define RC_SRC_SOURCE_FILE_HPP

#include "types.hpp"
#include "identifier.hpp"

namespace source
{
struct Import
{
  Import (const Location& location, const Identifier& package_name, const std::string& import_path);

  const Location& location () const { return location_; }
  const Identifier& package_name () const { return package_name_; }
  const std::string& import_path () const { return import_path_; }
  const Package* package () const { return package_; }

  void set_package (const Package* package)
  {
    assert (package_ == NULL);
    assert (package != NULL);
    package_ = package;
  }

private:
  Location location_;
  Identifier package_name_;
  std::string import_path_;
  const Package* package_;
};

struct SourceFile
{
  typedef std::vector<Import> ImportsType;

  SourceFile (const std::string& a_path);
  const std::string& path () const;
  Location location (unsigned int line) const;

  Identifier* package_name;
  ast::Node* top_level_decl_list;

  void add_import (const Location& location,
                   const Identifier& package_name,
                   const std::string& import_path);

  typedef ImportsType::const_iterator const_imports_iterator;
  const_imports_iterator imports_begin () const;
  const_imports_iterator imports_end () const;
  typedef ImportsType::iterator imports_iterator;
  imports_iterator imports_begin ();
  imports_iterator imports_end ();

  void generate_package_symbols (util::ErrorReporter& er,
                                 decl::SymbolTable& symbol_table) const;

  void process_symbols (util::ErrorReporter& er,
                        decl::SymbolTable& symbol_table) const;

  static const SourceFile builtin;
private:
  std::string const path_;
  ImportsType imports_;
};
}

#endif // RC_SRC_SOURCE_FILE_HPP
