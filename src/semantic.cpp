#include "semantic.hpp"

#include <error.h>

#include "debug.hpp"
#include "ast.hpp"
#include "action.hpp"
#include "reaction.hpp"
#include "Type.hpp"
#include "field.hpp"
#include "Symbol.hpp"
#include "MemoryModel.hpp"
#include "bind.hpp"
#include "Callable.hpp"
#include "AstVisitor.hpp"

using namespace ast;

// TODO:  Replace interacting with type_t* with typed_value_t.

void
allocate_symbol (MemoryModel& memory_model,
                 Symbol* symbol)
{
  struct visitor : public SymbolVisitor
  {
    MemoryModel& memory_model;

    visitor (MemoryModel& mm) : memory_model (mm) { }

    void defineAction (Symbol& symbol)
    {
      not_reached;
    }

    void visit (ParameterSymbol& symbol)
    {
      switch (symbol.kind)
        {
        case ParameterSymbol::Ordinary:
        case ParameterSymbol::Receiver:
        case ParameterSymbol::Return:
        case ParameterSymbol::Iota:
        {
          const Type::Type* type = symbol.type;
          memory_model.ArgumentsPush (type->Size ());
          static_cast<Symbol&> (symbol).offset (memory_model.ArgumentsOffset ());
          if (symbol.kind == ParameterSymbol::Receiver)
            {
              memory_model.SetReceiverOffset ();
            }
          if (symbol.kind == ParameterSymbol::Iota)
            {
              memory_model.SetIotaOffset ();
            }
        }
        break;
        case ParameterSymbol::ReceiverDuplicate:
        case ParameterSymbol::OrdinaryDuplicate:
          break;
        }
    }

    void visit (TypedConstantSymbol& symbol)
    {
      // No need to allocate.
    }

    void visit (VariableSymbol& symbol)
    {
      const Type::Type* type = symbol.value.type;
      static_cast<Symbol&>(symbol).offset (memory_model.LocalsOffset ());
      memory_model.LocalsPush (type->Size ());
    }

    void visit (HiddenSymbol& symbol)
    {
      // Do nothing.
    }
  };

  visitor v (memory_model);
  symbol->accept (v);
}

static void
allocate_symtab (ast::Node* node, MemoryModel& memory_model)
{
  // Allocate the parameters.
  for (Node::SymbolsType::const_iterator pos = node->SymbolsBegin (), limit = node->SymbolsEnd ();
       pos != limit;
       ++pos)
    {
      allocate_symbol (memory_model, *pos);
    }
}

