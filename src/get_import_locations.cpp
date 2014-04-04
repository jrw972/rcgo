/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "AstVisitor.hpp"
#include "functions.hpp"

namespace rcgo {

  ImportLocations
  get_import_locations (const SourceFiles& source_files)
  {
    struct ImportPathVisitor : public DefaultAstVisitor {
      ImportLocations import_locations;

      void visit (SourceFileAst& ast)
      {
        ast.imports->accept (*this);
      }

      void visit (ImportDeclListAst& ast)
      {
        ast.visit_children (*this);
      }

      void visit (ImportDeclAst& ast)
      {
        ast.visit_children (*this);
      }

      void visit (ImportSpecAst& ast)
      {
        ImportLocation il (ast.import_path, ast.location);
        import_locations.push_back (il);
      }
    };

    ImportPathVisitor visitor;
    for (SourceFiles::const_iterator pos = source_files.begin (),
           limit = source_files.end ();
         pos != limit;
         ++pos)
      {
        Ast* source_file = *pos;
        source_file->accept (visitor);
      }

    return visitor.import_locations;
  }

}
