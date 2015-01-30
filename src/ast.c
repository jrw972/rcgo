#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <strtab.h>
#include "debug.h"
#include "util.h"
#include "type.h"

typedef struct {
  void (*accept) (ast_t* ast, ast_visitor_t* visitor, void* data);
  void (*const_accept) (const ast_t* ast, ast_const_visitor_t* visitor, void* data);
  void (*set_type) (ast_t * node, typed_value_t typed_value, bool derived_from_receiver);
  bool (*get_derived_from_receiver) (const ast_t * node);
} vtable_t;

struct ast_t
{
  const vtable_t* vtable;
  AstKind kind;
    VECTOR_DECL (children, ast_t *);
  const char *file;
  unsigned int line;
  symtab_t *symtab;
  symbol_t *symbol;
  union
  {
    struct
    {
      string_t identifier;
    } identifier;
    struct
    {
      AstTypeSpecificationKind kind;
    } type_specification;
    struct
    {
      typed_value_t typed_value;
      bool derived_from_receiver;
    } expression;
    struct
    {
      AstReceiverKind kind;
    } receiver;
    struct
    {
      AstStatementKind kind;
    } statement;
    struct
    {
      AstBindStatementKind kind;
    } bind_statement;
  };
};

static void no_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  unimplemented;
}

void ast_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  ast->vtable->accept (ast, visitor, data);
}

void ast_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  ast->vtable->const_accept (ast, visitor, data);
}

static void set_type (ast_t * node, typed_value_t typed_value, bool derived_from_receiver)
{
  node->expression.typed_value = typed_value;
  node->expression.derived_from_receiver = derived_from_receiver;
}

static bool get_derived_from_receiver (const ast_t * node)
{
  return node->expression.derived_from_receiver;
}

static void print_visitor_visit_expression (void* data, const ast_t* node)
{
  size_t indent = *(size_t*)data;

  size_t i;
  for (i = 0; i != indent; ++i)
    {
      printf (" ");
    }

  printf ("AstExpression ");
  /* switch (node->expression.kind) */
  /*   { */
  /*   case AstAddressOfExpr: */
  /*     printf ("AstAddressOfExpr "); */
  /*     break; */
  /*   case AstCallExpr: */
  /*     printf ("AstCallExpr "); */
  /*     break; */
  /*   case AstDereferenceExpr: */
  /*     printf ("AstDeferenceExpr "); */
  /*     break; */
  /*   case AstEqualExpr: */
  /*     printf ("AstEqualExpr "); */
  /*     break; */
  /*   case AstExprList: */
  /*     printf ("AstExprList "); */
  /*     break; */
  /*   case AstIdentifierExpr: */
  /*     printf ("AstIdentifierExpr "); */
  /*     break; */
  /*   case AstLogicAndExpr: */
  /*     unimplemented; */
  /*   case AstLogicNotExpr: */
  /*     printf ("AstLogicNotExpr "); */
  /*     break; */
  /*   case AstLogicOrExpr: */
  /*     unimplemented; */
  /*   case AstMakeHeapExpr: */
  /*     unimplemented; */

  /*   case AstNewExpr: */
  /*     printf ("AstNewExpr "); */
  /*     break; */
  /*   case AstNotEqualExpr: */
  /*     printf ("AstNotEqualExpr "); */
  /*     break; */
  /*   case AstPortCallExpr: */
  /*     printf ("AstPortCallExpr "); */
  /*     break; */
  /*   case AstReleaseHeapExpr: */
  /*     unimplemented; */
  /*   case AstSelectExpr: */
  /*     printf ("AstSelectExpr "); */
  /*     break; */
  /*   case AstTakeHeapExpr: */
  /*     unimplemented; */
  /*   case AstTypedLiteral: */
  /*     printf ("AstTypedLiteral "); */
  /*     break; */
  /*   } */
  if (node->expression.typed_value.type != NULL)
    {
      printf ("type=%s has_value=%d derived_from_receiver=%d", type_to_string (node->expression.typed_value.type), node->expression.typed_value.has_value, node->expression.derived_from_receiver);
    }
  printf ("\n");
  AST_FOREACH (child, node)
  {
    ast_print (child, indent + 2);
  }
}

static void print_indent (size_t indent)
{
  size_t i;
  for (i = 0; i != indent; ++i)
    {
      printf (" ");
    }
}

static void print_identifier (void* data, const ast_t* node)
{
  size_t indent = *(size_t*)data;
  print_indent (indent);
  printf ("AstIdentifier: %s\n", get (node->identifier.identifier));
  AST_FOREACH (child, node)
  {
    ast_print (child, indent + 2);
  }
}

