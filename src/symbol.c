#include "symbol.h"
#include "debug.h"
#include "type.h"
#include "util.h"

struct symbol_t
{
  string_t identifier;
  SymbolKind kind;
  ast_t *defining_node;
  bool in_progress;
  union
  {
    struct
    {
      typed_value_t value;
    } typed_constant;
    struct
    {
      untyped_value_t value;
    } untyped_constant;
    struct
    {
      const type_t *type;
    } instance;
    struct
    {
      type_t *type;
      ParameterKind kind;
      symbol_t* original;
    } parameter;
    struct
    {
      const type_t *type;
    } variable;
    struct
    {
      type_t *type;
    } type;
  };
  ptrdiff_t offset;
};

const char* symbol_kind_string (SymbolKind kind)
{
  switch (kind)
    {
    case SymbolInstance:
      return "Instance";
    case SymbolParameter:
      return "Parameter";
    case SymbolType:
      return "Type";
    case SymbolTypedConstant:
      return "TypedConstant";
    case SymbolUntypedConstant:
      return "UntypedConstant";
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
    case SymbolUntypedConstant:
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
      return type_kind (symbol->type.type) != TypeUndefined;
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
  symbol_t *s = xmalloc (sizeof (symbol_t));
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
    case SymbolTypedConstant:
      unimplemented;
    case SymbolUntypedConstant:
      unimplemented;
    case SymbolInstance:
      return symbol_make_instance (identifier, NULL, defining_node);
    case SymbolParameter:
      unimplemented;
    case SymbolVariable:
      unimplemented;
    case SymbolType:
      return symbol_make_type (identifier, type_make_undefined (),
			       defining_node);
    }

  not_reached;
}

symbol_t *
symbol_make_variable (string_t identifier, const type_t * type)
{
  unimplemented;
  /* symbol_t *s = make (identifier, Variable); */
  /* s->variable.type = type; */
  /* return s; */
}

const type_t *
symbol_variable_type (const symbol_t * symbol)
{
  assert (symbol_kind (symbol) == SymbolVariable);
  return symbol->variable.type;
}

symbol_t *
symbol_make_type (string_t identifier, type_t * type, ast_t * defining_node)
{
  symbol_t *s = make (identifier, SymbolType, defining_node);
  type_set_name (type, identifier);
  s->type.type = type;
  return s;
}

type_t *
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
  s->typed_constant.value = value;
  return s;
}

typed_value_t
symbol_typed_constant_value (const symbol_t * symbol)
{
  assert (symbol_kind (symbol) == SymbolTypedConstant);
  return symbol->typed_constant.value;
}

symbol_t *
symbol_make_untyped_constant (string_t identifier, untyped_value_t value,
			      ast_t * defining_node)
{
  symbol_t *s = make (identifier, SymbolUntypedConstant, defining_node);
  s->untyped_constant.value = value;
  return s;
}

untyped_value_t
symbol_untyped_constant_value (const symbol_t * symbol)
{
  assert (symbol_kind (symbol) == SymbolUntypedConstant);
  return symbol->untyped_constant.value;
}

symbol_t *
symbol_make_instance (string_t identifier, const type_t * type,
		      ast_t * defining_node)
{
  symbol_t *s = make (identifier, SymbolInstance, defining_node);
  s->instance.type = type;
  return s;
}

void
symbol_set_instance_type (symbol_t * symbol, type_t * type)
{
  assert (symbol->kind == SymbolInstance);
  symbol->instance.type = type;
}

const type_t *
symbol_get_instance_type (const symbol_t * symbol)
{
  assert (symbol->kind == SymbolInstance);
  return symbol->instance.type;
}

symbol_t *
symbol_make_parameter (string_t identifier, type_t * type,
		       ast_t * defining_node)
{
  symbol_t *s = make (identifier, SymbolParameter, defining_node);
  s->parameter.type = type;
  s->parameter.kind = ParameterOrdinary;
  return s;
}

symbol_t *symbol_make_return_parameter (string_t identifier, type_t * type,
                                        ast_t * defining_node)
{
  symbol_t *s = make (identifier, SymbolParameter, defining_node);
  s->parameter.type = type;
  s->parameter.kind = ParameterReturn;
  return s;
}

symbol_t *
symbol_make_receiver (string_t identifier, type_t * type,
                      ast_t * defining_node)
{
  symbol_t *s = make (identifier, SymbolParameter, defining_node);
  s->parameter.type = type;
  s->parameter.kind = ParameterReceiver;
  return s;
}

symbol_t *
symbol_make_receiver_duplicate (symbol_t* receiver)
{
  symbol_t *s = make (symbol_identifier (receiver), SymbolParameter, symbol_defining_node (receiver));
  s->parameter.type = type_make_pointer (PointerToMutable,
                                         type_pointer_base_type
                                         (symbol_parameter_type
                                          (receiver)));
  s->parameter.kind = ParameterReceiverDuplicate;
  s->parameter.original = receiver;
  return s;
}

ParameterKind
symbol_parameter_kind (const symbol_t * symbol)
{
  assert (symbol_kind (symbol) == SymbolParameter);
  return symbol->parameter.kind;
}

type_t *
symbol_parameter_type (const symbol_t * symbol)
{
  assert (symbol_kind (symbol) == SymbolParameter);
  return symbol->parameter.type;
}

void symbol_set_offset (symbol_t* symbol,
                        ptrdiff_t offset)
{
  symbol->offset = offset;
}

ptrdiff_t symbol_get_offset (const symbol_t* symbol)
{
  if (symbol->kind == SymbolParameter && symbol->parameter.kind == ParameterReceiverDuplicate)
    {
      return symbol->parameter.original->offset;
    }
  return symbol->offset;
}
