#define _GNU_SOURCE
#include "codegen.h"
#include "debug.h"
#include "util.h"
#include "asm.h"

/* static const char* */
/* action_pre_str (const type_t * type, size_t action_number) */
/* { */
/*   char* str; */
/*   asprintf (&str, "%s_%zd_pre", get (type_get_name (type)), action_number); */
/*   return str; */
/* } */

/* static const char* */
/* action_post_str (const type_t * type, size_t action_number) */
/* { */
/*   char* str; */
/*   asprintf (&str, "%s_%zd_post", get (type_get_name (type)), action_number); */
/*   return str; */
/* } */

/* static const char* */
/* bind_str (const type_t * type, size_t bind_number) */
/* { */
/*   char* str; */
/*   asprintf (&str, "%s_%zd_bind", get (type_get_name (type)), bind_number); */
/*   return str; */
/* } */

/* static const char* */
/* reaction_str (const action_t* reaction) */
/* { */
/*   char* str; */
/*   asprintf (&str, "%s_%s_reaction", get (type_get_name (reaction_component_type (reaction))), get (reaction_name (reaction))); */
/*   return str; */
/* } */

/* static const char* */
/* action_table_str (const type_t* type) */
/* { */
/*   char* str; */
/*   asprintf (&str, "%s_actions", get (type_get_name (type))); */
/*   return str; */
/* } */

/* static const char* */
/* init_str (const type_t* type) */
/* { */
/*   char* str; */
/*   asprintf (&str, "%s_init", get (type_get_name (type))); */
/*   return str; */
/* } */

/* /\* /\\* Generate a type specifier. *\\/ *\/ */
/* /\* static void *\/ */
/* /\* gen_type_spec (asm_function_t* function, const type_t * type) *\/ */
/* /\* { *\/ */
/* /\*   unimplemented; *\/ */
/* /\*   /\\* /\\\* *\\/ *\/ */
/* /\*   /\\*    If the type has a name, then use the name relying on a previous typedef. *\\/ *\/ */
/* /\*   /\\*    Otherwise, dump it out. *\\/ *\/ */
/* /\*   /\\*  *\\\/ *\\/ *\/ */
/* /\*   /\\* if (type_is_named (type)) *\\/ *\/ */
/* /\*   /\\*   { *\\/ *\/ */
/* /\*   /\\*     fprintf (file, "%s", get (type_get_name (type))); *\\/ *\/ */
/* /\*   /\\*   } *\\/ *\/ */
/* /\*   /\\* else *\\/ *\/ */
/* /\*   /\\*   { *\\/ *\/ */
/* /\*   /\\*     switch (type_kind (type)) *\\/ *\/ */
/* /\*   /\\*       { *\\/ *\/ */
/* /\*   /\\*       case TypeUndefined: *\\/ *\/ */
/* /\*   /\\*         bug ("undefined type in code generation"); *\\/ *\/ */
/* /\*   /\\*       case TypeVoid: *\\/ *\/ */
/* /\*   /\\*         unimplemented; *\\/ *\/ */
/* /\*   /\\*       case TypeBool: *\\/ *\/ */
/* /\*   /\\*         unimplemented; *\\/ *\/ */
/* /\*   /\\*         fprintf (file, "bool"); *\\/ *\/ */
/* /\*   /\\*         break; *\\/ *\/ */
/* /\*   /\\*       case TypeComponent: *\\/ *\/ */
/* /\*   /\\*         unimplemented; *\\/ *\/ */
/* /\*   /\\*         //printf ("struct %s { CONTENTS };\n", get (id)); *\\/ *\/ */
/* /\*   /\\*       case TypePointer: *\\/ *\/ */
/* /\*   /\\*         unimplemented; *\\/ *\/ */
/* /\*   /\\*       case TypePort: *\\/ *\/ */
/* /\*   /\\*         fprintf (file, "port_t*"); *\\/ *\/ */
/* /\*   /\\*         break; *\\/ *\/ */
/* /\*   /\\*       case TypeReaction: *\\/ *\/ */
/* /\*   /\\*         unimplemented; *\\/ *\/ */
/* /\*   /\\*       } *\\/ *\/ */
/* /\*   /\\*   } *\\/ *\/ */
/* /\* } *\/ */

