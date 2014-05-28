#include "node.h"
#include <stdlib.h>
#include <string.h>
#include <strtab.h>
#include "debug.h"

struct node_t
{
  NodeType type;
  node_t *child;
  node_t *sibling;
  union
  {
    struct {
      DeclNodeType type;
      string_t identifier;
      ReceiverType receiver_type;
      const type_t* the_type;
    } decl;
    struct {
      DefNodeType type;
      size_t action_number;
    } def;
    struct {
      ExprNodeType type;
      string_t identifier;
      semval_t semval;
    } expr;
    struct {
      StmtNodeType type;
    } stmt;
    struct {
      TypeSpecNodeType type;
      string_t identifier;
      const type_t* the_type;
      FieldListType field_list_type;
    } type_spec;
  };
};

static node_t *
make (NodeType type)
{
  node_t *n = malloc (sizeof (node_t));
  memset (n, 0, sizeof (node_t));
  n->type = type;
  return n;
}

static void
add_sibling (node_t * c1, node_t * c2)
{
  if (c1->sibling == NULL)
    {
      c1->sibling = c2;
    }
  else
    {
      add_sibling (c1->sibling, c2);
    }
}

node_t *
node_add_child (node_t * parent, node_t * child)
{
  if (parent->child == NULL)
    {
      parent->child = child;
    }
  else
    {
      add_sibling (parent->child, child);
    }
  return parent;
}

static node_t *
make1 (NodeType type, node_t * child)
{
  node_t *n = make (type);
  return node_add_child (n, child);
}

static node_t *
make2 (NodeType type, node_t * child1, node_t * child2)
{
  node_t *n = make (type);
  node_add_child (n, child1);
  return node_add_child (n, child2);
}

static void
decl_print (const node_t* node)
{
  switch (node->decl.type) {
  case Field:
    printf ("Field");
    break;
  case Identifier:
    printf ("Identifier: %s", get (node_identifier (node)));
    break;
  case IdentifierList:
    printf ("IdentifierList");
    break;
  case Receiver:
    printf ("Receiver");
    break;
  }
}

static void
def_print (const node_t* node)
{
  switch (node_def_type (node)) {
  case ActionDef:
    printf ("ActionDef");
    break;
  case InstanceDef:
    printf ("InstanceDef");
    break;
  case ListDef:
    printf ("ListDef");
    break;
  case TypeDef:
    printf ("TypeDef");
    break;
  }
}

static void
expr_print (const node_t* node)
{
  switch (node_expr_type (node)) {
  case ExplicitDereferenceExpr:
    printf ("ExplicitDereferenceExpr");
    break;
  case IdentifierExpr:
    printf ("IdentifierExpr");
    break;
  case ImplicitDereferenceExpr:
    printf ("ImplicitDereferenceExpr");
    break;
  case LogicAndExpr:
    printf ("LogicAndExpr");
    break;
  case LogicNotExpr:
    printf ("LogicNotExpr");
    break;
  case LogicOrExpr:
    printf ("LogicOrExpr");
    break;
  case SelectExpr:
    printf ("SelectExpr");
    break;
  }
}

static void
stmt_print (const node_t* node)
{
  switch (node_stmt_type (node)) {
  case AssignmentStmt:
    printf ("AssignmentStmt");
    break;
  case ExprStmt:
    printf ("ExprStmt");
    break;
  case ListStmt:
    printf ("ListStmt");
    break;
  case PrintStmt:
    printf ("PrintStmt");
    break;
  case TriggerStmt:
    printf ("TriggerStmt");
    break;
  case VarStmt:
    printf ("VarStmt");
    break;
  }
}

static void
type_spec_print (const node_t* node)
{
  switch (node_type_spec_type (node)) {
  case FieldListTypeSpec:
    printf ("FieldListTypeSpec");
    break;
  case IdentifierTypeSpec:
    printf ("IdentifierTypeSpec");
    break;
  }
}

void
node_print (const node_t * node, size_t indent)
{
  size_t i;
  for (i = 0; i != indent; ++i)
    {
      printf (" ");
    }

  switch (node->type)
    {
    case Decl:
      decl_print (node);
      break;
    case Def:
      def_print (node);
      break;
    case Expr:
      expr_print (node);
      break;
    case Stmt:
      stmt_print (node);
      break;
    case TypeSpec:
      type_spec_print (node);
      break;
    }

  printf ("\n");

  node_t *child;
  for (child = node->child; child != NULL; child = node_sibling (child))
    {
      node_print (child, indent + 2);
    }
}

