#include "codegen.h"
#include "debug.h"

static void
print_action_pre (FILE * file, const type_t * type, size_t action_number)
{
  fprintf (file, "%s_%zd_pre", get (type_get_name (type)), action_number);
}

static void
print_action_post (FILE * file, const type_t * type, size_t action_number)
{
  fprintf (file, "%s_%zd_post", get (type_get_name (type)), action_number);
}

static void
print_bind (FILE * file, const type_t * type, size_t bind_number)
{
  fprintf (file, "%s_%zd_bind", get (type_get_name (type)), bind_number);
}

static void
print_reaction_post (FILE * file, const action_t* reaction)
{
  fprintf (file, "%s_%s_post", get (type_get_name (reaction_component_type (reaction))), get (reaction_name (reaction)));
}

/* Generate a type specifier. */
static void
gen_type_spec (FILE * file, const type_t * type)
{
  /*
     If the type has a name, then use the name relying on a previous typedef.
     Otherwise, dump it out.
   */
  if (type_is_named (type))
    {
      fprintf (file, "%s", get (type_get_name (type)));
    }
  else
    {
      switch (type_kind (type))
	{
	case TypeUndefined:
	  bug ("undefined type in code generation");
        case TypeVoid:
          unimplemented;
	case TypeBool:
	  unimplemented;
	  fprintf (file, "bool");
	  break;
	case TypeComponent:
	  unimplemented;
	  //printf ("struct %s { CONTENTS };\n", get (id));
	case TypePointer:
	  unimplemented;
        case TypePort:
          fprintf (file, "port_t*");
          break;
        case TypeReaction:
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
    case TypeVoid:
      unimplemented;
    case TypeBool:
      return "%d";
    case TypeComponent:
      unimplemented;
    case TypePointer:
      unimplemented;
    case TypePort:
      unimplemented;
    case TypeReaction:
      unimplemented;
    }
  bug ("unhandled case");
}

static void
gen_expr (FILE * file, node_t * node)
{
  fprintf (file, "(");

  switch (node_expr_type (node))
    {
    case CallExpr:
      unimplemented;

    case ExplicitDereferenceExpr:
      fprintf (file, "*");
      gen_expr (file, node_child (node));
      break;

    case IdentifierExpr:
      fprintf (file, "%s", get (node_identifier_expr_identifier (node)));
      break;

    case ImplicitDereferenceExpr:
      /* Do nothing. */
      gen_expr (file, node_child (node));
      break;

    case LogicAndExpr:
      {
	node_t *left = node_child (node);
	node_t *right = node_sibling (left);
	gen_expr (file, left);
	fprintf (file, " && ");
	gen_expr (file, right);
      }
      break;

    case LogicNotExpr:
      fprintf (file, "!");
      gen_expr (file, node_child (node));
      break;

    case LogicOrExpr:
      {
	node_t *left = node_child (node);
	node_t *right = node_sibling (left);
	gen_expr (file, left);
	fprintf (file, " || ");
	gen_expr (file, right);
      }
      break;

    case SelectExpr:
      {
        semval_t semval = node_get_semval (node);
        action_t* reaction = semval_get_reaction (semval);

        if (reaction != NULL) {
          print_reaction_post (file, reaction);
        }
        else {
          gen_expr (file, node_child (node));
          fprintf (file, ".%s", get (node_get_select_identifier (node)));
        }
      }
      break;
    }

  fprintf (file, ")");
}

static void
gen_stmt (FILE * file, node_t * node)
{
  switch (node_stmt_type (node))
    {
    case AssignmentStmt:
      {
	node_t *left = node_child (node);
	node_t *right = node_sibling (left);
	gen_expr (file, left);
	fprintf (file, " = ");
	gen_expr (file, right);
	fprintf (file, ";\n");
      }
      break;

    case BindListStmt:
      {
	fprintf (file, "{\n");
        NODE_FOREACH (child, node)
          {
            gen_stmt (file, child);
          }
	fprintf (file, "}\n");
      }
      break;

    case BindStmt:
      {
        node_t* output = node_child (node);
        node_t* input = node_sibling (output);

        fprintf (file, "bind_port_reaction (&");
        gen_expr (file, output);
        fprintf (file, ", ");
        gen_expr (file, input);
        fprintf (file, ");\n");
      }
      break;

    case ExprStmt:
      gen_expr (file, node_child (node));
      fprintf (file, ";\n");
      break;

    case ListStmt:
      {
	fprintf (file, "{\n");
        NODE_FOREACH (child, node)
	  {
	    gen_stmt (file, child);
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
	    gen_expr (file, child);
	    fprintf (file, ");\n");
	    break;

	  case UndefinedValue:
	    bug ("undefined value in code generation");
	  }
      }
      fprintf (file, "printf (\"\\n\");\n");
      break;

    case TriggerStmt:
      {
        /* TODO */
	/* Just print the body for now. */
        node_t* expr_list = node_child (node);
	node_t *body = node_sibling (expr_list);
	gen_stmt (file, body);
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
gen_type_defs (FILE * file, node_t * node)
{
  switch (node_def_type (node))
    {
    case ActionDef:
      /* Do nothing. */
      break;

    case BindDef:
      /* Do nothing. */
      break;

    case InstanceDef:
      /* Do nothing. */
      break;

    case ListDef:
      {
	NODE_FOREACH (child, node)
	{
	  gen_type_defs (file, child);
	}
      }
      break;

    case ReactionDef:
      /* Do nothing. */
      break;

    case TypeDef:
      {
	node_t *identifier = node_child (node);
	node_t *type_spec = node_sibling (identifier);
	const type_t *type = node_get_type (type_spec);
	string_t name = type_get_name (type);
	switch (type_kind (type))
	  {
	  case TypeUndefined:
	    unimplemented;
          case TypeVoid:
            unimplemented;
	  case TypeBool:
	    unimplemented;
	  case TypeComponent:
	    {
	      const field_t *field;
	      fprintf (file, "typedef struct %s %s;\n", get (name),
		       get (name));
	      fprintf (file, "struct %s {\n", get (name));
	      /* Embed an instance. */
	      fprintf (file, "instance_t instance;\n");
	      for (field = type_field_list (type); field != NULL;
		   field = field_next (field))
		{
		  gen_type_spec (file, field_type (field));
		  fprintf (file, " %s;\n", get (field_name (field)));
		}
	      fprintf (file, "};\n");
	    }
	    break;
	  case TypePointer:
	    unimplemented;
          case TypePort:
            unimplemented;
        case TypeReaction:
          unimplemented;

	  }
      }
      break;
    }
}

static void
print_signature (FILE * file, signature_t* signature)
{
  size_t idx, limit;
  for (idx = 0, limit = signature_size (signature); idx != limit; ++idx)
    {
      const type_t* type = signature_type (signature, idx);
      string_t name = signature_name (signature, idx);
      fprintf (file, ", ");
      gen_type_spec (file, type);
      fprintf (file, " %s", get (name));
  }
}

static void
gen_action_defs (FILE * file, node_t * node)
{
  switch (node_def_type (node))
    {
    case ActionDef:
      {
	action_t* action = node_get_action (node);
	node_t *receiver = node_child (node);
	const type_t *type = node_receiver_get_type (receiver);
	string_t type_id = type_get_name (type);
	string_t this_id = node_identifier (node_child (receiver));
	node_t *precondition = node_sibling (receiver);
	node_t *body = node_sibling (precondition);

	/* Output the precondition as a function. */
	fprintf (file, "static bool ");
	print_action_pre (file, type, action_number (action));
	fprintf (file, " (const %s * %s) { return ", get (type_id),
		 get (this_id));
	gen_expr (file, precondition);
	fprintf (file, "; }\n");

	/* Output the body as a function. */
	fprintf (file, "static void ");
	print_action_post (file, type, action_number (action));
	fprintf (file, " (%s * %s)", get (type_id), get (this_id));
	gen_stmt (file, body);
      }
      break;

    case BindDef:
      {
        size_t bind_number = node_get_bind_number (node);
	node_t *receiver = node_child (node);
	const type_t *type = node_receiver_get_type (receiver);
	string_t type_id = type_get_name (type);
	string_t this_id = node_identifier (node_child (receiver));
	node_t *body = node_sibling (receiver);

	/* Output the body as a function. */
	fprintf (file, "static void ");
	print_bind (file, type, bind_number);
	fprintf (file, " (%s * %s)", get (type_id), get (this_id));
	gen_stmt (file, body);
      }
      break;

    case InstanceDef:
      /* Do nothing. */
      break;

    case ListDef:
      {
	NODE_FOREACH (child, node)
	{
	  gen_action_defs (file, child);
	}
      }
      break;

    case ReactionDef:
      {
	node_t *receiver = node_child (node);
        node_t *identifier = node_sibling (receiver);
        node_t *signature = node_sibling (identifier);
        node_t *body = node_sibling (signature);

        action_t* reaction = node_get_action (node);
        const type_t* type = reaction_component_type (reaction);
	string_t type_id = type_get_name (type);
	string_t this_id = node_identifier (node_child (receiver));

	/* Output the body as a function. */
	fprintf (file, "static void ");
	print_reaction_post (file, reaction);
	fprintf (file, " (%s * %s", get (type_id), get (this_id));
        print_signature (file, node_get_signature (signature));
        fprintf (file, ")");
	gen_stmt (file, body);
      }
      break;

    case TypeDef:
      /* Do nothing. */
      break;
    }
}

static void
gen_pre_post_tables (FILE * file, node_t * node)
{
  switch (node_def_type (node))
    {
    case ActionDef:
      /* Do nothing. */
      break;

    case BindDef:
      /* Do nothing. */
      break;

    case InstanceDef:
      /* Do nothing. */
      break;

    case ListDef:
      {
	NODE_FOREACH (child, node)
	{
	  gen_pre_post_tables (file, child);
	}
      }
      break;

    case ReactionDef:
      /* Do nothing. */
      break;

    case TypeDef:
      {
	node_t *identifier = node_child (node);
	node_t *type_spec = node_sibling (identifier);
	const type_t *type = node_get_type (type_spec);
	if (type_is_component (type))
	  {
	    bool flag = false;
	    size_t count = type_action_count (type);
	    string_t type_name = type_get_name (type);
	    fprintf (file, "static action_t %s_actions[%zd] = {\n",
		     get (type_name), count + 1);
            action_t *action_pos, *action_limit;
            for (action_pos = type_actions_begin (type), action_limit = type_actions_end (type);
                 action_pos != action_limit;
                 action_pos = action_next (action_pos))
              {
                if (action_is_action (action_pos))
                  {
                    if (flag)
                      {
                        fprintf (file, ",\n");
                      }
                    fprintf (file, "{ (pre_t)");
                    size_t number = action_number (action_pos);
                    print_action_pre (file, type, number);
                    fprintf (file, ", (post_t)");
                    print_action_post (file, type, number);
                    fprintf (file, " }");
                    flag = true;
                  }
	      }
            if (flag) {
              fprintf (file, ",\n");
            }
            fprintf (file, "{ 0, 0 }\n};\n");

	    /* Generate a function that initializes the component instance. */
	    fprintf (file, "static void %s_init (%s * instance) {\n",
		     get (type_name), get (type_name));
            /* Initialize the actions. */
	    fprintf (file, "instance->instance.actions = %s_actions;\n",
		     get (type_name));
            /* Iniialize sub-components. */
            const field_t* field;
            for (field = type_field_list (type);
                 field != NULL;
                 field = field_next (field)) {
              const type_t* ft = field_type (field);
              if (type_is_component (ft)) {
                fprintf (file, "%s_init (&(instance->%s));\n", get (type_get_name (ft)), get (field_name (field)));
              }
            }
            /* Emit code to bind ports. */
            size_t bind_number;
            for (bind_number = 0;
                 bind_number != type_bind_count (type);
                 ++bind_number) {
              print_bind (file, type, bind_number);
              fprintf (file, "(instance);\n");
            }
            /* Schedule the instance. */
            fprintf (file, "schedule (&(instance->instance));\n");
	    fprintf (file, "}\n");
	  }
      }
      break;
    }
}

static void
gen_instances (FILE * file, node_t * node)
{
  switch (node_def_type (node))
    {
    case ActionDef:
      /* Do nothing. */
      break;

    case BindDef:
      /* Do nothing. */
      break;

    case InstanceDef:
      {
	node_t *instance_id = node_child (node);
	string_t instance_str = node_identifier (instance_id);
	node_t *type_id = node_sibling (instance_id);
	string_t type_str = node_identifier (type_id);

	fprintf (file, "static %s %s;\n", get (type_str), get (instance_str));

	/* Emit a thunk to initialize the instance. */
	fprintf (file,
		 "__attribute__((constructor)) static void %s_init (void) {\n",
		 get (instance_str));
	fprintf (file, "%s_init (&%s);\n", get (type_str),
		 get (instance_str));
	fprintf (file, "}\n");
      }

      break;

    case ListDef:
      {
	NODE_FOREACH (child, node)
	{
	  gen_instances (file, child);
	}
      }
      break;

    case ReactionDef:
      /* Do nothing. */
      break;

    case TypeDef:
      /* Do nothing. */
      break;
    }
}

int
generate_code (FILE * file, node_t * node)
{
  if (debug)
    {
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
