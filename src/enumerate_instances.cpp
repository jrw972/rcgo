#include "type.hpp"
#include "instance_table.hpp"
#include "field.hpp"
#include "ast.hpp"
#include "Symbol.hpp"

static void
instantiate_contained_instances (const type_t * type,
                                 instance_table_t& instance_table,
                                 instance_t* parent,
                                 initializer_t* initializer,
                                 size_t address)
{
  struct visitor : public const_type_visitor_t
  {
    instance_table_t& instance_table;
    instance_t* const parent;
    initializer_t* const initializer;
    size_t const address;
    field_t* const field;

    const named_type_t* named_type;

    visitor (instance_table_t& it, instance_t* p, initializer_t* i, size_t a, field_t* f)
      : instance_table (it)
      , parent (p)
      , initializer (i)
      , address (a)
      , field (f)

      , named_type (NULL)
    { }

    void default_action (const type_t& type)
    {
      type_not_reached (type);
    }

    void visit (const named_type_t& type)
    {
      // Save the named typed.
      named_type = &type;
      type.subtype ()->accept (*this);
    }

    void visit (const component_type_t& type)
    {
      assert (named_type != NULL);

      instance_t* instance = new instance_t (parent, address, named_type, initializer);
      instance_table.insert (instance);

      // Recur changing instance.
      visitor v (instance_table, instance, NULL, address, NULL);
      type.field_list ()->accept (v);
    }

    void visit (const field_list_type_t& type)
    {
      for (field_list_type_t::const_iterator pos = type.begin (),
             limit = type.end ();
           pos != limit;
           ++pos)
        {
          // Recur changing address (and field).
          visitor v (instance_table, parent, NULL, address + (*pos)->offset, *pos);
          (*pos)->type->accept (v);
        }
    }

    void visit (const array_type_t& type)
    {
      for (int_type_t::ValueType idx = 0; idx != type.dimension; ++idx)
        {
          // Recur changing address.
          visitor v (instance_table, parent, NULL, address + idx * type.element_size (), field);
          type.base_type ()->accept (v);
        }
    }

    void visit (const struct_type_t& type)
    {
      type.field_list ()->accept (*this);
    }

    void visit (const bool_type_t& type)
    { }

    void visit (const int_type_t& type)
    { }

    void visit (const int8_type_t& type)
    { }

    void visit (const uint_type_t& type)
    { }

    void visit (const uint8_type_t& type)
    { }

    void visit (const uint32_type_t& type)
    { }

    void visit (const uint64_type_t& type)
    { }

    void visit (const uint128_type_t& type)
    { }

    void visit (const float64_type_t& type)
    { }

    void visit (const enum_type_t& type)
    { }

    void visit (const pointer_type_t& type)
    { }

    void visit (const push_port_type_t& type)
    {
      instance_table.insert_push_port (address, parent, field);
    }

    void visit (const pull_port_type_t& type)
    {
      instance_table.insert_pull_port (address, parent, field);
    }
  };
  visitor v (instance_table, parent, initializer, address, NULL);
  type->accept (v);
}

/*
  The goal of this function is to generate the following look-up
  tables:
  1.  (instance) -> (type)
  2.  (instance, field) -> instance
*/
void
enumerate_instances (ast_t * node, instance_table_t& instance_table)
{
  struct visitor : public ast_visitor_t
  {
    instance_table_t& instance_table;
    size_t address;

    visitor (instance_table_t& it) : instance_table (it), address (0) { }

    void visit (ast_instance_t& node)
    {
      Symbol *symbol = node.symbol.symbol ();
      const named_type_t *type = SymbolCast<InstanceSymbol> (symbol)->type;
      initializer_t* initializer = SymbolCast<InstanceSymbol> (symbol)->initializer;
      instantiate_contained_instances (type, instance_table, NULL, initializer, address);
      address += type->size ();
    }

    void visit (ast_top_level_list_t& node)
    {
      for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          (*pos)->accept (*this);
        }
    }
  };

  visitor v (instance_table);
  node->accept (v);
}
