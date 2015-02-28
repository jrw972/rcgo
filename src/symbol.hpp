#ifndef symbol_h
#define symbol_h

#include "types.hpp"
#include "strtab.hpp"
#include "typed_value.hpp"
#include "type.hpp"

typedef enum
{
  SymbolFunction,
  SymbolInstance,
  SymbolParameter,
  SymbolType,
  SymbolTypedConstant,
  SymbolVariable,
} SymbolKind;

const char* symbol_kind_string (SymbolKind kind);

string_t symbol_identifier (const symbol_t * symbol);

SymbolKind symbol_kind (const symbol_t * symbol);

ast_t *symbol_defining_node (const symbol_t * symbol);

bool symbol_defined (const symbol_t * symbol);

bool symbol_get_in_progress (const symbol_t * symbol);

void symbol_set_in_progress (symbol_t * symbol, bool in_progress);

symbol_t *symbol_make_undefined (string_t identifier, SymbolKind kind,
				 ast_t * defining_node);

typedef enum
  {
    VariableOrdinary,
    VariableDuplicate
  } VariableKind;

symbol_t *symbol_make_variable (string_t identifier, const type_t * type, ast_t* defining_node);

symbol_t *symbol_make_variable_duplicate (symbol_t* symbol, const type_t * type);

const type_t *symbol_variable_type (const symbol_t * symbol);

typed_value_t symbol_variable_value (const symbol_t * symbol);

symbol_t *symbol_make_type (string_t identifier, named_type_t * type,
			    ast_t * defining_node);

named_type_t *symbol_get_type_type (const symbol_t * symbol);

symbol_t *symbol_make_typed_constant (string_t identifier,
				      typed_value_t value,
				      ast_t * defining_node);

typed_value_t symbol_typed_constant_value (const symbol_t * symbol);

symbol_t *symbol_make_instance (string_t identifier, const named_type_t * type,
				ast_t * defining_node);

void symbol_set_instance_type (symbol_t * symbol, named_type_t * type);

const named_type_t *symbol_get_instance_type (const symbol_t * symbol);

void symbol_set_instance_method (symbol_t* symbol, method_t* method);

method_t* symbol_get_instance_method (symbol_t* symbol);

typedef enum
{
  ParameterOrdinary,
  ParameterReceiver,
  ParameterReceiverDuplicate,
  ParameterReturn,
  ParameterDuplicate,
} ParameterKind;

symbol_t *symbol_make_parameter (string_t identifier, const type_t * type,
				 ast_t * defining_node);

symbol_t *symbol_make_return_parameter (string_t identifier, const type_t * type,
                                        ast_t * defining_node);

symbol_t *symbol_make_receiver (string_t identifier, const type_t * type,
                                ast_t* defining_node);

symbol_t *symbol_make_receiver_duplicate (symbol_t* receiver);

symbol_t *symbol_make_parameter_duplicate (symbol_t* symbol, type_t * type);

ParameterKind symbol_parameter_kind (const symbol_t * symbol);

const type_t *symbol_parameter_type (const symbol_t * symbol);

typed_value_t symbol_parameter_value (const symbol_t * symbol);

void symbol_set_offset (symbol_t* symbol,
                        ptrdiff_t offset);

ptrdiff_t symbol_get_offset (const symbol_t* symbol);

symbol_t* symbol_make_function (string_t identifier, ast_t* defining_node);

void symbol_set_function_function (symbol_t* symbol, function_t* function);

function_t* symbol_get_function_function (const symbol_t* symbol);

#endif /* symbol_h */
