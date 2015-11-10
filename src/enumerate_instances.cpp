#include "Type.hpp"
#include "instance_table.hpp"
#include "field.hpp"
#include "ast.hpp"
#include "Symbol.hpp"
#include "SymbolVisitor.hpp"

using namespace Type;

static void
instantiate_contained_instances (const Type::Type * type,
                                 instance_table_t& instance_table,
                                 instance_t* parent,
                                 Initializer* initializer,
                                 size_t address,
                                 unsigned int line,
                                 ast_instance_t* node)
{
  struct visitor : public DefaultVisitor
  {
    instance_table_t& instance_table;
    instance_t* const parent;
    Initializer* const initializer;
    size_t const address;
    field_t* const field;
    unsigned int const line;
    ast_instance_t* const node;

    const NamedType* named_type;

    visitor (instance_table_t& it, instance_t* p, Initializer* i, size_t a, field_t* f, unsigned int l, ast_instance_t* n)
      : instance_table (it)
      , parent (p)
      , initializer (i)
      , address (a)
      , field (f)
      , line (l)
      , node (n)
      , named_type (NULL)
    { }

    void default_action (const Type::Type& type)
    {
      type_not_reached (type);
    }

    void visit (const NamedType& type)
    {
      // Save the named typed.
      named_type = &type;
      type.UnderlyingType ()->Accept (*this);
    }

    void visit (const Component& type)
    {
      assert (named_type != NULL);

      instance_t* instance = new instance_t (parent, address, named_type, initializer, line, node);
      instance_table.insert (instance);

      // Recur changing instance.
      visitor v (instance_table, instance, NULL, address, NULL, line, NULL);
      v.visit (static_cast<const Struct&> (type));
    }

    void visit (const Struct& type)
    {
      for (Struct::const_iterator pos = type.Begin (),
           limit = type.End ();
           pos != limit;
           ++pos)
        {
          // Recur changing address (and field).
          visitor v (instance_table, parent, NULL, address + (*pos)->offset, *pos, line, NULL);
          (*pos)->type->Accept (v);
        }
    }

    void visit (const Array& type)
    {
      for (Int::ValueType idx = 0; idx != type.dimension; ++idx)
        {
          // Recur changing address.
          visitor v (instance_table, parent, NULL, address + idx * type.ElementSize (), field, line, NULL);
          type.Base ()->Accept (v);
        }
    }

    void visit (const Bool& type) { }

    void visit (const Int& type) { }

    void visit (const Int8& type) { }

    void visit (const Uint& type) { }

    void visit (const Uint8& type) { }

    void visit (const Uint32& type) { }

    void visit (const Uint64& type) { }

    void visit (const Float64& type) { }

    void visit (const Enum& type) { }

    void visit (const Pointer& type) { }

    void visit (const Type::Function& type)
    {
      switch (type.kind)
        {
        case Type::Function::FUNCTION:
          // Do nothing.
          break;
        case Type::Function::PUSH_PORT:
          instance_table.insert_push_port (address, parent, field);
          break;
        case Type::Function::PULL_PORT:
          instance_table.insert_pull_port (address, parent, field);
          break;
        }
    }

    void visit (const Type::FileDescriptor& type) { }
  };
  visitor v (instance_table, parent, initializer, address, NULL, line, node);
  type->Accept (v);
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
      Symbol *symbol = node.symbol;
      const NamedType *type = SymbolCast<InstanceSymbol> (symbol)->type;
      Initializer* initializer = SymbolCast<InstanceSymbol> (symbol)->initializer;
      instantiate_contained_instances (type, instance_table, NULL, initializer, address, node.location.Line, &node);
      address += type->Size ();
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