static void print_type_specification (void* data, const ast_t* node)
{
  size_t indent = *(size_t*)data;
  print_indent (indent);

  printf ("AstTypeSpecification ");
  switch (node->type_specification.kind)
    {
    case AstComponent:
      printf ("AstComponent");
      break;
    case AstEmptyTypeSpec:
      printf ("AstEmptyTypeSpec");
      break;
    case AstFieldList:
      printf ("AstFieldList");
      break;
    case AstForeign:
      printf ("AstForeign");
      break;
    case AstHeap:
      printf ("AstHeap");
      break;
    case AstIdentifierListTypeSpec:
      printf ("AstIdentifierListTypeSpec");
      break;
    case AstIdentifierTypeSpec:
      printf ("AstIdentifierTypeSpec");
      break;
    case AstPointer:
      printf ("AstPointer");
      break;
    case AstPointerToForeign:
      printf ("AstPointerToForeign");
      break;
    case AstPointerToImmutable:
      printf ("AstPointerToImmutable");
      break;
    case AstPort:
      printf ("AstPort");
      break;
    case AstSignature:
      printf ("AstSignature");
      break;
    case AstStruct:
      printf ("AstStruct");
      break;
    }
  AST_FOREACH (child, node)
  {
    ast_print (child, indent + 2);
  }
}

#define PRINT_EXPR(name, str)                   \
static void name (void* data, const ast_t* node) \
{ \
  size_t indent = *(size_t*)data; \
  print_indent (indent); \
  printf (str); \
  if (node->expression.typed_value.type != NULL) \
    { \
      printf ("type=%s has_value=%d derived_from_receiver=%d", type_to_string (node->expression.typed_value.type), node->expression.typed_value.has_value, node->expression.derived_from_receiver); \
    } \
  printf ("\n"); \
  AST_FOREACH (child, node) \
  { \
    ast_print (child, indent + 2); \
  } \
}

PRINT_EXPR(print_dereference_expr, "AstDereferenceExpr")
PRINT_EXPR(print_identifier_expr, "AstIdentifierExpr")
PRINT_EXPR(print_select_expr, "AstSelectExpr");

static void print_statement (void* data, const ast_t* node)
{
  size_t indent = *(size_t*)data; \
  print_indent (indent);
  printf ("AstStatement ");
  switch (node->statement.kind)
    {
    case AstAddAssignStmt:
      printf ("AstAddAssignStmt");
      break;
    case AstAssignmentStmt:
      printf ("AstAssignmentStmt");
      break;
    case AstChangeStmt:
      printf ("AstChangeStmt");
      break;
    case AstExpressionStmt:
      printf ("AstExpressionStmt");
      break;
    case AstIfStmt:
      printf ("AstIfStmt");
      break;
    case AstPrintlnStmt:
      printf ("AstPrintlnStmt");
      break;
    case AstReturnStmt:
      printf ("AstReturnStmt");
      break;
    case AstStmtList:
      printf ("AstStmtList");
      break;
    case AstSubtractAssignStmt:
      printf ("AstSubtractAssignStmt");
      break;
    case AstTriggerStmt:
      printf ("AstTriggerStmt");
      break;
    case AstVarStmt:
      printf ("AstVarStmt");
      break;
    }
  printf ("\n");
  AST_FOREACH (child, node)
    {
      ast_print (child, indent + 2);
    }
}

static void print_bind_statement (void* data, const ast_t* node)
{
  size_t indent = *(size_t*)data;
  print_indent (indent);
  printf ("AstBindStatement ");
  switch (node->bind_statement.kind)
    {
    case AstBindStmtList:
      printf ("AstBindStmtList");
      break;
    case AstBindStmt:
      printf ("AstBindStmt");
      break;
    }
  printf ("\n");
  AST_FOREACH (child, node)
  {
    ast_print (child, indent + 2);
  }
}

static void print_top_level_list (void* data, const ast_t* node)
{
  size_t indent = *(size_t*)data;
  print_indent (indent);
  printf ("AstTopLevelList\n");
  AST_FOREACH (child, node)
  {
    ast_print (child, indent + 2);
  }
}

static ast_const_visitor_t print_visitor = {
 visit_expression: print_visitor_visit_expression,

 visit_identifier: print_identifier,

 visit_type_specification: print_type_specification,

 visit_dereference_expr: print_dereference_expr,
 visit_identifier_expr: print_identifier_expr,
 visit_select_expr: print_select_expr,

 visit_statement: print_statement,
 visit_bind_statement: print_bind_statement,

 visit_top_level_list: print_top_level_list,
};

