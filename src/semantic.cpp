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

// TODO:  Replace interacting with type_t* with typed_value_t.

static void
allocate_symbol (memory_model_t* memory_model,
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
              memory_model_arguments_push (memory_model, type->size ());
              symbol_set_offset (symbol, memory_model_arguments_offset (memory_model));
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
        symbol_set_offset (symbol, memory_model_locals_offset (memory_model));
        memory_model_locals_push (memory_model, type->size ());
      }
      break;
    }
}

static void
allocate_symtab (ast_t* node, memory_model_t* memory_model)
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
allocate_statement_stack_variables (ast_t* node, memory_model_t* memory_model)
{
  struct visitor : public ast_visitor_t
  {
    memory_model_t* memory_model;

    visitor (memory_model_t* m) : memory_model (m) { }

    void default_action (ast_t& node)
    {
      not_reached;
    }

    void visit (ast_assign_statement_t& node)
    {
      // Do nothing.
    }

    void visit (ast_change_statement_t& node)
    {
      ptrdiff_t offset_before = memory_model_locals_offset (memory_model);
      allocate_symtab (&node, memory_model);
      ptrdiff_t offset_after = memory_model_locals_offset (memory_model);
      allocate_statement_stack_variables (node.body (), memory_model);
      memory_model_locals_pop (memory_model, offset_after - offset_before);
      assert (memory_model_locals_offset (memory_model) == offset_before);
    }

    void visit (ast_expression_statement_t& node)
    {
      // Do nothing.
    }

    void visit (ast_if_statement_t& node)
    {
      ast_t* true_branch = node.at (IF_TRUE_BRANCH);
      allocate_statement_stack_variables (true_branch, memory_model);
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
      ptrdiff_t offset_before = memory_model_locals_offset (memory_model);
      allocate_symtab (&node, memory_model);
      ptrdiff_t offset_after = memory_model_locals_offset (memory_model);
      AST_FOREACH (child, &node)
        {
          allocate_statement_stack_variables (child, memory_model);
        }
      memory_model_locals_pop (memory_model, offset_after - offset_before);
      assert (memory_model_locals_offset (memory_model) == offset_before);
    }

    void visit (ast_return_statement_t& node)
    {
      // Do nothing.
    }

    void visit (ast_trigger_statement_t& node)
    {
      ptrdiff_t offset_before = memory_model_locals_offset (memory_model);
      allocate_symtab (&node, memory_model);
      ptrdiff_t offset_after = memory_model_locals_offset (memory_model);
      allocate_statement_stack_variables (node.body (), memory_model);
      memory_model_locals_pop (memory_model, offset_after - offset_before);
      assert (memory_model_locals_offset (memory_model) == offset_before);
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
allocate_parameter (memory_model_t* memory_model,
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
allocate_stack_variables (ast_t* node, memory_model_t* memory_model)
{
  struct visitor : public ast_visitor_t
  {
    memory_model_t* memory_model;

    visitor (memory_model_t* mm) : memory_model (mm) { }

    void visit (ast_action_t& node)
    {
      action_reaction_base_t* action = get_current_action (&node);
      size_t locals_size = allocate_stack_variables_helper (&node, node.body ());
      action->locals_size = locals_size;
    }

    void visit (ast_dimensioned_action_t& node)
    {
      action_reaction_base_t* action = get_current_action (&node);
      size_t locals_size = allocate_stack_variables_helper (&node, node.body ());
      action->locals_size = locals_size;
    }

    void visit (ast_function_t& node)
    {
      function_t* function = get_current_function (&node);
      size_t locals_size = allocate_stack_variables_helper (&node, node.at (FUNCTION_BODY));
      function_set_locals_size (function, locals_size);
    }

    void visit (ast_method_t& node)
    {
      method_t* method = get_current_method (&node);
      size_t locals_size = allocate_stack_variables_helper (&node, node.at (METHOD_BODY));
      method_set_locals_size (method, locals_size);
    }

    void visit (ast_reaction_t& node)
    {
      action_reaction_base_t* action = get_current_action (&node);
      size_t locals_size = allocate_stack_variables_helper (&node, node.body ());
      action->locals_size = locals_size;
    }

    void visit (ast_top_level_list_t& node)
    {
      AST_FOREACH (child, &node)
        {
          child->accept (*this);
        }
    }

    // Return the size of the locals.
    size_t
    allocate_stack_variables_helper (ast_t* node,
                                     ast_t* child)
    {
      assert (memory_model_arguments_empty (memory_model));
      assert (memory_model_locals_empty (memory_model));
      // Allocate the parameters.
      symtab_t::SymbolsType::const_iterator pos = node->symtab->begin ();
      symtab_t::SymbolsType::const_iterator limit = node->symtab->end ();
      ptrdiff_t offset_before = memory_model_arguments_offset (memory_model);
      allocate_parameter (memory_model, pos, limit);
      ptrdiff_t offset_after = memory_model_arguments_offset (memory_model);
      // Allocate the locals.
      allocate_statement_stack_variables (child, memory_model);
      // Deallocate the parameters.
      memory_model_arguments_pop (memory_model, offset_before - offset_after);
      assert (memory_model_arguments_empty (memory_model));
      assert (memory_model_locals_empty (memory_model));
      size_t locals_size = memory_model_locals_size (memory_model);
      memory_model_reset_locals_size (memory_model);
      return locals_size;
    }

  };

  visitor v (memory_model);
  node->accept (v);
}
