%{
#include "scanner.hpp"
#include "yyparse.hpp"
#include "debug.hpp"
%}

%union { ast_t* node; Mutability mutability; }
%token <node> IDENTIFIER
%token <node> LITERAL

%type <node> Action
%type <node> ActivateStatement
%type <node> AddExpression
%type <node> AndExpression
%type <node> ArrayDimension
%type <node> AssignmentStatement
%type <node> Bind
%type <node> BindStatement
%type <node> Block
%type <node> ChangeStatement
%type <node> CompareExpression
%type <node> Const
%type <node> Definition
%type <node> DefinitionList
%type <node> EmptyStatement
%type <node> Expression
%type <node> ExpressionList
%type <node> ExpressionStatement
%type <node> FieldList
%type <node> ForIotaStatement
%type <node> Func
%type <node> Getter
%type <node> IdentifierList
%type <node> IfStatement
%type <node> IncrementStatement
%type <node> IndexExpression
%type <node> Init
%type <node> Instance
%type <node> Method
%type <node> MultiplyExpression
%type <node> Operand
%type <node> OptionalExpressionList
%type <node> OptionalPushPortCallList
%type <node> OrExpression
%type <node> Parameter
%type <node> ParameterList
%type <node> PrimaryExpression
%type <node> PrintlnStatement
%type <node> PushPortCall
%type <node> PushPortCallList
%type <node> Reaction
%type <node> Receiver
%type <node> ReturnStatement
%type <node> Signature
%type <node> SimpleStatement
%type <node> Statement
%type <node> StatementList
%type <node> Type
%type <node> TypeSpec
%type <node> UnaryExpression
%type <node> VarStatement
%type <node> WhileStatement

%destructor { /* TODO:  Free the node. node_free ($$); */ } <node>

%type <mutability> Mutability
%type <mutability> DereferenceMutability

%token ACTION ACTIVATE BIND CAST CHANGE COMPONENT CONST COPY ELSE ENUM FOR FOREIGN_KW FUNC GETTER HEAP IF INIT INSTANCE MERGE MOVE NEW PRINTLN PULL PUSH REACTION RETURN_KW STRUCT TYPE VAR WHILE

%token ADD_ASSIGN AND_NOT_TOKEN RIGHT_ARROW LEFT_ARROW DECREMENT DOTDOT EQUAL_TOKEN INCREMENT LESS_EQUAL_TOKEN LEFT_SHIFT_TOKEN LOGIC_AND_TOKEN LOGIC_OR_TOKEN MORE_EQUAL_TOKEN NOT_EQUAL_TOKEN RIGHT_SHIFT_TOKEN

%%

top: DefinitionList { root = $1; }

DefinitionList: /* empty */ { $$ = new ast_top_level_list_t (); }
| DefinitionList Definition { $$ = $1->append ($2); }

Definition: Type { $$ = $1; }
| Init { $$ = $1; }
| Getter { $$ = $1; }
| Action { $$ = $1; }
| Reaction { $$ = $1; }
| Bind { $$ = $1; }
| Instance { $$ = $1; }
| Method { $$ = $1; }
| Func { $$ = $1; }
| Const { $$ = $1; }

Const: CONST IDENTIFIER TypeSpec '=' Expression ';' { $$ = new ast_const_t (@1, $2, $3, $5); }

Instance: INSTANCE IDENTIFIER IDENTIFIER IDENTIFIER ';' { $$ = new ast_instance_t (@1, $2, $3, $4); }

Type: TYPE IDENTIFIER TypeSpec ';' { $$ = new ast_type_definition_t (@1, $2, $3); }

Mutability:
/* Empty. */
{ $$ = MUTABLE; }
| CONST
{ $$ = IMMUTABLE; }
| FOREIGN_KW
{ $$ = FOREIGN; }

DereferenceMutability:
/* Empty. */
{ $$ = MUTABLE; }
| '+' CONST
{ $$ = IMMUTABLE; }
| '+' FOREIGN_KW
{ $$ = FOREIGN; }

Receiver:
'(' IDENTIFIER Mutability DereferenceMutability '*' IDENTIFIER ')'
{ $$ = new ast_receiver_t (@1, $2, $3, $4, $6); }

Action:
ACTION Receiver '(' Expression ')' Block
{ $$ = new ast_action_t (@1, $2, $4, $6); }
| ArrayDimension ACTION Receiver '(' Expression ')' Block
{ $$ = new ast_dimensioned_action_t (@2, $1, $3, $5, $7); }

