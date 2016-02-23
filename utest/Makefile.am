LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/build-aux/tap-driver.sh

AM_CXXFLAGS = -I $(top_srcdir)/src

TESTS = action arch heap error_reporter memory_model parameter_list reaction type
check_PROGRAMS = $(TESTS)

action_SOURCES = action.cpp tap.hpp
action_LDADD = $(top_builddir)/src/librc.la
action_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
action_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

arch_SOURCES = arch.cpp tap.hpp
arch_LDADD = $(top_builddir)/src/librc.la
arch_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
arch_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

heap_SOURCES = heap.cpp tap.hpp
heap_LDADD = $(top_builddir)/src/librc.la
heap_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
heap_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

error_reporter_SOURCES = error_reporter.cpp tap.hpp
error_reporter_LDADD = $(top_builddir)/src/librc.la
error_reporter_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
error_reporter_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

memory_model_SOURCES = memory_model.cpp tap.hpp
memory_model_LDADD = $(top_builddir)/src/librc.la
memory_model_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
memory_model_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

parameter_list_SOURCES = parameter_list.cpp tap.hpp
parameter_list_LDADD = $(top_builddir)/src/librc.la
parameter_list_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
parameter_list_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

reaction_SOURCES = reaction.cpp tap.hpp
reaction_LDADD = $(top_builddir)/src/librc.la
reaction_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
reaction_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

type_SOURCES = type.cpp tap.hpp
type_LDADD = $(top_builddir)/src/librc.la
type_CXXFLAGS=$(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
type_LDFLAGS=$(AM_LDFLAGS) $(COVERAGE_LDFLAGS)
