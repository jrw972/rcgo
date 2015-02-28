#include "symbol.hpp"
#include "debug.hpp"
#include "type.hpp"
#include "util.hpp"
#include <string.h>

struct symbol_t
{
  string_t identifier;
  SymbolKind kind;
  ast_t *defining_node;
  bool in_progress;
  struct
  {
    typed_value_t value;
  } typed_constant;
  struct
  {
    typed_value_t value;
    VariableKind kind;
    symbol_t* original;
  } variable;
  struct
  {
    typed_value_t value;
    ParameterKind kind;
    symbol_t* original;
  } parameter;
  union
  {
    struct
    {
      const named_type_t *type;
      method_t* method;
    } instance;
    struct
    {
      named_type_t *type;
    } type;
    struct
    {
      function_t* function;
    } function;
  };
  ptrdiff_t offset;
};

const char* symbol_kind_string (SymbolKind kind)
{
  switch (kind)
    {
    case SymbolFunction:
      return "Function";
    case SymbolInstance:
      return "Instance";
    case SymbolParameter:
      return "Parameter";
    case SymbolType:
      return "Type";
    case SymbolTypedConstant:
      return "TypedConstant";
    case SymbolVariable:
      return "Variable";
    }

  return "<Unknown Symbol Kind>";
}

string_t
symbol_identifier (const symbol_t * symbol)
{
  return symbol->identifier;
}

SymbolKind
symbol_kind (const symbol_t * symbol)
{
  return symbol->kind;
}

ast_t *
symbol_defining_node (const symbol_t * symbol)
{
  return symbol->defining_node;
}

bool
symbol_defined (const symbol_t * symbol)
{
  switch (symbol->kind)
    {
    case SymbolFunction:
      unimplemented;
    case SymbolTypedConstant:
      unimplemented;
    case SymbolInstance:
      unimplemented;
    case SymbolParameter:
      unimplemented;
    case SymbolVariable:
      unimplemented;
    case SymbolType:
      return symbol->type.type->subtype () != NULL;
    }

  not_reached;
}

bool
symbol_get_in_progress (const symbol_t * symbol)
{
  return symbol->in_progress;
}

void
symbol_set_in_progress (symbol_t * symbol, bool in_progress)
{
  symbol->in_progress = in_progress;
}

static symbol_t *
make (string_t identifier, SymbolKind kind, ast_t * defining_node)
{
  symbol_t *s = (symbol_t*)malloc (sizeof (symbol_t));
  memset (s, 0, sizeof (symbol_t));
  s->identifier = identifier;
  s->kind = kind;
  s->defining_node = defining_node;
  return s;
}

symbol_t *
symbol_make_undefined (string_t identifier, SymbolKind kind,
		       ast_t * defining_node)
{
  switch (kind)
    {
    case SymbolFunction:
      return symbol_make_function (identifier, defining_node);
    case SymbolTypedConstant:
      unimplemented;
    case SymbolInstance:
      return symbol_make_instance (identifier, NULL, defining_node);
    case SymbolParameter:
      unimplemented;
    case SymbolVariable:
      unimplemented;
    case SymbolType:
      return symbol_make_type (identifier, new named_type_t (identifier),
			       defining_node);
    }

  not_reached;
}

symbol_t *
symbol_make_variable (string_t identifier, const type_t * type, ast_t* defining_node)
{
  symbol_t *s = make (identifier, SymbolVariable, defining_node);
  s->variable.value = typed_value_t::make_ref (type, typed_value_t::MUTABLE);
  s->variable.kind = VariableOrdinary;
  return s;
}

symbol_t *
symbol_make_variable_duplicate (symbol_t* symbol, const type_t * type)
{
  assert (symbol->kind == SymbolVariable);
  symbol_t *s = make (symbol->identifier, SymbolVariable, symbol->defining_node);
  s->variable.value = typed_value_t::make_ref (type, typed_value_t::MUTABLE);
  s->variable.kind = VariableDuplicate;
  s->variable.original = symbol;
  return s;
}

const type_t *
symbol_variable_type (const symbol_t * symbol)
{
  assert (symbol_kind (symbol) == SymbolVariable);
  return symbol->variable.value.type;
}

typed_value_t
symbol_variable_value (const symbol_t * symbol)
{
  assert (symbol_kind (symbol) == SymbolVariable);
  return symbol->variable.value;
}

symbol_t *
symbol_make_type (string_t identifier, named_type_t * type, ast_t * defining_node)
{
  symbol_t *s = make (identifier, SymbolType, defining_node);
  s->type.type = type;
  return s;
}

named_type_t *
symbol_get_type_type (const symbol_t * symbol)
{
  assert (symbol_kind (symbol) == SymbolType);
  return symbol->type.type;
}

symbol_t *
symbol_make_typed_constant (string_t identifier, typed_value_t value,
			    ast_t * defining_node)
{
  symbol_t *s = make (identifier, SymbolTypedConstant, defining_node);
  s->typed_constant.value = typed_value_t::make_ref (value);
  return s;
}

