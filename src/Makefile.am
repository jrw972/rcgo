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
bin_PROGRAMS = rcgo
rcgo_SOURCES = main.cpp \
debug.hpp \
package.hpp \
parser.hpp parser.cpp \
scanner.hpp scanner.cpp \
scheduler.hpp \
types.hpp \
util.hpp \
yyparse.hpp yyparse.cpp
rcgo_LDADD=librcgo.la
rcgo_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
rcgo_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

noinst_LTLIBRARIES=librcgo.la
librcgo_la_SOURCES = arch.hpp arch.cpp \
builtin_function.hpp builtin_function.cpp \
callable.hpp callable.cpp \
check_types.hpp check_types.cpp \
composition.hpp composition.cpp \
compute_receiver_access.hpp compute_receiver_access.cpp \
enter_predeclared_identifiers.hpp enter_predeclared_identifiers.cpp \
enter_method_identifiers.hpp enter_method_identifiers.cpp \
enter_top_level_identifiers.hpp enter_top_level_identifiers.cpp \
error_reporter.hpp error_reporter.cpp \
evaluate_static.hpp evaluate_static.cpp \
executor_base.hpp executor_base.cpp \
expression_value.hpp expression_value.cpp \
generate_code.hpp generate_code.cpp \
heap.hpp heap.cpp \
instance_scheduler.hpp instance_scheduler.cpp \
location.hpp location.cpp \
memory_model.hpp memory_model.cpp \
node.hpp node.cpp \
node_cast.hpp \
node_visitor.hpp node_visitor.cpp \
operation.hpp operation.cpp \
parameter_list.hpp parameter_list.cpp \
partitioned_scheduler.hpp partitioned_scheduler.cpp \
process_definitions.cpp \
process_top_level_declarations.hpp process_top_level_declarations.cpp \
process_type.hpp process_type.cpp \
runtime.hpp runtime.cpp \
runtime_types.hpp runtime_types.cpp \
scope.hpp scope.cpp \
semantic.hpp semantic.cpp \
stack.hpp stack.cpp \
symbol.hpp symbol.cpp \
symbol_cast.hpp \
symbol_visitor.hpp symbol_visitor.cpp \
polymorphic_function.hpp polymorphic_function.cpp \
type.hpp type.cpp \
value.hpp value.cpp
librcgo_la_LIBADD=-lpthread
librcgo_la_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
librcgo_la_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)
