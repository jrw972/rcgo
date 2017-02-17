#ifndef RC_UTEST_TREEGEN_HPP
#define RC_UTEST_TREEGEN_HPP

#include "node.hpp"

extern const std::string DEFAULT_TYPE_NAME;

// Helpers for programmatically generating ASTs.

ast::ActionDecl* gen_action_decl (const std::string& name, long dimension = -1);
ast::ReactionDecl* gen_reaction_decl (const std::string& name, long dimension = -1);
ast::BinderDecl* gen_bind_decl (const std::string& name);
ast::TypeDecl* gen_type_decl (const std::string& name, ast::Node* type);
ast::MethodDecl* gen_method_decl (const std::string& name);
ast::InitializerDecl* gen_init_decl (const std::string& name);
ast::GetterDecl* gen_getter_decl (const std::string& name);
ast::FunctionDecl* gen_function_decl (const std::string& name);
ast::InstanceDecl* gen_instance_decl (const std::string& name, const std::string& init);
ast::ConstDecl* gen_const_decl (const std::string& name);

ast::FieldList* gen_field_list (const std::string& field_name,
                                const std::string& field_type);

#endif // RC_UTEST_TREEGEN_HPP
