%{
#include "scanner.hpp"
#include "yyparse.hpp"
#include "debug.hpp"
%}

%union { ast_t* node; Mutability mutability; }
%token <node> IDENTIFIER
%token <node> LITERAL

%type <node> action_def
%type <node> AddExpression
%type <node> AndExpression
%type <node> array_dimension
%type <node> assignment_stmt
%type <node> bind_def
%type <node> bind_stmt
%type <node> change_stmt
%type <node> CompareExpression
%type <node> const_def
%type <node> def
%type <node> def_list
%type <node> empty_stmt
%type <node> expr_list
%type <node> expr_stmt
%type <node> field_list
%type <node> for_iota_stmt
%type <node> func_def
%type <node> getter_def
%type <node> identifier
%type <node> identifier_list
%type <node> if_stmt
%type <node> increment_stmt
%type <node> init_def
%type <node> inner_stmt_list
%type <node> instance_def
%type <node> method_def
%type <node> MultiplyExpression
%type <node> optional_expr_list
%type <node> index_expr
%type <node> Receiver
%type <node> Operand
%type <node> optional_push_port_call_list
%type <node> OrExpression
%type <node> parameter
%type <node> parameter_list
%type <node> push_port_call
%type <node> push_port_call_list
%type <node> PrimaryExpression
%type <node> println_stmt
%type <node> reaction_def
%type <node> return_stmt
%type <node> Expression
%type <node> signature
%type <node> simple_stmt
%type <node> stmt
%type <node> stmt_list
%type <node> trigger_stmt
%type <node> type_def
%type <node> type_spec
%type <node> UnaryExpression
%type <node> var_stmt
%type <node> while_stmt
%destructor { /* TODO:  Free the node. node_free ($$); */ } <node>

%type <mutability> Mutability
%type <mutability> DereferenceMutability

%token ACTION BIND CAST CHANGE COMPONENT CONST COPY ELSE ENUM FOR FOREIGN_KW FUNC GETTER HEAP IF INIT INSTANCE MERGE MOVE NEW PRINTLN PULL PUSH REACTION RETURN_KW STRUCT TRIGGER TYPE VAR WHILE

%token ADD_ASSIGN AND_NOT_TOKEN RIGHT_ARROW LEFT_ARROW DECREMENT DOTDOT EQUAL_TOKEN INCREMENT LESS_EQUAL_TOKEN LEFT_SHIFT_TOKEN LOGIC_AND_TOKEN LOGIC_OR_TOKEN MORE_EQUAL_TOKEN NOT_EQUAL_TOKEN RIGHT_SHIFT_TOKEN

%%

top: def_list { root = $1; }

def_list: /* empty */ { $$ = new ast_top_level_list_t (); }
| def_list def { $$ = $1->append ($2); }

def: type_def { $$ = $1; }
| init_def { $$ = $1; }
| getter_def { $$ = $1; }
| action_def { $$ = $1; }
| reaction_def { $$ = $1; }
| bind_def { $$ = $1; }
| instance_def { $$ = $1; }
| method_def { $$ = $1; }
| func_def { $$ = $1; }
| const_def { $$ = $1; }

const_def: CONST identifier type_spec '=' Expression ';' { $$ = new ast_const_t (@1, $2, $3, $5); }

instance_def: INSTANCE identifier identifier identifier ';' { $$ = new ast_instance_t (@1, $2, $3, $4); }

type_def: TYPE identifier type_spec ';' { $$ = new ast_type_definition_t (@1, $2, $3); }

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
'(' identifier Mutability DereferenceMutability '*' identifier ')'
{ $$ = new ast_receiver_t (@1, $2, $3, $4, $6); }

action_def:
ACTION Receiver '(' Expression ')' stmt_list
{ $$ = new ast_action_t (@1, $2, $4, $6); }
| array_dimension ACTION Receiver '(' Expression ')' stmt_list
{ $$ = new ast_dimensioned_action_t (@2, $1, $3, $5, $7); }

reaction_def:
REACTION Receiver identifier signature stmt_list
{ $$ = new ast_reaction_t (@1, $2, $3, $4, $5); }
| array_dimension REACTION Receiver identifier signature stmt_list
{ $$ = new ast_dimensioned_reaction_t (@2, $1, $3, $4, $5, $6); }

bind_def:
BIND Receiver stmt_list
{ $$ = new ast_bind_t (@1, $2, $3); }

init_def:
INIT Receiver identifier signature stmt_list
{ $$ = new ast_initializer_t (@1, $2, $3, $4, $5); }

