#ifndef ast_h
#define ast_h

#include "strtab.h"
#include "symtab.h"

// TODO:  Divide these into top-level and others.
typedef enum
{
  AstAction,
  AstBind,
  AstBindStatement,
  AstExpression,
  AstIdentifier,
  AstIdentifierList,
  AstInstance,
  AstMethod,
  AstReceiverDefinition,
  AstReaction,
  AstStatement,
  AstTopLevelList,
  AstTypeDefinition,
  AstTypeSpecification,
} AstKind;

typedef enum
{
  AstPointerReceiver,
  AstPointerToImmutableReceiver,
} AstReceiverKind;

typedef enum
{
  AstComponent,
  AstEmptyTypeSpec,
  AstFieldList,
  AstForeign,
  AstHeap,
  AstIdentifierListTypeSpec,
  AstIdentifierTypeSpec,
  AstPointer,
  AstPointerToForeign,
  AstPointerToImmutable,
  AstPort,
  AstSignature,
  AstStruct,
} AstTypeSpecificationKind;

typedef enum
{
  AstAddAssignStmt,
  AstAssignmentStmt,
  AstChangeStmt,
  AstExpressionStmt,
  AstIfStmt,
  AstPrintlnStmt,
  AstReturnStmt,
  AstStmtList,
  AstSubtractAssignStmt,
  AstTriggerStmt,
  AstVarStmt,
} AstStatementKind;

typedef enum
{
  AstBindStmtList,
  AstBindStmt,
} AstBindStatementKind;

typedef struct {
  void (*visit_expression) (void* data, ast_t* ast); // TODO:  Remove this.

  void (*visit_identifier) (void* data, ast_t* ast);

  void (*visit_type_specification) (void* data, ast_t* ast);

  void (*visit_typed_literal) (void* data, ast_t* ast);
  void (*visit_address_of_expr) (void* data, ast_t* ast);
  void (*visit_call_expr) (void* data, ast_t* ast);
  void (*visit_dereference_expr) (void* data, ast_t* ast);
  void (*visit_equal_expr) (void* data, ast_t* ast);
  void (*visit_identifier_expr) (void* data, ast_t* ast);
  void (*visit_logic_not_expr) (void* data, ast_t* ast);
  void (*visit_merge_expr) (void* data, ast_t* ast);
  void (*visit_move_expr) (void* data, ast_t* ast);
  void (*visit_new_expr) (void* data, ast_t* ast);
  void (*visit_not_equal_expr) (void* data, ast_t* ast);
  void (*visit_port_call_expr) (void* data, ast_t* ast);
  void (*visit_select_expr) (void* data, ast_t* ast);

  void (*visit_expr_list) (void* data, ast_t* ast);

  void (*visit_statement) (void* data, ast_t* ast);
  void (*visit_bind_statement) (void* data, ast_t* ast);

  void (*visit_top_level_list) (void* data, ast_t* ast);
} ast_visitor_t;

typedef struct {
  void (*visit_expression) (void* data, const ast_t* ast); // TODO:  Remove this.

  void (*visit_identifier) (void* data, const ast_t* ast);

  void (*visit_type_specification) (void* data, const ast_t* ast);

  void (*visit_typed_literal) (void* data, const ast_t* ast);
  void (*visit_address_of_expr) (void* data, const ast_t* ast);
  void (*visit_call_expr) (void* data, const ast_t* ast);
  void (*visit_dereference_expr) (void* data, const ast_t* ast);
  void (*visit_equal_expr) (void* data, const ast_t* ast);
  void (*visit_identifier_expr) (void* data, const ast_t* ast);
  void (*visit_logic_not_expr) (void* data, const ast_t* ast);
  void (*visit_merge_expr) (void* data, const ast_t* ast);
  void (*visit_move_expr) (void* data, const ast_t* ast);
  void (*visit_new_expr) (void* data, const ast_t* ast);
  void (*visit_not_equal_expr) (void* data, const ast_t* ast);
  void (*visit_port_call_expr) (void* data, const ast_t* ast);
  void (*visit_select_expr) (void* data, const ast_t* ast);

  void (*visit_expr_list) (void* data, const ast_t* ast);

  void (*visit_statement) (void* data, const ast_t* ast);
  void (*visit_bind_statement) (void* data, const ast_t* ast);

  void (*visit_top_level_list) (void* data, const ast_t* ast);
} ast_const_visitor_t;

/* Generic functions. */

void ast_accept (ast_t* ast, ast_visitor_t* visitor, void* data);

void ast_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data);

void ast_print (const ast_t * ast, size_t indent);

AstKind ast_kind (const ast_t * ast);

ast_t *ast_prepend_child (ast_t * parent, ast_t * child);

