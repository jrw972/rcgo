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
%type <node> bind_inner_stmt_list
%type <node> bind_stmt
%type <node> bind_stmt_list
%type <node> change_stmt
%type <node> compare_expr
%type <node> def
%type <node> def_list
%type <node> expr_list
%type <node> expr_stmt
%type <node> field_list
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
%type <node> pointer_receiver
%type <node> pointer_to_immutable_receiver
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

%token ACTION BIND CHANGE COMPONENT CONST ELSE FOREIGN FUNC HEAP IF INSTANCE MERGE MOVE NEW PORT PRINTLN REACTION RETURN STRUCT TRIGGER TYPE VAR WHILE

%token ADD_ASSIGN ARROW DECREMENT DOTDOT EQUAL INCREMENT LOGIC_AND LOGIC_OR NOT_EQUAL

%%

top: def_list { root = $1; }

def_list: /* empty */ { $$ = ast_make_top_level_list (); }
| def_list def { $$ = $1->append ($2); }

def: type_def { $$ = $1; }
| action_def { $$ = $1; }
| reaction_def { $$ = $1; }
| bind_def { $$ = $1; }
| instance_def { $$ = $1; }
| method_def { $$ = $1; }
| func_def { $$ = $1; }

instance_def: INSTANCE identifier identifier identifier ';' { $$ = ast_make_instance_def (@1, $2, $3, $4); }

type_def: TYPE identifier type_spec ';' { $$ = ast_make_type_def (@1, $2, $3); }

action_def: ACTION pointer_to_immutable_receiver '(' rvalue ')' stmt_list { $$ = new ast_action_t (@1, $2, $4, $6); }
| array_dimension ACTION pointer_to_immutable_receiver '(' rvalue ')' stmt_list { $$ = new ast_dimensioned_action_t (@2, $1, $3, $5, $7); }

reaction_def: REACTION pointer_to_immutable_receiver identifier signature stmt_list { $$ = new ast_reaction_t (@1, $2, $3, $4, $5); }
| array_dimension REACTION pointer_to_immutable_receiver identifier signature stmt_list { $$ = new ast_dimensioned_reaction_t (@2, $1, $3, $4, $5, $6); }

bind_def: BIND pointer_receiver bind_stmt_list { $$ = new ast_bind_t (@1, $2, $3); }

method_def: FUNC pointer_to_immutable_receiver identifier signature type_spec stmt_list { $$ = ast_make_method_def (@1, $2, $3, $4, $5, $6); }
| FUNC pointer_to_immutable_receiver identifier signature stmt_list { $$ = ast_make_method_def (@1, $2, $3, $4, ast_make_empty_type_spec (@1), $5); }
| FUNC pointer_receiver identifier signature type_spec stmt_list { $$ = ast_make_method_def (@1, $2, $3, $4, $5, $6); }
| FUNC pointer_receiver identifier signature stmt_list { $$ = ast_make_method_def (@1, $2, $3, $4, ast_make_empty_type_spec (@1), $5); }

func_def: FUNC identifier signature stmt_list { $$ = ast_make_function_def (@1, $2, $3, ast_make_empty_type_spec (@1), $4); }
| FUNC identifier signature type_spec stmt_list { $$ = ast_make_function_def (@1, $2, $3, $4, $5); }

signature: '(' ')' { $$ = ast_make_signature (yyloc); }
| '(' parameter_list optional_semicolon ')' { $$ = $2; }

parameter_list: parameter { $$ = ast_make_signature (@1)->append ($1); }
| parameter_list ';' parameter { $$ = $1->append ($3); }

parameter: identifier_list type_spec { $$ = ast_make_identifier_list_type_spec (@1, $1, $2); }

optional_semicolon: /* Empty. */
| ';'

pointer_receiver: '(' identifier '@' identifier ')' { $$ = ast_make_receiver (@1, $2, $4, ast_receiver_definition_t::AstPointerReceiver); }