/* /\* static const char * *\/ */
/* /\* get_format (const type_t * type) *\/ */
/* /\* { *\/ */
/* /\*   switch (type_kind (type)) *\/ */
/* /\*     { *\/ */
/* /\*     case TypeUndefined: *\/ */
/* /\*       bug ("undefined type in code generation"); *\/ */
/* /\*     case TypeVoid: *\/ */
/* /\*       unimplemented; *\/ */
/* /\*     case TypeBool: *\/ */
/* /\*       return "%d"; *\/ */
/* /\*     case TypeComponent: *\/ */
/* /\*       unimplemented; *\/ */
/* /\*     case TypePointer: *\/ */
/* /\*       unimplemented; *\/ */
/* /\*     case TypePort: *\/ */
/* /\*       unimplemented; *\/ */
/* /\*     case TypeReaction: *\/ */
/* /\*       unimplemented; *\/ */
/* /\*     } *\/ */
/* /\*   bug ("unhandled case"); *\/ */
/* /\* } *\/ */

/* static void */
/* gen_expr (asm_function_t* function, node_t * node) */
/* { */
/*   unimplemented; */
/*   /\* fprintf (file, "("); *\/ */

/*   /\* switch (node_expr_type (node)) *\/ */
/*   /\*   { *\/ */
/*   /\*   case CallExpr: *\/ */
/*   /\*     unimplemented; *\/ */

/*   /\*   case ExplicitDereferenceExpr: *\/ */
/*   /\*     fprintf (file, "*"); *\/ */
/*   /\*     gen_expr (file, node_child (node)); *\/ */
/*   /\*     break; *\/ */

/*   /\*   case IdentifierExpr: *\/ */
/*   /\*     fprintf (file, "%s", get (node_identifier_expr_identifier (node))); *\/ */
/*   /\*     break; *\/ */

/*   /\*   case ImplicitDereferenceExpr: *\/ */
/*   /\*     /\\* Do nothing. *\\/ *\/ */
/*   /\*     gen_expr (file, node_child (node)); *\/ */
/*   /\*     break; *\/ */

/*   /\*   case LogicAndExpr: *\/ */
/*   /\*     { *\/ */
/*   /\*       node_t *left = node_child (node); *\/ */
/*   /\*       node_t *right = node_sibling (left); *\/ */
/*   /\*       gen_expr (file, left); *\/ */
/*   /\*       fprintf (file, " && "); *\/ */
/*   /\*       gen_expr (file, right); *\/ */
/*   /\*     } *\/ */
/*   /\*     break; *\/ */

/*   /\*   case LogicNotExpr: *\/ */
/*   /\*     fprintf (file, "!"); *\/ */
/*   /\*     gen_expr (file, node_child (node)); *\/ */
/*   /\*     break; *\/ */

/*   /\*   case LogicOrExpr: *\/ */
/*   /\*     { *\/ */
/*   /\*       node_t *left = node_child (node); *\/ */
/*   /\*       node_t *right = node_sibling (left); *\/ */
/*   /\*       gen_expr (file, left); *\/ */
/*   /\*       fprintf (file, " || "); *\/ */
/*   /\*       gen_expr (file, right); *\/ */
/*   /\*     } *\/ */
/*   /\*     break; *\/ */

/*   /\*   case SelectExpr: *\/ */
/*   /\*     { *\/ */
/*   /\*       semval_t semval = node_get_semval (node); *\/ */
/*   /\*       action_t* reaction = semval_get_reaction (semval); *\/ */

/*   /\*       if (reaction != NULL) { *\/ */
/*   /\*         print_reaction (file, reaction); *\/ */
/*   /\*       } *\/ */
/*   /\*       else { *\/ */
/*   /\*         gen_expr (file, node_child (node)); *\/ */
/*   /\*         fprintf (file, ".%s", get (node_get_select_identifier (node))); *\/ */
/*   /\*       } *\/ */
/*   /\*     } *\/ */
/*   /\*     break; *\/ */
/*   /\*   } *\/ */

/*   /\* fprintf (file, ")"); *\/ */
/* } */

