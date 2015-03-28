%{
#include "scanner.hpp"
#include "yyparse.hpp"
#include "debug.hpp"
%}

%union { ast_t* node; }
%token <node> IDENTIFIER
%token <node> LITERAL

%type <node> action_def
%type <node> add_expr
%type <node> and_expr
%type <node> array_dimension
%type <node> assignment_stmt
%type <node> bind_def
%type <node> bind_stmt
%type <node> change_stmt
%type <node> compare_expr
%type <node> def
%type <node> def_list
%type <node> empty_stmt
%type <node> expr_list
%type <node> expr_stmt
%type <node> field_list
%type <node> for_iota_stmt
%type <node> func_def
%type <node> identifier
%type <node> identifier_list
%type <node> if_stmt
%type <node> increment_stmt
%type <node> inner_stmt_list
%type <node> instance_def
%type <node> lvalue
%type <node> method_def
%type <node> optional_expr_list
%type <node> index_expr
%type <node> optional_port_call_list
%type <node> or_expr
%type <node> parameter
%type <node> parameter_list
%type <node> port_call
%type <node> port_call_list
%type <node> primary_expr
%type <node> println_stmt
%type <node> reaction_def
%type <node> return_stmt
%type <node> rvalue
%type <node> signature
%type <node> simple_stmt
%type <node> stmt
%type <node> stmt_list
%type <node> trigger_stmt
%type <node> type_def
%type <node> type_spec
%type <node> unary_expr
%type <node> var_stmt
%type <node> while_stmt
%destructor { /* TODO:  Free the node. node_free ($$); */ } <node>

%token ACTION BIND CHANGE COMPONENT CONST ELSE ENUM FOR FOREIGN_KW FUNC HEAP IF INSTANCE MERGE MOVE NEW PFUNC PORT PRINTLN REACTION RETURN_KW STRUCT TRIGGER TYPE VAR WHILE

%token ADD_ASSIGN RIGHT_ARROW LEFT_ARROW DECREMENT DOTDOT EQUAL INCREMENT LOGIC_AND LOGIC_OR NOT_EQUAL

%%

top: def_list { root = $1; }

def_list: /* empty */ { $$ = new ast_top_level_list_t (); }
| def_list def { $$ = $1->append ($2); }

def: type_def { $$ = $1; }
| action_def { $$ = $1; }
| reaction_def { $$ = $1; }
| bind_def { $$ = $1; }
| instance_def { $$ = $1; }
| method_def { $$ = $1; }
| func_def { $$ = $1; }

instance_def: INSTANCE identifier identifier identifier ';' { $$ = new ast_instance_t (@1, $2, $3, $4); }

type_def: TYPE identifier type_spec ';' { $$ = new ast_type_definition_t (@1, $2, $3); }

action_def: ACTION '(' identifier '@' identifier CONST ')' '(' rvalue ')' stmt_list { $$ = new ast_action_t (@1, $3, $5, $9, $11); }
| array_dimension ACTION '(' identifier '@' identifier CONST ')' '(' rvalue ')' stmt_list { $$ = new ast_dimensioned_action_t (@2, $1, $4, $6, $10, $12); }

reaction_def:
                  REACTION '(' identifier '@' identifier CONST ')' identifier signature stmt_list { $$ = new ast_reaction_t (@1, $3, $5, $8, $9, $10); }
| array_dimension REACTION '(' identifier '@' identifier CONST ')' identifier signature stmt_list { $$ = new ast_dimensioned_reaction_t (@2, $1, $4, $6, $9, $10, $11); }

bind_def: BIND '(' identifier '@' identifier ')' stmt_list { $$ = new ast_bind_t (@1, $3, $5, $7); }

method_def:
  FUNC '(' identifier '@' identifier       ')' identifier signature type_spec stmt_list { $$ = new ast_method_t (@1, $3, $5, MUTABLE, $7, $8, $9, MUTABLE, $10); }
| FUNC '(' identifier '@' identifier CONST ')' identifier signature type_spec stmt_list { $$ = new ast_method_t (@1, $3, $5, IMMUTABLE, $8, $9, $10, MUTABLE, $11); }
| FUNC '(' identifier '@' identifier       ')' identifier signature type_spec CONST stmt_list { $$ = new ast_method_t (@1, $3, $5, MUTABLE, $7, $8, $9, IMMUTABLE, $11); }
| FUNC '(' identifier '@' identifier CONST ')' identifier signature type_spec CONST stmt_list { $$ = new ast_method_t (@1, $3, $5, IMMUTABLE, $8, $9, $10, IMMUTABLE, $12); }
| FUNC '(' identifier '@' identifier       ')' identifier signature           stmt_list { $$ = new ast_method_t (@1, $3, $5, MUTABLE, $7, $8, new ast_empty_type_spec_t (@1), IMMUTABLE, $9); }
| FUNC '(' identifier '@' identifier CONST ')' identifier signature           stmt_list { $$ = new ast_method_t (@1, $3, $5, IMMUTABLE, $8, $9, new ast_empty_type_spec_t (@1), IMMUTABLE, $10); }


