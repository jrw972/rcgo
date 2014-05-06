%{
#include "scanner.h"
#include "yyparse.h"
#include "debug.h"
%}

%union { char* identifier; }
%token <identifier> IDENTIFIER
%destructor { free ($$); } <identifier>

%union { node_t* node; }
%type <node> and_expr
%type <node> assignment_stmt
%type <node> expr_stmt
%type <node> identifier
%type <node> identifier_list
%type <node> lvalue
%type <node> print_stmt
%type <node> or_expr
%type <node> primary_expr
%type <node> rvalue
%type <node> stmt
%type <node> stmt_list
%type <node> type_spec
%type <node> unary_expr
%type <node> var_stmt
%destructor { node_free ($$); } <node>

%token LOGIC_AND LOGIC_OR PRINT VAR

%%

top: stmt_list { root = $1; }

stmt_list: stmt { $$ = node_add_child (node_make_list_stmt (), $1); }
| stmt_list stmt { $$ = node_add_child ($1, $2); }

stmt: expr_stmt { $$ = $1; }
| var_stmt { $$ = $1; }
| assignment_stmt { $$ = $1; }
| print_stmt { $$ = $1; }

expr_stmt: rvalue ';' {
  $$ = node_make_expr_stmt ($1);
 }

print_stmt: PRINT rvalue ';' {
  $$ = node_make_print_stmt ($2);
 }

var_stmt: VAR identifier_list type_spec ';' { $$ = node_make_var_stmt ($2, $3); }

assignment_stmt: lvalue '=' rvalue ';' { $$ = node_make_assignment_stmt ($1, $3); }

identifier_list: identifier { $$ = node_add_child (node_make_identifier_list (), $1); }
| identifier_list ',' identifier { $$ = node_add_child ($1, $3); }

identifier: IDENTIFIER { $$ = node_make_identifier ($1); }

type_spec: IDENTIFIER { $$ = node_make_identifier_type_spec ($1); }

rvalue: or_expr { $$ = $1; }

or_expr: and_expr { $$ = $1; }
| and_expr LOGIC_OR or_expr { $$ = node_make_logic_or ($1, $3); }

and_expr: unary_expr { $$ = $1; }
| unary_expr LOGIC_AND and_expr { $$ = node_make_logic_and ($1, $3); }

unary_expr: primary_expr { $$ = $1; }
| '!' unary_expr { $$ = node_make_logic_not ($2); }

primary_expr: lvalue { $$ = node_make_implicit_dereference ($1); }

lvalue: IDENTIFIER { $$ = node_make_identifier_expr ($1); }

%%
