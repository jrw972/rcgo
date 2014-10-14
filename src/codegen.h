#ifndef codegen_h
#define codegen_h

#include "ast.h"
#include "debug.h"

int generate_code (FILE * file, ast_t * root);

#endif /* codegen_h */