pointer_to_immutable_receiver: '(' identifier '$' identifier ')' { $$ = ast_make_receiver (@1, $2, $4, ast_receiver_definition_t::AstPointerToImmutableReceiver); }

bind_stmt_list: '{' bind_inner_stmt_list '}' { $$ = $2; }

bind_inner_stmt_list: /* empty */ { $$ = ast_make_bind_list_stmt (yyloc); }
| bind_inner_stmt_list bind_stmt { $$ = $1->append ($2); }

bind_stmt: lvalue ARROW rvalue ';' { $$ = new ast_bind_statement_t (@1, $1, $3); }
| lvalue ARROW rvalue DOTDOT rvalue';' { $$ = new ast_bind_param_statement_t (@1, $1, $3, $5); }

stmt_list: '{' inner_stmt_list '}' { $$ = $2; }

inner_stmt_list: /* empty */ { $$ = ast_make_stmt_list (yyloc); }
| inner_stmt_list stmt ';' { $$ = $1->append ($2); }

stmt: simple_stmt { $$ = $1; }
| var_stmt { $$ = $1; }
| trigger_stmt { $$ = $1; }
| stmt_list { $$ = $1; }
| println_stmt { $$ = $1; }
| return_stmt { $$ = $1; }
| if_stmt { $$ = $1; }
| while_stmt { $$ = $1; }
| change_stmt { $$ = $1; }

simple_stmt: empty_stmt { unimplemented; }
| expr_stmt { $$ = $1; }
| increment_stmt { $$ = $1; }
| assignment_stmt { $$ = $1; }

empty_stmt: /* empty */ { unimplemented; }

trigger_stmt: TRIGGER optional_port_call_list stmt_list { $$ = new ast_trigger_statement_t (@1, $2, $3); }

change_stmt: CHANGE '(' rvalue ',' identifier type_spec ')' stmt_list { $$ = new ast_change_statement_t (@1, $3, $5, $6, $8); }

println_stmt: PRINTLN expr_list { $$ = ast_make_println_stmt (@1, $2); }

return_stmt: RETURN rvalue { $$ = ast_make_return_stmt (@1, $2); }

increment_stmt: lvalue INCREMENT { $$ = new ast_increment_statement_t (@1, $1); }
| lvalue DECREMENT { $$ = new ast_decrement_statement_t (@1, $1); }

optional_port_call_list: /* Empty. */ { $$ = ast_make_expression_list (yyloc); }
| port_call_list { $$ = $1; }

port_call_list: port_call { $$ = ast_make_expression_list (@1)->append ($1); }
| port_call_list ',' port_call { unimplemented; }

port_call: identifier index_expr '(' optional_expr_list ')' { $$ = new ast_indexed_port_call_expr_t (@1, $1, $2, $4); }
| identifier '(' optional_expr_list ')' { $$ = new ast_port_call_expr_t (@1, $1, $3); }

index_expr: '[' rvalue ']' { $$ = $2; }

optional_expr_list: /* Empty. */ { $$ = ast_make_expression_list (yyloc); }
| expr_list { $$ = $1; }

expr_list: rvalue { $$ = ast_make_expression_list (@1)->append ($1); }
| expr_list ',' rvalue { $$ = $1->append ($3); }

expr_stmt: rvalue {
  $$ = ast_make_expr_stmt (@1, $1);
 }

var_stmt: VAR identifier_list type_spec { $$ = ast_make_var_stmt (@1, $2, $3); }
| VAR identifier_list type_spec '=' expr_list { unimplemented; }
| VAR identifier_list '=' expr_list { unimplemented; }

assignment_stmt: lvalue '=' rvalue { $$ = new ast_assign_statement_t (@1, $1, $3); }
| lvalue ADD_ASSIGN rvalue { $$ = new ast_add_assign_statement_t (@1, $1, $3); }

if_stmt: IF rvalue stmt_list { $$ = new ast_if_statement_t (@1, $2, $3); }
| IF rvalue stmt_list ELSE if_stmt { unimplemented; }
| IF rvalue stmt_list ELSE stmt_list { unimplemented; }
| IF simple_stmt ';' rvalue stmt_list { unimplemented; }
| IF simple_stmt ';' rvalue stmt_list ELSE if_stmt { unimplemented; }
| IF simple_stmt ';' rvalue stmt_list ELSE stmt_list { unimplemented; }