void
ast_print (const ast_t * node, size_t indent)
{
  ast_const_accept (node, &print_visitor, &indent);

  /* switch (node->kind) */
  /*   { */
  /*   case AstAction: */
  /*     printf ("AstAction"); */
  /*     break; */
  /*   case AstBind: */
  /*     printf ("AstBind"); */
  /*     break; */
  /*   case AstExpression: */
  /*     printf ("AstExpression "); */
  /*     switch (node->expression.kind) */
  /*       { */
  /*       case AstAddressOfExpr: */
  /*         printf ("AstAddressOfExpr "); */
  /*         break; */
  /*       case AstCallExpr: */
  /*         printf ("AstCallExpr "); */
  /*         break; */
  /*       case AstDereferenceExpr: */
  /*         printf ("AstDeferenceExpr "); */
  /*         break; */
  /*       case AstEqualExpr: */
  /*         printf ("AstEqualExpr "); */
  /*         break; */
  /*       case AstExprList: */
  /*         printf ("AstExprList "); */
  /*         break; */
  /*       case AstLogicAndExpr: */
  /*         unimplemented; */
  /*       case AstLogicNotExpr: */
  /*         printf ("AstLogicNotExpr "); */
  /*         break; */
  /*       case AstLogicOrExpr: */
  /*         unimplemented; */
  /*       case AstNewExpr: */
  /*         printf ("AstNewExpr "); */
  /*         break; */
  /*       case AstNotEqualExpr: */
  /*         printf ("AstNotEqualExpr "); */
  /*         break; */
  /*       case AstPortCallExpr: */
  /*         printf ("AstPortCallExpr "); */
  /*         break; */
  /*       case AstTypedLiteral: */
  /*         printf ("AstTypedLiteral "); */
  /*         break; */
  /*       } */
  /*     if (node->expression.typed_value.type != NULL) */
  /*       { */
  /*         printf ("type=%s has_value=%d derived_from_receiver=%d", type_to_string (node->expression.typed_value.type), node->expression.typed_value.has_value, node->expression.derived_from_receiver); */
  /*       } */
  /*     break; */
  /*   case AstMethod: */
  /*     printf ("AstMethod"); */
  /*     break; */
  /*   case AstIdentifierList: */
  /*     printf ("AstIdentifierList"); */
  /*     break; */
  /*   case AstInstance: */
  /*     printf ("AstInstance"); */
  /*     break; */
  /*   case AstReceiverDefinition: */
  /*     printf ("AstReceiverDefinition "); */
  /*     switch (node->receiver.kind) */
  /*       { */
  /*       case AstPointerReceiver: */
  /*         printf ("Pointer"); */
  /*         break; */
  /*       case AstPointerToImmutableReceiver: */
  /*         printf ("PointerToImmutable"); */
  /*         break; */
  /*       } */
  /*     break; */
  /*   case AstReaction: */
  /*     printf ("AstReaction"); */
  /*     break; */
  /*   case AstTypeDefinition: */
  /*     printf ("AstTypeDefinition"); */
  /*     break; */
  /*   } */
}

AstKind
ast_kind (const ast_t * ast)
{
  return ast->kind;
}

ast_t *ast_prepend_child (ast_t * parent, ast_t * child)
{
  VECTOR_PUSH_FRONT (parent->children, ast_t, child);
  return parent;
}

ast_t *
ast_append_child (ast_t * parent, ast_t * child)
{
  VECTOR_PUSH (parent->children, ast_t, child);
  return parent;
}

static ast_t *
make (unsigned int line, const vtable_t* vtable, AstKind kind, size_t children_count)
{
  assert (line != 0);
  ast_t *n = xmalloc (sizeof (ast_t));
  n->vtable = vtable;
  n->kind = kind;
  VECTOR_INIT (n->children, ast_t *, children_count, NULL);
  n->file = in_file;
  n->line = line;
  return n;
}

static void identifier_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_identifier (data, ast);
}

static void identifier_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_identifier (data, ast);
}

static vtable_t identifier_vtable = {
 accept: identifier_accept,
 const_accept: identifier_const_accept,
};

ast_t *
ast_make_identifier (unsigned int line, char *id)
{
  ast_t *n = make (line, &identifier_vtable, AstIdentifier, 0);
  n->identifier.identifier = id;
  return n;
}

string_t
ast_get_identifier (const ast_t * node)
{
  assert (node->kind == AstIdentifier);
  return node->identifier.identifier;
}

static vtable_t identifier_list_vtable = {
  no_accept,
};

ast_t *
ast_make_identifier_list (unsigned int line)
{
  return make (line, &identifier_list_vtable, AstIdentifierList, 0);
}

static void type_specification_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_type_specification (data, ast);
}

static void type_specification_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_type_specification (data, ast);
}

static vtable_t type_specification_vtable = {
 accept: type_specification_accept,
 const_accept: type_specification_const_accept,
};

static ast_t *
make_type_spec (unsigned int line, AstTypeSpecificationKind kind, size_t children_count)
{
  ast_t *retval = make (line, &type_specification_vtable, AstTypeSpecification, children_count);
  retval->type_specification.kind = kind;
  return retval;
}