/* /\* static void *\/ */
/* /\* mov (asm_function_t* function, *\/ */
/* /\*      semval_t semval, *\/ */
/* /\*      reg_t* source, *\/ */
/* /\*      reg_t* target) *\/ */
/* /\* { *\/ */
/*   /\* fprintf (file, "\tmov "); *\/ */
/*   /\* reg_print (file, source, semval); *\/ */
/*   /\* fprintf (file, ", "); *\/ */
/*   /\* reg_print (file, target, semval); *\/ */
/*   /\* fprintf (file, "\n"); *\/ */
/* /\* } *\/ */

/* static reg_t* */
/* tree_code_generator (asm_function_t* function, */
/*                      node_t* node) */
/* { */
/*   semval_t sv = node_get_semval (node); */

/*   /\* Handle literals. *\/ */
/*   if (semval_is_value (sv)) */
/*     { */
/*       abstract_value_t av = semval_get_value (sv); */
/*       switch (abstract_value_kind (av)) */
/*         { */
/*         case UntypedValue: */
/*           return reg_get (sv); */
/*         case TypedValue: */
/*           unimplemented; */
/*         case Typed: */
/*           break; */
/*         case UndefinedValue: */
/*           unimplemented; */
/*         } */
/*     } */

/*   switch (node_expr_type (node)) */
/*     { */
/*     case CallExpr: */
/*       unimplemented; */
/*     case ExplicitDereferenceExpr: */
/*       return tree_code_generator (function, node_child (node, UNARY_CHILD)); */
/*       break; */
/*     case IdentifierExpr: */
/*       { */
/*         symbol_t* symbol = node_get_symbol (node); */
/*         reg_t* reg = symbol_get_reg (symbol); */
/*         return reg_ref (reg); */
/*       } */
/*       break; */
/*     case ImplicitDereferenceExpr: */
/*       { */
/*         /\* Determine the source register. *\/ */
/*         reg_t* source = tree_code_generator (function, node_child (node, UNARY_CHILD)); */
/*         /\* Allocate a register for the result. *\/ */
/*         reg_t* target = reg_get (sv); */
/*         if (reg_is_memory (source) && reg_is_memory (target)) */
/*           { */
/*             /\* Use a working register. *\/ */
/*             asm_function_mov (function, sv, source, reg_get_rax ()); */
/*             asm_function_mov (function, sv, reg_get_rax (), target); */
/*           } */
/*         else */
/*           { */
/*             asm_function_mov (function, sv, source, target); */
/*           } */
/*         /\* Release the source. *\/ */
/*         reg_decref (source); */
/*         return target; */
/*       } */
/*       break; */
/*     case LogicAndExpr: */
/*       unimplemented; */
/*     case LogicNotExpr: */
/*       { */
/*         /\* Determine the source register. *\/ */
/*         reg_t* source = tree_code_generator (function, node_child (node, UNARY_CHILD)); */
/*         /\* Allocate a register for the result. *\/ */
/*         reg_t* target = reg_get (sv); */
/*         /\* Use a working register. *\/ */
/*         asm_function_mov (function, sv, source, reg_get_rax ()); */
/*         asm_function_test (function, sv, reg_get_rax (), reg_get_rax ()); */
/*         /\* fprintf (file, "\ttest "); *\/ */
/*         /\* reg_print (file, reg_get_rax (), sv); *\/ */
/*         /\* fprintf (file, ", "); *\/ */
/*         /\* reg_print (file, reg_get_rax (), sv); *\/ */
/*         /\* fprintf (file, "\n"); *\/ */
/*         asm_function_setz (function, sv, target); */
/*         /\* fprintf (file, "\tsetz "); *\/ */
/*         /\* reg_print (file, target, sv); *\/ */
/*         /\* fprintf (file, "\n"); *\/ */
/*         /\* Release the source. *\/ */
/*         reg_decref (source); */
/*         return target; */
/*       } */
/*       break; */
/*     case LogicOrExpr: */
/*       unimplemented; */
/*     case SelectExpr: */
/*       { */
/*         select_result_t sr = semval_selected_field (sv); */
/*         switch (sr.kind) */
/*           { */
/*           case SelectResultField: */
/*             { */
/*               reg_t* source = tree_code_generator (function, node_child (node, UNARY_CHILD)); */
/*               reg_t* target = reg_offset (source, sr.field); */
/*               reg_decref (source); */
/*               return target; */
/*             } */
/*           case SelectResultReaction: */
/*             unimplemented; */
/*           case SelectResultUndefined: */
/*             unimplemented; */
/*           } */
/*         unimplemented; */
/*       } */
/*       break; */
/*     } */

