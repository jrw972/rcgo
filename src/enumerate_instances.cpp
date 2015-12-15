#include "type.hpp"
#include "composition.hpp"
#include "field.hpp"
#include "ast.hpp"
#include "symbol.hpp"
#include "symbol_visitor.hpp"
#include "ast_visitor.hpp"

namespace semantic
{

using namespace type;
using namespace ast;

static composition::Instance*
instantiate_contained_instances (const type::Type * type,
                                 composition::Composer& instance_table,
                                 composition::Instance* parent,
                                 decl::Initializer* initializer,
                                 size_t address,
                                 unsigned int line,
                                 ast_instance_t* node,
                                 const std::string& name)
{
  struct visitor : public ::type::DefaultVisitor
  {
    composition::Composer& instance_table;
    composition::Instance* const parent;
    decl::Initializer* const initializer;
    size_t const address;
    field_t* const field;
    unsigned int const line;
    ast_instance_t* const node;
    std::string const name;

    const NamedType* named_type;

    composition::Instance* instance;

    visitor (composition::Composer& it, composition::Instance* p, decl::Initializer* i, size_t a, field_t* f, unsigned int l, ast_instance_t* n, const std::string& aName)
      : instance_table (it)
      , parent (p)
      , initializer (i)
      , address (a)
      , field (f)
      , line (l)
      , node (n)
      , name (aName)
      , named_type (NULL)
      , instance (NULL)
    { }

    void default_action (const type::Type& type)
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

      composition::Instance* instance = new composition::Instance (parent, address, named_type, initializer, node, name);
      this->instance = instance;
      instance_table.AddInstance (instance);

      // Recur changing instance.
      visitor v (instance_table, instance, NULL, address, NULL, line, NULL, name);
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
          visitor v (instance_table, parent, NULL, address + (*pos)->offset, *pos, line, NULL, name + "." + (*pos)->name);
          (*pos)->type->Accept (v);
        }
    }

    void visit (const Array& type)
    {
      for (Int::ValueType idx = 0; idx != type.dimension; ++idx)
        {
          // Recur changing address.
          std::stringstream newname;
          newname << name << '[' << idx << ']';
          visitor v (instance_table, parent, NULL, address + idx * type.UnitSize (), field, line, NULL, newname.str ());
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

    void visit (const Pointer& type) { }

    void visit (const type::Function& type)
    {
      switch (type.function_kind)
        {
        case type::Function::FUNCTION:
          // Do nothing.
          break;
        case type::Function::PUSH_PORT:
          instance_table.AddPushPort (address, parent, field, name);
          break;
        case type::Function::PULL_PORT:
          instance_table.AddPullPort (address, parent, field, name);
          break;
        }
    }

    void visit (const type::FileDescriptor& type) { }
  };
  visitor v (instance_table, parent, initializer, address, NULL, line, node, name);
  type->Accept (v);
  return v.instance;
}

/*
  The goal of this function is to generate the following look-up
  tables:
  1.  (instance) -> (type)
  2.  (instance, field) -> instance
*/
void
enumerate_instances (Node * node, composition::Composer& instance_table)
{
  struct visitor : public ast::DefaultVisitor
  {
    composition::Composer& instance_table;
    size_t address;

    visitor (composition::Composer& it) : instance_table (it), address (0) { }

    void visit (ast_instance_t& node)
    {
      const NamedType *type = node.symbol->type;
      decl::Initializer* initializer = node.symbol->initializer;
      node.symbol->instance = instantiate_contained_instances (type, instance_table, NULL, initializer, address, node.location.Line, &node, ast_get_identifier (node.identifier ()));
      address += type->Size ();
    }

    void visit (SourceFile& node)
    {
      for (Node::ConstIterator pos = node.Begin (), limit = node.End ();
           pos != limit;
           ++pos)
        {
          (*pos)->Accept (*this);
        }
    }
  };

  visitor v (instance_table);
  node->Accept (v);
}

}