Reaction:
REACTION Receiver IDENTIFIER Signature Block
{ $$ = new ast_reaction_t (@1, $2, $3, $4, $5); }
| ArrayDimension REACTION Receiver IDENTIFIER Signature Block
{ $$ = new ast_dimensioned_reaction_t (@2, $1, $3, $4, $5, $6); }

Bind:
BIND Receiver Block
{ $$ = new ast_bind_t (@1, $2, $3); }

Init:
INIT Receiver IDENTIFIER Signature Block
{ $$ = new ast_initializer_t (@1, $2, $3, $4, $5); }

Getter:
GETTER Receiver IDENTIFIER Signature DereferenceMutability TypeSpec Block
{ $$ = new ast_getter_t (@1, $2, $3, $4, $5, $6, $7); }

Method:
FUNC Receiver IDENTIFIER Signature DereferenceMutability TypeSpec Block
{ $$ = new ast_method_t (@1, $2, $3, $4, $5, $6, $7); }
| FUNC Receiver IDENTIFIER Signature           Block
{ $$ = new ast_method_t (@1, $2, $3, $4, IMMUTABLE, new ast_empty_type_spec_t (@1), $5); }

Func:
FUNC IDENTIFIER Signature Block
{ $$ = new ast_function_t (@1, $2, $3, IMMUTABLE, new ast_empty_type_spec_t (@1), $4); }
| FUNC IDENTIFIER Signature DereferenceMutability TypeSpec Block
{ $$ = new ast_function_t (@1, $2, $3, $4, $5, $6); }

Signature: '(' ')' { $$ = new ast_signature_type_spec_t (yyloc); }
| '(' ParameterList optional_semicolon ')' { $$ = $2; }

ParameterList: Parameter { $$ = (new ast_signature_type_spec_t (@1))->append ($1); }
| ParameterList ';' Parameter { $$ = $1->append ($3); }

Parameter:
  IdentifierList Mutability DereferenceMutability TypeSpec
{ $$ = new ast_identifier_list_type_spec_t (@1, $1, $2, $3, $4); }

optional_semicolon: /* Empty. */
| ';'

BindStatement: Expression RIGHT_ARROW Expression ';' { $$ = new ast_bind_push_port_statement_t (@1, $1, $3); } /* CHECK */
| Expression RIGHT_ARROW Expression DOTDOT Expression';' { $$ = new ast_bind_push_port_param_statement_t (@1, $1, $3, $5); } /* CHECK */
| Expression LEFT_ARROW Expression ';' { $$ = new ast_bind_pull_port_statement_t (@1, $1, $3); } /* CHECK */

Block: '{' StatementList '}' { $$ = $2; }

StatementList: /* empty */ { $$ = new ast_list_statement_t (yyloc); }
| StatementList Statement { $$ = $1->append ($2); }

Statement: SimpleStatement { $$ = $1; }
| VarStatement { $$ = $1; }
| ActivateStatement { $$ = $1; }
| Block { $$ = $1; }
| PrintlnStatement { $$ = $1; }
| ReturnStatement { $$ = $1; }
| IfStatement { $$ = $1; }
| WhileStatement { $$ = $1; }
| ChangeStatement { $$ = $1; }
| ForIotaStatement { $$ = $1; }
| BindStatement { $$ = $1; }
| Const { $$ = $1; }

SimpleStatement: EmptyStatement { $$ = $1; }
| ExpressionStatement { $$ = $1; }
| IncrementStatement { $$ = $1; }
| AssignmentStatement { $$ = $1; }

EmptyStatement: /* empty */ ';' { $$ = new ast_empty_statement_t (yyloc); }

ActivateStatement: ACTIVATE OptionalPushPortCallList Block { $$ = new ast_activate_statement_t (@1, $2, $3); }

ChangeStatement: CHANGE '(' Expression ',' IDENTIFIER Mutability DereferenceMutability TypeSpec ')' Block { $$ = new ast_change_statement_t (@1, $3, $5, $6, $7, $8, $10); }

ForIotaStatement: FOR IDENTIFIER DOTDOT Expression Block { $$ = new ast_for_iota_statement_t (@1, $2, $4, $5); }

PrintlnStatement: PRINTLN ExpressionList ';' { $$ = new ast_println_statement_t (@1, $2); }

ReturnStatement: RETURN_KW Expression ';' { $$ = new ast_return_statement_t (@1, $2); }

IncrementStatement: Expression INCREMENT ';' { $$ = new ast_increment_statement_t (@1, $1); } /* CHECK */
| Expression DECREMENT ';' { $$ = new ast_decrement_statement_t (@1, $1); } /* CHECK */

OptionalPushPortCallList: /* Empty. */ { $$ = new ast_list_expr_t (yyloc); }
| PushPortCallList { $$ = $1; }

