#ifndef rc_src_yyparse_hpp
#define rc_src_yyparse_hpp

#include "ast.hpp"
#include "type.hpp"

#define YYLTYPE size_t

#define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current)   = YYRHSLOC (Rhs, 1);        \
        }                                                               \
      else                                                              \
        {                                                               \
         (Current) =  YYRHSLOC (Rhs, 0);                                \
        }                                                               \
    while (0)

extern ast::Node* root;

int yyparse (void);
void yyerror (char const *);

#endif // rc_src_yyparse_hpp
