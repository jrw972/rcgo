#include "codegen.h"
#include <stdio.h>
#include "debug.h"

void
gen (node_t * node, FILE * file)
{
  switch (node_type (node))
    {
    case Dereference:
      unimplemented;
    case Identifier:
      unimplemented;
    case LogicAnd:
      unimplemented;
    case LogicNot:
      unimplemented;
    case LogicOr:
      unimplemented;
    case ToTypedValue:
      {
	semval_t semval = node_get_semval (node);
	assert (semval_is_value (semval));
	abstract_value_t value = semval_get_value (semval);
	if (abstract_value_is_typed_value (value))
	  {
	    typed_value_t typed_value =
	      abstract_value_get_typed_value (value);
	    switch (type_kind (typed_value.type))
	      {
	      case Bool:
		fprintf (file, "%d\n", typed_value.bool_value);
		break;
	      }
	  }
	else
	  {
	    unimplemented;
	  }
      }
    }
}

int
generate_code (node_t * node, FILE * file)
{
  fputs ("#include <stdio.h>\n"
	 "int main (int argc, char** argv) {\n"
	 "printf (\"result of expression is %d\\n\", ", file);
  gen (node, file);
  fputs (");\n" "return 0;\n" "}\n", file);

  fflush (file);
  fclose (file);

  return 0;
}
