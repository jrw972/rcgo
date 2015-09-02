#ifndef symbol_h
#define symbol_h

#include "types.hpp"
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
  SymbolHidden,
} SymbolKind;

const char* symbol_kind_string (SymbolKind kind);

const std::string& symbol_identifier (const symbol_t * symbol);

SymbolKind symbol_kind (const symbol_t * symbol);

ast_t *symbol_defining_node (const symbol_t * symbol);

bool symbol_defined (const symbol_t * symbol);

bool symbol_get_in_progress (const symbol_t * symbol);

void symbol_set_in_progress (symbol_t * symbol, bool in_progress);

symbol_t *symbol_make_undefined (const std::string& identifier, SymbolKind kind,
				 ast_t * defining_node);

typedef enum
  {
    VariableOrdinary,
    VariableDuplicate
  } VariableKind;

symbol_t *symbol_make_variable (const std::string& identifier, const typed_value_t& tv, ast_t* defining_node);

symbol_t *symbol_make_variable_duplicate (symbol_t* symbol);

const type_t *symbol_variable_type (const symbol_t * symbol);

typed_value_t symbol_variable_value (const symbol_t * symbol);

symbol_t *symbol_make_type (const std::string& identifier, named_type_t * type,
			    ast_t * defining_node);

named_type_t *symbol_get_type_type (const symbol_t * symbol);

symbol_t *symbol_make_typed_constant (const std::string& identifier,
				      typed_value_t value,
				      ast_t * defining_node);

typed_value_t symbol_typed_constant_value (const symbol_t * symbol);

symbol_t *symbol_make_instance (const std::string& identifier, const named_type_t * type,
				ast_t * defining_node);

void symbol_set_instance_type (symbol_t * symbol, named_type_t * type);

const named_type_t *symbol_get_instance_type (const symbol_t * symbol);

void symbol_set_instance_initializer (symbol_t* symbol, initializer_t* initializer);

initializer_t* symbol_get_instance_initializer (symbol_t* symbol);

typedef enum
{
  ParameterOrdinary,
  ParameterReceiver,
  ParameterReceiverDuplicate,
  ParameterReturn,
  ParameterDuplicate,
} ParameterKind;

symbol_t *symbol_make_parameter (const parameter_t* parameter);

symbol_t *symbol_make_return_parameter (const parameter_t* parameter);

symbol_t *symbol_make_receiver (const parameter_t* parameter);

symbol_t *symbol_make_receiver_duplicate (symbol_t* receiver);

symbol_t *symbol_make_parameter_duplicate (symbol_t* symbol);

ParameterKind symbol_parameter_kind (const symbol_t * symbol);

const type_t *symbol_parameter_type (const symbol_t * symbol);

typed_value_t symbol_parameter_value (const symbol_t * symbol);

void symbol_set_offset (symbol_t* symbol,
                        ptrdiff_t offset);

ptrdiff_t symbol_get_offset (const symbol_t* symbol);

symbol_t* symbol_make_function (const std::string& identifier, ast_t* defining_node);

void symbol_set_function_function (symbol_t* symbol, function_t* function);

function_t* symbol_get_function_function (const symbol_t* symbol);

const type_t* symbol_type (const symbol_t* symbol);

symbol_t* symbol_make_hidden (const symbol_t* symbol, ast_t* defining_node);

#endif /* symbol_h */