while_stmt: WHILE rvalue stmt_list { $$ = new ast_while_statement_t (@1, $2, $3); }

identifier_list: identifier { $$ = ast_make_identifier_list (@1)->append ($1); }
| identifier_list ',' identifier { $$ = $1->append ($3); }

identifier: IDENTIFIER { $$ = $1; }

type_spec: identifier { $$ = ast_make_identifier_type_spec (@1, $1); }
| COMPONENT '{' field_list '}' { $$ = ast_make_component_type_spec (@1, $3); }
| STRUCT '{' field_list '}' { $$ = ast_make_struct_type_spec (@1, $3); }
| PORT signature { $$ = ast_make_port (@1, $2); }
| '@' type_spec { $$ = ast_make_pointer_type_spec (@1, $2); }
| '$' type_spec { $$ = ast_make_pointer_to_immutable_type_spec (@1, $2); }
| '?' type_spec { $$ = ast_make_pointer_to_foreign_type_spec (@1, $2); }
| HEAP type_spec { $$ = ast_make_heap_type_spec (@1, $2); }
| array_dimension type_spec { $$ = ast_make_array_type_spec (@1, $1, $2); }

array_dimension: '[' LITERAL ']' { $$ = $2; }

field_list: /* empty */ { $$ = ast_make_field_list (yyloc); }
| field_list identifier_list type_spec ';' { $$ = $1->append (ast_make_identifier_list_type_spec (@1, $2, $3)); }

rvalue: or_expr { $$ = $1; }

or_expr: and_expr { $$ = $1; }
| and_expr LOGIC_OR or_expr {
  if (dynamic_cast<ast_logic_or_expr_t*> ($3))
    {
      $$ = $3->prepend ($1);
    }
  else
    {
      $$ = ast_make_logic_or (@1, $1, $3);
    }
 }

and_expr: compare_expr { $$ = $1; }
| compare_expr LOGIC_AND and_expr {
  if (dynamic_cast<ast_logic_and_expr_t*> ($3))
    {
      $$ = $3->prepend ($1);
    }
  else
    {
      $$ = ast_make_logic_and (@1, $1, $3);
    }
 }

compare_expr: add_expr { $$ = $1; }
| add_expr EQUAL compare_expr { $$ = new ast_equal_expr_t (@1, $1, $3); }
| add_expr NOT_EQUAL compare_expr { $$ = new ast_not_equal_expr_t (@1, $1, $3); }

add_expr: unary_expr { $$ = $1; }
| unary_expr '+' add_expr { $$ = new ast_binary_arithmetic_expr_t (@1, ADD, $1, $3); }
| unary_expr '-' add_expr { unimplemented; }

unary_expr: primary_expr { $$ = $1; }
| unary_expr '!' { $$ = new ast_logic_not_expr_t (@1, $1); }
| unary_expr '&' { $$ = new ast_address_of_expr_t (@1, $1); }

primary_expr: lvalue { $$ = $1; }
| primary_expr '(' optional_expr_list ')' { $$ = new ast_call_expr_t (@1, $1, $3); }
| LITERAL { $$ = $1; }
| NEW type_spec { $$ = ast_make_new_expr (@1, $2); }
| MOVE '(' rvalue ')' { $$ = new ast_move_expr_t (@1, $3); }
| MERGE '(' rvalue ')' { $$ = new ast_merge_expr_t (@1, $3); }

lvalue: identifier { $$ = new ast_identifier_expr_t (@1, $1); }
| lvalue '.' identifier { $$ = new ast_select_expr_t (@1, $1, $3); }
| lvalue '[' rvalue ']' { $$ = new ast_index_expr_t (@1, $1, $3); }
| unary_expr '@' { $$ = new ast_dereference_expr_t (@1, $1); }

%%