static node_t *
make_decl0 (DeclNodeType type)
{
  node_t* retval = make (Decl);
  retval->decl.type = type;
  return retval;
}

static node_t *
make_decl1 (DeclNodeType type,
            node_t * child1)
{
  return node_add_child (make_decl0 (type), child1);
}

static node_t *
make_decl2 (DeclNodeType type,
            node_t * child1,
            node_t* child2)
{
  return node_add_child (make_decl1 (type, child1), child2);
}

node_t *
node_make_identifier (char *id)
{
  node_t *n = make_decl0 (Identifier);
  n->decl.identifier = id;
  return n;
}

string_t
node_identifier (const node_t * node)
{
  assert (node->type == Decl);
  assert (node->decl.type == Identifier);
  return node->decl.identifier;
}

node_t *
node_make_identifier_list (void)
{
  return make_decl0 (IdentifierList);
}

node_t * node_make_field (node_t * identifier_list, node_t * type_spec)
{
  return make_decl2 (Field, identifier_list, type_spec);
}

static node_t *
make_expr0 (ExprNodeType type)
{
  node_t* retval = make (Expr);
  retval->expr.type = type;
  retval->expr.semval = semval_undefined ();
  return retval;
}

static node_t *
make_expr1 (ExprNodeType type,
            node_t * child)
{
  return node_add_child (make_expr0 (type), child);
}

static node_t *
make_expr2 (ExprNodeType type,
            node_t * child1,
            node_t * child2)
{
  return node_add_child (make_expr1 (type, child1), child2);
}

node_t *node_make_identifier_expr (string_t identifier)
{
  node_t* retval = make_expr0 (IdentifierExpr);
  retval->expr.identifier = identifier;
  return retval;
}

string_t node_identifier_expr_identifier (const node_t * node)
{
  assert (node->type == Expr);
  assert (node->expr.type == IdentifierExpr);
  return node->expr.identifier;
}

node_t *
node_make_implicit_dereference (node_t * child)
{
  return make_expr1 (ImplicitDereferenceExpr, child);
}

node_t *
node_make_logic_not (node_t * child)
{
  return make_expr1 (LogicNotExpr, child);
}

node_t *
node_make_logic_and (node_t * left, node_t * right)
{
  return make_expr2 (LogicAndExpr, left, right);
}

node_t *
node_make_logic_or (node_t * left, node_t * right)
{
  return make_expr2 (LogicOrExpr, left, right);
}

static node_t *
make_stmt0 (StmtNodeType type)
{
  node_t* retval = make (Stmt);
  retval->stmt.type = type;
  return retval;
}

static node_t *
make_stmt1 (StmtNodeType type,
            node_t * child)
{
  node_t* retval = make1 (Stmt, child);
  retval->stmt.type = type;
  return retval;
}

static node_t *
make_stmt2 (StmtNodeType type,
            node_t * child1,
            node_t * child2)
{
  node_t* retval = make2 (Stmt, child1, child2);
  retval->stmt.type = type;
  return retval;
}

node_t *
node_make_expr_stmt (node_t * expr)
{
  return make_stmt1 (ExprStmt, expr);
}

node_t *
node_make_print_stmt (node_t * expr)
{
  return make_stmt1 (PrintStmt, expr);
}

node_t *
node_make_var_stmt (node_t * identifier_list, node_t * type_spec)
{
  return make_stmt2 (VarStmt, identifier_list, type_spec);
}

node_t *
node_make_assignment_stmt (node_t * lvalue, node_t * rvalue)
{
  return make_stmt2 (AssignmentStmt, lvalue, rvalue);
}

node_t *node_make_list_stmt (void)
{
  return make_stmt0 (ListStmt);
}

static node_t *
make_type_spec0 (TypeSpecNodeType type)
{
  node_t* retval = make (TypeSpec);
  retval->type_spec.type = type;
  return retval;
}

node_t *node_make_identifier_type_spec (string_t identifier)
{
  node_t* retval = make_type_spec0 (IdentifierTypeSpec);
  retval->type_spec.identifier = identifier;
  return retval;
}

string_t node_identifier_type_spec_identifier (const node_t* node)
{
  assert (node->type == TypeSpec);
  assert (node->type_spec.type == IdentifierTypeSpec);
  return node->type_spec.identifier;
}

node_t * node_make_field_list (void)
{
  return make_type_spec0 (FieldListTypeSpec);
}

node_t * node_set_field_list_type (node_t* field_list, FieldListType type)
{
  assert (field_list->type == TypeSpec);
  assert (field_list->type_spec.type == FieldListTypeSpec);
  field_list->type_spec.field_list_type = type;
  return field_list;
}