getter_def:
GETTER Receiver identifier signature DereferenceMutability type_spec stmt_list
{ $$ = new ast_getter_t (@1, $2, $3, $4, $5, $6, $7); }

method_def:
FUNC Receiver identifier signature DereferenceMutability type_spec stmt_list
{ $$ = new ast_method_t (@1, $2, $3, $4, $5, $6, $7); }
| FUNC Receiver identifier signature           stmt_list
{ $$ = new ast_method_t (@1, $2, $3, $4, IMMUTABLE, new ast_empty_type_spec_t (@1), $5); }

func_def:
FUNC identifier signature stmt_list
{ $$ = new ast_function_t (@1, $2, $3, IMMUTABLE, new ast_empty_type_spec_t (@1), $4); }
| FUNC identifier signature DereferenceMutability type_spec stmt_list
{ $$ = new ast_function_t (@1, $2, $3, $4, $5, $6); }

signature: '(' ')' { $$ = new ast_signature_type_spec_t (yyloc); }
| '(' parameter_list optional_semicolon ')' { $$ = $2; }

parameter_list: parameter { $$ = (new ast_signature_type_spec_t (@1))->append ($1); }
| parameter_list ';' parameter { $$ = $1->append ($3); }

parameter:
  identifier_list Mutability DereferenceMutability type_spec
{ $$ = new ast_identifier_list_type_spec_t (@1, $1, $2, $3, $4); }

optional_semicolon: /* Empty. */
| ';'

bind_stmt: Expression RIGHT_ARROW Expression ';' { $$ = new ast_bind_push_port_statement_t (@1, $1, $3); } /* CHECK */
| Expression RIGHT_ARROW Expression DOTDOT Expression';' { $$ = new ast_bind_push_port_param_statement_t (@1, $1, $3, $5); } /* CHECK */
| Expression LEFT_ARROW Expression ';' { $$ = new ast_bind_pull_port_statement_t (@1, $1, $3); } /* CHECK */

stmt_list: '{' inner_stmt_list '}' { $$ = $2; }

inner_stmt_list: /* empty */ { $$ = new ast_list_statement_t (yyloc); }
| inner_stmt_list stmt { $$ = $1->append ($2); }

stmt: simple_stmt { $$ = $1; }
| var_stmt { $$ = $1; }
| trigger_stmt { $$ = $1; }
| stmt_list { $$ = $1; }
| println_stmt { $$ = $1; }
| return_stmt { $$ = $1; }
| if_stmt { $$ = $1; }
| while_stmt { $$ = $1; }
| change_stmt { $$ = $1; }
| for_iota_stmt { $$ = $1; }
| bind_stmt { $$ = $1; }
| const_def { $$ = $1; }

simple_stmt: empty_stmt { $$ = $1; }
| expr_stmt { $$ = $1; }
| increment_stmt { $$ = $1; }
| assignment_stmt { $$ = $1; }

empty_stmt: /* empty */ ';' { $$ = new ast_empty_statement_t (yyloc); }

trigger_stmt: TRIGGER optional_push_port_call_list stmt_list { $$ = new ast_trigger_statement_t (@1, $2, $3); }

change_stmt: CHANGE '(' Expression ',' identifier type_spec ')' stmt_list { $$ = new ast_change_statement_t (@1, $3, $5, $6, $8); }

for_iota_stmt: FOR identifier DOTDOT Expression stmt_list { $$ = new ast_for_iota_statement_t (@1, $2, $4, $5); }

println_stmt: PRINTLN expr_list ';' { $$ = new ast_println_statement_t (@1, $2); }

return_stmt: RETURN_KW Expression ';' { $$ = new ast_return_statement_t (@1, $2); }

increment_stmt: Expression INCREMENT ';' { $$ = new ast_increment_statement_t (@1, $1); } /* CHECK */
| Expression DECREMENT ';' { $$ = new ast_decrement_statement_t (@1, $1); } /* CHECK */

optional_push_port_call_list: /* Empty. */ { $$ = new ast_list_expr_t (yyloc); }
| push_port_call_list { $$ = $1; }

push_port_call_list: push_port_call { $$ = (new ast_list_expr_t (@1))->append ($1); }
| push_port_call_list ',' push_port_call { $$ = $1->append ($3); }

push_port_call: identifier index_expr '(' optional_expr_list ')' { $$ = new ast_indexed_port_call_expr_t (@1, $1, $2, $4); }
| identifier '(' optional_expr_list ')' { $$ = new ast_push_port_call_expr_t (@1, $1, $3); }

index_expr: '[' Expression ']' { $$ = $2; }

optional_expr_list: /* Empty. */ { $$ = new ast_list_expr_t (yyloc); }
| expr_list { $$ = $1; }

