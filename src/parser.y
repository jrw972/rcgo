%{
#include "scanner.h"
#include "yyparse.h"
#include "debug.h"
%}

%union { char* identifier;
  typed_value_t literal; }
%token <identifier> IDENTIFIER
%destructor { free ($$); } <identifier>
%token <literal> LITERAL

%union { ast_t* node; }
%type <node> action_def
%type <node> and_expr
%type <node> assignment_stmt
%type <node> bind_def
%type <node> bind_inner_stmt_list
%type <node> bind_stmt
%type <node> bind_stmt_list
%type <node> compare_expr
%type <node> def
%type <node> def_list
%type <node> expr_list
%type <node> expr_stmt
%type <node> field_list
%type <node> getter_def
%type <node> identifier
%type <node> identifier_list
%type <node> if_stmt
%type <node> increment_stmt
%type <node> inner_stmt_list
%type <node> instance_def
%type <node> lvalue
%type <node> optional_expr_list
%type <node> optional_port_call_list
%type <node> or_expr
%type <node> parameter
%type <node> parameter_list
%type <node> pointer_receiver
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
%destructor { /* TODO:  Free the node. node_free ($$); */ } <node>

%token ACTION BIND COMPONENT ELSE GETTER IF INSTANCE NEW PORT PRINTLN REACTION RETURN STRUCT TRIGGER TYPE VAR

%token ARROW EQUAL INCREMENT LOGIC_AND LOGIC_OR NOT_EQUAL

%%

top: def_list { root = $1; }

def_list: /* empty */ { $$ = ast_make_top_level_list (); }
| def_list def { $$ = ast_append_child ($1, $2); }

def: type_def { $$ = $1; }
| action_def { $$ = $1; }
| reaction_def { $$ = $1; }
| bind_def { $$ = $1; }
| instance_def { $$ = $1; }
| getter_def { $$ = $1; }

instance_def: INSTANCE identifier identifier ';' { $$ = ast_make_instance_def ($2, $3); }

type_def: TYPE identifier type_spec ';' { $$ = ast_make_type_def ($2, $3); }

action_def: ACTION pointer_receiver '(' rvalue ')' stmt_list { $$ = ast_make_action_def ($2, $4, $6); }

reaction_def: REACTION pointer_receiver identifier signature stmt_list { $$ = ast_make_reaction_def ($2, $3, $4, $5); }

bind_def: BIND pointer_receiver bind_stmt_list { $$ = ast_make_bind_def ($2, $3); }

getter_def: GETTER pointer_receiver identifier signature type_spec stmt_list { $$ = ast_make_getter_def ($2, $3, $4, $5, $6); }

signature: '(' ')' { $$ = ast_make_signature (); }
| '(' parameter_list optional_semicolon ')' { $$ = $2; }

parameter_list: parameter { $$ = ast_append_child (ast_make_signature (), $1); }
| parameter_list ';' parameter { $$ = ast_append_child ($1, $3); }

parameter: identifier_list type_spec { $$ = ast_make_identifier_list_type_spec ($1, $2); }

optional_semicolon: /* Empty. */
| ';'

pointer_receiver: '(' identifier '$' identifier ')' { $$ = ast_make_pointer_receiver ($2, $4); }

bind_stmt_list: '{' bind_inner_stmt_list '}' { $$ = $2; }

bind_inner_stmt_list: /* empty */ { $$ = ast_make_bind_list_stmt (); }
| bind_inner_stmt_list bind_stmt { $$ = ast_append_child ($1, $2); }

bind_stmt: lvalue ARROW lvalue ';' { $$ = ast_make_bind_stmt ($1, $3); }

stmt_list: '{' inner_stmt_list '}' { $$ = $2; }

inner_stmt_list: /* empty */ { $$ = ast_make_stmt_list (); }
| inner_stmt_list stmt ';' { $$ = ast_append_child ($1, $2); }

stmt: simple_stmt { $$ = $1; }
| var_stmt { $$ = $1; }
| trigger_stmt { $$ = $1; }
| stmt_list { $$ = $1; }
| println_stmt { $$ = $1; }
| return_stmt { $$ = $1; }
| if_stmt { $$ = $1; }

simple_stmt: empty_stmt { unimplemented; }
| expr_stmt { unimplemented; }
| increment_stmt { $$ = $1; }
| assignment_stmt { $$ = $1; }

empty_stmt: /* empty */ { unimplemented; }