ast_t *
ast_make_identifier_list_type_spec (unsigned int line,
                                    ast_t * identifier_list,
				    ast_t * type_spec)
{
  ast_t *node = make_type_spec (line, AstIdentifierListTypeSpec, 2);
  ast_set_child (node, IDENTIFIER_LIST, identifier_list);
  ast_set_child (node, TYPE_SPEC, type_spec);
  return node;
}

static ast_t *
make_expr (unsigned int line, vtable_t* vtable, size_t children_count)
{
  return make (line, vtable, AstExpression, children_count);
}

static void identifier_expr_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_identifier_expr (data, ast);
}

static void identifier_expr_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_identifier_expr (data, ast);
}

static vtable_t identifier_expr_vtable = {
 accept: identifier_expr_accept,
 const_accept: identifier_expr_const_accept,
 set_type: set_type,
 get_derived_from_receiver: get_derived_from_receiver
};

ast_t *
ast_make_identifier_expr (unsigned int line, ast_t * identifier)
{
  ast_t *retval = make_expr (line, &identifier_expr_vtable, 1);
  ast_set_child (retval, UNARY_CHILD, identifier);
  return retval;
}

static void logic_not_expr_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_logic_not_expr (data, ast);
}

static void logic_not_expr_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_logic_not_expr (data, ast);
}

static vtable_t logic_not_expr_vtable = {
 accept: logic_not_expr_accept,
 const_accept: logic_not_expr_const_accept,
 set_type: set_type,
 get_derived_from_receiver: get_derived_from_receiver
};

ast_t *
ast_make_logic_not (unsigned int line, ast_t * child)
{
  ast_t *retval = make_expr (line, &logic_not_expr_vtable, 1);
  ast_set_child (retval, UNARY_CHILD, child);
  return retval;
}

static vtable_t logic_and_expr_vtable;

ast_t *
ast_make_logic_and (unsigned int line, ast_t * left, ast_t * right)
{
  ast_t *retval = make_expr (line, &logic_and_expr_vtable, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, left);
  ast_set_child (retval, BINARY_RIGHT_CHILD, right);
  return retval;
}

static vtable_t logic_or_expr_vtable;

ast_t *
ast_make_logic_or (unsigned int line, ast_t * left, ast_t * right)
{
  ast_t *retval = make_expr (line, &logic_or_expr_vtable, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, left);
  ast_set_child (retval, BINARY_RIGHT_CHILD, right);
  return retval;
}

static void statement_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_statement (data, ast);
}

static void statement_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_statement (data, ast);
}

static vtable_t statement_vtable = {
 accept: statement_accept,
 const_accept: statement_const_accept,
};

static ast_t *
make_stmt (unsigned int line, AstStatementKind kind, size_t children_count)
{
  ast_t *retval = make (line, &statement_vtable, AstStatement, children_count);
  retval->statement.kind = kind;
  return retval;
}

ast_t* ast_make_change_stmt (unsigned int line,
                            ast_t * expr,
                            ast_t * identifier,
                            ast_t * type,
                            ast_t * body)
{
  ast_t * retval = make_stmt (line, AstChangeStmt, 4);
  ast_set_child (retval, CHANGE_EXPR, expr);
  ast_set_child (retval, CHANGE_IDENTIFIER, identifier);
  ast_set_child (retval, CHANGE_TYPE, type);
  ast_set_child (retval, CHANGE_BODY, body);
  return retval;
}

ast_t *
ast_make_expr_stmt (unsigned int line, ast_t * expr)
{
  ast_t *retval = make_stmt (line, AstExpressionStmt, 1);
  ast_set_child (retval, UNARY_CHILD, expr);
  return retval;
}

ast_t *
ast_make_var_stmt (unsigned int line, ast_t * identifier_list, ast_t * type_spec)
{
  ast_t *retval = make_stmt (line, AstVarStmt, 2);
  ast_set_child (retval, VAR_IDENTIFIER_LIST, identifier_list);
  ast_set_child (retval, VAR_TYPE_SPEC, type_spec);
  return retval;
}

ast_t *
ast_make_assignment_stmt (unsigned int line, ast_t * lvalue, ast_t * rvalue)
{
  ast_t *retval = make_stmt (line, AstAssignmentStmt, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, lvalue);
  ast_set_child (retval, BINARY_RIGHT_CHILD, rvalue);
  return retval;
}

ast_t *ast_make_add_assign_stmt (unsigned int line, ast_t * lvalue, ast_t * rvalue)
{
  ast_t *retval = make_stmt (line, AstAddAssignStmt, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, lvalue);
  ast_set_child (retval, BINARY_RIGHT_CHILD, rvalue);
  return retval;
}

ast_t *ast_make_subtract_assign_stmt (unsigned int line, ast_t * lvalue, ast_t * rvalue)
{
  ast_t *retval = make_stmt (line, AstSubtractAssignStmt, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, lvalue);
  ast_set_child (retval, BINARY_RIGHT_CHILD, rvalue);
  return retval;
}

