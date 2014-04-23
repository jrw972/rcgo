%{
#include "scanner.h"
#include "yyparse.h"
%}

%union { char* identifier; }
%token <identifier> IDENTIFIER
%destructor { free ($$); } <identifier>

%union { node_t* node; }
%type <node> lvalue rvalue unary_expr primary_expr and_expr or_expr
%destructor { node_free ($$); } <node>

%token LOGIC_AND LOGIC_OR

%%

top: rvalue { root = $1; }

rvalue: or_expr { $$ = $1; }

or_expr: and_expr { $$ = $1; }
| and_expr LOGIC_OR or_expr { $$ = node_make_logic_or ($1, $3); }

and_expr: unary_expr { $$ = $1; }
| unary_expr LOGIC_AND and_expr { $$ = node_make_logic_and ($1, $3); }

unary_expr: primary_expr { $$ = $1; }
| '!' unary_expr { $$ = node_make_logic_not ($2); }

primary_expr: lvalue { $$ = node_make_dereference ($1); }

lvalue: IDENTIFIER { $$ = node_make_identifier ($1); }

%%
