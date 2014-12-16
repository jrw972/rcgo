#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <strtab.h>
#include "debug.h"
#include "util.h"
#include "type.h"

struct ast_t
{
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
      AstExpressionKind kind;
      typed_value_t typed_value;
      bool derived_from_immutable;
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

void
ast_print (const ast_t * node, size_t indent)
{
  size_t i;
  for (i = 0; i != indent; ++i)
    {
      printf (" ");
    }

  switch (node->kind)
    {
    case AstAction:
      printf ("AstAction");
      break;
    case AstBind:
      unimplemented;
    case AstBindStatement:
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
      break;
    case AstExpression:
      printf ("AstExpression ");
      switch (node->expression.kind)
	{
        case AstAddressOfExpr:
          printf ("AstAddressOfExpr ");
          break;
	case AstCallExpr:
	  printf ("AstCallExpr ");
	  break;
	case AstDereferenceExpr:
	  printf ("AstDeferenceExpr ");
	  break;
        case AstEqualExpr:
          printf ("AstEqualExpr ");
          break;
	case AstExprList:
	  printf ("AstExprList ");
	  break;
	case AstIdentifierExpr:
	  printf ("AstIdentifierExpr ");
	  break;
	case AstLogicAndExpr:
	  unimplemented;
	case AstLogicNotExpr:
	  printf ("AstLogicNotExpr ");
	  break;
	case AstLogicOrExpr:
	  unimplemented;
        case AstNewExpr:
          printf ("AstNewExpr ");
          break;
        case AstNotEqualExpr:
          printf ("AstNotEqualExpr ");
          break;
	case AstSelectExpr:
	  printf ("AstSelectExpr ");
	  break;
	case AstPortCallExpr:
	  printf ("AstPortCallExpr ");
	  break;
	case AstTypedLiteral:
          printf ("AstTypedLiteral ");
          break;
	}
      if (node->expression.typed_value.type != NULL)
        {
          printf ("type=%s has_value=%d derived_from_immutable=%d derived_from_receiver=%d", type_to_string (node->expression.typed_value.type), node->expression.typed_value.has_value, node->expression.derived_from_immutable, node->expression.derived_from_receiver);
        }
      break;
    case AstMethod:
      printf ("AstMethod");
      break;
    case AstIdentifier:
      printf ("AstIdentifier: %s", get (node->identifier.identifier));
      break;
    case AstIdentifierList:
      printf ("AstIdentifierList");
      break;
    case AstInstance:
      printf ("AstInstance");
      break;
    case AstTopLevelList:
      printf ("AstTopLevelList");
      break;
    case AstReceiverDefinition:
      printf ("AstReceiverDefinition ");
      switch (node->receiver.kind)
        {
        case AstPointerReceiver:
          printf ("Pointer");
          break;
        case AstPointerToImmutableReceiver:
          printf ("PointerToImmutable");
          break;
        }
      break;
    case AstReaction:
      printf ("AstReaction");
      break;
    case AstStatement:
      printf ("AstStatement ");
      switch (node->statement.kind)
	{
        case AstAddAssignStmt:
          printf ("AstAddAssignStmt");
          break;
	case AstAssignmentStmt:
	  printf ("AstAssignmentStmt");
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
	case AstTriggerStmt:
	  printf ("AstTriggerStmt");
	  break;
	case AstVarStmt:
          printf ("AstVarStmt");
          break;
	}
      break;
    case AstTypeDefinition:
      printf ("AstTypeDefinition");
      break;
    case AstTypeSpecification:
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
	case AstIdentifierListTypeSpec:
	  printf ("AstIdentifierListTypeSpec");
	  break;
	case AstIdentifierTypeSpec:
	  printf ("AstIdentifierTypeSpec");
	  break;
        case AstPointer:
          printf ("AstPointer");
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
      break;
    }

  printf ("\n");

  AST_FOREACH (child, node)
  {
    ast_print (child, indent + 2);
  }
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
make (AstKind kind, size_t children_count)
{
  ast_t *n = xmalloc (sizeof (ast_t));
  n->kind = kind;
  VECTOR_INIT (n->children, ast_t *, children_count, NULL);
  n->file = in_file;
  n->line = in_line;
  return n;
}

ast_t *
ast_make_identifier (char *id)
{
  ast_t *n = make (AstIdentifier, 0);
  n->identifier.identifier = id;
  return n;
}

string_t
ast_get_identifier (const ast_t * node)
{
  assert (node->kind == AstIdentifier);
  return node->identifier.identifier;
}

ast_t *
ast_make_identifier_list (void)
{
  return make (AstIdentifierList, 0);
}

static ast_t *
make_type_spec (AstTypeSpecificationKind kind, size_t children_count)
{
  ast_t *retval = make (AstTypeSpecification, children_count);
  retval->type_specification.kind = kind;
  return retval;
}

ast_t *
ast_make_identifier_list_type_spec (ast_t * identifier_list,
				    ast_t * type_spec)
{
  ast_t *node = make_type_spec (AstIdentifierListTypeSpec, 2);
  ast_set_child (node, IDENTIFIER_LIST, identifier_list);
  ast_set_child (node, TYPE_SPEC, type_spec);
  return node;
}

static ast_t *
make_expr (AstExpressionKind kind, size_t children_count)
{
  ast_t *retval = make (AstExpression, children_count);
  retval->expression.kind = kind;
  return retval;
}

ast_t *
ast_make_identifier_expr (ast_t * identifier)
{
  ast_t *retval = make_expr (AstIdentifierExpr, 1);
  ast_set_child (retval, UNARY_CHILD, identifier);
  return retval;
}

ast_t *
ast_make_logic_not (ast_t * child)
{
  ast_t *retval = make_expr (AstLogicNotExpr, 1);
  ast_set_child (retval, UNARY_CHILD, child);
  return retval;
}

ast_t *
ast_make_logic_and (ast_t * left, ast_t * right)
{
  ast_t *retval = make_expr (AstLogicAndExpr, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, left);
  ast_set_child (retval, BINARY_RIGHT_CHILD, right);
  return retval;
}

ast_t *
ast_make_logic_or (ast_t * left, ast_t * right)
{
  ast_t *retval = make_expr (AstLogicOrExpr, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, left);
  ast_set_child (retval, BINARY_RIGHT_CHILD, right);
  return retval;
}

static ast_t *
make_stmt (AstStatementKind kind, size_t children_count)
{
  ast_t *retval = make (AstStatement, children_count);
  retval->statement.kind = kind;
  return retval;
}

ast_t *
ast_make_expr_stmt (ast_t * expr)
{
  ast_t *retval = make_stmt (AstExpressionStmt, 1);
  ast_set_child (retval, UNARY_CHILD, expr);
  return retval;
}

ast_t *
ast_make_var_stmt (ast_t * identifier_list, ast_t * type_spec)
{
  ast_t *retval = make_stmt (AstVarStmt, 2);
  ast_set_child (retval, VAR_IDENTIFIER_LIST, identifier_list);
  ast_set_child (retval, VAR_TYPE_SPEC, type_spec);
  return retval;
}

ast_t *
ast_make_assignment_stmt (ast_t * lvalue, ast_t * rvalue)
{
  ast_t *retval = make_stmt (AstAssignmentStmt, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, lvalue);
  ast_set_child (retval, BINARY_RIGHT_CHILD, rvalue);
  return retval;
}

ast_t *ast_make_add_assign_stmt (ast_t * lvalue, ast_t * rvalue)
{
  ast_t *retval = make_stmt (AstAddAssignStmt, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, lvalue);
  ast_set_child (retval, BINARY_RIGHT_CHILD, rvalue);
  return retval;
}

ast_t *
ast_make_stmt_list (void)
{
  return make_stmt (AstStmtList, 0);
}

static ast_t *
make_bind_stmt (AstBindStatementKind kind, size_t children_count)
{
  ast_t *retval = make (AstBindStatement, children_count);
  retval->bind_statement.kind = kind;
  return retval;
}

ast_t *ast_make_println_stmt (ast_t * expr)
{
  ast_t *retval = make_stmt (AstPrintlnStmt, 1);
  ast_set_child (retval, UNARY_CHILD, expr);
  return retval;
}

ast_t *
ast_make_bind_list_stmt (void)
{
  return make_bind_stmt (AstBindStmtList, 0);
}

ast_t *
ast_make_bind_stmt (ast_t * output, ast_t * input)
{
  ast_t *retval = make_bind_stmt (AstBindStmt, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, output);
  ast_set_child (retval, BINARY_RIGHT_CHILD, input);
  return retval;
}

ast_t *
ast_make_identifier_type_spec (ast_t * identifier)
{
  ast_t *retval = make_type_spec (AstIdentifierTypeSpec, 1);
  ast_set_child (retval, UNARY_CHILD, identifier);
  return retval;
}

ast_t *
ast_make_field_list (void)
{
  return make_type_spec (AstFieldList, 0);
}

ast_t *
ast_make_top_level_list (void)
{
  return make (AstTopLevelList, 0);
}

ast_t *
ast_make_reaction_def (ast_t * receiver, ast_t * identifier,
		       ast_t * signature, ast_t * body)
{
  ast_t *retval = make (AstReaction, 4);
  ast_set_child (retval, REACTION_RECEIVER, receiver);
  ast_set_child (retval, REACTION_IDENTIFIER, identifier);
  ast_set_child (retval, REACTION_SIGNATURE, signature);
  ast_set_child (retval, REACTION_BODY, body);
  return retval;
}

ast_t *
ast_make_type_def (ast_t * identifier, ast_t * type_spec)
{
  ast_t *retval = make (AstTypeDefinition, 2);
  ast_set_child (retval, TYPE_IDENTIFIER, identifier);
  ast_set_child (retval, TYPE_TYPE_SPEC, type_spec);
  return retval;
}

ast_t *ast_make_method_def (ast_t * receiver, ast_t * identifier,
                            ast_t * signature, ast_t * return_type,
                            ast_t* body)
{
  ast_t *retval = make (AstMethod, 5);
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
ast_get_symbol (ast_t * node)
{
  return node->symbol;
}

ast_t *
ast_make_receiver (ast_t * this_identifier, ast_t * type_identifier, AstReceiverKind kind)
{
  ast_t *retval = make (AstReceiverDefinition, 2);
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

ast_t *
ast_make_action_def (ast_t * receiver, ast_t * precondition, ast_t * body)
{
  ast_t *retval = make (AstAction, 3);
  ast_set_child (retval, ACTION_RECEIVER, receiver);
  ast_set_child (retval, ACTION_PRECONDITION, precondition);
  ast_set_child (retval, ACTION_BODY, body);
  return retval;
}

ast_t *
ast_make_bind_def (ast_t * receiver, ast_t * list)
{
  ast_t *retval = make (AstBind, 2);
  ast_set_child (retval, BIND_RECEIVER, receiver);
  ast_set_child (retval, BIND_BODY, list);
  return retval;
}

ast_t *
ast_make_instance_def (ast_t * instance_id, ast_t * type_id)
{
  ast_t *retval = make (AstInstance, 2);
  ast_set_child (retval, INSTANCE_IDENTIFIER, instance_id);
  ast_set_child (retval, INSTANCE_TYPE_IDENTIFIER, type_id);
  return retval;
}

ast_t *
ast_make_return_stmt (ast_t * expr)
{
  ast_t *retval = make_stmt (AstReturnStmt, 1);
  ast_set_child (retval, UNARY_CHILD, expr);
  return retval;
}

ast_t *
ast_make_trigger_stmt (ast_t * expr_list, ast_t * stmt)
{
  ast_t *retval = make_stmt (AstTriggerStmt, 2);
  ast_set_child (retval, TRIGGER_EXPRESSION_LIST, expr_list);
  ast_set_child (retval, TRIGGER_BODY, stmt);
  return retval;
}

ast_t *
ast_make_select (ast_t * expr, ast_t * identifier)
{
  ast_t *retval = make_expr (AstSelectExpr, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, expr);
  ast_set_child (retval, BINARY_RIGHT_CHILD, identifier);
  return retval;
}

ast_t *ast_make_address_of (ast_t * expr)
{
  ast_t *retval = make_expr (AstAddressOfExpr, 1);
  ast_set_child (retval, UNARY_CHILD, expr);
  return retval;

}

ast_t *
ast_make_call_expr (ast_t * expr, ast_t * args)
{
  ast_t *retval = make_expr (AstCallExpr, 2);
  ast_set_child (retval, CALL_EXPR, expr);
  ast_set_child (retval, CALL_ARGS, args);
  return retval;
}

ast_t *
ast_make_dereference (ast_t * expr)
{
  ast_t *retval = make_expr (AstDereferenceExpr, 1);
  ast_set_child (retval, UNARY_CHILD, expr);
  return retval;
}

ast_t *
ast_make_port (ast_t * signature)
{
  ast_t *retval = make_type_spec (AstPort, 1);
  ast_set_child (retval, PORT_SIGNATURE, signature);
  return retval;
}

ast_t *
ast_make_component_type_spec (ast_t * field_list)
{
  ast_t *retval = make_type_spec (AstComponent, 1);
  ast_set_child (retval, COMPONENT_FIELD_LIST, field_list);
  return retval;
}

ast_t *
ast_make_struct_type_spec (ast_t * field_list)
{
  ast_t *retval = make_type_spec (AstStruct, 1);
  ast_set_child (retval, STRUCT_FIELD_LIST, field_list);
  return retval;
}

ast_t *ast_make_empty_type_spec (void)
{
  return make_type_spec (AstEmptyTypeSpec, 0);
}

AstTypeSpecificationKind
ast_type_specification_kind (const ast_t * node)
{
  assert (node->kind == AstTypeSpecification);
  return node->type_specification.kind;
}

ast_t *
ast_make_expression_list (void)
{
  return make_expr (AstExprList, 0);
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

AstExpressionKind
ast_expression_kind (const ast_t * node)
{
  assert (node->kind == AstExpression);
  return node->expression.kind;
}

ast_t *
ast_make_typed_literal (typed_value_t value)
{
  ast_t *node = make_expr (AstTypedLiteral, 0);
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
ast_set_type (ast_t * node, typed_value_t typed_value, bool derived_from_immutable,
	      bool derived_from_receiver)
{
  assert (node->kind == AstExpression);
  assert (node->expression.kind != AstTypedLiteral);
  node->expression.typed_value = typed_value;
  node->expression.derived_from_immutable = derived_from_immutable;
  node->expression.derived_from_receiver = derived_from_receiver;
}

bool
ast_get_derived_from_immutable (const ast_t * node)
{
  assert (node->kind == AstExpression);
  assert (node->expression.kind != AstTypedLiteral);
  return node->expression.derived_from_immutable;
}

bool
ast_get_derived_from_receiver (const ast_t * node)
{
  assert (node->kind == AstExpression);
  assert (node->expression.kind != AstTypedLiteral);
  return node->expression.derived_from_receiver;
}


ast_t *
ast_make_signature (void)
{
  return make_type_spec (AstSignature, 0);
}

ast_t *
ast_make_port_call (ast_t * identifier, ast_t * args)
{
  ast_t *a = make_expr (AstPortCallExpr, 2);
  ast_set_child (a, CALL_EXPR, identifier);
  ast_set_child (a, CALL_ARGS, args);
  return a;
}

ast_t *ast_make_pointer_type_spec (ast_t* type_spec)
{
  ast_t *retval = make_type_spec (AstPointer, 1);
  ast_set_child (retval, POINTER_BASE_TYPE, type_spec);
  return retval;
}

ast_t *ast_make_foreign_type_spec (ast_t* type_spec)
{
  ast_t *retval = make_type_spec (AstForeign, 1);
  ast_set_child (retval, FOREIGN_BASE_TYPE, type_spec);
  return retval;
}

ast_t *ast_make_equal (ast_t* left, ast_t* right)
{
  ast_t *retval = make_expr (AstEqualExpr, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, left);
  ast_set_child (retval, BINARY_RIGHT_CHILD, right);
  return retval;
}

ast_t *ast_make_not_equal (ast_t* left, ast_t* right)
{
  ast_t *retval = make_expr (AstNotEqualExpr, 2);
  ast_set_child (retval, BINARY_LEFT_CHILD, left);
  ast_set_child (retval, BINARY_RIGHT_CHILD, right);
  return retval;
}

ast_t *ast_make_if_stmt (ast_t* condition, ast_t* true_branch)
{
  ast_t *retval = make_stmt (AstIfStmt, 2);
  ast_set_child (retval, IF_CONDITION, condition);
  ast_set_child (retval, IF_TRUE_BRANCH, true_branch);
  return retval;
}

ast_t *ast_make_new_expr (ast_t* identifier)
{
  ast_t *retval = make_expr (AstNewExpr, 1);
  ast_set_child (retval, UNARY_CHILD, identifier);
  return retval;

}
