#include "semantic.hpp"
#include "type.hpp"
#include "ast.hpp"
#include <error.h>
#include "symbol.hpp"
#include "parameter.hpp"

typed_value_t
process_array_dimension (ast_t* ptr)
{
  typed_value_t tv = type_check_expr (ptr);

  if (!type_is_integral (tv.type))
    {
      error_at_line (-1, 0, ptr->location.file, ptr->location.line,
                     "array dimension is not integral");
    }

  if (!tv.value.present)
    {
      error_at_line (-1, 0, ptr->location.file, ptr->location.line,
                     "array dimension is not constant");
    }

  if (tv.value.integral_value (tv.type) < 0)
    {
      error_at_line (-1, 0, ptr->location.file, ptr->location.line,
                     "array dimension is negative");
    }

  return tv;
}

void
check_port_reaction_signature (const signature_type_t* signature)
{
  for (signature_type_t::const_iterator pos = signature->begin (), limit = signature->end ();
       pos != limit;
       ++pos)
    {
      const parameter_t* parameter = *pos;
      if (!parameter->value.is_foreign_safe ())
        {
          error_at_line (-1, 0, parameter->defining_node->location.file, parameter->defining_node->location.line,
                         "signature leaks pointers");
        }
    }
}

const type_t *
process_type_spec (ast_t * node, bool force_identifiers, bool is_component, named_type_t* named_type)
{
  struct type_spec_visitor_t : public ast_visitor_t
  {
    const type_t* type;
    bool force_identifiers;
    bool is_component;
    named_type_t* named_type;

    type_spec_visitor_t (bool fi,
                         bool ic,
                         named_type_t* nt)
      : type (NULL)
      , force_identifiers (fi)
      , is_component (ic)
      , named_type (nt)
    { }

    void default_action (ast_t& node)
    {
      ast_not_reached (node);
    }

    void visit (ast_array_type_spec_t& node)
    {
      typed_value_t dimension = process_array_dimension (node.dimension ());
      const type_t* base_type = process_type_spec (node.base_type (), true);
      type = new array_type_t (dimension.value.integral_value (dimension.type), base_type);
    }

    void visit (ast_component_type_spec_t& node)
    {
      type = new component_type_t (type_cast<field_list_type_t> (process_type_spec (node.child (), true, true)));
    }

    void visit (ast_empty_type_spec_t& node)
    {
      type = void_type_t::instance ();
    }

    void visit (ast_enum_type_spec_t& node)
    {
      type = new enum_type_t ();

      ast_t* value = node.values ();
      size_t e = 0;
      for (ast_t::const_iterator pos = value->begin (), limit = value->end ();
           pos != limit;
           ++pos, ++e)
        {
          std::string id = ast_get_identifier (*pos);
          if (node.symtab->find_current (id) != NULL)
            {
              error_at_line (-1, 0, (*pos)->location.file, (*pos)->location.line,
                             "%s is already defined in this scope", id.c_str ());
            }

          node.symtab->enter (symbol_make_typed_constant (id,
                                                          typed_value_t (named_type, e),
                                                          *pos));
        }
    }

    void visit (ast_field_list_type_spec_t& node)
    {
      field_list_type_t *field_list = new field_list_type_t (is_component);
      for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          ast_t* child = *pos;
          ast_identifier_list_type_spec_t* c = static_cast<ast_identifier_list_type_spec_t*> (child);
          ast_t *identifier_list = c->identifier_list ();
          ast_t *type_spec = c->type_spec ();
          const type_t *type = process_type_spec (type_spec, true);
          for (ast_t::const_iterator pos2 = identifier_list->begin (),
                 limit2 = identifier_list->end ();
               pos2 != limit2;
               ++pos2)
            {
              ast_t* id = *pos2;
              const std::string& identifier = ast_get_identifier (id);
              const type_t *field = type_select (field_list, identifier);
              if (field == NULL)
                {
                  field_list->append (identifier, type);
                }
              else
                {
                  error_at_line (-1, 0, id->location.file, id->location.line,
                                 "duplicate field name %s", identifier.c_str ());
                }
            }
        }
      type = field_list;
    }

    void visit (ast_heap_type_spec_t& node)
    {
      type = new heap_type_t (process_type_spec (node.child (), false));
    }

    void visit (ast_identifier_type_spec_t& node)
    {
      ast_t *child = node.child ();
      const std::string& identifier = ast_get_identifier (child);
      symbol_t *symbol;
      if (force_identifiers)
        {
          symbol = lookup_force (child, identifier);
        }
      else
        {
          symbol = lookup_no_force (child, identifier);
        }

      if (symbol_kind (symbol) != SymbolType)
        {
          error_at_line (-1, 0, child->location.file, child->location.line,
                         "%s does not refer to a type", identifier.c_str ());
        }
      type = symbol_get_type_type (symbol);
    }

    void visit (ast_pointer_type_spec_t& node)
    {
      type = pointer_type_t::make (process_type_spec (node.child (), false));
    }

    void visit (ast_push_port_type_spec_t& node)
    {
      const signature_type_t* signature = type_cast<signature_type_t> (process_type_spec (node.signature (), true));
      check_port_reaction_signature (signature);
      type = new push_port_type_t (signature);
    }

    void visit (ast_pull_port_type_spec_t& node)
    {
      const signature_type_t* signature = type_cast<signature_type_t> (process_type_spec (node.signature (), true));
      check_port_reaction_signature (signature);
      const type_t* return_type = process_type_spec (node.return_type (), true);
      typed_value_t return_value = typed_value_t::make_value (return_type, typed_value_t::STACK, IMMUTABLE, FOREIGN);
      parameter_t* return_parameter = new parameter_t (node.return_type (),
                                                       "0return",
                                                       return_value,
                                                       false);

      if (!return_value.is_foreign_safe ())
        {
          error_at_line (-1, 0, node.return_type ()->location.file, node.return_type ()->location.line,
                         "return type leaks pointers");
        }
      type = new pull_port_type_t (signature, return_parameter);
    }

    void visit (ast_signature_type_spec_t& node)
    {
      signature_type_t *signature = new signature_type_t ();
      for (ast_t::iterator pos1 = node.begin (), limit1 = node.end ();
           pos1 != limit1;
           ++pos1)
        {
          ast_identifier_list_type_spec_t* child = static_cast<ast_identifier_list_type_spec_t*> (*pos1);
          ast_t *identifier_list = child->identifier_list ();
          ast_t *type_spec = child->type_spec ();
          const type_t *type = process_type_spec (type_spec, true);
          for (ast_t::iterator pos2 = identifier_list->begin (), limit2 = identifier_list->end ();
               pos2 != limit2;
               ++pos2)
            {
              ast_t* id = *pos2;
              const std::string& identifier = ast_get_identifier (id);
              const parameter_t *parameter = signature->find (identifier);
              if (parameter == NULL)
                {
                  typed_value_t tv = typed_value_t::make_value (type,
                                                                typed_value_t::STACK,
                                                                MUTABLE,
                                                                child->dereference_mutability);
                  signature->append (new parameter_t (id, identifier, tv, false));
                }
              else
                {
                  error_at_line (-1, 0, id->location.file, id->location.line,
                                 "duplicate parameter name %s",
                                 identifier.c_str ());
                }
            }
        }
      type = signature;
    }

    void visit (ast_struct_type_spec_t& node)
    {
      type = new struct_type_t (type_cast<field_list_type_t> (process_type_spec (node.child (), true)));
    }
  };
  type_spec_visitor_t type_spec_visitor (force_identifiers, is_component, named_type);
  node->accept (type_spec_visitor);
  return type_spec_visitor.type;
}
