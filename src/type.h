#ifndef type_h
#define type_h

typedef struct type_t type_t;

typedef enum
{ Bool } TypeKind;

type_t *type_make_bool (void);

TypeKind type_kind (const type_t * type);

#endif /* type_h */