ast_t *ast_append_child (ast_t * parent, ast_t * child);

ast_t *ast_get_child (const ast_t * ast, size_t idx);

ast_t **ast_get_child_ptr (const ast_t * ast, size_t idx);

void ast_set_child (ast_t * parent, size_t idx, ast_t * child);

size_t ast_children_count (const ast_t * ast);

ast_t **ast_children_begin (const ast_t * ast);

ast_t **ast_children_end (const ast_t * ast);

ast_t **ast_next_child (ast_t ** child);

const char *ast_file (const ast_t * ast);

unsigned int ast_line (const ast_t * ast);

void ast_set_symtab (ast_t * ast, symtab_t * symtab);

symtab_t *ast_get_symtab (const ast_t * ast);

/* Decl */

ast_t *ast_make_identifier (unsigned int line, string_t identifier);

string_t ast_get_identifier (const ast_t * ast);

ast_t *ast_make_identifier_list (unsigned int line);

#define IDENTIFIER_LIST 0
#define TYPE_SPEC 1

ast_t *ast_make_identifier_list_type_spec (unsigned int line,
                                           ast_t * identifier_list,
					   ast_t * type_spec);

#define RECEIVER_THIS_IDENTIFIER 0
#define RECEIVER_TYPE_IDENTIFIER 1

ast_t *ast_make_receiver (unsigned int line, ast_t * this_identifier,
                          ast_t * type_identifier,
                          AstReceiverKind kind);

AstReceiverKind ast_receiver_kind (const ast_t* receiver);

/* Def */

#define ACTION_RECEIVER 0
#define ACTION_PRECONDITION 1
#define ACTION_BODY 2

ast_t *ast_make_action_def (unsigned int line, ast_t * receiver,
			    ast_t * precondition, ast_t * body);

#define BIND_RECEIVER 0
#define BIND_BODY 1

ast_t *ast_make_bind_def (unsigned int line, ast_t * receiver, ast_t * list);

#define INSTANCE_IDENTIFIER 0
#define INSTANCE_TYPE_IDENTIFIER 1

ast_t *ast_make_instance_def (unsigned int line, ast_t * instance_id, ast_t * type_id);

void ast_instance_set_type (ast_t * ast, type_t * type);

type_t *ast_instance_get_type (ast_t * ast);

ast_t *ast_make_top_level_list (void);

#define REACTION_RECEIVER 0
#define REACTION_IDENTIFIER 1
#define REACTION_SIGNATURE 2
#define REACTION_BODY 3

ast_t *ast_make_reaction_def (unsigned int line, ast_t * receiver, ast_t * identifier,
			      ast_t * signature, ast_t * body);

#define TYPE_IDENTIFIER 0
#define TYPE_TYPE_SPEC 1

ast_t *ast_make_type_def (unsigned int line, ast_t * identifier, ast_t * type_spec);

#define METHOD_RECEIVER 0
#define METHOD_IDENTIFIER 1
#define METHOD_SIGNATURE 2
#define METHOD_RETURN_TYPE 3
#define METHOD_BODY 4

ast_t *ast_make_method_def (unsigned int line, ast_t * receiver, ast_t * identifier,
                            ast_t * signature, ast_t * return_type,
                            ast_t* body);

/* Expr */

void ast_set_symbol (ast_t * ast, symbol_t * symbol);

symbol_t *ast_get_symbol (const ast_t * ast);

#define CALL_EXPR 0
#define CALL_ARGS 1

ast_t *ast_make_address_of (unsigned int line, ast_t * expr);

ast_t *ast_make_call_expr (unsigned int line, ast_t * expr, ast_t * args);
#define UNARY_CHILD 0
#define BINARY_LEFT_CHILD 0
#define BINARY_RIGHT_CHILD 1

ast_t *ast_make_dereference (unsigned int line, ast_t * expr);

ast_t *ast_make_identifier_expr (unsigned int line, ast_t * identifier);

ast_t *ast_make_logic_not (unsigned int line, ast_t * child);

ast_t *ast_make_logic_and (unsigned int line, ast_t * left, ast_t * right);

ast_t *ast_make_logic_or (unsigned int line, ast_t * left, ast_t * right);

ast_t *ast_make_select (unsigned int line, ast_t * expr, ast_t * identifier);

/* Stmt */

#define CHANGE_EXPR 0
#define CHANGE_IDENTIFIER 1
#define CHANGE_TYPE 2
#define CHANGE_BODY 3

ast_t* ast_make_change_stmt (unsigned int line,
                             ast_t * expr,
                             ast_t * identifier,
                             ast_t * type,
                             ast_t * body);

