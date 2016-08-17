LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/build-aux/tap-driver.sh

AM_CXXFLAGS = -I $(top_srcdir)/src

EXTRA_DIST = node.test

%.cpp : %.test
	awk -f $(top_srcdir)/utest/testpro.awk $< > $@

TESTS = \
 action \
 arch \
 bind \
 callable \
 check_types \
 error_reporter \
 expression_value \
 heap \
 location memory_model \
 node_cast \
 parameter_list \
 polymorphic_function \
 runtime_types \
 semantic \
 stack \
 symbol \
 symbol_cast \
 symbol_table \
 type \
 value \
 unit_test
check_PROGRAMS = $(TESTS)

HELPERS=unit_test.hpp tap.hpp visitor_helper.hpp mock_execution.hpp

action_SOURCES = action.cpp $(HELPERS)
action_LDADD = $(top_builddir)/src/librcgo.la
action_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
action_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

arch_SOURCES = arch.cpp $(HELPERS)
arch_LDADD = $(top_builddir)/src/librcgo.la
arch_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
arch_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

bind_SOURCES = bind.cpp $(HELPERS)
bind_LDADD = $(top_builddir)/src/librcgo.la
bind_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
bind_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

callable_SOURCES = callable.cpp $(HELPERS)
callable_LDADD = $(top_builddir)/src/librcgo.la
callable_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
callable_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

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

error_reporter_SOURCES = error_reporter.cpp $(HELPERS)
error_reporter_LDADD = $(top_builddir)/src/librcgo.la
error_reporter_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
error_reporter_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

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

symbol_SOURCES = symbol.cpp $(HELPERS)
symbol_LDADD = $(top_builddir)/src/librcgo.la
symbol_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
symbol_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

symbol_cast_SOURCES = symbol_cast.cpp $(HELPERS)
symbol_cast_LDADD = $(top_builddir)/src/librcgo.la
symbol_cast_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
symbol_cast_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

symbol_table_SOURCES = symbol_table.cpp $(HELPERS)
symbol_table_LDADD = $(top_builddir)/src/librcgo.la
symbol_table_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
symbol_table_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

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

unit_test_SOURCES = unit_test.cpp node.cpp $(HELPERS)
unit_test_LDADD = $(top_builddir)/src/librcgo.la
unit_test_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
unit_test_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)
