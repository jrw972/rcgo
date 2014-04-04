/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "AstVisitor.hpp"
#include "Error.hpp"
#include "functions.hpp"

namespace rcgo {

  std::string
  get_and_check_package_name (const SourceFiles& source_files,
                              ErrorReporter& error_reporter)
  {
    struct PackageNameVisitor : public DefaultAstVisitor {
      ErrorReporter& error_reporter;

      PackageNameVisitor (ErrorReporter& a_error_reporter)
        : error_reporter (a_error_reporter)
      { }

      // TODO:  Cannot be the blank identifier.
      std::string package_name;

      void visit (SourceFileAst& ast)
      {
        IdentifierAst* package = to_identifier (ast.package);
        if (package != NULL)
          {
            if (package_name.empty ())
              {
                package_name = package->identifier;
              }
            else if (package_name != package->identifier)
              {
                error_reporter << PackageMismatch (package->location, package_name, package->identifier);
              }
          }
      }
    };

    PackageNameVisitor visitor (error_reporter);
    for (SourceFiles::const_iterator pos = source_files.begin (),
           limit = source_files.end ();
         pos != limit;
         ++pos)
      {
        Ast* source_file = *pos;
        source_file->accept (visitor);
      }

    return visitor.package_name;
  }

}