ast_t *
ast_make_stmt_list (unsigned int line)
{
  return make_stmt (line, AstStmtList, 0);
}

static void bind_statement_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_bind_statement (data, ast);
}

static void bind_statement_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_bind_statement (data, ast);
}

static vtable_t bind_statement_vtable = {
 accept: bind_statement_accept,
 const_accept: bind_statement_const_accept,
};

static ast_t *
make_bind_stmt (unsigned int line, AstBindStatementKind kind, size_t children_count)
{
  ast_t *retval = make (line, &bind_statement_vtable, AstBindStatement, children_count);
  retval->bind_statement.kind = kind;
  return retval;
}

ast_t *ast_make_println_stmt (unsigned int line, ast_t * expr)
{
  ast_t *retval = make_stmt (line, AstPrintlnStmt, 1);
  ast_set_child (retval, UNARY_CHILD, expr);
  return retval;
}

ast_t *
ast_make_bind_list_stmt (unsigned int line)
{
  return make_bind_stmt (line, AstBindStmtList, 0);
}

ast_t *
ast_make_bind_stmt (unsigned int line, ast_t * output, ast_t * input)
{
  ast_t *retval = make_bind_stmt (line, AstBindStmt, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, output);
  ast_set_child (retval, BINARY_RIGHT_CHILD, input);
  return retval;
}

ast_t *
ast_make_identifier_type_spec (unsigned int line, ast_t * identifier)
{
  ast_t *retval = make_type_spec (line, AstIdentifierTypeSpec, 1);
  ast_set_child (retval, UNARY_CHILD, identifier);
  return retval;
}

ast_t *
ast_make_field_list (unsigned int line)
{
  return make_type_spec (line, AstFieldList, 0);
}

static void top_level_list_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_top_level_list (data, ast);
}

static void top_level_list_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_top_level_list (data, ast);
}

static vtable_t top_level_list_vtable = {
 accept: top_level_list_accept,
 const_accept: top_level_list_const_accept,
};

ast_t *
ast_make_top_level_list (void)
{
  return make (1, &top_level_list_vtable, AstTopLevelList, 0);
}

static vtable_t reaction_vtable = {
  no_accept,
};

ast_t *
ast_make_reaction_def (unsigned int line, ast_t * receiver, ast_t * identifier,
		       ast_t * signature, ast_t * body)
{
  ast_t *retval = make (line, &reaction_vtable, AstReaction, 4);
  ast_set_child (retval, REACTION_RECEIVER, receiver);
  ast_set_child (retval, REACTION_IDENTIFIER, identifier);
  ast_set_child (retval, REACTION_SIGNATURE, signature);
  ast_set_child (retval, REACTION_BODY, body);
  return retval;
}

static vtable_t type_definition_vtable = {
  no_accept,
};

ast_t *
ast_make_type_def (unsigned int line, ast_t * identifier, ast_t * type_spec)
{
  ast_t *retval = make (line, &type_definition_vtable, AstTypeDefinition, 2);
  ast_set_child (retval, TYPE_IDENTIFIER, identifier);
  ast_set_child (retval, TYPE_TYPE_SPEC, type_spec);
  return retval;
}

static vtable_t method_vtable = {
  no_accept,
};

ast_t *ast_make_method_def (unsigned int line, ast_t * receiver, ast_t * identifier,
                            ast_t * signature, ast_t * return_type,
                            ast_t* body)
{
  ast_t *retval = make (line, &method_vtable, AstMethod, 5);
  ast_set_child (retval, METHOD_RECEIVER, receiver);
  ast_set_child (retval, METHOD_IDENTIFIER, identifier);
  ast_set_child (retval, METHOD_SIGNATURE, signature);
  ast_set_child (retval, METHOD_RETURN_TYPE, return_type);
  ast_set_child (retval, METHOD_BODY, body);
  return retval;
}

AstTypeSpecificationKind
ast_type_spec_type (const ast_t * node)
{
  assert (node->kind == AstTypeSpecification);
  return node->type_specification.kind;
}

ast_t *
ast_get_child (const ast_t * node, size_t idx)
{
  assert (idx < VECTOR_SIZE (node->children));
  return VECTOR_AT (node->children, idx);
}

ast_t **
ast_get_child_ptr (const ast_t * ast, size_t idx)
{
  assert (idx < VECTOR_SIZE (ast->children));
  return VECTOR_BEGIN (ast->children) + idx;
}

void
ast_set_child (ast_t * parent, size_t idx, ast_t * child)
{
  assert (idx < VECTOR_SIZE (parent->children));
  VECTOR_SET (parent->children, idx, child);
}

size_t
ast_children_count (const ast_t * node)
{
  return VECTOR_SIZE (node->children);
}

