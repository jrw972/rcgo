#ifndef RC_SRC_PACKAGE_HPP
#define RC_SRC_PACKAGE_HPP

#include "types.hpp"

namespace source
{
struct Package
{
  typedef std::vector<SourceFile*> SourceFilesType;
  void add_source_file (SourceFile* source_file);
  typedef SourceFilesType::const_iterator const_source_files_iterator;
  const_source_files_iterator source_files_begin () const;
  const_source_files_iterator source_files_end () const;

  typedef std::set<Package*> DependenciesType;
  void add_dependency (Package* package);

  // Topological sort interface.
  typedef DependenciesType::const_iterator const_adjacency_iterator;
  const_adjacency_iterator adjacency_begin () const;
  const_adjacency_iterator adjacency_end () const;

  void determine_package_name (util::ErrorReporter& er);
  void process_symbols (util::ErrorReporter& er,
                        decl::SymbolTable& symbol_table);

  const std::string& package_name () const;

  typedef std::vector<decl::Symbol*> SymbolsType;
  typedef SymbolsType::const_iterator const_symbols_iterator;
  const_symbols_iterator symbols_begin () const;
  const_symbols_iterator symbols_end () const;

private:
  SourceFilesType source_files_;
  DependenciesType dependencies_;
  std::string package_name_;
  SymbolsType symbols_;
};
}

#endif // RC_SRC_PACKAGE_HPP