ast_t *ast_make_expr_stmt (unsigned int line, ast_t * expr);

#define VAR_IDENTIFIER_LIST 0
#define VAR_TYPE_SPEC 1

ast_t *ast_make_var_stmt (unsigned int line, ast_t * identifier_list, ast_t * type_spec);

ast_t *ast_make_assignment_stmt (unsigned int line, ast_t * lvalue, ast_t * rvalue);

ast_t *ast_make_add_assign_stmt (unsigned int line, ast_t * lvalue, ast_t * rvalue);

ast_t *ast_make_subtract_assign_stmt (unsigned int line, ast_t * lvalue, ast_t * rvalue);

ast_t *ast_make_stmt_list (unsigned int line);

ast_t *ast_make_return_stmt (unsigned int line, ast_t * expr);

#define TRIGGER_EXPRESSION_LIST 0
#define TRIGGER_BODY 1

ast_t *ast_make_trigger_stmt (unsigned int line, ast_t * expr_list, ast_t * stmt);

ast_t *ast_make_bind_list_stmt (unsigned int line);

ast_t *ast_make_bind_stmt (unsigned int line, ast_t * output, ast_t * input);

ast_t *ast_make_println_stmt (unsigned int line, ast_t * expr);

/* TypeSpec */

ast_t *ast_make_field_list (unsigned int line);

#define IDENTIFIER_TYPE_SPEC_CHILD 0

ast_t *ast_make_identifier_type_spec (unsigned int line, ast_t * identifier);

#define PORT_SIGNATURE 0

ast_t *ast_make_port (unsigned int line, ast_t * signature);

#define COMPONENT_FIELD_LIST 0

ast_t *ast_make_component_type_spec (unsigned int line, ast_t * field_list);

#define STRUCT_FIELD_LIST 0

ast_t *ast_make_struct_type_spec (unsigned int line, ast_t * field_list);

ast_t *ast_make_empty_type_spec (unsigned int line);

#define POINTER_BASE_TYPE 0

ast_t *ast_make_pointer_type_spec (unsigned int line, ast_t* type_spec);

ast_t *ast_make_pointer_to_immutable_type_spec (unsigned int line, ast_t* type_spec);

ast_t *ast_make_pointer_to_foreign_type_spec (unsigned int line, ast_t* type_spec);

#define FOREIGN_BASE_TYPE 0

ast_t *ast_make_foreign_type_spec (unsigned int line, ast_t* type_spec);

#define HEAP_BASE_TYPE 0

ast_t *ast_make_heap_type_spec (unsigned int line, ast_t * type);

AstTypeSpecificationKind ast_type_specification_kind (const ast_t * node);



ast_t *ast_make_expression_list (unsigned int line);

AstStatementKind ast_statement_kind (const ast_t * node);

AstBindStatementKind ast_bind_statement_kind (const ast_t * node);

ast_t *ast_make_typed_literal (unsigned int line, typed_value_t value);

void ast_set_typed_value (ast_t* node, typed_value_t value);

typed_value_t ast_get_typed_value (const ast_t* node);

void ast_set_type (ast_t * node, typed_value_t typed_value,
		   bool derived_from_receiver);

bool ast_get_derived_from_receiver (const ast_t * node);

ast_t *ast_make_signature (unsigned int line);

ast_t *ast_make_port_call (unsigned int line, ast_t * identifier, ast_t * args);

action_t *ast_get_current_action (const ast_t * node);

type_t *ast_get_current_receiver_type (const ast_t * node);

ast_t *ast_make_equal (unsigned int line, ast_t* left, ast_t* right);

ast_t *ast_make_not_equal (unsigned int line, ast_t* left, ast_t* right);

#define IF_CONDITION 0
#define IF_TRUE_BRANCH 1

ast_t *ast_make_if_stmt (unsigned int line, ast_t* condition, ast_t* true_branch);

ast_t *ast_make_new_expr (unsigned int line, ast_t* identifier);

ast_t *ast_make_move_expr (unsigned int line, ast_t* expr);

ast_t *ast_make_merge_expr (unsigned int line, ast_t* expr);

#define AST_FOREACH(child, parent) size_t idx; size_t limit; ast_t* child; \
  for (idx = 0, limit = ast_children_count (parent), child = ((idx < ast_children_count (parent)) ? ast_get_child (parent, idx) : NULL); idx != limit; ++idx, child = (idx < (ast_children_count (parent)) ? ast_get_child (parent, idx) : NULL))

#define AST_FOREACH_PTR(child, parent) ast_t** child; ast_t** limit; \
  for (child = ast_children_begin (parent), limit = ast_children_end (parent); child != limit; child = ast_next_child (child))

#endif /* ast_h */