ast_t **
ast_children_begin (const ast_t * ast)
{
  return VECTOR_BEGIN (ast->children);
}

ast_t **
ast_children_end (const ast_t * ast)
{
  return VECTOR_END (ast->children);
}

ast_t **
ast_next_child (ast_t ** child)
{
  return child + 1;
}

const char *
ast_file (const ast_t * node)
{
  return node->file;
}

unsigned int
ast_line (const ast_t * node)
{
  return node->line;
}

void
ast_set_symtab (ast_t * node, symtab_t * symtab)
{
  assert (node->symtab == NULL);
  node->symtab = symtab;
}

symtab_t *
ast_get_symtab (const ast_t * node)
{
  return node->symtab;
}

void
ast_set_symbol (ast_t * node, symbol_t * symbol)
{
  node->symbol = symbol;
}

symbol_t *
ast_get_symbol (const ast_t * node)
{
  return node->symbol;
}

static vtable_t receiver_definition_vtable = {
  no_accept,
};

ast_t *
ast_make_receiver (unsigned int line, ast_t * this_identifier, ast_t * type_identifier, AstReceiverKind kind)
{
  ast_t *retval = make (line, &receiver_definition_vtable, AstReceiverDefinition, 2);
  retval->receiver.kind = kind;
  ast_set_child (retval, RECEIVER_THIS_IDENTIFIER, this_identifier);
  ast_set_child (retval, RECEIVER_TYPE_IDENTIFIER, type_identifier);
  return retval;
}

AstReceiverKind ast_receiver_kind (const ast_t* receiver)
{
  assert (receiver->kind == AstReceiverDefinition);
  return receiver->receiver.kind;
}

static vtable_t action_vtable = {
  no_accept,
};

ast_t *
ast_make_action_def (unsigned int line, ast_t * receiver, ast_t * precondition, ast_t * body)
{
  ast_t *retval = make (line, &action_vtable, AstAction, 3);
  ast_set_child (retval, ACTION_RECEIVER, receiver);
  ast_set_child (retval, ACTION_PRECONDITION, precondition);
  ast_set_child (retval, ACTION_BODY, body);
  return retval;
}

static vtable_t bind_vtable;

ast_t *
ast_make_bind_def (unsigned int line, ast_t * receiver, ast_t * list)
{
  ast_t *retval = make (line, &bind_vtable, AstBind, 2);
  ast_set_child (retval, BIND_RECEIVER, receiver);
  ast_set_child (retval, BIND_BODY, list);
  return retval;
}

static vtable_t instance_vtable = {
  no_accept,
};

ast_t *
ast_make_instance_def (unsigned int line, ast_t * instance_id, ast_t * type_id)
{
  ast_t *retval = make (line, &instance_vtable, AstInstance, 2);
  ast_set_child (retval, INSTANCE_IDENTIFIER, instance_id);
  ast_set_child (retval, INSTANCE_TYPE_IDENTIFIER, type_id);
  return retval;
}

ast_t *
ast_make_return_stmt (unsigned int line, ast_t * expr)
{
  ast_t *retval = make_stmt (line, AstReturnStmt, 1);
  ast_set_child (retval, UNARY_CHILD, expr);
  return retval;
}

ast_t *
ast_make_trigger_stmt (unsigned int line, ast_t * expr_list, ast_t * stmt)
{
  ast_t *retval = make_stmt (line, AstTriggerStmt, 2);
  ast_set_child (retval, TRIGGER_EXPRESSION_LIST, expr_list);
  ast_set_child (retval, TRIGGER_BODY, stmt);
  return retval;
}

static void select_expr_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_select_expr (data, ast);
}

static void select_expr_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_select_expr (data, ast);
}

static vtable_t select_expr_vtable = {
 accept: select_expr_accept,
 const_accept: select_expr_const_accept,
 set_type: set_type,
 get_derived_from_receiver: get_derived_from_receiver
};

ast_t *
ast_make_select (unsigned int line, ast_t * expr, ast_t * identifier)
{
  ast_t *retval = make_expr (line, &select_expr_vtable, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, expr);
  ast_set_child (retval, BINARY_RIGHT_CHILD, identifier);
  return retval;
}

static void address_of_expr_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_address_of_expr (data, ast);
}

static void address_of_expr_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_address_of_expr (data, ast);
}

static vtable_t address_of_expr_vtable = {
 accept: address_of_expr_accept,
 const_accept: address_of_expr_const_accept,
 set_type: set_type,
 get_derived_from_receiver: get_derived_from_receiver
};

ast_t *ast_make_address_of (unsigned int line, ast_t * expr)
{
  ast_t *retval = make_expr (line, &address_of_expr_vtable, 1);
  ast_set_child (retval, UNARY_CHILD, expr);
  return retval;

}

static void call_expr_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_call_expr (data, ast);
}

