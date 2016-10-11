LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/build-aux/tap-driver.sh

AM_CXXFLAGS = -I $(top_srcdir)/src

EXTRA_DIST = callable.test \
enter_method_identifiers.test \
enter_predeclared_identifiers.test \
enter_top_level_identifiers.test \
error_reporter.test \
node.test \
process_top_level_identifiers.test \
process_type.test \
symbol.test

%.cpp : %.test
	awk -f $(top_srcdir)/utest/testpro.awk $< > $@

TESTS = \
 arch \
 check_types \
 expression_value \
 heap \
 location memory_model \
 node_cast \
 parameter_list \
 polymorphic_function \
 runtime_types \
 semantic \
 stack \
 symbol_cast \
 scope \
 type \
 value \
 unit_test
check_PROGRAMS = $(TESTS)

HELPERS=unit_test.hpp tap.hpp visitor_helper.hpp mock_execution.hpp astgen.hpp astgen.cpp

arch_SOURCES = arch.cpp $(HELPERS)
arch_LDADD = $(top_builddir)/src/librcgo.la
arch_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
arch_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

check_types_SOURCES = check_types.cpp $(HELPERS)
check_types_LDADD = $(top_builddir)/src/librcgo.la
check_types_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
check_types_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

expression_value_SOURCES = expression_value.cpp $(HELPERS)
expression_value_LDADD = $(top_builddir)/src/librcgo.la
expression_value_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
expression_value_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

heap_SOURCES = heap.cpp $(HELPERS)
heap_LDADD = $(top_builddir)/src/librcgo.la
heap_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
heap_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

location_SOURCES = location.cpp $(HELPERS)
location_LDADD = $(top_builddir)/src/librcgo.la
location_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
location_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

memory_model_SOURCES = memory_model.cpp $(HELPERS)
memory_model_LDADD = $(top_builddir)/src/librcgo.la
memory_model_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
memory_model_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

node_cast_SOURCES = node_cast.cpp $(HELPERS)
node_cast_LDADD = $(top_builddir)/src/librcgo.la
node_cast_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
node_cast_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

parameter_list_SOURCES = parameter_list.cpp $(HELPERS)
parameter_list_LDADD = $(top_builddir)/src/librcgo.la
parameter_list_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
parameter_list_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

semantic_SOURCES = semantic.cpp $(HELPERS)
semantic_LDADD = $(top_builddir)/src/librcgo.la
semantic_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
semantic_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

stack_SOURCES = stack.cpp $(HELPERS)
stack_LDADD = $(top_builddir)/src/librcgo.la
stack_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
stack_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

symbol_cast_SOURCES = symbol_cast.cpp $(HELPERS)
symbol_cast_LDADD = $(top_builddir)/src/librcgo.la
symbol_cast_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
symbol_cast_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

scope_SOURCES = scope.cpp $(HELPERS)
scope_LDADD = $(top_builddir)/src/librcgo.la
scope_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
scope_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

polymorphic_function_SOURCES = polymorphic_function.cpp $(HELPERS)
polymorphic_function_LDADD = $(top_builddir)/src/librcgo.la
polymorphic_function_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
polymorphic_function_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

runtime_types_SOURCES = runtime_types.cpp $(HELPERS)
runtime_types_LDADD = $(top_builddir)/src/librcgo.la
runtime_types_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
runtime_types_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

type_SOURCES = type.cpp $(HELPERS)
type_LDADD = $(top_builddir)/src/librcgo.la
type_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
type_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

value_SOURCES = value.cpp $(HELPERS)
value_LDADD = $(top_builddir)/src/librcgo.la
value_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
value_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

unit_test_SOURCES = $(HELPERS) \
unit_test.cpp \
callable.cpp \
enter_method_identifiers.cpp \
enter_predeclared_identifiers.cpp \
enter_top_level_identifiers.cpp \
error_reporter.cpp \
node.cpp \
process_top_level_identifiers.cpp \
process_type.cpp symbol.cpp
unit_test_LDADD = $(top_builddir)/src/librcgo.la
unit_test_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
unit_test_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)
