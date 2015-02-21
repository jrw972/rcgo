#include "semantic.hpp"
#include "type.hpp"
#include "ast.hpp"
#include <error.h>
#include "symbol.hpp"

size_t
process_array_dimension (ast_t::iterator ptr)
{
  type_check_rvalue (ptr);
  typed_value_t tv = ast_get_typed_value (*ptr);

  if (!tv.has_value)
    {
      error_at_line (-1, 0, (*ptr)->file, (*ptr)->line,
                     "array dimension is not constant");
    }

  struct visitor : public const_type_visitor_t
  {
    size_t dimension;
    ast_t* dimension_node;
    const typed_value_t& dimension_tv;

    visitor (ast_t* d, const typed_value_t& dt) : dimension_node (d), dimension_tv (dt) { }

    void default_action (const type_t& type)
    {
      error_at_line (-1, 0, dimension_node->file, dimension_node->line,
                     "%s is not a valid array dimension type", type.to_string ().c_str ());
    }

    void visit (const int_type_t& type)
    {
      if (dimension_tv.int_value >= 0)
        {
          dimension = dimension_tv.int_value;
        }
      else
        {
          error_at_line (-1, 0, dimension_node->file, dimension_node->line,
                         "array dimension is negative");

        }
    }
  };
  visitor v (*ptr, tv);
  tv.type->accept (v);
  return v.dimension;
}

const type_t *
process_type_spec (ast_t * node, bool force_identifiers, bool is_component)
{
  struct type_spec_visitor_t : public ast_visitor_t
  {
    const type_t* type;
    bool force_identifiers;
    bool is_component;

    type_spec_visitor_t (bool fi,
                         bool ic)
      : type (NULL)
      , force_identifiers (fi)
      , is_component (ic)
    { }

    void default_action (ast_t& node)
    {
      not_reached;
    }

    void visit (ast_array_type_spec_t& node)
    {
      size_t dimension = process_array_dimension (node.get_child_ptr (ARRAY_DIMENSION));
      const type_t* base_type = process_type_spec (node.at (ARRAY_BASE_TYPE), true);
      type = new array_type_t (dimension, base_type);
    }

    void visit (ast_component_type_spec_t& node)
    {
      type = new component_type_t (dynamic_cast<const field_list_type_t*> (process_type_spec (node.at (COMPONENT_FIELD_LIST), true, true)));
    }

    void visit (ast_empty_type_spec_t& node)
    {
      type = void_type_t::instance ();
    }

    void visit (ast_field_list_type_spec_t& node)
    {
      field_list_type_t *field_list = new field_list_type_t (is_component);
      AST_FOREACH (child, &node)
        {
          ast_t *identifier_list = child->at (IDENTIFIER_LIST);
          ast_t *type_spec = child->at (TYPE_SPEC);
          const type_t *type = process_type_spec (type_spec, true);
          AST_FOREACH (id, identifier_list)
            {
              string_t identifier = ast_get_identifier (id);
              const type_t *field = type_select (field_list, identifier);
              if (field == NULL)
                {
                  field_list->append (identifier, type);
                }
              else
                {
                  error_at_line (-1, 0, id->file, id->line,
                                 "duplicate field name %s", get (identifier));
                }
            }
        }
      type = field_list;
    }

    void visit (ast_foreign_type_spec_t& node)
    {
      unimplemented;
      //type = foreign_type_t::make (process_type_spec (node.at (FOREIGN_BASE_TYPE), false));
    }

    void visit (ast_heap_type_spec_t& node)
    {
      type = new heap_type_t (process_type_spec (node.at (HEAP_BASE_TYPE), false));
    }

    void visit (ast_identifier_type_spec_t& node)
    {
      ast_t *child = node.at (IDENTIFIER_TYPE_SPEC_CHILD);
      string_t identifier = ast_get_identifier (child);
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
          error_at_line (-1, 0, child->file, child->line,
                         "%s does not refer to a type", get (identifier));
        }
      type = symbol_get_type_type (symbol);
    }

    void visit (ast_pointer_type_spec_t& node)
    {
      type = pointer_type_t::make (process_type_spec (node.at (POINTER_BASE_TYPE), false));
    }

    void visit (ast_pointer_to_foreign_type_spec_t& node)
    {
      type = pointer_to_foreign_type_t::make (process_type_spec (node.at (POINTER_BASE_TYPE), false));
    }

    void visit (ast_pointer_to_immutable_type_spec_t& node)
    {
      type = pointer_to_immutable_type_t::make (process_type_spec (node.at (POINTER_BASE_TYPE), false));
    }

    void visit (ast_port_type_spec_t& node)
    {
      const signature_type_t* signature = dynamic_cast<const signature_type_t*> (process_type_spec (node.at (PORT_SIGNATURE), true));
      if (!type_is_foreign_safe (signature))
        {
          error_at_line (-1, 0, node.file, node.line,
                         "signature leaks pointers");
        }
      type = new port_type_t (signature);
    }

    void visit (ast_signature_type_spec_t& node)
    {
      signature_type_t *signature = new signature_type_t ();
      AST_FOREACH (child, &node)
        {
          ast_t *identifier_list = child->at (IDENTIFIER_LIST);
          ast_t *type_spec = child->at (TYPE_SPEC);
          const type_t *type = process_type_spec (type_spec, true);
          AST_FOREACH (id, identifier_list)
            {
              string_t identifier = ast_get_identifier (id);
              const parameter_t *parameter = signature->find (identifier);
              if (parameter == NULL)
                {
                  signature->append (identifier, type, false);
                }
              else
                {
                  error_at_line (-1, 0, id->file, id->line,
                                 "duplicate parameter name %s",
                                 get (identifier));
                }
            }
        }
      type = signature;
    }

    void visit (ast_struct_type_spec_t& node)
    {
      type = new struct_type_t (dynamic_cast<const field_list_type_t*> (process_type_spec (node.at (STRUCT_FIELD_LIST), true)));
    }
  };
  type_spec_visitor_t type_spec_visitor (force_identifiers, is_component);
  node->accept (type_spec_visitor);
  return type_spec_visitor.type;
}