PushPortCallList: PushPortCall { $$ = (new ast_list_expr_t (@1))->append ($1); }
| PushPortCallList ',' PushPortCall { $$ = $1->append ($3); }

PushPortCall: IDENTIFIER IndexExpression '(' OptionalExpressionList ')' { $$ = new ast_indexed_port_call_expr_t (@1, $1, $2, $4); }
| IDENTIFIER '(' OptionalExpressionList ')' { $$ = new ast_push_port_call_expr_t (@1, $1, $3); }

IndexExpression: '[' Expression ']' { $$ = $2; }

OptionalExpressionList: /* Empty. */ { $$ = new ast_list_expr_t (yyloc); }
| ExpressionList { $$ = $1; }

ExpressionList: Expression { $$ = (new ast_list_expr_t (@1))->append ($1); }
| ExpressionList ',' Expression { $$ = $1->append ($3); }

ExpressionStatement: Expression ';' {
  $$ = new ast_expression_statement_t (@1, $1);
 }

VarStatement: VAR IdentifierList Mutability DereferenceMutability TypeSpec ';' { $$ = new ast_var_statement_t (@1, $2, $3, $4, $5); }
| VAR IdentifierList Mutability DereferenceMutability TypeSpec '=' ExpressionList ';' { $$ = new ast_var_type_init_statement_t (@1, $2, $3, $4, $5, $7); }
| VAR IdentifierList Mutability DereferenceMutability '=' ExpressionList ';' { unimplemented; }

AssignmentStatement: Expression '=' Expression ';' { $$ = new ast_assign_statement_t (@1, $1, $3); } /* CHECK */
| Expression ADD_ASSIGN Expression ';' { $$ = new ast_add_assign_statement_t (@1, $1, $3); } /* CHECK */

IfStatement: IF Expression Block { $$ = new ast_if_statement_t (@1, $2, $3, new ast_list_statement_t (@1)); }
| IF Expression Block ELSE IfStatement { unimplemented; }
| IF Expression Block ELSE Block { $$ = new ast_if_statement_t (@1, $2, $3, $5); }
| IF SimpleStatement ';' Expression Block { unimplemented; }
| IF SimpleStatement ';' Expression Block ELSE IfStatement { unimplemented; }
| IF SimpleStatement ';' Expression Block ELSE Block { unimplemented; }

WhileStatement: WHILE Expression Block { $$ = new ast_while_statement_t (@1, $2, $3); }

IdentifierList: IDENTIFIER { $$ = (new ast_identifier_list_t (@1))->append ($1); }
| IdentifierList ',' IDENTIFIER { $$ = $1->append ($3); }

TypeSpec: IDENTIFIER { $$ = new ast_identifier_type_spec_t (@1, $1); }
| COMPONENT '{' FieldList '}' { $$ = new ast_component_type_spec_t (@1, $3); }
| STRUCT '{' FieldList '}' { $$ = new ast_struct_type_spec_t (@1, $3); }
| PUSH Signature { $$ = new ast_push_port_type_spec_t (@1, $2); }
| PULL Signature DereferenceMutability TypeSpec { $$ = new ast_pull_port_type_spec_t (@1, $2, $3, $4); }
| '*' TypeSpec { $$ = new ast_pointer_type_spec_t (@1, $2); }
| HEAP TypeSpec { $$ = new ast_heap_type_spec_t (@1, $2); }
| ArrayDimension TypeSpec { $$ = new ast_array_type_spec_t (@1, $1, $2); }
| '[' ']' TypeSpec { $$ = new ast_slice_type_spec_t (@1, $3); }
| ENUM '{' IdentifierList '}' { $$ = new ast_enum_type_spec_t (@1, $3); }

ArrayDimension: '[' Expression ']' { $$ = $2; }

FieldList: /* empty */ { $$ = new ast_field_list_type_spec_t (yyloc); }
| FieldList IdentifierList TypeSpec ';' { $$ = $1->append (new ast_identifier_list_type_spec_t (@1, $2, MUTABLE, MUTABLE, $3)); }

Expression: OrExpression { $$ = $1; }

OrExpression: AndExpression { $$ = $1; }
| AndExpression LOGIC_OR_TOKEN OrExpression { $$ = new ast_binary_arithmetic_expr_t (@1, LOGIC_OR, $1, $3); }

AndExpression: CompareExpression { $$ = $1; }
| CompareExpression LOGIC_AND_TOKEN AndExpression { $$ = new ast_binary_arithmetic_expr_t (@1, LOGIC_AND, $1, $3); }