static void
allocate_statement_stack_variables (ast::Node* node, MemoryModel& memory_model)
{
  struct visitor : public DefaultVisitor
  {
    MemoryModel& memory_model;

    visitor (MemoryModel& m) : memory_model (m) { }

    void default_action (Node& node)
    {
      ast_not_reached (node);
    }

    void visit (ast_const_t& node)
    {
      // Do nothing.
    }

    void visit (ast_empty_statement_t& node)
    { }

    void visit (ast_for_iota_statement_t& node)
    {
      ptrdiff_t offset_before = memory_model.LocalsOffset ();
      allocate_symtab (&node, memory_model);
      ptrdiff_t offset_after = memory_model.LocalsOffset ();
      allocate_statement_stack_variables (node.body (), memory_model);
      memory_model.LocalsPop (offset_after - offset_before);
      assert (memory_model.LocalsOffset () == offset_before);
    }

    void visit (ast_bind_push_port_statement_t& node)
    {
      // Do nothing.
    }

    void visit (ast_bind_push_port_param_statement_t& node)
    {
      // Do nothing.
    }

    void visit (ast_bind_pull_port_statement_t& node)
    {
      // Do nothing.
    }

    void visit (ast_assign_statement_t& node)
    {
      // Do nothing.
    }

    void visit (ast_change_statement_t& node)
    {
      ptrdiff_t offset_before = memory_model.LocalsOffset ();
      allocate_symtab (&node, memory_model);
      ptrdiff_t offset_after = memory_model.LocalsOffset ();
      allocate_statement_stack_variables (node.body (), memory_model);
      memory_model.LocalsPop (offset_after - offset_before);
      assert (memory_model.LocalsOffset () == offset_before);
    }

    void visit (ast_expression_statement_t& node)
    {
      // Do nothing.
    }

    void visit (ast_if_statement_t& node)
    {
      allocate_statement_stack_variables (node.true_branch (), memory_model);
      allocate_statement_stack_variables (node.false_branch (), memory_model);
    }

    void visit (ast_while_statement_t& node)
    {
      allocate_statement_stack_variables (node.body (), memory_model);
    }

    void visit (ast_add_assign_statement_t& node)
    {
      // Do nothing.
    }

    void visit (ast_subtract_assign_statement_t& node)
    {
      // Do nothing.
    }

    void visit (ast_list_statement_t& node)
    {
      ptrdiff_t offset_before = memory_model.LocalsOffset ();
      allocate_symtab (&node, memory_model);
      ptrdiff_t offset_after = memory_model.LocalsOffset ();
      for (Node::ConstIterator pos = node.Begin (), limit = node.End ();
           pos != limit;
           ++pos)
        {
          allocate_statement_stack_variables (*pos, memory_model);
        }
      memory_model.LocalsPop (offset_after - offset_before);
      assert (memory_model.LocalsOffset () == offset_before);
    }

    void visit (ast_return_statement_t& node)
    {
      // Do nothing.
    }

    void visit (ast_increment_statement_t& node)
    {
      // Do nothing.
    }

    void visit (ast_activate_statement_t& node)
    {
      ptrdiff_t offset_before = memory_model.LocalsOffset ();
      allocate_symtab (&node, memory_model);
      ptrdiff_t offset_after = memory_model.LocalsOffset ();
      allocate_statement_stack_variables (node.body (), memory_model);
      memory_model.LocalsPop (offset_after - offset_before);
      assert (memory_model.LocalsOffset () == offset_before);
      node.memoryModel = &memory_model;
    }

    void visit (ast_var_statement_t& node)
    {
      // Do nothing.  The variables are allocated in the StmtList containing this.
    }
  };
  visitor v (memory_model);
  node->Accept (v);
}

void
allocate_stack_variables (ast::Node* node)
{
  struct visitor : public DefaultVisitor
  {
    void visit (ast_action_t& node)
    {
      allocate_stack_variables_helper (node.action->memory_model, node, node.body ());
    }

    void visit (ast_dimensioned_action_t& node)
    {
      allocate_stack_variables_helper (node.action->memory_model, node, node.body ());
    }

    void visit (ast_bind_t& node)
    {
      allocate_stack_variables_helper (node.bind->memory_model, node, node.body ());
    }

    void visit (ast_function_t& node)
    {
      allocate_stack_variables_helper (node.function->memoryModel, node, node.body ());
    }

    void visit (ast_method_t& node)
    {
      allocate_stack_variables_helper (node.method->memoryModel, node, node.body ());
    }

    void visit (ast_initializer_t& node)
    {
      allocate_stack_variables_helper (node.initializer->memoryModel, node, node.body ());
    }

    void visit (ast_getter_t& node)
    {
      allocate_stack_variables_helper (node.getter->memoryModel, node, node.body ());
    }

    void visit (ast_reaction_t& node)
    {
      allocate_stack_variables_helper (node.reaction->memory_model, node, node.body ());
    }

    void visit (ast_dimensioned_reaction_t& node)
    {
      allocate_stack_variables_helper (node.reaction->memory_model, node, node.body ());
    }

    void visit (SourceFile& node)
    {
      node.VisitChildren (*this);
    }

    // Return the size of the locals.
    void
    allocate_stack_variables_helper (MemoryModel& memoryModel,
                                     ast::Node& node,
                                     ast::Node* child)
    {
      // Allocate the parameters.
      Node::SymbolsType::const_iterator pos = node.SymbolsBegin ();
      Node::SymbolsType::const_iterator limit = node.SymbolsEnd ();
      allocate_parameter (memoryModel, pos, limit);
      // Allocate the locals.
      allocate_statement_stack_variables (child, memoryModel);
      assert (memoryModel.LocalsEmpty ());
    }

  };

  visitor v;
  node->Accept (v);
}
