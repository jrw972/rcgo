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
#include "symbol.hpp"
#include "memory_model.hpp"
#include "method.hpp"
#include "function.hpp"
#include "bind.hpp"

// TODO:  Replace interacting with type_t* with typed_value_t.

static void
allocate_symbol (memory_model_t& memory_model,
                 symbol_t* symbol)
{
  switch (symbol_kind (symbol))
    {
    case SymbolFunction:
      unimplemented;
      break;

    case SymbolInstance:
      unimplemented;
      break;

    case SymbolParameter:
      {
        switch (symbol_parameter_kind (symbol))
          {
          case ParameterOrdinary:
          case ParameterReceiver:
          case ParameterReturn:
            {
              const type_t* type = symbol_parameter_type (symbol);
              memory_model.arguments_push (type->size ());
              symbol_set_offset (symbol, memory_model.arguments_offset ());
            }
            break;
          case ParameterReceiverDuplicate:
          case ParameterDuplicate:
            break;
          }
      }
      break;

    case SymbolType:
      unimplemented;
      break;

    case SymbolTypedConstant:
      unimplemented;
      break;

    case SymbolVariable:
      {
        const type_t* type = symbol_variable_type (symbol);
        symbol_set_offset (symbol, memory_model.locals_offset ());
        memory_model.locals_push (type->size ());
      }
      break;
    }
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
      not_reached;
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

    void visit (ast_bind_statement_t& node)
    {
      // Do nothing.
    }

    void visit (ast_bind_param_statement_t& node)
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
      AST_FOREACH (child, &node)
        {
          allocate_statement_stack_variables (child, memory_model);
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
      node.function->memory_model = allocate_stack_variables_helper (&node, node.at (FUNCTION_BODY));
    }

    void visit (ast_method_t& node)
    {
      node.method->memory_model = allocate_stack_variables_helper (&node, node.at (METHOD_BODY));
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
      AST_FOREACH (child, &node)
        {
          child->accept (*this);
        }
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
