#include "semantic.hpp"

#include <error.h>

#include "debug.hpp"
#include "ast.hpp"
#include "action.hpp"
#include "reaction.hpp"
#include "type.hpp"
#include "symbol.hpp"
#include "memory_model.hpp"
#include "bind.hpp"
#include "callable.hpp"
#include "ast_visitor.hpp"
#include "parameter_list.hpp"

namespace semantic
{

using namespace ast;
using namespace decl;

void
allocate_symbol (runtime::MemoryModel& memory_model,
                 Symbol* symbol)
{
  struct visitor : public SymbolVisitor
  {
    runtime::MemoryModel& memory_model;

    visitor (runtime::MemoryModel& mm) : memory_model (mm) { }

    void defineAction (Symbol& symbol)
    {
      NOT_REACHED;
    }

    void visit (ParameterSymbol& symbol)
    {
      switch (symbol.kind)
        {
        case ParameterSymbol::Ordinary:
        case ParameterSymbol::Receiver:
        case ParameterSymbol::Return:
        {
          const type::Type* type = symbol.type;
          memory_model.arguments_push (type->Size ());
          static_cast<Symbol&> (symbol).offset (memory_model.arguments_offset ());
          if (symbol.kind == ParameterSymbol::Receiver)
            {
              memory_model.set_receiver_offset ();
            }
        }
        break;
        case ParameterSymbol::ReceiverDuplicate:
        case ParameterSymbol::OrdinaryDuplicate:
          break;
        }
    }

    void visit (ConstantSymbol& symbol)
    {
      // No need to allocate.
    }

    void visit (VariableSymbol& symbol)
    {
      const type::Type* type = symbol.type;
      static_cast<Symbol&>(symbol).offset (memory_model.locals_offset ());
      memory_model.locals_push (type->Size ());
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
allocate_statement_stack_variables (ast::Node* node, runtime::MemoryModel& memory_model)
{
  struct visitor : public DefaultVisitor
  {
    runtime::MemoryModel& memory_model;

    visitor (runtime::MemoryModel& m) : memory_model (m) { }

    void default_action (Node& node)
    {
      AST_NOT_REACHED (node);
    }

    void visit (Const& node)
    {
      // Do nothing.
    }

    void visit (EmptyStatement& node)
    { }

    void visit (ForIotaStatement& node)
    {
      ptrdiff_t offset_before = memory_model.locals_offset ();
      allocate_symbol (memory_model, node.symbol);
      ptrdiff_t offset_after = memory_model.locals_offset ();
      allocate_statement_stack_variables (node.body, memory_model);
      memory_model.locals_pop (offset_after - offset_before);
      assert (memory_model.locals_offset () == offset_before);
    }

    void visit (BindPushPortStatement& node)
    {
      // Do nothing.
    }

    void visit (BindPushPortParamStatement& node)
    {
      // Do nothing.
    }

    void visit (BindPullPortStatement& node)
    {
      // Do nothing.
    }

    void visit (AssignStatement& node)
    {
      // Do nothing.
    }

    void visit (ChangeStatement& node)
    {
      ptrdiff_t offset_before = memory_model.locals_offset ();
      allocate_symbol (memory_model, node.root_symbol);
      ptrdiff_t offset_after = memory_model.locals_offset ();
      allocate_statement_stack_variables (node.body, memory_model);
      memory_model.locals_pop (offset_after - offset_before);
      assert (memory_model.locals_offset () == offset_before);
    }

    void visit (ExpressionStatement& node)
    {
      // Do nothing.
    }

    void visit (IfStatement& node)
    {
      allocate_statement_stack_variables (node.true_branch, memory_model);
      allocate_statement_stack_variables (node.false_branch, memory_model);
    }

    void visit (WhileStatement& node)
    {
      allocate_statement_stack_variables (node.body, memory_model);
    }

    void visit (AddAssignStatement& node)
    {
      // Do nothing.
    }

    void visit (SubtractAssignStatement& node)
    {
      // Do nothing.
    }

    void visit (ListStatement& node)
    {
      ptrdiff_t offset_before = memory_model.locals_offset ();
      for (List::ConstIterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          allocate_statement_stack_variables (*pos, memory_model);
        }
      ptrdiff_t offset_after = memory_model.locals_offset ();
      memory_model.locals_pop (offset_after - offset_before);
      assert (memory_model.locals_offset () == offset_before);
    }

    void visit (ReturnStatement& node)
    {
      // Do nothing.
    }

    void visit (IncrementDecrementStatement& node)
    {
      // Do nothing.
    }

    void visit (ActivateStatement& node)
    {
      allocate_statement_stack_variables (node.body, memory_model);
      node.memory_model = &memory_model;
    }

    void visit (VarStatement& node)
    {
      for (VarStatement::SymbolsType::const_iterator pos = node.symbols.begin (),
           limit = node.symbols.end ();
           pos != limit;
           ++pos)
        {
          allocate_symbol (memory_model, *pos);
        }
    }
  };
  visitor v (memory_model);
  node->accept (v);
}

void
allocate_stack_variables (ast::Node* node)
{
  struct visitor : public DefaultVisitor
  {
    void visit (ast::Action& node)
    {
      allocate_symbol (node.action->memory_model, node.action->receiver_parameter);
      allocate_statement_stack_variables (node.body, node.action->memory_model);
      assert (node.action->memory_model.locals_empty ());
    }

    void visit (DimensionedAction& node)
    {
      allocate_symbol (node.action->memory_model, node.action->iota_parameter);
      allocate_symbol (node.action->memory_model, node.action->receiver_parameter);
      allocate_statement_stack_variables (node.body, node.action->memory_model);
      assert (node.action->memory_model.locals_empty ());
    }

    void visit (ast::Bind& node)
    {
      allocate_symbol (node.bind->memory_model, node.bind->receiver_parameter);
      allocate_statement_stack_variables (node.body, node.bind->memory_model);
      assert (node.bind->memory_model.locals_empty ());
    }

    void visit (ast::Function& node)
    {
      allocate_parameters (node.function->memoryModel, node.function->signature ());
      allocate_symbol (node.function->memoryModel, node.function->return_parameter ());
      allocate_statement_stack_variables (node.body, node.function->memoryModel);
      assert (node.function->memoryModel.locals_empty ());
    }

    void visit (ast::Method& node)
    {
      allocate_parameters (node.method->memoryModel, node.method->signature ());
      allocate_symbol (node.method->memoryModel, node.method->receiver_parameter ());
      allocate_symbol (node.method->memoryModel, node.method->return_parameter ());
      allocate_statement_stack_variables (node.body, node.method->memoryModel);
      assert (node.method->memoryModel.locals_empty ());
    }

    void visit (ast::Initializer& node)
    {
      allocate_parameters (node.initializer->memoryModel, node.initializer->signature ());
      allocate_symbol (node.initializer->memoryModel, node.initializer->receiver_parameter ());
      allocate_symbol (node.initializer->memoryModel, node.initializer->return_parameter ());
      allocate_statement_stack_variables (node.body, node.initializer->memoryModel);
      assert (node.initializer->memoryModel.locals_empty ());
    }

    void visit (ast::Getter& node)
    {
      allocate_parameters (node.getter->memoryModel, node.getter->signature ());
      allocate_symbol (node.getter->memoryModel, node.getter->receiver_parameter ());
      allocate_symbol (node.getter->memoryModel, node.getter->return_parameter ());
      allocate_statement_stack_variables (node.body, node.getter->memoryModel);
      assert (node.getter->memoryModel.locals_empty ());
    }

    void visit (ast::Reaction& node)
    {
      allocate_parameters (node.reaction->memory_model, node.reaction->signature ());
      allocate_symbol (node.reaction->memory_model, node.reaction->receiver);
      allocate_statement_stack_variables (node.body, node.reaction->memory_model);
      assert (node.reaction->memory_model.locals_empty ());
    }

    void visit (DimensionedReaction& node)
    {
      allocate_parameters (node.reaction->memory_model, node.reaction->signature ());
      allocate_symbol (node.reaction->memory_model, node.reaction->iota);
      allocate_symbol (node.reaction->memory_model, node.reaction->receiver);
      allocate_statement_stack_variables (node.body, node.reaction->memory_model);
      assert (node.reaction->memory_model.locals_empty ());
    }

    void visit (SourceFile& node)
    {
      node.visit_children (*this);
    }
  };

  visitor v;
  node->accept (v);
}

void
allocate_parameters (runtime::MemoryModel& memory_model,
                     const decl::ParameterList* signature)
{
  for (decl::ParameterList::const_reverse_iterator pos = signature->rbegin (), limit = signature->rend ();
       pos != limit;
       ++pos)
    {
      allocate_symbol (memory_model, *pos);
    }
}

}
