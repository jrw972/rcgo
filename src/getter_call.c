#include "getter_call.h"
#include "util.h"
#include "debug.h"

struct getter_call_t {
  VECTOR_DECL (fields, field_t *);
};

getter_call_t*
getter_call_make (void)
{
  getter_call_t* gc = xmalloc (sizeof (getter_call_t));
  VECTOR_INIT (gc->fields, field_t *, 0, NULL);
  return gc;
}

void
getter_call_add_field (getter_call_t* gc,
                       field_t* field)
{
  VECTOR_PUSH (gc->fields, field_t*, field);
}
