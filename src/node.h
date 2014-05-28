#ifndef node_h
#define node_h

#include "strtab.h"
#include "semval.h"

typedef struct node_t node_t;

typedef enum {
  Field,
  Identifier,
  IdentifierList,
  Receiver
} DeclNodeType;

typedef enum {
  ActionDef,
  InstanceDef,
  ListDef,
  TypeDef,
} DefNodeType;

typedef enum {
  ExplicitDereferenceExpr,
  IdentifierExpr,
  ImplicitDereferenceExpr,
  LogicAndExpr,
  LogicNotExpr,
  LogicOrExpr,
  SelectExpr,
} ExprNodeType;

typedef enum {
  AssignmentStmt,
  ExprStmt,
  ListStmt,
  PrintStmt,
  TriggerStmt,
  VarStmt,
} StmtNodeType;

typedef enum {
  FieldListTypeSpec,
  IdentifierTypeSpec,
} TypeSpecNodeType;

typedef enum {
  Component,
} FieldListType;

typedef enum
{
  Decl,
  Def,
  Expr,
  Stmt,
  TypeSpec,
} NodeType;

typedef enum {
  PointerToImmutable
} ReceiverType;

/* Generic functions. */

void node_print (const node_t * node, size_t indent);

void node_free (node_t * node);

node_t *node_add_child (node_t * parent, node_t * child);

node_t *node_child (node_t * node);

node_t *node_sibling (node_t * node);

/* Decl */

node_t * node_make_field (node_t * identifier_list, node_t * type_spec);

node_t *node_make_identifier (string_t identifier);

string_t node_identifier (const node_t * node);

node_t *node_make_identifier_list (void);

node_t* node_make_receiver (ReceiverType type,
                            node_t* this_identifier,
                            node_t* type_identifier);

ReceiverType node_receiver_type (node_t* node);

void node_receiver_set_type (node_t* node,
                             const type_t* type);

const type_t* node_receiver_get_type (const node_t* node);

/* Def */

DefNodeType node_def_type (const node_t* node);

node_t* node_make_action_def (node_t* receiver,
                              node_t* precondition,
                              node_t* body);

void node_set_action_number (node_t* node,
                             size_t x);

size_t node_get_action_number (const node_t* node);

node_t * node_make_instance_def (node_t* instance_id,
                                 node_t* type_id);

node_t * node_make_list_def (void);

node_t * node_make_type_def (node_t * identifier, node_t * type_spec);

/* Expr */

ExprNodeType node_expr_type (const node_t * node);

semval_t node_set_semval (node_t * node, semval_t semval);

semval_t node_get_semval (node_t * node);

node_t* node_make_explicit_dereference (node_t* expr);

node_t *node_make_identifier_expr (string_t identifier);

string_t node_identifier_expr_identifier (const node_t * node);

node_t *node_make_implicit_dereference (node_t * child);

node_t *node_make_logic_not (node_t * child);

node_t *node_make_logic_and (node_t * left, node_t * right);

node_t *node_make_logic_or (node_t * left, node_t * right);

node_t* node_make_select (node_t* expr,
                          string_t identifier);

string_t node_get_select_identifier (node_t* node);

/* Stmt */

StmtNodeType node_stmt_type (const node_t * node);

node_t *node_make_expr_stmt (node_t * expr);

node_t *node_make_print_stmt (node_t * expr);

/* TODO:  Use a field instead. */
node_t *node_make_var_stmt (node_t * identifier_list, node_t * type_spec);

node_t *node_make_assignment_stmt (node_t * lvalue, node_t * rvalue);

node_t *node_make_list_stmt (void);

node_t* node_make_trigger_stmt (node_t* stmt);

/* TypeSpec */

TypeSpecNodeType node_type_spec_type (const node_t * node);

void node_set_type (node_t * node, const type_t* type);

const type_t* node_get_type (node_t* node);

node_t * node_make_field_list (void);

node_t * node_set_field_list_type (node_t* field_list, FieldListType type);

FieldListType node_get_field_list_type (node_t* node);

node_t *node_make_identifier_type_spec (string_t identifier);

string_t node_identifier_type_spec_identifier (const node_t* node);

#define NODE_FOREACH(child, parent) node_t* child;                      \
  for (child = node_child (parent); child != NULL; child = node_sibling (child))

#endif /* node_h */
