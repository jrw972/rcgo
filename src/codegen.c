#include "codegen.h"
#include <stdio.h>
#include "debug.h"

static void
gen_type_spec (FILE * file, const type_t* type)
{
  /*
    If the type has a name, then use the name relying on a previous typedef.
    Otherwise, dump it out.
  */
  if (type_is_named (type)) {
    fprintf (file, "%s", get (type_get_name (type)));
  }
  else {
    switch (type_kind (type))
      {
      case TypeUndefined:
        bug ("undefined type in code generation");
      case TypeBool:
        unimplemented;
        fprintf (file, "bool");
        break;
      case TypeComponent:
        unimplemented;
        //printf ("struct %s { CONTENTS };\n", get (id));
      case TypePointerToImmutable:
        unimplemented;
      case TypePointerToMutable:
        unimplemented;
      }
  }
}

static const char *
get_format (const type_t * type)
{
  switch (type_kind (type))
    {
    case TypeUndefined:
      bug ("undefined type in code generation");
    case TypeBool:
      return "%d";
    case TypeComponent:
      unimplemented;
    case TypePointerToImmutable:
      unimplemented;
    case TypePointerToMutable:
      unimplemented;
    }
  bug ("unhandled case");
}

static void
gen_expr (node_t* node, FILE* file)
{
  fprintf (file, "(");

  switch (node_expr_type (node)) {
  case ExplicitDereferenceExpr:
    fprintf (file, "*");
    gen_expr (node_child (node), file);
    break;

  case IdentifierExpr:
    fprintf (file, "%s", get (node_identifier_expr_identifier (node)));
    break;

  case ImplicitDereferenceExpr:
    /* Do nothing. */
    gen_expr (node_child (node), file);
    break;

  case LogicAndExpr:
    {
      node_t *left = node_child (node);
      node_t *right = node_sibling (left);
      gen_expr (left, file);
      fprintf (file, " && ");
      gen_expr (right, file);
    }
    break;

  case LogicNotExpr:
    fprintf (file, "!");
    gen_expr (node_child (node), file);
    break;

  case LogicOrExpr:
    {
      node_t *left = node_child (node);
      node_t *right = node_sibling (left);
      gen_expr (left, file);
      fprintf (file, " || ");
      gen_expr (right, file);
    }
    break;

  case SelectExpr:
    {
      gen_expr (node_child (node), file);
      fprintf (file, ".%s", get (node_get_select_identifier (node)));
    }
    break;
  }

  fprintf (file, ")");
}

static void
gen_stmt (node_t* node, FILE* file)
{
  switch (node_stmt_type (node)) {
  case AssignmentStmt:
    {
      node_t *left = node_child (node);
      node_t *right = node_sibling (left);
      gen_expr (left, file);
      fprintf (file, " = ");
      gen_expr (right, file);
      fprintf (file, ";\n");
    }
    break;

  case ExprStmt:
    gen_expr (node_child (node), file);
    fprintf (file, ";\n");
    break;

  case ListStmt:
    {
      fprintf (file, "{\n");
      node_t *child;
      for (child = node_child (node); child != NULL;
           child = node_sibling (child))
        {
          gen_stmt (child, file);
        }
      fprintf (file, "}\n");
    }
    break;

  case PrintStmt:
    {
      node_t *child = node_child (node);
      abstract_value_t a = semval_get_value (node_get_semval (child));
      switch (abstract_value_kind (a))
        {
        case UntypedValue:
          {
            untyped_value_t u = abstract_value_get_untyped_value (a);
            switch (untyped_value_kind (u))
              {
              case UntypedUndefined:
                bug ("undefined untyped value in code generation");
              case UntypedBool:
                if (untyped_value_bool_value (u))
                  {
                    fprintf (file, "printf (\"1\");\n");
                  }
                else
                  {
                    fprintf (file, "printf (\"0\");\n");
                  }
                break;
              }
          }
          break;

        case TypedValue:
          unimplemented;

        case Typed:
          fprintf (file, "printf (\"");
          fprintf (file, "%s", get_format (abstract_value_get_typed (a)));
          fprintf (file, "\", ");
          gen_expr (child, file);
          fprintf (file, ");\n");
          break;

        case UndefinedValue:
          bug ("undefined value in code generation");
        }
    }
    break;

  case TriggerStmt:
    {
      /* Just print the body for now. */
      node_t* body = node_child (node);
      gen_stmt (body, file);
    }
    break;

  case VarStmt:
    {
      node_t *identifier_list = node_child (node);
      node_t *type_spec = node_sibling (identifier_list);

      /* Iterate over the identifier list. */
      node_t *child;
      for (child = node_child (identifier_list); child != NULL;
           child = node_sibling (child))
        {
          gen_type_spec (file, node_get_type (type_spec));
          fprintf (file, " %s;\n", get (node_identifier (child)));
        }
    }
    break;

  }
}

