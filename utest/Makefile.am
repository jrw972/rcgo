LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/build-aux/tap-driver.sh

AM_CXXFLAGS = -I $(top_srcdir)/src

TESTS = action arch bind builtin_function check_types expression_value heap error_reporter location memory_model node_cast parameter_list reaction semantic stack symbol_cast symbol_table template type
check_PROGRAMS = $(TESTS)

HELPERS=tap.hpp visitor_helper.hpp

action_SOURCES = action.cpp $(HELPERS)
action_LDADD = $(top_builddir)/src/librc.la
action_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
action_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

arch_SOURCES = arch.cpp $(HELPERS)
arch_LDADD = $(top_builddir)/src/librc.la
arch_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
arch_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

bind_SOURCES = bind.cpp $(HELPERS)
bind_LDADD = $(top_builddir)/src/librc.la
bind_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
bind_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

builtin_function_SOURCES = builtin_function.cpp $(HELPERS)
builtin_function_LDADD = $(top_builddir)/src/librc.la
builtin_function_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
builtin_function_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

check_types_SOURCES = check_types.cpp $(HELPERS)
check_types_LDADD = $(top_builddir)/src/librc.la
check_types_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
check_types_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

expression_value_SOURCES = expression_value.cpp $(HELPERS)
expression_value_LDADD = $(top_builddir)/src/librc.la
expression_value_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
expression_value_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

heap_SOURCES = heap.cpp $(HELPERS)
heap_LDADD = $(top_builddir)/src/librc.la
heap_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
heap_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

error_reporter_SOURCES = error_reporter.cpp $(HELPERS)
error_reporter_LDADD = $(top_builddir)/src/librc.la
error_reporter_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
error_reporter_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

location_SOURCES = location.cpp $(HELPERS)
location_LDADD = $(top_builddir)/src/librc.la
location_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
location_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

memory_model_SOURCES = memory_model.cpp $(HELPERS)
memory_model_LDADD = $(top_builddir)/src/librc.la
memory_model_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
memory_model_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

node_cast_SOURCES = node_cast.cpp $(HELPERS)
node_cast_LDADD = $(top_builddir)/src/librc.la
node_cast_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
node_cast_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

parameter_list_SOURCES = parameter_list.cpp $(HELPERS)
parameter_list_LDADD = $(top_builddir)/src/librc.la
parameter_list_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
parameter_list_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

reaction_SOURCES = reaction.cpp $(HELPERS)
reaction_LDADD = $(top_builddir)/src/librc.la
reaction_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
reaction_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

semantic_SOURCES = semantic.cpp $(HELPERS)
semantic_LDADD = $(top_builddir)/src/librc.la
semantic_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
semantic_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

stack_SOURCES = stack.cpp $(HELPERS)
stack_LDADD = $(top_builddir)/src/librc.la
stack_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
stack_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

symbol_cast_SOURCES = symbol_cast.cpp $(HELPERS)
symbol_cast_LDADD = $(top_builddir)/src/librc.la
symbol_cast_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
symbol_cast_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

symbol_table_SOURCES = symbol_table.cpp $(HELPERS)
symbol_table_LDADD = $(top_builddir)/src/librc.la
symbol_table_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
symbol_table_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

template_SOURCES = template.cpp $(HELPERS)
template_LDADD = $(top_builddir)/src/librc.la
template_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
template_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

type_SOURCES = type.cpp $(HELPERS)
type_LDADD = $(top_builddir)/src/librc.la
type_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
type_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)
