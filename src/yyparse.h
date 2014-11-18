#ifndef yyparse_h
#define yyparse_h

#include "ast.h"
#include "rtstring.h"

extern ast_t *root;

int yyparse (void);
void yyerror (char const *);

#endif /* yyparse_h */
