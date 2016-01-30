EXTRA_DIST = scanner.l parser.y
BUILT_SOURCES = scanner.hpp scanner.cpp parser.hpp parser.cpp

scanner.hpp scanner.cpp: scanner.l
	flex --header-file=scanner.hpp --outfile=scanner.cpp scanner.l

parser.hpp parser.cpp: parser.y
	bison --defines=parser.hpp --output=parser.cpp -v -d parser.y

#AM_LFLAGS = --header=$(addsuffix .hpp,$(basename $@))
#AM_YFLAGS = -v -d
bin_PROGRAMS = rc
rc_SOURCES = main.cpp \
ast_cast.hpp \
bind.hpp \
builtin_function.hpp builtin_function.cpp \
composition.hpp composition.cpp \
debug.hpp \
enter_symbols.cpp \
evaluate_static.hpp evaluate_static.cpp \
executor_base.hpp \
field.hpp \
generate_code.hpp generate_code.cpp \
instance_scheduler.hpp instance_scheduler.cpp \
parser.hpp parser.cpp \
partitioned_scheduler.hpp partitioned_scheduler.cpp \
process_definitions.cpp \
process_functions_and_methods.hpp process_functions_and_methods.cpp \
runtime.hpp runtime.cpp \
scanner.hpp scanner.cpp \
scheduler.hpp \
template.hpp template.cpp \
types.hpp \
util.hpp \
yyparse.hpp yyparse.cpp
rc_LDADD=librc.la

noinst_LTLIBRARIES=librc.la
librc_la_SOURCES = action.hpp action.cpp \
arch.hpp arch.cpp \
ast.hpp ast.cpp \
ast_visitor.hpp ast_visitor.cpp \
callable.hpp callable.cpp \
check_types.hpp check_types.cpp \
compute_receiver_access.hpp compute_receiver_access.cpp \
error_reporter.hpp error_reporter.cpp \
heap.hpp heap.cpp \
location.hpp location.cpp \
memory_model.hpp memory_model.cpp \
process_type_spec.cpp \
process_types_and_constants.hpp process_types_and_constants.cpp \
reaction.hpp reaction.cpp \
semantic.hpp semantic.cpp \
stack.hpp stack.cpp \
symbol.hpp symbol.cpp \
symbol_table.hpp symbol_table.cpp \
symbol_visitor.hpp \
type.hpp type.cpp \
value.hpp value.cpp
librc_la_LIBADD=-lpthread
