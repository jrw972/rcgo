LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/build-aux/tap-driver.sh

AM_CXXFLAGS = -I $(top_srcdir)/src

TESTS = action arch check_types expression_value heap error_reporter location memory_model node_cast parameter_list reaction semantic symbol_cast symbol_table type
check_PROGRAMS = $(TESTS)

action_SOURCES = action.cpp tap.hpp
action_LDADD = $(top_builddir)/src/librc.la
action_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
action_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

arch_SOURCES = arch.cpp tap.hpp
arch_LDADD = $(top_builddir)/src/librc.la
arch_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
arch_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

check_types_SOURCES = check_types.cpp tap.hpp
check_types_LDADD = $(top_builddir)/src/librc.la
check_types_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
check_types_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

expression_value_SOURCES = expression_value.cpp tap.hpp
expression_value_LDADD = $(top_builddir)/src/librc.la
expression_value_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
expression_value_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

heap_SOURCES = heap.cpp tap.hpp
heap_LDADD = $(top_builddir)/src/librc.la
heap_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
heap_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

error_reporter_SOURCES = error_reporter.cpp tap.hpp
error_reporter_LDADD = $(top_builddir)/src/librc.la
error_reporter_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
error_reporter_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

location_SOURCES = location.cpp tap.hpp
location_LDADD = $(top_builddir)/src/librc.la
location_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
location_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

memory_model_SOURCES = memory_model.cpp tap.hpp
memory_model_LDADD = $(top_builddir)/src/librc.la
memory_model_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
memory_model_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

node_cast_SOURCES = node_cast.cpp tap.hpp
node_cast_LDADD = $(top_builddir)/src/librc.la
node_cast_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
node_cast_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

parameter_list_SOURCES = parameter_list.cpp tap.hpp
parameter_list_LDADD = $(top_builddir)/src/librc.la
parameter_list_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
parameter_list_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

reaction_SOURCES = reaction.cpp tap.hpp
reaction_LDADD = $(top_builddir)/src/librc.la
reaction_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
reaction_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

semantic_SOURCES = semantic.cpp tap.hpp
semantic_LDADD = $(top_builddir)/src/librc.la
semantic_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
semantic_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

symbol_cast_SOURCES = symbol_cast.cpp tap.hpp
symbol_cast_LDADD = $(top_builddir)/src/librc.la
symbol_cast_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
symbol_cast_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

symbol_table_SOURCES = symbol_table.cpp tap.hpp
symbol_table_LDADD = $(top_builddir)/src/librc.la
symbol_table_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
symbol_table_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

type_SOURCES = type.cpp tap.hpp
type_LDADD = $(top_builddir)/src/librc.la
type_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
type_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)
