#include "astgen.hpp"
#include "type.hpp"

using namespace ast;

static ast::Receiver* gen_receiver ()
{
  Identifier* this_id = new ast::Identifier (1, "this");
  Identifier* type_id = new ast::Identifier (1, "T");
  return new ast::Receiver (1, this_id, Mutable, Mutable, false, type_id);
}

ast::ActionDecl* gen_action_decl (const std::string& name, long dimension)
{
  if (dimension == -1)
    {
      return new ActionDecl (1, new EmptyExpression (1), gen_receiver (), new Identifier (1, name), NULL, NULL);
    }
  else
    {
      semantic::Value v;
      v.present = true;
      v.untyped_integer_value = dimension;
      return new ActionDecl (1, new Literal (1, type::UntypedInteger::instance (), v), gen_receiver (), new Identifier (1, name), NULL, NULL);
    }
}

ast::ReactionDecl* gen_reaction_decl (const std::string& name, long dimension)
{
  if (dimension == -1)
    {
      return new ReactionDecl (1, new EmptyExpression (1), gen_receiver (), new Identifier (1, name), new ParameterList (1), NULL);
    }
  else
    {
      semantic::Value v;
      v.present = true;
      v.untyped_integer_value = dimension;
      return new ReactionDecl (1, new Literal (1, type::UntypedInteger::instance (), v), gen_receiver (), new Identifier (1, name), new ParameterList (1), NULL);
    }
}

ast::BindDecl* gen_bind_decl (const std::string& name)
{
  return new BindDecl (1, gen_receiver (), new Identifier (1, name), NULL);
}

ast::TypeDecl* gen_type_decl (const std::string& name, ast::Node* type)
{
  return new ast::TypeDecl (1, new Identifier (1, name), type);
}

ast::MethodDecl* gen_method_decl (const std::string& name)
{
  return new ast::MethodDecl (1, gen_receiver (), new Identifier (1, name), new ParameterList (1), new ParameterList (1), NULL);
}

ast::InitDecl* gen_init_decl (const std::string& name)
{
  return new ast::InitDecl (1, gen_receiver (), new Identifier (1, name), new ParameterList (1), new ParameterList (1), NULL);
}

ast::GetterDecl* gen_getter_decl (const std::string& name)
{
  return new ast::GetterDecl (1, gen_receiver (), new Identifier (1, name), new ParameterList (1), new ParameterList (1), NULL);
}

ast::FunctionDecl* gen_function_decl (const std::string& name)
{
  return new ast::FunctionDecl (1, new Identifier (1, name), new ParameterList (1), new ParameterList (1), NULL);
}

ast::FieldList* gen_field_list (const std::string& field_name,
                                const std::string& field_type)
{
  IdentifierList* idlist = new IdentifierList (1);
  idlist->append (new Identifier (1, field_name));
  Identifier* tid = new Identifier (1, field_type);
  IdentifierType* type = new IdentifierType (1, tid);

  VariableList* varlist = new VariableList (1, idlist, Mutable, Mutable, type);
  FieldList* fl = new ast::FieldList (1);
  fl->append (varlist);
  return fl;
}
