#ifndef ast_h
#define ast_h

#include "strtab.h"
#include "symtab.h"

typedef enum
{
  AstAction,
  AstBind,
  AstBindStatement,
  AstExpression,
  AstIdentifier,
  AstIdentifierList,
  AstInstance,
  AstPointerReceiverDefinition,
  AstReaction,
  AstStatement,
  AstTopLevelList,
  AstTypeDefinition,
  AstTypeSpecification,
} AstKind;

typedef enum
{
  AstComponent,
  AstFieldList,
  AstIdentifierListTypeSpec,
  AstIdentifierTypeSpec,
  AstPort,
  AstSignature,
} AstTypeSpecificationKind;

typedef enum
{
  AstCallExpr,
  AstDereferenceExpr,
  AstExprList,
  AstIdentifierExpr,
  AstLogicAndExpr,
  AstLogicNotExpr,
  AstLogicOrExpr,
  AstPortCallExpr,
  AstSelectExpr,
  AstTypedLiteral,
  AstUntypedLiteral,
} AstExpressionKind;

typedef enum
{
  AstAssignmentStmt,
  AstExpressionStmt,
  AstStmtList,
  AstReturnStmt,
  AstTriggerStmt,
  AstVarStmt,
} AstStatementKind;

typedef enum
{
  AstBindStmtList,
  AstBindStmt,
} AstBindStatementKind;

/* Generic functions. */

void ast_print (const ast_t * ast, size_t indent);

AstKind ast_kind (const ast_t * ast);

ast_t *ast_add_child (ast_t * parent, ast_t * child);

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

ast_t *ast_make_identifier (string_t identifier);

string_t ast_get_identifier (const ast_t * ast);

ast_t *ast_make_identifier_list (void);

#define IDENTIFIER_LIST 0
#define TYPE_SPEC 1

ast_t *ast_make_identifier_list_type_spec (ast_t * identifier_list,
					   ast_t * type_spec);

#define RECEIVER_THIS_IDENTIFIER 0
#define RECEIVER_TYPE_IDENTIFIER 1

ast_t *ast_make_pointer_receiver (ast_t * this_identifier,
				  ast_t * type_identifier);

/* Def */

#define ACTION_RECEIVER 0
#define ACTION_PRECONDITION 1
#define ACTION_BODY 2

ast_t *ast_make_action_def (ast_t * receiver,
			    ast_t * precondition, ast_t * body);

#define BIND_RECEIVER 0
#define BIND_BODY 1

ast_t *ast_make_bind_def (ast_t * receiver, ast_t * list);

#define INSTANCE_IDENTIFIER 0
#define INSTANCE_TYPE_IDENTIFIER 1

ast_t *ast_make_instance_def (ast_t * instance_id, ast_t * type_id);

void ast_instance_set_type (ast_t * ast, type_t * type);

type_t *ast_instance_get_type (ast_t * ast);

ast_t *ast_make_top_level_list (void);

#define REACTION_RECEIVER 0
#define REACTION_IDENTIFIER 1
#define REACTION_SIGNATURE 2
#define REACTION_BODY 3

ast_t *ast_make_reaction_def (ast_t * receiver, ast_t * identifier,
			      ast_t * signature, ast_t * body);

#define TYPE_IDENTIFIER 0
#define TYPE_TYPE_SPEC 1

ast_t *ast_make_type_def (ast_t * identifier, ast_t * type_spec);

/* Expr */

void ast_set_symbol (ast_t * ast, symbol_t * symbol);

symbol_t *ast_get_symbol (ast_t * ast);

#define CALL_EXPR 0
#define CALL_ARGS 1

ast_t *ast_make_call_expr (ast_t * expr, ast_t * args);
#define UNARY_CHILD 0
#define BINARY_LEFT_CHILD 0
#define BINARY_RIGHT_CHILD 1

ast_t *ast_make_dereference (ast_t * expr);

ast_t *ast_make_identifier_expr (ast_t * identifier);

ast_t *ast_make_logic_not (ast_t * child);

ast_t *ast_make_logic_and (ast_t * left, ast_t * right);

ast_t *ast_make_logic_or (ast_t * left, ast_t * right);

ast_t *ast_make_select (ast_t * expr, ast_t * identifier);

/* Stmt */

ast_t *ast_make_expr_stmt (ast_t * expr);

/* TODO:  Use a field instead. */
#define VAR_IDENTIFIER_LIST 0
#define VAR_TYPE_SPEC 1

ast_t *ast_make_var_stmt (ast_t * identifier_list, ast_t * type_spec);

ast_t *ast_make_assignment_stmt (ast_t * lvalue, ast_t * rvalue);

ast_t *ast_make_stmt_list (void);

ast_t *ast_make_return_stmt (ast_t * expr);

#define TRIGGER_EXPRESSION_LIST 0
#define TRIGGER_BODY 1

ast_t *ast_make_trigger_stmt (ast_t * expr_list, ast_t * stmt);

ast_t *ast_make_bind_list_stmt (void);

ast_t *ast_make_bind_stmt (ast_t * output, ast_t * input);

/* TypeSpec */

ast_t *ast_make_field_list (void);

#define IDENTIFIER_TYPE_SPEC_CHILD 0

ast_t *ast_make_identifier_type_spec (ast_t * identifier);

#define PORT_SIGNATURE 0

ast_t *ast_make_port (ast_t * signature);

#define COMPONENT_FIELD_LIST 0

ast_t *ast_make_component_type_spec (ast_t * field_list);

AstTypeSpecificationKind ast_type_specification_kind (const ast_t * node);

ast_t *ast_make_expression_list (void);

AstStatementKind ast_statement_kind (const ast_t * node);

AstBindStatementKind ast_bind_statement_kind (const ast_t * node);

AstExpressionKind ast_expression_kind (const ast_t * node);

ast_t *ast_make_untyped_literal (untyped_value_t value);

untyped_value_t ast_get_untyped_value (const ast_t * node);

ast_t *ast_make_typed_literal (typed_value_t value);

typed_value_t ast_get_typed_value (const ast_t* node);

bool ast_is_typed_literal (const ast_t * node);

bool ast_is_untyped_literal (const ast_t * node);

bool ast_is_literal (const ast_t * node);

bool ast_is_boolean (const ast_t * node);

void ast_set_type (ast_t * node, type_t * type, bool immutable,
		   bool derived_from_receiver);

type_t *ast_get_type (const ast_t * node);

type_t *ast_get_type2 (const ast_t * node);

bool ast_get_immutable (const ast_t * node);

bool ast_get_derived_from_receiver (const ast_t * node);

ast_t *ast_make_signature (void);

ast_t *ast_make_port_call (ast_t * identifier, ast_t * args);

action_t *ast_get_current_action (const ast_t * node);

type_t *ast_get_current_receiver_type (const ast_t * node);

#define AST_FOREACH(child, parent) size_t idx; size_t limit; ast_t* child; \
  for (idx = 0, limit = ast_children_count (parent), child = ((idx < ast_children_count (parent)) ? ast_get_child (parent, idx) : NULL); idx != limit; ++idx, child = (idx < (ast_children_count (parent)) ? ast_get_child (parent, idx) : NULL))

#define AST_FOREACH_PTR(child, parent) ast_t** child; ast_t** limit; \
  for (child = ast_children_begin (parent), limit = ast_children_end (parent); child != limit; child = ast_next_child (child))

#endif /* ast_h */