/*   unimplemented; */
/* } */

/* /\* static void *\/ */
/* /\* print_label (FILE* file, asm_label_t label) *\/ */
/* /\* { *\/ */
/* /\*   fprintf (file, "l%zd", label); *\/ */
/* /\* } *\/ */

/* /\* static asm_label_t *\/ */
/* /\* label_make () *\/ */
/* /\* { *\/ */
/* /\*   static asm_label_t label = 0; *\/ */
/* /\*   return label++; *\/ */
/* /\* } *\/ */

/* static void */
/* gen_stmt (asm_function_t* function, node_t * node) */
/* { */
/*   switch (node_stmt_type (node)) */
/*     { */
/*     case AssignmentStmt: */
/*       { */
/* 	node_t *left = node_child (node, BINARY_LEFT_CHILD); */
/* 	node_t *right = node_child (node, BINARY_RIGHT_CHILD); */
/*         reg_t* target = tree_code_generator (function, left); */
/*         reg_t* source = tree_code_generator (function, right); */
/*         semval_t sv = node_get_semval (right); */
/*         asm_function_mov (function, sv, source, reg_get_rax ()); */
/*         asm_function_mov (function, sv, reg_get_rax (), target); */
/*         reg_decref (source); */
/*         reg_decref (target); */
/*       } */
/*       break; */

/*     case BindListStmt: */
/*       { */
/*         NODE_FOREACH (child, node) */
/*           { */
/*             gen_stmt (function, child); */
/*           } */
/*       } */
/*       break; */

/*     case BindStmt: */
/*       { */
/*         /\* node_t* output = node_child (node); *\/ */
/*         /\* node_t* input = node_sibling (output); *\/ */

/*         /\* reg_t* r = tree_code_generator (file, input); *\/ */
/*         /\* fprintf (file, "\tpush "); *\/ */
/*         /\* reg_print (file, r); *\/ */
/*         /\* fprintf (file, "\n"); *\/ */
/*         /\* reg_decref (r); *\/ */

/*         /\* r = tree_code_generator (file, node_child (input)); *\/ */
/*         /\* fprintf (file, "\tpush "); *\/ */
/*         /\* reg_print (file, r); *\/ */
/*         /\* fprintf (file, "\n"); *\/ */
/*         /\* reg_decref (r); *\/ */

/*         /\* r = tree_code_generator (file, output); *\/ */
/*         /\* fprintf (file, "\tpush "); *\/ */
/*         /\* reg_print (file, r); *\/ */
/*         /\* fprintf (file, "\n"); *\/ */
/*         /\* reg_decref (r); *\/ */

/*         asm_function_comment (function, "\t# TODO: SAVE REGISTERS"); */
/*         asm_function_comment (function, "\t# TODO: call bind_port_reaction\n"); */
/*         asm_function_comment (function, "\t# TODO: RESTORE REGISTERS\n"); */
/*         asm_function_comment (function, "\t# TODO: POP THE STACK\n"); */

/*         /\* fprintf (file, "bind_port_reaction (&"); *\/ */
/*         /\* gen_expr (file, output); *\/ */
/*         /\* fprintf (file, ", &"); *\/ */
/*         /\* gen_expr (file, node_child (input)); *\/ */
/*         /\* fprintf (file, ".instance, "); *\/ */
/*         /\* gen_expr (file, input); *\/ */
/*         /\* fprintf (file, ");\n"); *\/ */
/*       } */
/*       break; */

/*     case ExprStmt: */
/*       gen_expr (function, node_child (node, UNARY_CHILD)); */
/*       break; */

/*     case ListStmt: */
/*       { */
/*         NODE_FOREACH (child, node) */
/* 	  { */
/* 	    gen_stmt (function, child); */
/* 	  } */
/*       } */
/*       break; */

