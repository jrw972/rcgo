#include "semantic.hpp"
#include "debug.hpp"
#include "symtab.hpp"
#include "ast.hpp"
#include <error.h>
#include "action.hpp"
#include "type.hpp"
#include "parameter.hpp"
#include "instance.hpp"
#include "trigger.hpp"
#include "field.hpp"
#include "Symbol.hpp"
#include "memory_model.hpp"
#include "bind.hpp"
#include "Callable.hpp"

// TODO:  Replace interacting with type_t* with typed_value_t.

static void
allocate_symbol (memory_model_t& memory_model,
                 Symbol* symbol)
{
  struct visitor : public SymbolVisitor {
    memory_model_t& memory_model;

    visitor (memory_model_t& mm) : memory_model (mm) { }

    void defineAction (Symbol& symbol) {
      not_reached;
    }

    void visit (ParameterSymbol& symbol) {
      switch (symbol.kind)
        {
        case ParameterSymbol::Ordinary:
        case ParameterSymbol::Receiver:
        case ParameterSymbol::Return:
          {
            const type_t* type = symbol.value.type;
            memory_model.arguments_push (type->size ());
            static_cast<Symbol&> (symbol).offset (memory_model.arguments_offset ());
          }
          break;
        case ParameterSymbol::ReceiverDuplicate:
        case ParameterSymbol::OrdinaryDuplicate:
          break;
        }
    }

    void visit (TypedConstantSymbol& symbol) {
      // No need to allocate.
    }

    void visit (VariableSymbol& symbol) {
      const type_t* type = symbol.value.type;
      static_cast<Symbol&>(symbol).offset (memory_model.locals_offset ());
      memory_model.locals_push (type->size ());
    }

    void visit (HiddenSymbol& symbol) {
      // Do nothing.
    }
  };

  visitor v (memory_model);
  symbol->accept (v);
}

static void
allocate_symtab (ast_t* node, memory_model_t& memory_model)
{
  // Allocate the parameters.
  for (symtab_t::SymbolsType::const_iterator pos = node->symtab->begin (), limit = node->symtab->end ();
       pos != limit;
       ++pos)
    {
      allocate_symbol (memory_model, *pos);
    }
}

static void
allocate_statement_stack_variables (ast_t* node, memory_model_t& memory_model)
{
  struct visitor : public ast_visitor_t
  {
    memory_model_t& memory_model;

    visitor (memory_model_t& m) : memory_model (m) { }

    void default_action (ast_t& node)
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
      ptrdiff_t offset_before = memory_model.locals_offset ();
      allocate_symtab (&node, memory_model);
      ptrdiff_t offset_after = memory_model.locals_offset ();
      allocate_statement_stack_variables (node.body (), memory_model);
      memory_model.locals_pop (offset_after - offset_before);
      assert (memory_model.locals_offset () == offset_before);
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
      ptrdiff_t offset_before = memory_model.locals_offset ();
      allocate_symtab (&node, memory_model);
      ptrdiff_t offset_after = memory_model.locals_offset ();
      allocate_statement_stack_variables (node.body (), memory_model);
      memory_model.locals_pop (offset_after - offset_before);
      assert (memory_model.locals_offset () == offset_before);
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
      ptrdiff_t offset_before = memory_model.locals_offset ();
      allocate_symtab (&node, memory_model);
      ptrdiff_t offset_after = memory_model.locals_offset ();
      for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          allocate_statement_stack_variables (*pos, memory_model);
        }
      memory_model.locals_pop (offset_after - offset_before);
      assert (memory_model.locals_offset () == offset_before);
    }

    void visit (ast_return_statement_t& node)
    {
      // Do nothing.
    }

    void visit (ast_increment_statement_t& node)
    {
      // Do nothing.
    }

    void visit (ast_trigger_statement_t& node)
    {
      ptrdiff_t offset_before = memory_model.locals_offset ();
      allocate_symtab (&node, memory_model);
      ptrdiff_t offset_after = memory_model.locals_offset ();
      allocate_statement_stack_variables (node.body (), memory_model);
      memory_model.locals_pop (offset_after - offset_before);
      assert (memory_model.locals_offset () == offset_before);
     }

    void visit (ast_var_statement_t& node)
    {
      // Do nothing.  The variables are allocated in the StmtList containing this.
    }

    void visit (ast_var_type_init_statement_t& node)
    {
      // Do nothing.  The variables are allocated in the StmtList containing this.
    }

    void visit (ast_println_statement_t& node)
    {
      // Do nothing.
    }
  };
  visitor v (memory_model);
  node->accept (v);
}

static void
allocate_parameter (memory_model_t& memory_model,
                    symtab_t::SymbolsType::const_iterator pos,
                    symtab_t::SymbolsType::const_iterator limit)
{
  if (pos != limit)
    {
      allocate_parameter (memory_model, pos + 1, limit);
      allocate_symbol (memory_model, *pos);
    }
}

void
allocate_stack_variables (ast_t* node)
{
  struct visitor : public ast_visitor_t
  {
    void visit (ast_action_t& node)
    {
      node.action->memory_model = allocate_stack_variables_helper (&node, node.body ());
    }

    void visit (ast_dimensioned_action_t& node)
    {
      node.action->memory_model = allocate_stack_variables_helper (&node, node.body ());
    }

    void visit (ast_bind_t& node)
    {
      node.bind->memory_model = allocate_stack_variables_helper (&node, node.body ());
    }

    void visit (ast_function_t& node)
    {
      node.function->memoryModel = allocate_stack_variables_helper (&node, node.body ());
    }

    void visit (ast_method_t& node)
    {
      node.method->memoryModel = allocate_stack_variables_helper (&node, node.body ());
    }

    void visit (ast_initializer_t& node)
    {
      node.initializer->memoryModel = allocate_stack_variables_helper (&node, node.body ());
    }

    void visit (ast_getter_t& node)
    {
      node.getter->memoryModel = allocate_stack_variables_helper (&node, node.body ());
    }

    void visit (ast_reaction_t& node)
    {
      node.reaction->memory_model = allocate_stack_variables_helper (&node, node.body ());
    }

    void visit (ast_dimensioned_reaction_t& node)
    {
      node.reaction->memory_model = allocate_stack_variables_helper (&node, node.body ());
    }

    void visit (ast_top_level_list_t& node)
    {
      node.visit_children (*this);
    }

    // Return the size of the locals.
    memory_model_t
    allocate_stack_variables_helper (ast_t* node,
                                     ast_t* child)
    {
      memory_model_t memory_model;
      // Allocate the parameters.
      symtab_t::SymbolsType::const_iterator pos = node->symtab->begin ();
      symtab_t::SymbolsType::const_iterator limit = node->symtab->end ();
      allocate_parameter (memory_model, pos, limit);
      // Allocate the locals.
      allocate_statement_stack_variables (child, memory_model);
      assert (memory_model.locals_empty ());
      return memory_model;
    }

  };

  visitor v;
  node->accept (v);
}
