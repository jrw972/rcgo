// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/get_import_locations.h"

namespace rcgo {

ImportLocations GetImportLocations(const ast::SourceFiles& source_files) {
  struct ImportPathVisitor : public ast::DefaultNodeVisitor {
    ImportLocations import_locations;

    void Visit(ast::SourceFile* ast) override { VisitAll(ast->imports); }

    void Visit(ast::ImportDecl* ast) override { VisitAll(ast->import_specs); }

    void Visit(ast::ImportSpec* ast) override {
      ImportLocation il(ast->import_path, ast->location);
      import_locations.push_back(il);
    }
  };

  ImportPathVisitor visitor;
  for (ast::SourceFiles::const_iterator pos = source_files.begin (),
           limit = source_files.end(); pos != limit; ++pos) {
    ast::Node* source_file = *pos;
    source_file->Accept(&visitor);
  }

  return visitor.import_locations;
}

}  // namespace rcgo
