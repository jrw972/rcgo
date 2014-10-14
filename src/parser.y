%{
#include "scanner.h"
#include "yyparse.h"
#include "debug.h"
%}

%union { char* identifier; }
%token <identifier> IDENTIFIER
%destructor { free ($$); } <identifier>

%union { ast_t* node; }
%type <node> action_def
%type <node> and_expr
%type <node> assignment_stmt
%type <node> bind_def
%type <node> bind_inner_stmt_list
%type <node> bind_stmt
%type <node> bind_stmt_list
%type <node> def
%type <node> def_list
%type <node> expr_list
%type <node> expr_stmt
%type <node> field_list
%type <node> identifier
%type <node> identifier_list
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
%type <node> reaction_def
%type <node> rvalue
%type <node> signature
%type <node> stmt
%type <node> stmt_list
%type <node> trigger_stmt
%type <node> type_def
%type <node> type_spec
%type <node> unary_expr
%type <node> var_stmt
%destructor { /* TODO:  Free the node. node_free ($$); */ } <node>

%token ACTION BIND COMPONENT INSTANCE PORT REACTION TRIGGER TYPE VAR

%token ARROW LOGIC_AND LOGIC_OR

%%

top: def_list { root = $1; }

def_list: /* empty */ { $$ = ast_make_top_level_list (); }
| def_list def { $$ = ast_add_child ($1, $2); }

def: type_def { $$ = $1; }
| action_def { $$ = $1; }
| reaction_def { $$ = $1; }
| bind_def { $$ = $1; }
| instance_def { $$ = $1; }

instance_def: INSTANCE identifier identifier ';' { $$ = ast_make_instance_def ($2, $3); }

type_def: TYPE identifier type_spec ';' { $$ = ast_make_type_def ($2, $3); }

action_def: ACTION pointer_receiver '(' rvalue ')' stmt_list { $$ = ast_make_action_def ($2, $4, $6); }

reaction_def: REACTION pointer_receiver identifier signature stmt_list { $$ = ast_make_reaction_def ($2, $3, $4, $5); }

bind_def: BIND pointer_receiver bind_stmt_list { $$ = ast_make_bind_def ($2, $3); }

signature: '(' ')' { $$ = ast_make_signature (); }
| '(' parameter_list optional_semicolon ')' { $$ = $2; }

parameter_list: parameter { $$ = ast_add_child (ast_make_signature (), $1); }
| parameter_list ';' parameter { $$ = ast_add_child ($1, $3); }

parameter: identifier_list type_spec { $$ = ast_make_identifier_list_type_spec ($1, $2); }

optional_semicolon: /* Empty. */
| ';'

pointer_receiver: '(' identifier '#' identifier ')' { $$ = ast_make_pointer_receiver ($2, $4); }

bind_stmt_list: '{' bind_inner_stmt_list '}' { $$ = $2; }

bind_inner_stmt_list: /* empty */ { $$ = ast_make_bind_list_stmt (); }
| bind_inner_stmt_list bind_stmt { $$ = ast_add_child ($1, $2); }

bind_stmt: lvalue ARROW lvalue ';' { $$ = ast_make_bind_stmt ($1, $3); }

stmt_list: '{' inner_stmt_list '}' { $$ = $2; }

inner_stmt_list: /* empty */ { $$ = ast_make_stmt_list (); }
| inner_stmt_list stmt { $$ = ast_add_child ($1, $2); }

stmt: expr_stmt { $$ = $1; }
| var_stmt { $$ = $1; }
| assignment_stmt { $$ = $1; }
| trigger_stmt { $$ = $1; }
| stmt_list { $$ = $1; }

trigger_stmt: TRIGGER optional_port_call_list stmt_list { $$ = ast_make_trigger_stmt ($2, ast_add_child ($3, ast_make_stmt_list ())); }

optional_port_call_list: /* Empty. */ { $$ = ast_make_expression_list (); }
| port_call_list { $$ = $1; }

port_call_list: port_call { $$ = ast_add_child (ast_make_expression_list (), $1); }
| port_call_list ',' port_call { unimplemented; }

port_call: identifier '(' optional_expr_list ')' { $$ = ast_make_port_call ($1, $3); }

optional_expr_list: /* Empty. */ { $$ = ast_make_expression_list (); }
| expr_list { $$ = $1; }

expr_list: rvalue { $$ = ast_add_child (ast_make_expression_list (), $1); }
| expr_list ',' rvalue { unimplemented; }

expr_stmt: rvalue ';' {
  $$ = ast_make_expr_stmt ($1);
 }

var_stmt: VAR identifier_list type_spec ';' { $$ = ast_make_var_stmt ($2, $3); }

assignment_stmt: lvalue '=' rvalue ';' { $$ = ast_make_assignment_stmt ($1, $3); }

identifier_list: identifier { $$ = ast_add_child (ast_make_identifier_list (), $1); }
| identifier_list ',' identifier { $$ = ast_add_child ($1, $3); }

identifier: IDENTIFIER { $$ = ast_make_identifier ($1); }

type_spec: identifier { $$ = ast_make_identifier_type_spec ($1); }
| COMPONENT '{' field_list '}' { $$ = ast_make_component_type_spec ($3); }
| PORT signature { $$ = ast_make_port ($2); }

field_list: /* empty */ { $$ = ast_make_field_list (); }
| field_list identifier_list type_spec ';' { $$ = ast_add_child ($1, ast_make_identifier_list_type_spec ($2, $3)); }

rvalue: or_expr { $$ = $1; }

or_expr: and_expr { $$ = $1; }
| and_expr LOGIC_OR or_expr { $$ = ast_make_logic_or ($1, $3); }

and_expr: unary_expr { $$ = $1; }
| unary_expr LOGIC_AND and_expr { $$ = ast_make_logic_and ($1, $3); }

unary_expr: primary_expr { $$ = $1; }
| unary_expr '!' { $$ = ast_make_logic_not ($1); }

primary_expr: lvalue { $$ = $1; }
| primary_expr '(' optional_expr_list ')' { $$ = ast_make_call_expr ($1, $3); }

lvalue: identifier { $$ = ast_make_identifier_expr ($1); }
| lvalue '.' identifier { $$ = ast_make_select ($1, $3); }
| unary_expr '#' { $$ = ast_make_dereference ($1); }

%%