static void
gen_type_defs (FILE * file, node_t* node)
{
  switch (node_def_type (node)) {
  case ActionDef:
    /* Do nothing. */
    break;

  case InstanceDef:
    /* Do nothing. */
    break;

  case ListDef:
    {
      NODE_FOREACH (child, node) {
        gen_type_defs (file, child);
      }
    }
    break;

  case TypeDef:
    {
      node_t* identifier = node_child (node);
      node_t* type_spec = node_sibling (identifier);
      const type_t* type = node_get_type (type_spec);
      string_t name = type_get_name (type);
      switch (type_kind (type)) {
      case TypeUndefined:
        unimplemented;
      case TypeBool:
        unimplemented;
      case TypeComponent:
        {
          const field_t* field;
          fprintf (file, "typedef struct %s %s;\n", get (name), get (name));
          fprintf (file, "struct %s {\n", get (name));
          /* Embed an instance. */
          fprintf (file, "instance_t instance;\n");
          for (field = type_field_list (type); field != NULL; field = field_next (field)) {
            gen_type_spec (file, field_type (field));
            fprintf (file, " %s;\n", get (field_name (field)));
          }
          fprintf (file, "};\n");
        }
        break;
      case TypePointerToImmutable:
        unimplemented;
      case TypePointerToMutable:
        unimplemented;
      }
    }
    break;
  }
}

static void
print_pre (FILE * file,
           const type_t* type,
           size_t action_number)
{
  fprintf (file, "%s_%zd_pre", get (type_get_name (type)), action_number);
}

static void
print_post (FILE * file,
            const type_t* type,
            size_t action_number)
{
  fprintf (file, "%s_%zd_post", get (type_get_name (type)), action_number);
}

static void
gen_action_defs (FILE * file, node_t* node)
{
  switch (node_def_type (node)) {
  case ActionDef:
    {
      size_t action_number = node_get_action_number (node);
      node_t* receiver = node_child (node);
      const type_t* type = node_receiver_get_type (receiver);
      string_t type_id = type_get_name (type);
      string_t this_id = node_identifier (node_child (receiver));
      node_t* precondition = node_sibling (receiver);
      node_t* body = node_sibling (precondition);

      /* Output the precondition as a function. */
      fprintf (file, "static bool ");
      print_pre (file, type, action_number);
      fprintf (file, " (const %s * %s) { return ", get (type_id), get (this_id));
      gen_expr (precondition, file);
      fprintf (file, "; }\n");

      /* Output the body as a function. */
      fprintf (file, "static void ");
      print_post (file, type, action_number);
      fprintf (file, " (%s * %s)", get (type_id), get (this_id));
      gen_stmt (body, file);
    }
    break;

  case InstanceDef:
    /* Do nothing. */
    break;

  case ListDef:
    {
      NODE_FOREACH (child, node) {
        gen_action_defs (file, child);
      }
    }
    break;

  case TypeDef:
    /* Do nothing. */
    break;
  }
}

static void
gen_pre_post_tables (FILE * file, node_t* node)
{
  switch (node_def_type (node)) {
  case ActionDef:
    /* Do nothing. */
    break;

  case InstanceDef:
    /* Do nothing. */
    break;

  case ListDef:
    {
      NODE_FOREACH (child, node) {
        gen_pre_post_tables (file, child);
      }
    }
    break;

  case TypeDef:
    {
      node_t* identifier = node_child (node);
      node_t* type_spec = node_sibling (identifier);
      const type_t* type = node_get_type (type_spec);
      if (type_is_component (type)) {
        size_t i;
        size_t count = type_action_count (type);
        string_t type_name = type_get_name (type);
        fprintf (file, "static action_t %s_actions[%zd] = {\n", get (type_name), count + 1);
        for (i = 0; i != count; ++i) {
          if (i != 0) {
            fprintf (file, ",\n");
          }
          fprintf (file, "{ (pre_t)");
          print_pre (file, type, i);
          fprintf (file, ", (post_t)");
          print_post (file, type, i);
          fprintf (file, " }");
        }
        fprintf (file, ",\n{ 0, 0 }\n};\n");

        /* Generate a function that initializes the component instance. */
        /* TODO:  Recursive initialization. */
        fprintf (file, "static void %s_init (%s * instance) {\n", get (type_name), get (type_name));
        fprintf (file, "instance->instance.actions = %s_actions;\n", get (type_name));
        fprintf (file, "}\n");
      }
    }
    break;
  }
}

static void
gen_instances (FILE * file, node_t* node)
{
  switch (node_def_type (node)) {
  case ActionDef:
    /* Do nothing. */
    break;

  case InstanceDef:
    {
      node_t* instance_id = node_child (node);
      string_t instance_str = node_identifier (instance_id);
      node_t* type_id = node_sibling (instance_id);
      string_t type_str = node_identifier (type_id);

      fprintf (file, "static %s %s;\n", get (type_str), get (instance_str));

      /* Emit a thunk to initialize the instance. */
      fprintf (file, "__attribute__((constructor)) static void %s_init (void) {\n", get (instance_str));
      fprintf (file, "%s_init (&%s);\n", get (type_str), get (instance_str));
      fprintf (file, "schedule (&%s.instance);\n", get (instance_str));
      fprintf (file, "}\n");
    }

    break;

  case ListDef:
    {
      NODE_FOREACH (child, node) {
        gen_instances (file, child);
      }
    }
    break;

  case TypeDef:
    /* Do nothing. */
    break;
  }
}

int
generate_code (FILE * file, node_t * node)
{
  if (debug) {
    node_print (node, 0);
  }

  /* Includes. */
  fputs ("#include <fbu.h>\n", file);

  /* Typedefs. */
  gen_type_defs (file, node);

  /* Action defs. */
  gen_action_defs (file, node);

  /* Generate the list of pre/posts for each component type. */
  gen_pre_post_tables (file, node);

  /* Generate the list of instances. */
  gen_instances (file, node);

  fflush (file);
  fclose (file);

  return 0;
}