static void call_expr_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_call_expr (data, ast);
}

static vtable_t call_expr_vtable = {
 accept: call_expr_accept,
 const_accept: call_expr_const_accept,
 set_type: set_type,
 get_derived_from_receiver: get_derived_from_receiver
};

ast_t *
ast_make_call_expr (unsigned int line, ast_t * expr, ast_t * args)
{
  ast_t *retval = make_expr (line, &call_expr_vtable, 2);
  ast_set_child (retval, CALL_EXPR, expr);
  ast_set_child (retval, CALL_ARGS, args);
  return retval;
}

static void dereference_expr_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_dereference_expr (data, ast);
}

static void dereference_expr_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_dereference_expr (data, ast);
}

static vtable_t dereference_expr_vtable = {
 accept: dereference_expr_accept,
 const_accept: dereference_expr_const_accept,
 set_type: set_type,
 get_derived_from_receiver: get_derived_from_receiver
};

ast_t *
ast_make_dereference (unsigned int line, ast_t * expr)
{
  ast_t *retval = make_expr (line, &dereference_expr_vtable, 1);
  ast_set_child (retval, UNARY_CHILD, expr);
  return retval;
}

ast_t *
ast_make_port (unsigned int line, ast_t * signature)
{
  ast_t *retval = make_type_spec (line, AstPort, 1);
  ast_set_child (retval, PORT_SIGNATURE, signature);
  return retval;
}

ast_t *
ast_make_component_type_spec (unsigned int line, ast_t * field_list)
{
  ast_t *retval = make_type_spec (line, AstComponent, 1);
  ast_set_child (retval, COMPONENT_FIELD_LIST, field_list);
  return retval;
}

ast_t *
ast_make_struct_type_spec (unsigned int line, ast_t * field_list)
{
  ast_t *retval = make_type_spec (line, AstStruct, 1);
  ast_set_child (retval, STRUCT_FIELD_LIST, field_list);
  return retval;
}

ast_t *ast_make_empty_type_spec (unsigned int line)
{
  return make_type_spec (line, AstEmptyTypeSpec, 0);
}

ast_t *ast_make_heap_type_spec (unsigned int line, ast_t * type)
{
  ast_t * retval = make_type_spec (line, AstHeap, 1);
  ast_set_child (retval, HEAP_BASE_TYPE, type);
  return retval;
}

AstTypeSpecificationKind
ast_type_specification_kind (const ast_t * node)
{
  assert (node->kind == AstTypeSpecification);
  return node->type_specification.kind;
}

static void expr_list_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_expr_list (data, ast);
}

static void expr_list_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_expr_list (data, ast);
}

static vtable_t expr_list_vtable = {
 accept: expr_list_accept,
 const_accept: expr_list_const_accept,
};

ast_t *
ast_make_expression_list (unsigned int line)
{
  return make_expr (line, &expr_list_vtable, 0);
}

AstStatementKind
ast_statement_kind (const ast_t * node)
{
  assert (node->kind == AstStatement);
  return node->statement.kind;
}

AstBindStatementKind
ast_bind_statement_kind (const ast_t * node)
{
  assert (node->kind == AstBindStatement);
  return node->bind_statement.kind;
}

static void typed_literal_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_typed_literal (data, ast);
}

static void typed_literal_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_typed_literal (data, ast);
}

static vtable_t typed_literal_vtable = {
 accept: typed_literal_accept,
 const_accept: typed_literal_const_accept,
};

ast_t *
ast_make_typed_literal (unsigned int line, typed_value_t value)
{
  ast_t *node = make_expr (line, &typed_literal_vtable, 0);
  node->expression.typed_value = value;
  return node;
}

void ast_set_typed_value (ast_t* node, typed_value_t value)
{
  assert (node->kind == AstExpression);
  node->expression.typed_value = value;
}

typed_value_t ast_get_typed_value (const ast_t* node)
{
  assert (node->kind == AstExpression);
  return node->expression.typed_value;
}

void
ast_set_type (ast_t * node, typed_value_t typed_value, bool derived_from_receiver)
{
  node->vtable->set_type (node, typed_value, derived_from_receiver);
}

bool
ast_get_derived_from_receiver (const ast_t * node)
{
  return node->vtable->get_derived_from_receiver (node);
}


ast_t *
ast_make_signature (unsigned int line)
{
  return make_type_spec (line, AstSignature, 0);
}

static void port_call_expr_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_port_call_expr (data, ast);
}

static void port_call_expr_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_port_call_expr (data, ast);
}

static vtable_t port_call_expr_vtable = {
 accept: port_call_expr_accept,
 const_accept: port_call_expr_const_accept,
 set_type: set_type,
 get_derived_from_receiver: get_derived_from_receiver
};

