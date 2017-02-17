LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/build-aux/tap-driver.sh

AM_CXXFLAGS = -I $(top_srcdir)/src

EXTRA_DIST = callable.test \
check_all.test \
enter_method_identifiers.test \
error_reporter.test \
expression_value.test \
node.test \
polymorphic_function.test \
populate_package_block.test \
populate_universe_block.test \
process_top_level_identifiers.test \
process_type.test \
symbol.test \
symbol_table.test \
value.test

%.cpp : %.test
	awk -f $(top_srcdir)/utest/testpro.awk $< > $@

TESTS = \
 arch \
 heap \
 location memory_model \
 node_cast \
 parameter_list \
 runtime_types \
 operation \
 stack \
 symbol_cast \
 scope \
 type \
 unit_test
check_PROGRAMS = $(TESTS)

HELPERS=unit_test.hpp tap.hpp visitor_helper.hpp mock_execution.hpp astgen.hpp astgen.cpp

arch_SOURCES = arch.cpp $(HELPERS)
arch_LDADD = $(top_builddir)/src/librcgo.la
arch_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
arch_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

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

operation_SOURCES = operation.cpp $(HELPERS)
operation_LDADD = $(top_builddir)/src/librcgo.la
operation_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
operation_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

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

runtime_types_SOURCES = runtime_types.cpp $(HELPERS)
runtime_types_LDADD = $(top_builddir)/src/librcgo.la
runtime_types_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
runtime_types_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

type_SOURCES = type.cpp $(HELPERS)
type_LDADD = $(top_builddir)/src/librcgo.la
type_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
type_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

unit_test_SOURCES = $(HELPERS) \
unit_test.cpp \
callable.cpp \
check_all.cpp \
enter_method_identifiers.cpp \
error_reporter.cpp \
expression_value.cpp \
node.cpp \
polymorphic_function.cpp \
populate_package_block.cpp \
populate_universe_block.cpp \
process_top_level_identifiers.cpp \
process_type.cpp \
symbol.cpp \
symbol_table.cpp \
value.cpp
unit_test_LDADD = $(top_builddir)/src/librcgo.la
unit_test_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
unit_test_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)
