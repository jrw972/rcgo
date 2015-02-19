#include "type.hpp"
#include "instance.hpp"
#include "field.hpp"
#include "ast.hpp"
#include "symbol.hpp"

static void
instantiate_contained_instances (const named_type_t * type, instance_table_t * instance_table, method_t* method, size_t address)
{
  struct visitor : public const_type_visitor_t
  {
    instance_table_t* instance_table;
    method_t* method;
    size_t address;
    const named_type_t* last_named_type;
    instance_t* parent;
    instance_t* last_instance;

    visitor (instance_table_t* it, method_t* m, size_t a) : instance_table (it), method (m), address (a), last_named_type (NULL), parent (NULL), last_instance (NULL) { }

    void default_action (const type_t& type)
    {
      not_reached;
    }

    void visit (const named_type_t& type)
    {
      last_named_type = &type;
      type.subtype ()->accept (*this);
    }

    void visit (const component_type_t& type)
    {
      assert (last_named_type != NULL);
      last_instance = instance_table_insert (instance_table, parent, last_named_type, method, address);
      if (parent == NULL)
        {
          parent = last_instance;
        }
      last_named_type = NULL;
      method = NULL;
      type.field_list ()->accept (*this);
    }

    void visit (const field_list_type_t& type)
    {
      for (field_list_type_t::const_iterator field = type.begin (),
             limit = type.end ();
           field != limit;
           ++field)
        {
          address += field_offset (*field);
          field_type (*field)->accept (*this);
          address -= field_offset (*field);
        }
    }

    void visit (const array_type_t& type)
    {
      size_t address_before = address;
      for (size_t idx = 0; idx != type.dimension (); ++idx)
        {
          address = address_before + idx * type.element_size ();
          type.base_type ()->accept (*this);
        }
      address = address_before;
    }

    void visit (const immutable_type_t& type)
    { }

    void visit (const struct_type_t& type)
    { }

    void visit (const bool_type_t& type)
    { }

    void visit (const uint_type_t& type)
    { }

    void visit (const port_type_t& type)
    {
      instance_table_insert_port (instance_table, address, last_instance);
    }
  };
  visitor v (instance_table, method, address);
  type->accept (v);
}

/*
  The goal of this function is to generate the following look-up
  tables:
  1.  (instance) -> (type)
  2.  (instance, field) -> instance
*/
void
enumerate_instances (ast_t * node, instance_table_t * instance_table)
{
  struct visitor : public ast_visitor_t
  {
    instance_table_t* instance_table;
    size_t address;

    visitor (instance_table_t* it) : instance_table (it), address (0) { }

    void visit (ast_instance_t& node)
    {
      symbol_t *symbol = node.symbol.symbol ();
      const named_type_t *type = symbol_get_instance_type (symbol);
      method_t* method = symbol_get_instance_method (symbol);
      instantiate_contained_instances (type, instance_table, method, address);
      address += type->size ();
    }

    void visit (ast_top_level_list_t& node)
    {
      AST_FOREACH (child, &node)
        {
          child->accept (*this);
        }
    }
  };

  visitor v (instance_table);
  node->accept (v);
}