CompareExpression: AddExpression { $$ = $1; }
| AddExpression EQUAL_TOKEN CompareExpression { $$ = new ast_binary_arithmetic_expr_t (@1, EQUAL, $1, $3); }
| AddExpression NOT_EQUAL_TOKEN CompareExpression { $$ = new ast_binary_arithmetic_expr_t (@1, NOT_EQUAL, $1, $3); }
| AddExpression '<' CompareExpression { $$ = new ast_binary_arithmetic_expr_t (@1, LESS_THAN, $1, $3); }
| AddExpression LESS_EQUAL_TOKEN CompareExpression { $$ = new ast_binary_arithmetic_expr_t (@1, LESS_EQUAL, $1, $3); }
| AddExpression '>' CompareExpression { $$ = new ast_binary_arithmetic_expr_t (@1, MORE_THAN, $1, $3); }
| AddExpression MORE_EQUAL_TOKEN CompareExpression { $$ = new ast_binary_arithmetic_expr_t (@1, MORE_EQUAL, $1, $3); }

AddExpression: MultiplyExpression { $$ = $1; }
| MultiplyExpression '+' AddExpression { $$ = new ast_binary_arithmetic_expr_t (@1, ADD, $1, $3); }
| MultiplyExpression '-' AddExpression { $$ = new ast_binary_arithmetic_expr_t (@1, SUBTRACT, $1, $3); }
| MultiplyExpression '|' AddExpression { $$ = new ast_binary_arithmetic_expr_t (@1, BIT_OR, $1, $3); }
| MultiplyExpression '^' AddExpression { $$ = new ast_binary_arithmetic_expr_t (@1, BIT_XOR, $1, $3); }

MultiplyExpression: UnaryExpression { $$ = $1; }
| UnaryExpression '*' MultiplyExpression { $$ = new ast_binary_arithmetic_expr_t (@1, MULTIPLY, $1, $3); }
| UnaryExpression '/' MultiplyExpression { $$ = new ast_binary_arithmetic_expr_t (@1, DIVIDE, $1, $3); }
| UnaryExpression '%' MultiplyExpression { $$ = new ast_binary_arithmetic_expr_t (@1, MODULUS, $1, $3); }
| UnaryExpression LEFT_SHIFT_TOKEN MultiplyExpression { $$ = new ast_binary_arithmetic_expr_t (@1, LEFT_SHIFT, $1, $3); }
| UnaryExpression RIGHT_SHIFT_TOKEN MultiplyExpression { $$ = new ast_binary_arithmetic_expr_t (@1, RIGHT_SHIFT, $1, $3); }
| UnaryExpression '&' MultiplyExpression { $$ = new ast_binary_arithmetic_expr_t (@1, BIT_AND, $1, $3); }
| UnaryExpression AND_NOT_TOKEN MultiplyExpression { $$ = new ast_binary_arithmetic_expr_t (@1, BIT_AND_NOT, $1, $3); }

UnaryExpression: PrimaryExpression { $$ = $1; }
| '+' UnaryExpression { unimplemented; }
| '-' UnaryExpression { unimplemented; }
| '!' UnaryExpression { $$ = new ast_logic_not_expr_t (@1, $2); }
| '^' UnaryExpression { unimplemented; }
| '*' UnaryExpression { $$ = new ast_dereference_expr_t (@1, $2); }
| '&' UnaryExpression { $$ = new ast_address_of_expr_t (@1, $2); }

PrimaryExpression:
Operand
{ $$ = $1; }
| CAST '<' TypeSpec '>' '(' Expression ')'
{ $$ = new ast_cast_expr_t (@1, $3, $6); }
| PrimaryExpression '.' IDENTIFIER
{ $$ = new ast_select_expr_t (@1, $1, $3); }
| PrimaryExpression '[' Expression ']'
{ $$ = new ast_index_expr_t (@1, $1, $3); }
| PrimaryExpression '[' Expression ':' Expression ']'
{ $$ = new ast_slice_expr_t (@1, $1, $3, $5); }
/* | PrimaryExpression TypeAssertion { unimplemented; } */
| PrimaryExpression '(' OptionalExpressionList ')'
{ $$ = new ast_call_expr_t (@1, $1, $3); }
| NEW TypeSpec
{ $$ = new ast_new_expr_t (@1, $2); }
| MOVE '(' Expression ')'
{ $$ = new ast_move_expr_t (@1, $3); }
| MERGE '(' Expression ')'
{ $$ = new ast_merge_expr_t (@1, $3); }
| COPY '(' Expression ')'
{ $$ = new ast_copy_expr_t (@1, $3); }

Operand: LITERAL { $$ = $1; }
| IDENTIFIER { $$ = new ast_identifier_expr_t (@1, $1); }
| '(' Expression ')' { $$ = $2; }

%%
