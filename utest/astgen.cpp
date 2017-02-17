#include "astgen.hpp"
#include "type.hpp"
#include "identifier.hpp"

using namespace ast;
using namespace source;

const std::string DEFAULT_TYPE_NAME ("T");

static ast::Receiver* gen_receiver ()
{
  return new ast::Receiver (Location::builtin, Identifier ("this", Location::builtin), Mutable, Mutable, false, Identifier (DEFAULT_TYPE_NAME, Location::builtin));
}

ast::ActionDecl* gen_action_decl (const std::string& name, long dimension)
{
  if (dimension == -1)
    {
      return new ActionDecl (Location::builtin, new EmptyExpression (Location::builtin), gen_receiver (), Identifier (name, Location::builtin), NULL, NULL);
    }
  else
    {
      semantic::ExpressionValue v;
      v.kind = semantic::ExpressionValue::Constant;
      v.type = type::UntypedInteger::instance ();
      v.value.untyped_integer_value = dimension;
      return new ActionDecl (Location::builtin, new Literal (Location::builtin, v), gen_receiver (), Identifier (name, Location::builtin), NULL, NULL);
    }
}

ast::ReactionDecl* gen_reaction_decl (const std::string& name, long dimension)
{
  if (dimension == -1)
    {
      return new ReactionDecl (Location::builtin, new EmptyExpression (Location::builtin), gen_receiver (), Identifier (name, Location::builtin), new ParameterList (Location::builtin), NULL);
    }
  else
    {
      semantic::ExpressionValue v;
      v.kind = semantic::ExpressionValue::Constant;
      v.type = type::UntypedInteger::instance ();
      v.value.untyped_integer_value = dimension;
      return new ReactionDecl (Location::builtin, new Literal (Location::builtin, v), gen_receiver (), Identifier (name, Location::builtin), new ParameterList (Location::builtin), NULL);
    }
}

ast::BinderDecl* gen_bind_decl (const std::string& name)
{
  return new BinderDecl (Location::builtin, gen_receiver (), Identifier (name, Location::builtin), NULL);
}

ast::TypeDecl* gen_type_decl (const std::string& name, ast::Node* type)
{
  return new ast::TypeDecl (Location::builtin, Identifier (name, Location::builtin), type);
}

ast::MethodDecl* gen_method_decl (const std::string& name)
{
  return new ast::MethodDecl (Location::builtin, gen_receiver (), Identifier (name, Location::builtin), new ParameterList (Location::builtin), new ParameterList (Location::builtin), NULL);
}

ast::InitializerDecl* gen_init_decl (const std::string& name)
{
  return new ast::InitializerDecl (Location::builtin, gen_receiver (), Identifier (name, Location::builtin), new ParameterList (Location::builtin), new ParameterList (Location::builtin), NULL);
}

ast::GetterDecl* gen_getter_decl (const std::string& name)
{
  return new ast::GetterDecl (Location::builtin, gen_receiver (), Identifier (name, Location::builtin), new ParameterList (Location::builtin), new ParameterList (Location::builtin), NULL);
}

ast::FunctionDecl* gen_function_decl (const std::string& name)
{
  return new ast::FunctionDecl (Location::builtin, Identifier (name, Location::builtin), new ParameterList (Location::builtin), new ParameterList (Location::builtin), NULL);
}

ast::InstanceDecl* gen_instance_decl (const std::string& name, const std::string& init)
{
  return new ast::InstanceDecl (Location::builtin, Identifier (name, Location::builtin), new IdentifierType (Location::builtin, Identifier (DEFAULT_TYPE_NAME, Location::builtin)), Identifier (init, Location::builtin), new ExpressionList (Location::builtin));
}

ast::ConstDecl* gen_const_decl (const std::string& name)
{
  IdentifierList* ids = new IdentifierList (Location::builtin);
  ids->append (Identifier (name, Location::builtin));
  ExpressionList* exprs = new ExpressionList (Location::builtin);
  semantic::ExpressionValue v;
  v.kind = semantic::ExpressionValue::Constant;
  v.type = type::UntypedInteger::instance ();
  v.value.untyped_integer_value = 1;
  exprs->append (new Literal (Location::builtin, v));
  return new ast::ConstDecl (Location::builtin, ids, new EmptyType (Location::builtin), exprs);
}

ast::FieldList* gen_field_list (const std::string& field_name,
                                const std::string& field_type)
{
  IdentifierList* idlist = new IdentifierList (Location::builtin);
  idlist->append (Identifier (field_name, Location::builtin));
  IdentifierType* type = new IdentifierType (Location::builtin, Identifier (field_type, Location::builtin));

  VariableList* varlist = new VariableList (Location::builtin, idlist, Mutable, Mutable, type);
  FieldList* fl = new ast::FieldList (Location::builtin);
  fl->append (varlist);
  return fl;
}
