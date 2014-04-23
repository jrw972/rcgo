#ifndef node_h
#define node_h

#include "strtab.h"
#include "semval.h"

typedef struct node_t node_t;

typedef enum
{
  Dereference,
  Identifier,
  LogicAnd,
  LogicNot,
  LogicOr,
  ToTypedValue,
} NodeType;

node_t *node_make_identifier (string_t identifier);

string_t node_identifier (node_t * node);

node_t *node_make_dereference (node_t * child);

node_t *node_make_to_typed_value (node_t * child);

node_t *node_make_logic_not (node_t * child);

node_t *node_make_logic_and (node_t * left,
                             node_t * right);

node_t *node_make_logic_or (node_t * left,
                            node_t * right);

void node_free (node_t * node);

NodeType node_type (node_t * node);

node_t *node_child (node_t * node);

node_t *node_sibling (node_t * node);

void node_set_semval (node_t * node, semval_t semval);

semval_t node_get_semval (node_t * node);

#endif /* node_h */