/*     case PrintStmt: */
/*       { */
/*         asm_function_comment (function, "\t# TODO: PRINT"); */
/*       /\*   node_t *child = node_child (node); *\/ */
/*       /\*   abstract_value_t a = semval_get_value (node_get_semval (child)); *\/ */
/*       /\*   switch (abstract_value_kind (a)) *\/ */
/*       /\*     { *\/ */
/*       /\*     case UntypedValue: *\/ */
/*       /\*       { *\/ */
/*       /\*         untyped_value_t u = abstract_value_get_untyped_value (a); *\/ */
/*       /\*         switch (untyped_value_kind (u)) *\/ */
/*       /\*   	{ *\/ */
/*       /\*   	case UntypedUndefined: *\/ */
/*       /\*   	  bug ("undefined untyped value in code generation"); *\/ */
/*       /\*   	case UntypedBool: *\/ */
/*       /\*   	  if (untyped_value_bool_value (u)) *\/ */
/*       /\*   	    { *\/ */
/*       /\*   	      fprintf (file, "printf (\"1\");\n"); *\/ */
/*       /\*   	    } *\/ */
/*       /\*   	  else *\/ */
/*       /\*   	    { *\/ */
/*       /\*   	      fprintf (file, "printf (\"0\");\n"); *\/ */
/*       /\*   	    } *\/ */
/*       /\*   	  break; *\/ */
/*       /\*   	} *\/ */
/*       /\*       } *\/ */
/*       /\*       break; *\/ */

/*       /\*     case TypedValue: *\/ */
/*       /\*       unimplemented; *\/ */

/*       /\*     case Typed: *\/ */
/*       /\*       fprintf (file, "printf (\""); *\/ */
/*       /\*       fprintf (file, "%s", get_format (abstract_value_get_typed (a))); *\/ */
/*       /\*       fprintf (file, "\", "); *\/ */
/*       /\*       gen_expr (file, child); *\/ */
/*       /\*       fprintf (file, ");\n"); *\/ */
/*       /\*       break; *\/ */

/*       /\*     case UndefinedValue: *\/ */
/*       /\*       bug ("undefined value in code generation"); *\/ */
/*       /\*     } *\/ */
/*       /\* fprintf (file, "printf (\"\\n\");\n"); *\/ */
/*       } */
/*       break; */

/*     case ReturnStmt: */
/*       { */
/*         node_t* child = node_child (node, UNARY_CHILD); */
/*         reg_t* source = tree_code_generator (function, child); */
/*         semval_t sv = node_get_semval (child); */
/*         asm_function_mov (function, sv, source, reg_get_rax ()); */
/*         reg_decref (source); */
/*         asm_function_jmp (function, asm_function_end_label (function)); */
/*         /\* fprintf (file, "\tjmp "); *\/ */
/*         /\* print_label (file, end_label); *\/ */
/*         /\* fprintf (file, "\n"); *\/ */
/*       } */
/*       break; */

/*     case TriggerStmt: */
/*       { */
/*         /\* Link this. *\/ */
/*         symtab_t* symtab = node_get_symtab (node); */
/*         const symtab_t* parent_symtab = symtab_parent (symtab); */
/*         symbol_t* this = symtab_get_this (symtab); */
/*         symbol_t* parent_this = symtab_get_this (parent_symtab); */
/*         reg_t* reg = symbol_get_reg (parent_this); */
/*         symbol_set_reg (this, reg); */

/*         asm_function_comment (function, "\t# TODO: TRIGGER"); */
/* 	/\* Just print the body for now. *\/ */
/*         /\* node_t* expr_list = node_child (node, TRIGGER_EXPR_LIST); *\/ */
/* 	node_t *body = node_child (node, TRIGGER_BODY); */
/* 	gen_stmt (function, body); */
/*       } */
/*       break; */

/*     case VarStmt: */
/*       { */
/*         unimplemented; */
/* 	/\* node_t *identifier_list = node_child (node); *\/ */
/* 	/\* node_t *type_spec = node_sibling (identifier_list); *\/ */

/* 	/\* /\\* Iterate over the identifier list. *\\/ *\/ */
/* 	/\* node_t *child; *\/ */
/* 	/\* for (child = node_child (identifier_list); child != NULL; *\/ */
/* 	/\*      child = node_sibling (child)) *\/ */
/* 	/\*   { *\/ */
/* 	/\*     gen_type_spec (function, node_get_type (type_spec)); *\/ */
/* 	/\*     fprintf (file, " %s;\n", get (node_identifier (child))); *\/ */
/* 	/\*   } *\/ */
/*       } */
/*       break; */

