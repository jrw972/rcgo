#include "Template.hpp"
#include "SymbolVisitor.hpp"
#include "ast.hpp"
#include <error.h>
#include "runtime.hpp"

void
Template::accept (SymbolVisitor& visitor)
{
  visitor.visit (*this);
}

void
Template::accept (ConstSymbolVisitor& visitor) const
{
  visitor.visit (*this);
}

New::New (ast_t* dn)
  : Template ("new",
              dn,
              new template_type_t ())
{ }

typed_value_t
New::instantiate (TypedValueListType& tvlist)
{
  if (tvlist.size () != 1) {
    error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                   "E36: new expects one argument");
  }

  typed_value_t tv = tvlist[0];
  tvlist.clear ();

  if (tv.kind != typed_value_t::TYPE) {
    error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                   "E67: new expects a type");
  }

  return typed_value_t (new runtime::NewImpl (tv.type, definingNode));
}