trigger_stmt: TRIGGER optional_port_call_list stmt_list { $$ = ast_make_trigger_stmt ($2, ast_append_child ($3, ast_make_stmt_list ())); }

println_stmt: PRINTLN expr_list { $$ = ast_make_println_stmt ($2); }

return_stmt: RETURN rvalue { $$ = ast_make_return_stmt ($2); }

increment_stmt: lvalue INCREMENT { $$ = ast_make_add_assign_stmt ($1, ast_make_typed_literal (typed_value_make_integer (1))); }

optional_port_call_list: /* Empty. */ { $$ = ast_make_expression_list (); }
| port_call_list { $$ = $1; }

port_call_list: port_call { $$ = ast_append_child (ast_make_expression_list (), $1); }
| port_call_list ',' port_call { unimplemented; }

port_call: identifier '(' optional_expr_list ')' { $$ = ast_make_port_call ($1, $3); }

optional_expr_list: /* Empty. */ { $$ = ast_make_expression_list (); }
| expr_list { $$ = $1; }

expr_list: rvalue { $$ = ast_append_child (ast_make_expression_list (), $1); }
| expr_list ',' rvalue { $$ = ast_append_child ($1, $3); }

expr_stmt: rvalue {
  $$ = ast_make_expr_stmt ($1);
 }

var_stmt: VAR identifier_list type_spec { $$ = ast_make_var_stmt ($2, $3); }
| VAR identifier_list type_spec '=' expr_list { unimplemented; }
| VAR identifier_list '=' expr_list { unimplemented; }


assignment_stmt: lvalue '=' rvalue { $$ = ast_make_assignment_stmt ($1, $3); }

if_stmt: IF rvalue stmt_list { $$ = ast_make_if_stmt ($2, $3); }
| IF rvalue stmt_list ELSE if_stmt { unimplemented; }
| IF rvalue stmt_list ELSE stmt_list { unimplemented; }
| IF simple_stmt ';' rvalue stmt_list { unimplemented; }
| IF simple_stmt ';' rvalue stmt_list ELSE if_stmt { unimplemented; }
| IF simple_stmt ';' rvalue stmt_list ELSE stmt_list { unimplemented; }

identifier_list: identifier { $$ = ast_append_child (ast_make_identifier_list (), $1); }
| identifier_list ',' identifier { $$ = ast_append_child ($1, $3); }

identifier: IDENTIFIER { $$ = ast_make_identifier ($1); }

type_spec: identifier { $$ = ast_make_identifier_type_spec ($1); }
| COMPONENT '{' field_list '}' { $$ = ast_make_component_type_spec ($3); }
| STRUCT '{' field_list '}' { $$ = ast_make_struct_type_spec ($3); }
| PORT signature { $$ = ast_make_port ($2); }
| '$' type_spec { $$ = ast_make_pointer_type_spec ($2); }

field_list: /* empty */ { $$ = ast_make_field_list (); }
| field_list identifier_list type_spec ';' { $$ = ast_append_child ($1, ast_make_identifier_list_type_spec ($2, $3)); }

rvalue: or_expr { $$ = $1; }

or_expr: and_expr { $$ = $1; }
| and_expr LOGIC_OR or_expr { $$ = ast_make_logic_or ($1, $3); }

and_expr: compare_expr { $$ = $1; }
| compare_expr LOGIC_AND and_expr { $$ = ast_make_logic_and ($1, $3); }

compare_expr: unary_expr { $$ = $1; }
| unary_expr EQUAL compare_expr { $$ = ast_make_equal ($1, $3); }
| unary_expr NOT_EQUAL compare_expr { $$ = ast_make_not_equal ($1, $3); }

unary_expr: primary_expr { $$ = $1; }
| unary_expr '!' { $$ = ast_make_logic_not ($1); }
| unary_expr '&' { $$ = ast_make_address_of ($1); }

primary_expr: lvalue { $$ = $1; }
| primary_expr '(' optional_expr_list ')' { $$ = ast_make_call_expr ($1, $3); }
| LITERAL { $$ = ast_make_typed_literal ($1); }
| NEW identifier { $$ = ast_make_new_expr ($2); }

lvalue: identifier { $$ = ast_make_identifier_expr ($1); }
| lvalue '.' identifier { $$ = ast_make_select ($1, $3); }
| unary_expr '$' { $$ = ast_make_dereference ($1); }

%%
