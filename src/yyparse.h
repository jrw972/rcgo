#ifndef yyparse_h
#define yyparse_h

#include "node.h"

extern node_t *root;

int yyparse (void);
void yyerror (char const *);

#endif /* yyparse_h */