expr_list: Expression { $$ = (new ast_list_expr_t (@1))->append ($1); }
| expr_list ',' Expression { $$ = $1->append ($3); }

expr_stmt: Expression ';' {
  $$ = new ast_expression_statement_t (@1, $1);
 }

var_stmt: VAR identifier_list Mutability DereferenceMutability type_spec ';' { $$ = new ast_var_statement_t (@1, $2, $3, $4, $5); }
| VAR identifier_list Mutability DereferenceMutability type_spec '=' expr_list ';' { $$ = new ast_var_type_init_statement_t (@1, $2, $3, $4, $5, $7); }
| VAR identifier_list Mutability DereferenceMutability '=' expr_list ';' { unimplemented; }

assignment_stmt: Expression '=' Expression ';' { $$ = new ast_assign_statement_t (@1, $1, $3); } /* CHECK */
| Expression ADD_ASSIGN Expression ';' { $$ = new ast_add_assign_statement_t (@1, $1, $3); } /* CHECK */

if_stmt: IF Expression stmt_list { $$ = new ast_if_statement_t (@1, $2, $3, new ast_list_statement_t (@1)); }
| IF Expression stmt_list ELSE if_stmt { unimplemented; }
| IF Expression stmt_list ELSE stmt_list { $$ = new ast_if_statement_t (@1, $2, $3, $5); }
| IF simple_stmt ';' Expression stmt_list { unimplemented; }
| IF simple_stmt ';' Expression stmt_list ELSE if_stmt { unimplemented; }
| IF simple_stmt ';' Expression stmt_list ELSE stmt_list { unimplemented; }

while_stmt: WHILE Expression stmt_list { $$ = new ast_while_statement_t (@1, $2, $3); }

identifier_list: identifier { $$ = (new ast_identifier_list_t (@1))->append ($1); }
| identifier_list ',' identifier { $$ = $1->append ($3); }

identifier: IDENTIFIER { $$ = $1; }

type_spec: identifier { $$ = new ast_identifier_type_spec_t (@1, $1); }
| COMPONENT '{' field_list '}' { $$ = new ast_component_type_spec_t (@1, $3); }
| STRUCT '{' field_list '}' { $$ = new ast_struct_type_spec_t (@1, $3); }
| PUSH signature { $$ = new ast_push_port_type_spec_t (@1, $2); }
| PULL signature type_spec { $$ = new ast_pull_port_type_spec_t (@1, $2, $3); }
| '*' type_spec { $$ = new ast_pointer_type_spec_t (@1, $2); }
| HEAP type_spec { $$ = new ast_heap_type_spec_t (@1, $2); }
| array_dimension type_spec { $$ = new ast_array_type_spec_t (@1, $1, $2); }
| '[' ']' type_spec { $$ = new ast_slice_type_spec_t (@1, $3); }
| ENUM '{' identifier_list '}' { $$ = new ast_enum_type_spec_t (@1, $3); }

array_dimension: '[' Expression ']' { $$ = $2; }

field_list: /* empty */ { $$ = new ast_field_list_type_spec_t (yyloc); }
| field_list identifier_list type_spec ';' { $$ = $1->append (new ast_identifier_list_type_spec_t (@1, $2, MUTABLE, MUTABLE, $3)); }

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
| CAST '<' type_spec '>' '(' Expression ')'
{ $$ = new ast_cast_expr_t (@1, $3, $6); }
| PrimaryExpression '.' identifier
{ $$ = new ast_select_expr_t (@1, $1, $3); }
| PrimaryExpression '[' Expression ']'
{ $$ = new ast_index_expr_t (@1, $1, $3); }
| PrimaryExpression '[' Expression ':' Expression ']'
{ $$ = new ast_slice_expr_t (@1, $1, $3, $5); }
/* | PrimaryExpression TypeAssertion { unimplemented; } */
| PrimaryExpression '(' optional_expr_list ')'
{ $$ = new ast_call_expr_t (@1, $1, $3); }
| NEW type_spec
{ $$ = new ast_new_expr_t (@1, $2); }
| MOVE '(' Expression ')'
{ $$ = new ast_move_expr_t (@1, $3); }
| MERGE '(' Expression ')'
{ $$ = new ast_merge_expr_t (@1, $3); }
| COPY '(' Expression ')'
{ $$ = new ast_copy_expr_t (@1, $3); }

Operand: LITERAL { $$ = $1; }
| identifier { $$ = new ast_identifier_expr_t (@1, $1); }
| '(' Expression ')' { $$ = $2; }

%%
