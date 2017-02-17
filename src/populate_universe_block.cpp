#include "builtin_function.hpp"
#include "scope.hpp"
#include "polymorphic_function.hpp"
#include "symbol_table.hpp"
#include "identifier.hpp"

namespace decl
{

using namespace ast;
using namespace semantic;
using namespace runtime;

void
populate_universe_block (SymbolTable& symbol_table)
{
  // Insert types.
  symbol_table.enter_symbol (&type::named_bool);

  symbol_table.enter_symbol (&type::named_uint8);
  symbol_table.enter_symbol (&type::named_uint16);
  symbol_table.enter_symbol (&type::named_uint32);
  symbol_table.enter_symbol (&type::named_uint64);

  symbol_table.enter_symbol (&type::named_int8);
  symbol_table.enter_symbol (&type::named_int16);
  symbol_table.enter_symbol (&type::named_int32);
  symbol_table.enter_symbol (&type::named_int64);

  symbol_table.enter_symbol (&type::named_float32);
  symbol_table.enter_symbol (&type::named_float64);

  symbol_table.enter_symbol (&type::named_complex64);
  symbol_table.enter_symbol (&type::named_complex128);

  symbol_table.enter_symbol (&type::named_byte);
  symbol_table.enter_symbol (&type::named_rune);

  symbol_table.enter_symbol (&type::named_uint);
  symbol_table.enter_symbol (&type::named_int);
  symbol_table.enter_symbol (&type::named_uintptr);

  symbol_table.enter_symbol (&type::named_string);
  symbol_table.enter_symbol (&type::named_error);

  // Insert functions.
  symbol_table.enter_symbol (&decl::append_func);
  symbol_table.enter_symbol (&decl::cap_func);
  symbol_table.enter_symbol (&decl::complex_func);
  symbol_table.enter_symbol (&decl::copy_func);
  symbol_table.enter_symbol (&decl::delete_func);
  symbol_table.enter_symbol (&decl::imag_func);
  symbol_table.enter_symbol (&decl::len_func);
  symbol_table.enter_symbol (&decl::make_func);
  symbol_table.enter_symbol (&decl::merge_func);
  symbol_table.enter_symbol (&decl::move_func);
  symbol_table.enter_symbol (&decl::new_func);
  symbol_table.enter_symbol (&decl::panic_func);
  symbol_table.enter_symbol (&decl::print_func);
  symbol_table.enter_symbol (&decl::println_func);
  symbol_table.enter_symbol (&decl::real_func);
  symbol_table.enter_symbol (&decl::recover_func);

  // I/O facilities.
  symbol_table.enter_symbol (&type::named_file_descriptor);
  symbol_table.enter_symbol (&type::named_timespec);
  // TODO:  Allocate statically.
  symbol_table.enter_symbol (new Readable (source::Location::builtin));
  symbol_table.enter_symbol (new Read (source::Location::builtin));
  symbol_table.enter_symbol (new Writable (source::Location::builtin));
  symbol_table.enter_symbol (new ClockGettime (source::Location::builtin));
  symbol_table.enter_symbol (new TimerfdCreate (source::Location::builtin));
  symbol_table.enter_symbol (new TimerfdSettime (source::Location::builtin));
  symbol_table.enter_symbol (new UdpSocket (source::Location::builtin));
  symbol_table.enter_symbol (new Sendto (source::Location::builtin));

  // Insert zero constant.
  ExpressionValue v;
  v.kind = ExpressionValue::Constant;
  v.type = type::UntypedNil::instance ();
  // TODO:  Allocate statically.
  symbol_table.enter_symbol (new Constant (source::Identifier ("nil", source::Location::builtin), v));

  // Insert untyped boolean constants.
  v.type = type::UntypedBoolean::instance ();
  v.value.bool_value = true;
  // TODO:  Allocate statically.
  symbol_table.enter_symbol (new Constant (source::Identifier ("true", source::Location::builtin), v));
  v.value.bool_value = false;
  // TODO:  Allocate statically.
  symbol_table.enter_symbol (new Constant (source::Identifier ("false", source::Location::builtin), v));
}

}