/*     } */
/* } */

/* /\* static asm_label_t *\/ */
/* /\* enter_function (FILE* file) *\/ */
/* /\* { *\/ */
/* /\*   fprintf (file, "\tpush %%rbp\n"); *\/ */
/* /\*   fprintf (file, "\tmov %%rsp, %%rbp\n"); *\/ */
/* /\*   return label_make (); *\/ */
/* /\* } *\/ */

/* /\* static void *\/ */
/* /\* leave_function (FILE* file, *\/ */
/* /\*                 asm_label_t label) *\/ */
/* /\* { *\/ */
/* /\*   print_label (file, label); *\/ */
/* /\*   fprintf (file, ":\n"); *\/ */
/* /\*   fprintf (file, "\tmov %%rbp, %%rsp\n"); *\/ */
/* /\*   fprintf (file, "\tpop %%rbp\n"); *\/ */
/* /\*   fprintf (file, "\tretq\n"); *\/ */
/* /\*   fprintf (file, "\n"); *\/ */
/* /\* } *\/ */

/* static void */
/* gen_action_defs (asm_module_t* module, node_t * node) */
/* { */
/*   switch (node_def_type (node)) */
/*     { */
/*     case ActionDef: */
/*       { */
/* 	action_t* action = node_get_action (node); */
/* 	node_t *receiver = node_child (node, ACTION_RECEIVER); */
/* 	const type_t *type = node_receiver_get_type (receiver); */
/* 	node_t *precondition = node_child (node, ACTION_PRECONDITION); */
/* 	node_t *body = node_child (node, ACTION_BODY); */
/*         symtab_t* symtab = node_get_symtab (node); */

/*         reg_check (); */
/*         context_t context = context_make (); */
/*         reg_allocate (&context, symtab); */

/* 	/\* Output the precondition as a function. *\/ */

/*         asm_function_t* pre_function = asm_module_make_function (module, action_pre_str (type, action_number (action))); */
/*         /\* fprintf (file, ".text\n"); *\/ */
/*         /\* fprintf (file, ":\n"); *\/ */
/*         /\* label_t label = enter_function (file); *\/ */
/* 	gen_stmt (pre_function, precondition); */
/*         asm_function_end (pre_function); */

/* 	/\* Output the body as a function. *\/ */
/*         asm_function_t* post_function = asm_module_make_function (module, action_post_str (type, action_number (action))); */

/*         /\* fprintf (file, ".text\n"); *\/ */
/* 	/\* print_action_post (file, type, action_number (action)); *\/ */
/*         /\* fprintf (file, ":\n"); *\/ */
/*         /\* label = enter_function (file); *\/ */
/* 	gen_stmt (post_function, body); */
/*         asm_function_end (post_function); */

/*         reg_deallocate (symtab); */
/*         reg_check (); */
/*       } */
/*       break; */

/*     case BindDef: */
/*       { */
/*         size_t bind_number = node_get_bind_number (node); */
/* 	node_t *receiver = node_child (node, BIND_RECEIVER); */
/* 	const type_t *type = node_receiver_get_type (receiver); */
/* 	node_t *body = node_child (node, BIND_BODY); */

/* 	/\* Output the body as a function. *\/ */
/*         asm_function_t* function = asm_module_make_function (module, bind_str (type, bind_number)); */
/*         /\* fprintf (file, ".text\n"); *\/ */
/*         /\* fprintf (file, ":\n"); *\/ */
/* 	gen_stmt (function, body); */
/*         asm_function_end (function); */
/*       } */
/*       break; */

/*     case InstanceDef: */
/*       /\* Do nothing. *\/ */
/*       break; */

/*     case ListDef: */
/*       { */
/* 	NODE_FOREACH (child, node) */
/* 	{ */
/* 	  gen_action_defs (module, child); */
/* 	} */
/*       } */
/*       break; */

