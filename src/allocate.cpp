#include "allocate.hpp"

#include "symbol.hpp"
#include "symbol_visitor.hpp"
#include "memory_model.hpp"
#include "node_visitor.hpp"
#include "node.hpp"
#include "callable.hpp"

namespace code
{

using namespace decl;
using namespace ast;

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

    void visit (Parameter& symbol)
    {
      switch (symbol.kind)
        {
        case Parameter::Ordinary:
        case Parameter::Receiver:
        case Parameter::Return:
        {
          const type::Type* type = symbol.type;
          memory_model.arguments_push (type);
          static_cast<Symbol&> (symbol).offset (memory_model.arguments_offset ());
          if (symbol.kind == Parameter::Receiver)
            {
              memory_model.set_receiver_offset ();
            }
        }
        break;
        case Parameter::Receiver_Duplicate:
        case Parameter::Ordinary_Duplicate:
          break;
        }
    }

    void visit (Constant& symbol)
    {
      // No need to allocate.
    }

    void visit (Variable& symbol)
    {
      const type::Type* type = symbol.type;
      static_cast<Symbol&>(symbol).offset (memory_model.locals_offset ());
      memory_model.locals_push (type);
    }

    void visit (decl::Hidden& symbol)
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
  struct visitor : public DefaultNodeVisitor
  {
    runtime::MemoryModel& memory_model;

    visitor (runtime::MemoryModel& m) : memory_model (m) { }

    void default_action (Node& node)
    {
      NODE_NOT_REACHED (node);
    }

    void visit (ConstDecl& node)
    {
      // Do nothing.
    }

    void visit (EmptyStatement& node)
    { }

    void visit (ForIota& node)
    {
      ptrdiff_t offset_before = memory_model.locals_offset ();
      allocate_symbol (memory_model, node.symbol);
      ptrdiff_t offset_after = memory_model.locals_offset ();
      allocate_statement_stack_variables (node.body, memory_model);
      memory_model.locals_pop (offset_after - offset_before);
      assert (memory_model.locals_offset () == offset_before);
    }

    void visit (BindPushPort& node)
    {
      // Do nothing.
    }

    void visit (BindPushPortParameter& node)
    {
      // Do nothing.
    }

    void visit (BindPullPort& node)
    {
      // Do nothing.
    }

    void visit (Assign& node)
    {
      // Do nothing.
    }

    void visit (Change& node)
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

    void visit (If& node)
    {
      allocate_statement_stack_variables (node.before, memory_model);
      allocate_statement_stack_variables (node.true_body, memory_model);
      allocate_statement_stack_variables (node.false_body, memory_model);
    }

    void visit (While& node)
    {
      allocate_statement_stack_variables (node.body, memory_model);
    }

    void visit (AddAssign& node)
    {
      // Do nothing.
    }

    void visit (SubtractAssign& node)
    {
      // Do nothing.
    }

    void visit (StatementList& node)
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

    void visit (Return& node)
    {
      // Do nothing.
    }

    void visit (IncrementDecrement& node)
    {
      // Do nothing.
    }

    void visit (Activate& node)
    {
      allocate_statement_stack_variables (node.body, memory_model);
      node.memory_model = &memory_model;
    }

    void visit (VarDecl& node)
    {
      for (VarDecl::SymbolsType::const_iterator pos = node.symbols.begin (),
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
  struct visitor : public DefaultNodeVisitor
  {
    void visit (ast::ActionDecl& node)
    {
      allocate_symbol (node.action->memory_model, node.action->receiver_parameter ());
      if (node.action->dimension () != -1)
        {
          allocate_symbol (node.action->memory_model, node.action->iota_parameter ());
        }

      allocate_statement_stack_variables (node.body, node.action->memory_model);
      assert (node.action->memory_model.locals_empty ());
    }

    void visit (ast::BinderDecl& node)
    {
      allocate_symbol (node.bind->memory_model, node.bind->receiver_parameter ());
      allocate_statement_stack_variables (node.body, node.bind->memory_model);
      assert (node.bind->memory_model.locals_empty ());
    }

    void visit (ast::FunctionDecl& node)
    {
      allocate_parameters (node.symbol->memory_model, node.symbol->parameter_list ());
      allocate_parameters (node.symbol->memory_model, node.symbol->return_parameter_list ());
      allocate_statement_stack_variables (node.body, node.symbol->memory_model);
      assert (node.symbol->memory_model.locals_empty ());
    }

    void visit (ast::MethodDecl& node)
    {
      allocate_parameters (node.method->memory_model, node.method->parameter_list ());
      allocate_parameters (node.method->memory_model, node.method->return_parameter_list ());
      allocate_statement_stack_variables (node.body, node.method->memory_model);
      assert (node.method->memory_model.locals_empty ());
    }

    void visit (ast::InitializerDecl& node)
    {
      allocate_parameters (node.initializer->memory_model, node.initializer->parameter_list ());
      allocate_parameters (node.initializer->memory_model, node.initializer->return_parameter_list ());
      allocate_statement_stack_variables (node.body, node.initializer->memory_model);
      assert (node.initializer->memory_model.locals_empty ());
    }

    void visit (ast::GetterDecl& node)
    {
      allocate_parameters (node.getter->memory_model, node.getter->parameter_list ());
      allocate_parameters (node.getter->memory_model, node.getter->return_parameter_list ());
      allocate_statement_stack_variables (node.body, node.getter->memory_model);
      assert (node.getter->memory_model.locals_empty ());
    }

    void visit (ast::ReactionDecl& node)
    {
      allocate_parameters (node.reaction->memory_model, node.reaction->parameter_list ());
      if (node.reaction->dimension () != -1)
        {
          allocate_symbol (node.reaction->memory_model, node.reaction->iota ());
        }
      allocate_statement_stack_variables (node.body, node.reaction->memory_model);
      assert (node.reaction->memory_model.locals_empty ());
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