func_def: FUNC identifier signature stmt_list { $$ = new ast_function_t (@1, $2, $3, new ast_empty_type_spec_t (@1), $4); }
| FUNC identifier signature type_spec stmt_list { $$ = new ast_function_t (@1, $2, $3, $4, $5); }

signature: '(' ')' { $$ = new ast_signature_type_spec_t (yyloc); }
| '(' parameter_list optional_semicolon ')' { $$ = $2; }

parameter_list: parameter { $$ = (new ast_signature_type_spec_t (@1))->append ($1); }
| parameter_list ';' parameter { $$ = $1->append ($3); }

parameter: identifier_list type_spec { $$ = new ast_identifier_list_type_spec_t (@1, $1, $2, MUTABLE); }
| identifier_list type_spec CONST { $$ = new ast_identifier_list_type_spec_t (@1, $1, $2, IMMUTABLE); }
| identifier_list type_spec FOREIGN_KW { $$ = new ast_identifier_list_type_spec_t (@1, $1, $2, FOREIGN); }


optional_semicolon: /* Empty. */
| ';'

bind_stmt: lvalue RIGHT_ARROW rvalue ';' { $$ = new ast_bind_port_statement_t (@1, $1, $3); }
| lvalue RIGHT_ARROW rvalue DOTDOT rvalue';' { $$ = new ast_bind_port_param_statement_t (@1, $1, $3, $5); }
| lvalue LEFT_ARROW rvalue ';' { $$ = new ast_bind_pfunc_statement_t (@1, $1, $3); }

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

simple_stmt: empty_stmt { $$ = $1; }
| expr_stmt { $$ = $1; }
| increment_stmt { $$ = $1; }
| assignment_stmt { $$ = $1; }

empty_stmt: /* empty */ ';' { $$ = new ast_empty_statement_t (yyloc); }

trigger_stmt: TRIGGER optional_port_call_list stmt_list { $$ = new ast_trigger_statement_t (@1, $2, $3); }

change_stmt: CHANGE '(' rvalue ',' identifier type_spec ')' stmt_list { $$ = new ast_change_statement_t (@1, $3, $5, $6, $8); }

for_iota_stmt: FOR identifier DOTDOT rvalue stmt_list { $$ = new ast_for_iota_statement_t (@1, $2, $4, $5); }

println_stmt: PRINTLN expr_list ';' { $$ = new ast_println_statement_t (@1, $2); }

return_stmt: RETURN_KW rvalue ';' { $$ = new ast_return_statement_t (@1, $2); }

increment_stmt: lvalue INCREMENT ';' { $$ = new ast_increment_statement_t (@1, $1); }
| lvalue DECREMENT ';' { $$ = new ast_decrement_statement_t (@1, $1); }

optional_port_call_list: /* Empty. */ { $$ = new ast_list_expr_t (yyloc); }
| port_call_list { $$ = $1; }

port_call_list: port_call { $$ = (new ast_list_expr_t (@1))->append ($1); }
| port_call_list ',' port_call { $$ = $1->append ($3); }

port_call: identifier index_expr '(' optional_expr_list ')' { $$ = new ast_indexed_port_call_expr_t (@1, $1, $2, $4); }
| identifier '(' optional_expr_list ')' { $$ = new ast_port_call_expr_t (@1, $1, $3); }

index_expr: '[' rvalue ']' { $$ = $2; }

optional_expr_list: /* Empty. */ { $$ = new ast_list_expr_t (yyloc); }
| expr_list { $$ = $1; }

expr_list: rvalue { $$ = (new ast_list_expr_t (@1))->append ($1); }
| expr_list ',' rvalue { $$ = $1->append ($3); }

expr_stmt: rvalue ';' {
  $$ = new ast_expression_statement_t (@1, $1);
 }

var_stmt: VAR identifier_list type_spec ';' { $$ = new ast_var_statement_t (@1, $2, $3); }
| VAR identifier_list type_spec '=' expr_list ';' { unimplemented; }
| VAR identifier_list '=' expr_list ';' { unimplemented; }

assignment_stmt: lvalue '=' rvalue ';' { $$ = new ast_assign_statement_t (@1, $1, $3); }
| lvalue ADD_ASSIGN rvalue ';' { $$ = new ast_add_assign_statement_t (@1, $1, $3); }