/*     case ReactionDef: */
/*       { */
/* 	/\* node_t *receiver = node_child (node, REACTION_RECEIVER); *\/ */
/*         /\* node_t *identifier = node_child (node, REACTION_IDENTIFIER); *\/ */
/*         /\* node_t *signature = node_child (node, REACTION_SIGNATURE); *\/ */
/*         node_t *body = node_child (node, REACTION_BODY); */

/*         action_t* reaction = node_get_action (node); */

/* 	/\* Output the body as a function. *\/ */
/*         asm_function_t* function = asm_module_make_function (module, reaction_str (reaction)); */
/*         /\* fprintf (file, ".text\n"); *\/ */
/* 	/\* print_reaction (file, reaction); *\/ */
/*         /\* fprintf (file, ":\n"); *\/ */
/* 	gen_stmt (function, body); */
/*         asm_function_end (function); */
/*       } */
/*       break; */

/*     case TypeDef: */
/*       /\* Do nothing. *\/ */
/*       break; */
/*     } */
/* } */

/* static void */
/* gen_pre_post_tables (asm_module_t* module, node_t * node) */
/* { */
/*   switch (node_def_type (node)) */
/*     { */
/*     case ActionDef: */
/*       /\* Do nothing. *\/ */
/*       break; */

/*     case BindDef: */
/*       /\* Do nothing. *\/ */
/*       break; */

/*     case InstanceDef: */
/*       /\* Do nothing. *\/ */
/*       break; */

/*     case ListDef: */
/*       { */
/* 	NODE_FOREACH (child, node) */
/* 	{ */
/* 	  gen_pre_post_tables (module, child); */
/* 	} */
/*       } */
/*       break; */

/*     case ReactionDef: */
/*       /\* Do nothing. *\/ */
/*       break; */

/*     case TypeDef: */
/*       { */
/* 	/\* node_t *identifier = node_child (node, TYPE_IDENTIFIER); *\/ */
/* 	node_t *type_spec = node_child (node, TYPE_TYPE_SPEC); */
/* 	const type_t *type = node_get_type (type_spec); */
/* 	if (type_is_component (type)) */
/* 	  { */
/*             asm_data_t* data = asm_module_make_data (module, action_table_str (type)); */
/*             /\* fprintf (file, ".text\n"); *\/ */
/*             action_t *action_pos, *action_limit; */
/*             for (action_pos = type_actions_begin (type), action_limit = type_actions_end (type); */
/*                  action_pos != action_limit; */
/*                  action_pos = action_next (action_pos)) */
/*               { */
/*                 if (action_is_action (action_pos)) */
/*                   { */
/*                     size_t number = action_number (action_pos); */

/*                     asm_data_label (data, action_pre_str (type, number)); */
/*                     asm_data_label (data, action_post_str (type, number)); */
/*                     /\* fprintf (file, ".quad "); *\/ */
/*                     /\* fputs (action_pre_str (type, number), file); *\/ */
/*                     /\* fprintf (file, "\n"); *\/ */
/*                     /\* fprintf (file, ".quad "); *\/ */
/*                     /\* fputs (action_post_str (type, number), file); *\/ */
/*                     /\* fprintf (file, "\n"); *\/ */
/*                   } */
/* 	      } */
/*             asm_data_null_pointer (data); */
/*             asm_data_null_pointer (data); */
/*             /\* fprintf (file, ".quad 0\n"); *\/ */
/*             /\* fprintf (file, ".quad 0\n\n"); *\/ */

/* 	    /\* Generate a function that initializes the component instance. *\/ */
/*             reg_check (); */
/*             context_t context = context_make (); */
/*             reg_t* this_reg = reg_allocate_argument (&context, type_make_pointer (type)); */

