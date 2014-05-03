#include "codegen.h"
#include <stdio.h>
#include "debug.h"

static void
generate_type_spec (node_t* node,
                    FILE * file)
{
  semval_t semval = node_get_semval (node);
  assert (semval_is_type (semval));
  type_t* type = semval_get_type (semval);

  switch (type_kind (type)) {
  case TypeUndefined:
    bug ("undefined type in code generation");
  case TypeBool:
    fprintf (file, "bool");
    break;
  }
}

static const char*
get_format (const type_t* type)
{
  switch (type_kind (type)) {
  case TypeUndefined:
    unimplemented;
  case TypeBool:
    return "%d";
  }
  bug ("unhandled case");
}

static void
gen (node_t * node, FILE * file)
{
  switch (node_type (node))
    {
    case AssignmentStmt:
      {
        node_t* left = node_child (node);
        node_t* right = node_sibling (left);
        gen (left, file);
        fprintf (file, " = ");
        gen (right, file);
        fprintf (file, ";\n");
      }
      break;

    case Dereference:
      /* Do nothing. */
      gen (node_child (node), file);
      break;

    case ExprStmt:
      gen (node_child (node), file);
      fprintf (file, ";\n");
      break;

    case Identifier:
      fprintf (file, "%s", get (node_identifier (node)));
      break;

    case List:
      {
        node_t* child;
        for (child = node_child (node); child != NULL; child = node_sibling (child)) {
          gen (child, file);
        }
      }
      break;

    case LogicAnd:
      {
        node_t* left = node_child (node);
        node_t* right = node_sibling (left);
        fprintf (file, "(");
        gen (left, file);
        fprintf (file, ")");
        fprintf (file, " && ");
        fprintf (file, "(");
        gen (right, file);
        fprintf (file, ")");
      }
      break;

    case LogicNot:
      fprintf (file, "!");
      fprintf (file, "(");
      gen (node_child (node), file);
      fprintf (file, ")");
      break;

    case LogicOr:
      {
        node_t* left = node_child (node);
        node_t* right = node_sibling (left);
        fprintf (file, "(");
        gen (left, file);
        fprintf (file, ")");
        fprintf (file, " || ");
        fprintf (file, "(");
        gen (right, file);
        fprintf (file, ")");
      }
      break;

    case PrintStmt:
      {
        node_t* child = node_child (node);
        abstract_value_t a = semval_get_value (node_get_semval (child));
        switch (abstract_value_kind (a)) {
        case UntypedValue:
          {
            untyped_value_t u = abstract_value_get_untyped_value (a);
            switch (untyped_value_kind (u)) {
            case UntypedUndefined:
              unimplemented;
            case UntypedBool:
              if (untyped_value_bool_value (u)) {
                fprintf (file, "printf (\"1\");\n");
              }
              else {
                fprintf (file, "printf (\"0\");\n");
              }
              break;
            }
          }
          break;
          unimplemented;
        case TypedValue:
          unimplemented;
        case Typed:
          fprintf (file, "printf (\"");
          fprintf (file, "%s", get_format (abstract_value_get_typed (a)));
          fprintf (file, "\", ");
          gen (child, file);
          fprintf (file, ");\n");
          break;
        case UndefinedValue:
          unimplemented;
        }
      }
      break;

    case VarStmt:
      {
        node_t* identifier_list = node_child (node);
        node_t* type_spec = node_sibling (identifier_list);

        /* Iterate over the identifier list. */
        node_t* child;
        for (child = node_child (identifier_list); child != NULL; child = node_sibling (child)) {
          generate_type_spec (type_spec, file);
          fprintf (file, " %s;\n", get (node_identifier (child)));
        }
      }
      break;
    }
}

int
generate_code (node_t * node, FILE * file)
{
  fputs ("#include <stdio.h>\n"
         "#include <stdbool.h>\n"
	 "int main (int argc, char** argv) {\n", file);
  gen (node, file);
  fputs ("return 0;\n" "}\n", file);

  fflush (file);
  fclose (file);

  return 0;
}