if_stmt: IF rvalue stmt_list { $$ = new ast_if_statement_t (@1, $2, $3, new ast_list_statement_t (@1)); }
| IF rvalue stmt_list ELSE if_stmt { unimplemented; }
| IF rvalue stmt_list ELSE stmt_list { $$ = new ast_if_statement_t (@1, $2, $3, $5); }
| IF simple_stmt ';' rvalue stmt_list { unimplemented; }
| IF simple_stmt ';' rvalue stmt_list ELSE if_stmt { unimplemented; }
| IF simple_stmt ';' rvalue stmt_list ELSE stmt_list { unimplemented; }

while_stmt: WHILE rvalue stmt_list { $$ = new ast_while_statement_t (@1, $2, $3); }

identifier_list: identifier { $$ = (new ast_identifier_list_t (@1))->append ($1); }
| identifier_list ',' identifier { $$ = $1->append ($3); }

identifier: IDENTIFIER { $$ = $1; }

type_spec: identifier { $$ = new ast_identifier_type_spec_t (@1, $1); }
| COMPONENT '{' field_list '}' { $$ = new ast_component_type_spec_t (@1, $3); }
| STRUCT '{' field_list '}' { $$ = new ast_struct_type_spec_t (@1, $3); }
| PORT signature { $$ = new ast_port_type_spec_t (@1, $2); }
| PFUNC signature type_spec { $$ = new ast_pfunc_type_spec_t (@1, $2, $3); }
| '@' type_spec { $$ = new ast_pointer_type_spec_t (@1, $2); }
| HEAP type_spec { $$ = new ast_heap_type_spec_t (@1, $2); }
| array_dimension type_spec { $$ = new ast_array_type_spec_t (@1, $1, $2); }
| ENUM '{' identifier_list '}' { $$ = new ast_enum_type_spec_t (@1, $3); }

array_dimension: '[' LITERAL ']' { $$ = $2; }

field_list: /* empty */ { $$ = new ast_field_list_type_spec_t (yyloc); }
| field_list identifier_list type_spec ';' { $$ = $1->append (new ast_identifier_list_type_spec_t (@1, $2, $3, MUTABLE)); }

rvalue: or_expr { $$ = $1; }

or_expr: and_expr { $$ = $1; }
| and_expr LOGIC_OR or_expr { $$ = new ast_binary_arithmetic_expr_t (@1, ast_binary_arithmetic_expr_t::LOGIC_OR, $1, $3); }

and_expr: compare_expr { $$ = $1; }
| compare_expr LOGIC_AND and_expr { $$ = new ast_binary_arithmetic_expr_t (@1, ast_binary_arithmetic_expr_t::LOGIC_AND, $1, $3); }

compare_expr: add_expr { $$ = $1; }
| add_expr EQUAL compare_expr { $$ = new ast_binary_arithmetic_expr_t (@1, ast_binary_arithmetic_expr_t::EQUAL, $1, $3); }
| add_expr NOT_EQUAL compare_expr { $$ = new ast_binary_arithmetic_expr_t (@1, ast_binary_arithmetic_expr_t::NOT_EQUAL, $1, $3); }

add_expr: unary_expr { $$ = $1; }
| unary_expr '+' add_expr { $$ = new ast_binary_arithmetic_expr_t (@1, ast_binary_arithmetic_expr_t::ADD, $1, $3); }
| unary_expr '-' add_expr { $$ = new ast_binary_arithmetic_expr_t (@1, ast_binary_arithmetic_expr_t::SUBTRACT, $1, $3); }

unary_expr: primary_expr { $$ = $1; }
| unary_expr '!' { $$ = new ast_logic_not_expr_t (@1, $1); }
| lvalue '&' { $$ = new ast_address_of_expr_t (@1, $1); }

primary_expr: lvalue { $$ = new ast_implicit_dereference_expr_t (@1, $1); }
| primary_expr '(' optional_expr_list ')' { $$ = new ast_call_expr_t (@1, $1, $3); }
| LITERAL { $$ = $1; }
| NEW type_spec { $$ = new ast_new_expr_t (@1, $2); }
| MOVE '(' rvalue ')' { $$ = new ast_move_expr_t (@1, $3); }
| MERGE '(' rvalue ')' { $$ = new ast_merge_expr_t (@1, $3); }

lvalue: identifier { $$ = new ast_identifier_expr_t (@1, $1); }
| lvalue '.' identifier { $$ = new ast_select_expr_t (@1, $1, $3); }
| lvalue '[' rvalue ']' { $$ = new ast_index_expr_t (@1, $1, $3); }
| unary_expr '@' { $$ = new ast_dereference_expr_t (@1, $1); }

%%