/*             asm_function_t* function = asm_module_make_function (module, init_str (type)); */
/*             /\* fprintf (file, ".text\n"); *\/ */
/* 	    /\* fprintf (file, "%s_init:\n", get (type_name)); *\/ */
/*             /\* asm_label_t label = enter_function (file); *\/ */
/*             /\* Clear the instance. *\/ */
/*             /\* TODO:  Move pointer to edi. *\/ */
/*             asm_function_memclr (function, type_size (type)); */
/*             /\* fprintf (file, "\txor %%rax, %%rax\n"); *\/ */
/*             /\* fprintf (file, "\tcld\n"); *\/ */
/*             /\* fprintf (file, "\tmov $%zd, %%rcx\n", type_size (type)); *\/ */
/*             /\* fprintf (file, "\trep stosb (%%rdi)\n"); *\/ */
/*             /\* Initialize the actions. *\/ */
/* 	    asm_function_comment (function, "\t# TODO instance->instance.actions = %s_actions;\n"); */
/*             /\* get (type_name) *\/ */
/*             /\* Initialize sub-components. *\/ */
/*             const field_t* field; */
/*             for (field = type_field_list (type); */
/*                  field != NULL; */
/*                  field = field_next (field)) { */
/*               const type_t* ft = field_type (field); */
/*               if (type_is_component (ft)) { */
/*                 asm_function_comment (function, "\t# TODO %s_init (&(instance->%s));\n"); */
/*                 /\* , get (type_get_name (ft)), get (field_name (field))); *\/ */
/*               } */
/*             } */
/*             /\* Emit code to bind ports. *\/ */
/*             size_t bind_number; */
/*             for (bind_number = 0; */
/*                  bind_number != type_bind_count (type); */
/*                  ++bind_number) { */
/*               asm_function_comment (function, "TODO BIND"); */
/*               /\* fprintf (file, "\t# TODO"); *\/ */
/*               /\* fputs (bind_str (type, bind_number), file); *\/ */
/*               /\* fprintf (file, "(instance);\n"); *\/ */
/*             } */
/*             /\* Schedule the instance. *\/ */
/*             asm_function_comment (function, "TODO SCHEDULE"); */
/*             /\* fprintf (file, "\t# TODO schedule (&(instance->instance));\n"); *\/ */
/*             asm_function_end (function); */

/*             reg_decref (this_reg); */
/*             reg_check (); */
/* 	  } */
/*       } */
/*       break; */
/*     } */
/* } */

/* static void */
/* gen_instances (asm_module_t* module, node_t * node) */
/* { */
/*   switch (node_def_type (node)) */
/*     { */
/*     case ActionDef: */
/*       /\* Do nothing. *\/ */
/*       break; */

/*     case BindDef: */
/*       /\* Do nothing. *\/ */
/*       break; */

/*     case InstanceDef: */
/*       { */
/* 	/\* node_t *instance_id = node_child (node); *\/ */
/* 	/\* string_t instance_str = node_identifier (instance_id); *\/ */
/* 	/\* node_t *type_id = node_sibling (instance_id); *\/ */
/* 	/\* string_t type_str = node_identifier (type_id); *\/ */

/* 	/\* fprintf (file, "static %s %s;\n", get (type_str), get (instance_str)); *\/ */

/* 	/\* /\\* Emit a thunk to initialize the instance. *\\/ *\/ */
/* 	/\* fprintf (file, *\/ */
/* 	/\* 	 "__attribute__((constructor)) static void %s_init (void) {\n", *\/ */
/* 	/\* 	 get (instance_str)); *\/ */
/* 	/\* fprintf (file, "%s_init (&%s);\n", get (type_str), *\/ */
/* 	/\* 	 get (instance_str)); *\/ */
/* 	/\* fprintf (file, "}\n"); *\/ */
/*       } */

/*       break; */

/*     case ListDef: */
/*       { */
/* 	NODE_FOREACH (child, node) */
/* 	{ */
/* 	  gen_instances (module, child); */
/* 	} */
/*       } */
/*       break; */

/*     case ReactionDef: */
/*       /\* Do nothing. *\/ */
/*       break; */

/*     case TypeDef: */
/*       /\* Do nothing. *\/ */
/*       break; */
/*     } */
/* } */

/* int */
/* generate_code (FILE * file, node_t * node) */
/* { */
/*   if (debug) */
/*     { */
/*       node_print (node, 0); */
/*     } */

/*   asm_module_t* module = asm_module_make (); */

/*   /\* Action defs. *\/ */
/*   gen_action_defs (module, node); */

/*   /\* Generate the list of pre/posts for each component type. *\/ */
/*   gen_pre_post_tables (module, node); */

/*   /\* Generate the list of instances. *\/ */
/*   gen_instances (module, node); */

/*   asm_module_write (module, file); */

/*   fflush (file); */
/*   fclose (file); */

/*   return 0; */
/* } */
