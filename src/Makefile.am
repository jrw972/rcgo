EXTRA_DIST = scanner.l parser.y
BUILT_SOURCES = scanner.hpp scanner.cpp parser.hpp parser.cpp

scanner.hpp scanner.cpp: scanner.l
	flex --header-file=$(srcdir)/scanner.hpp --outfile=$(srcdir)/scanner.cpp $(srcdir)/scanner.l
	sed -i -e '/#line/ { s@\.\./@@g ; s@src/@@g  }' $(srcdir)/scanner.hpp
	sed -i -e '/#line/ { s@\.\./@@g ; s@src/@@g  }' $(srcdir)/scanner.cpp

parser.hpp parser.cpp: parser.y
	bison --defines=$(srcdir)/parser.hpp --output=$(srcdir)/parser.cpp -v -d $(srcdir)/parser.y
	sed -i -e '/#line/ { s@\.\./@@g ; s@src/@@g  }' $(srcdir)/parser.hpp
	sed -i -e '/#line/ { s@\.\./@@g ; s@src/@@g  }' $(srcdir)/parser.cpp

#AM_LFLAGS = --header=$(addsuffix .hpp,$(basename $@))
#AM_YFLAGS = -v -d
bin_PROGRAMS = rc
rc_SOURCES = main.cpp \
bind.hpp \
debug.hpp \
evaluate_static.hpp evaluate_static.cpp \
executor_base.hpp executor_base.cpp \
field.hpp \
generate_code.hpp generate_code.cpp \
instance_scheduler.hpp instance_scheduler.cpp \
package.hpp \
parser.hpp parser.cpp \
partitioned_scheduler.hpp partitioned_scheduler.cpp \
process_definitions.cpp \
process_functions_and_methods.hpp process_functions_and_methods.cpp \
scanner.hpp scanner.cpp \
scheduler.hpp \
types.hpp \
util.hpp \
yyparse.hpp yyparse.cpp
rc_LDADD=librc.la
rc_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
rc_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

noinst_LTLIBRARIES=librc.la
librc_la_SOURCES = action.hpp action.cpp \
arch.hpp arch.cpp \
builtin_function.hpp builtin_function.cpp \
callable.hpp callable.cpp \
check_types.hpp check_types.cpp \
complex.hpp \
composition.hpp composition.cpp \
compute_receiver_access.hpp compute_receiver_access.cpp \
enter_symbols.cpp \
error_reporter.hpp error_reporter.cpp \
expression_value.hpp expression_value.cpp \
heap.hpp heap.cpp \
location.hpp location.cpp \
memory_model.hpp memory_model.cpp \
node.hpp node.cpp \
node_cast.hpp \
node_visitor.hpp node_visitor.cpp \
parameter_list.hpp parameter_list.cpp \
process_type_spec.cpp \
process_types_and_constants.hpp process_types_and_constants.cpp \
reaction.hpp reaction.cpp \
runtime.hpp runtime.cpp \
semantic.hpp semantic.cpp \
stack.hpp stack.cpp \
symbol.hpp symbol.cpp \
symbol_cast.hpp \
symbol_table.hpp symbol_table.cpp \
symbol_visitor.hpp symbol_visitor.cpp \
template.hpp template.cpp \
type.hpp type.cpp \
value.hpp value.cpp
librc_la_LIBADD=-lpthread
librc_la_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
librc_la_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)