ast_t *
ast_make_port_call (unsigned int line, ast_t * identifier, ast_t * args)
{
  ast_t *a = make_expr (line, &port_call_expr_vtable, 2);
  ast_set_child (a, CALL_EXPR, identifier);
  ast_set_child (a, CALL_ARGS, args);
  return a;
}

ast_t *ast_make_pointer_type_spec (unsigned int line, ast_t* type_spec)
{
  ast_t *retval = make_type_spec (line, AstPointer, 1);
  ast_set_child (retval, POINTER_BASE_TYPE, type_spec);
  return retval;
}

ast_t *ast_make_pointer_to_immutable_type_spec (unsigned int line, ast_t* type_spec)
{
  ast_t *retval = make_type_spec (line, AstPointerToImmutable, 1);
  ast_set_child (retval, POINTER_BASE_TYPE, type_spec);
  return retval;
}

ast_t *ast_make_pointer_to_foreign_type_spec (unsigned int line, ast_t* type_spec)
{
  ast_t *retval = make_type_spec (line, AstPointerToForeign, 1);
  ast_set_child (retval, POINTER_BASE_TYPE, type_spec);
  return retval;
}

ast_t *ast_make_foreign_type_spec (unsigned int line, ast_t* type_spec)
{
  ast_t *retval = make_type_spec (line, AstForeign, 1);
  ast_set_child (retval, FOREIGN_BASE_TYPE, type_spec);
  return retval;
}

static void equal_expr_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_equal_expr (data, ast);
}

static void equal_expr_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_equal_expr (data, ast);
}

static vtable_t equal_expr_vtable = {
 accept: equal_expr_accept,
 const_accept: equal_expr_const_accept,
 set_type: set_type,
 get_derived_from_receiver: get_derived_from_receiver
};

ast_t *ast_make_equal (unsigned int line, ast_t* left, ast_t* right)
{
  ast_t *retval = make_expr (line, &equal_expr_vtable, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, left);
  ast_set_child (retval, BINARY_RIGHT_CHILD, right);
  return retval;
}

static void not_equal_expr_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_not_equal_expr (data, ast);
}

static void not_equal_expr_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_not_equal_expr (data, ast);
}

static vtable_t not_equal_expr_vtable = {
 accept: not_equal_expr_accept,
 const_accept: not_equal_expr_const_accept,
 set_type: set_type,
 get_derived_from_receiver: get_derived_from_receiver
};

ast_t *ast_make_not_equal (unsigned int line, ast_t* left, ast_t* right)
{
  ast_t *retval = make_expr (line, &not_equal_expr_vtable, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, left);
  ast_set_child (retval, BINARY_RIGHT_CHILD, right);
  return retval;
}

ast_t *ast_make_if_stmt (unsigned int line, ast_t* condition, ast_t* true_branch)
{
  ast_t *retval = make_stmt (line, AstIfStmt, 2);
  ast_set_child (retval, IF_CONDITION, condition);
  ast_set_child (retval, IF_TRUE_BRANCH, true_branch);
  return retval;
}

static void new_expr_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_new_expr (data, ast);
}

static void new_expr_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_new_expr (data, ast);
}

static vtable_t new_expr_vtable = {
 accept: new_expr_accept,
 const_accept: new_expr_const_accept,
 set_type: set_type,
 get_derived_from_receiver: get_derived_from_receiver
};

ast_t *ast_make_new_expr (unsigned int line, ast_t* identifier)
{
  ast_t *retval = make_expr (line, &new_expr_vtable, 1);
  ast_set_child (retval, UNARY_CHILD, identifier);
  return retval;
}

static void move_expr_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_move_expr (data, ast);
}

static void move_expr_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_move_expr (data, ast);
}

static vtable_t move_expr_vtable = {
 accept: move_expr_accept,
 const_accept: move_expr_const_accept,
 set_type: set_type,
 get_derived_from_receiver: get_derived_from_receiver
};

ast_t *ast_make_move_expr (unsigned int line, ast_t* expr)
{
  ast_t *retval = make_expr (line, &move_expr_vtable, 1);
  ast_set_child (retval, UNARY_CHILD, expr);
  return retval;
}

static void merge_expr_accept (ast_t* ast, ast_visitor_t* visitor, void* data)
{
  visitor->visit_merge_expr (data, ast);
}

static void merge_expr_const_accept (const ast_t* ast, ast_const_visitor_t* visitor, void* data)
{
  visitor->visit_merge_expr (data, ast);
}

static vtable_t merge_expr_vtable = {
 accept: merge_expr_accept,
 const_accept: merge_expr_const_accept,
 set_type: set_type,
 get_derived_from_receiver: get_derived_from_receiver
};

ast_t *ast_make_merge_expr (unsigned int line, ast_t* expr)
{
  ast_t *retval = make_expr (line, &merge_expr_vtable, 1);
  ast_set_child (retval, UNARY_CHILD, expr);
  return retval;
}
