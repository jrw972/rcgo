#ifndef node_h
#define node_h

#include "strtab.h"
#include "semval.h"

typedef struct node_t node_t;

typedef enum {
  IdentifierExpr,
  ImplicitDereferenceExpr,
  LogicAndExpr,
  LogicNotExpr,
  LogicOrExpr,
} ExprNodeType;

typedef enum {
  AssignmentStmt,
  ExprStmt,
  ListStmt,
  PrintStmt,
  VarStmt,
} StmtNodeType;

typedef enum {
  Identifier,
  IdentifierList
} DeclNodeType;

typedef enum {
  IdentifierTypeSpec,
} TypeSpecNodeType;

typedef enum
{
  Decl,
  Expr,
  Stmt,
  TypeSpec,
} NodeType;

void node_print (const node_t * node, size_t indent);

node_t *node_make_identifier (string_t identifier);

string_t node_identifier (const node_t * node);

node_t *node_make_identifier_expr (string_t identifier);

string_t node_identifier_expr_identifier (const node_t * node);

node_t *node_make_implicit_dereference (node_t * child);

node_t *node_make_logic_not (node_t * child);

node_t *node_make_logic_and (node_t * left, node_t * right);

node_t *node_make_logic_or (node_t * left, node_t * right);

node_t *node_make_identifier_list (void);

node_t *node_make_expr_stmt (node_t * expr);

node_t *node_make_print_stmt (node_t * expr);

node_t *node_make_var_stmt (node_t * identifier_list, node_t * type_spec);

node_t *node_make_assignment_stmt (node_t * lvalue, node_t * rvalue);

node_t *node_make_list_stmt (void);

node_t *node_make_identifier_type_spec (string_t identifier);

string_t node_identifier_type_spec_identifier (const node_t* node);

void node_free (node_t * node);

ExprNodeType node_expr_type (const node_t * node);

StmtNodeType node_stmt_type (const node_t * node);

TypeSpecNodeType node_type_spec_type (const node_t * node);

node_t *node_add_child (node_t * parent, node_t * child);

node_t *node_child (node_t * node);

node_t *node_sibling (node_t * node);

semval_t node_set_semval (node_t * node, semval_t semval);

semval_t node_get_semval (node_t * node);

void node_set_type (node_t * node, const type_t* type);

const type_t* node_get_type (node_t* node);

#endif /* node_h */