typed_value_t
symbol_typed_constant_value (const symbol_t * symbol)
{
  assert (symbol_kind (symbol) == SymbolTypedConstant);
  return symbol->typed_constant.value;
}

symbol_t *
symbol_make_instance (string_t identifier, const named_type_t * type,
		      ast_t * defining_node)
{
  symbol_t *s = make (identifier, SymbolInstance, defining_node);
  s->instance.type = type;
  return s;
}

void
symbol_set_instance_type (symbol_t * symbol, named_type_t * type)
{
  assert (symbol->kind == SymbolInstance);
  symbol->instance.type = type;
}

const named_type_t *
symbol_get_instance_type (const symbol_t * symbol)
{
  assert (symbol->kind == SymbolInstance);
  return symbol->instance.type;
}

void symbol_set_instance_method (symbol_t* symbol, method_t* method)
{
  assert (symbol->kind == SymbolInstance);
  symbol->instance.method = method;
}

method_t* symbol_get_instance_method (symbol_t* symbol)
{
  assert (symbol->kind == SymbolInstance);
  return symbol->instance.method;
}

symbol_t *
symbol_make_parameter (string_t identifier, const type_t * type,
		       ast_t * defining_node)
{
  symbol_t *s = make (identifier, SymbolParameter, defining_node);
  s->parameter.value = typed_value_t::make_ref (type, typed_value_t::MUTABLE);
  s->parameter.kind = ParameterOrdinary;
  return s;
}

symbol_t *symbol_make_return_parameter (string_t identifier, const type_t * type,
                                        ast_t * defining_node)
{
  symbol_t *s = make (identifier, SymbolParameter, defining_node);
  s->parameter.value = typed_value_t::make_ref (type, typed_value_t::MUTABLE);
  s->parameter.kind = ParameterReturn;
  return s;
}

symbol_t *
symbol_make_receiver (string_t identifier, const type_t * type,
                      ast_t * defining_node)
{
  symbol_t *s = make (identifier, SymbolParameter, defining_node);
  s->parameter.value = typed_value_t::make_ref (type, typed_value_t::MUTABLE);
  s->parameter.kind = ParameterReceiver;
  return s;
}

symbol_t *
symbol_make_receiver_duplicate (symbol_t* receiver)
{
  // Strip out the immutable.
  const type_t* t = symbol_parameter_type (receiver);
  t = dynamic_cast<const pointer_to_immutable_type_t*> (t)->base_type ();
  t = pointer_type_t::make (t);

  symbol_t *s = make (symbol_identifier (receiver), SymbolParameter, symbol_defining_node (receiver));
  s->parameter.value = typed_value_t::make_ref (t, typed_value_t::MUTABLE);
  s->parameter.kind = ParameterReceiverDuplicate;
  s->parameter.original = receiver;
  return s;
}

symbol_t *
symbol_make_parameter_duplicate (symbol_t* symbol, type_t * type)
{
  assert (symbol->kind == SymbolParameter);
  symbol_t *s = make (symbol->identifier, SymbolParameter, symbol->defining_node);
  s->parameter.value = typed_value_t::make_ref (type, typed_value_t::MUTABLE);
  s->parameter.kind = ParameterDuplicate;
  s->parameter.original = symbol;
  return s;
}

ParameterKind
symbol_parameter_kind (const symbol_t * symbol)
{
  assert (symbol_kind (symbol) == SymbolParameter);
  return symbol->parameter.kind;
}

const type_t *
symbol_parameter_type (const symbol_t * symbol)
{
  assert (symbol_kind (symbol) == SymbolParameter);
  return symbol->parameter.value.type;
}

typed_value_t symbol_parameter_value (const symbol_t * symbol)
{
  assert (symbol_kind (symbol) == SymbolParameter);
  return symbol->parameter.value;
}

void symbol_set_offset (symbol_t* symbol,
                        ptrdiff_t offset)
{
  symbol->offset = offset;
}

ptrdiff_t symbol_get_offset (const symbol_t* symbol)
{
  if (symbol->kind == SymbolParameter &&
      (symbol->parameter.kind == ParameterReceiverDuplicate ||
       symbol->parameter.kind == ParameterDuplicate))
    {
      return symbol_get_offset (symbol->parameter.original);
    }
  else if (symbol->kind == SymbolVariable && symbol->variable.kind == VariableDuplicate)
    {
      return symbol_get_offset (symbol->variable.original);
    }
  return symbol->offset;
}

symbol_t* symbol_make_function (string_t identifier, ast_t* defining_node)
{
  return make (identifier, SymbolFunction, defining_node);
}

void symbol_set_function_function (symbol_t* symbol, function_t* function)
{
  assert (symbol->kind == SymbolFunction);
  symbol->function.function = function;
}

function_t* symbol_get_function_function (const symbol_t* symbol)
{
  assert (symbol->kind == SymbolFunction);
  return symbol->function.function;
}