FieldListType node_get_field_list_type (node_t* field_list)
{
  assert (field_list->type == TypeSpec);
  assert (field_list->type_spec.type == FieldListTypeSpec);
  return field_list->type_spec.field_list_type;
}

static node_t *
make_def0 (DefNodeType type)
{
  node_t* retval = make (Def);
  retval->def.type = type;
  return retval;
}

static node_t *
make_def1 (DefNodeType type,
            node_t * child1)
{
  return node_add_child (make_def0 (type), child1);
}

static node_t *
make_def2 (DefNodeType type,
            node_t * child1,
            node_t* child2)
{
  return node_add_child (make_def1 (type, child1), child2);
}

static node_t *
make_def3 (DefNodeType type,
           node_t * child1,
           node_t* child2,
           node_t* child3)
{
  return node_add_child (make_def2 (type, child1, child2), child3);
}

node_t *
node_make_list_def (void)
{
  return make_def0 (ListDef);
}

node_t * node_make_type_def (node_t * identifier, node_t * type_spec)
{
  return make_def2 (TypeDef, identifier, type_spec);
}

void
node_free (node_t * node)
{
  if (node != NULL)
    {
      node_free (node->child);
      node_free (node->sibling);
    }
}

ExprNodeType node_expr_type (const node_t * node)
{
  assert (node->type == Expr);
  return node->expr.type;
}

StmtNodeType node_stmt_type (const node_t * node)
{
  assert (node->type == Stmt);
  return node->stmt.type;
}

TypeSpecNodeType node_type_spec_type (const node_t * node)
{
  assert (node->type == TypeSpec);
  return node->type_spec.type;
}

node_t *
node_child (node_t * node)
{
  return node->child;
}

node_t *
node_sibling (node_t * node)
{
  return node->sibling;
}

semval_t
node_set_semval (node_t * node, semval_t semval)
{
  assert (node->type == Expr);
  node->expr.semval = semval;
  return semval;
}

semval_t
node_get_semval (node_t * node)
{
  assert (node->type == Expr);
  return node->expr.semval;
}

void node_set_type (node_t * node, const type_t* type)
{
  assert (node->type == TypeSpec);
  node->type_spec.the_type = type;
}

const type_t* node_get_type (node_t* node)
{
  assert (node->type == TypeSpec);
  return node->type_spec.the_type;
}

DefNodeType node_def_type (const node_t* node)
{
  assert (node->type == Def);
  return node->def.type;
}

node_t* node_make_receiver (ReceiverType type,
                            node_t* this_identifier,
                            node_t* type_identifier)
{
  node_t * retval = make_decl2 (Receiver, this_identifier, type_identifier);
  retval->decl.receiver_type = type;
  return retval;
}

void node_receiver_set_type (node_t* node,
                             const type_t* type)
{
  assert (node->type == Decl);
  assert (node->decl.type == Receiver);
  node->decl.the_type = type;
}

const type_t* node_receiver_get_type (const node_t* node)
{
  assert (node->type == Decl);
  assert (node->decl.type == Receiver);
  return node->decl.the_type;
}

ReceiverType node_receiver_type (node_t* node)
{
  assert (node->type == Decl);
  assert (node->decl.type == Receiver);
  return node->decl.receiver_type;
}

node_t* node_make_action_def (node_t* receiver,
                              node_t* precondition,
                              node_t* body)
{
  return make_def3 (ActionDef, receiver, precondition, body);
}

void node_set_action_number (node_t* node,
                             size_t x)
{
  assert (node->type == Def);
  assert (node->def.type == ActionDef);
  node->def.action_number = x;
}

size_t node_get_action_number (const node_t* node)
{
  assert (node->type == Def);
  assert (node->def.type == ActionDef);
  return node->def.action_number;
}

node_t * node_make_instance_def (node_t* instance_id,
                                 node_t* type_id)
{
  return make_def2 (InstanceDef, instance_id, type_id);
}

node_t* node_make_trigger_stmt (node_t* stmt)
{
  return make_stmt1 (TriggerStmt, stmt);
}

node_t* node_make_select (node_t* expr,
                          string_t identifier)
{
  node_t* retval = make_expr1 (SelectExpr, expr);
  retval->expr.identifier = identifier;
  return retval;
}

string_t node_get_select_identifier (node_t* node)
{
  assert (node->type == Expr);
  assert (node->expr.type == SelectExpr);
  return node->expr.identifier;
}

node_t* node_make_explicit_dereference (node_t* expr)
{
  return make_expr1 (ExplicitDereferenceExpr, expr);
}
