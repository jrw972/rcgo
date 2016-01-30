LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/build-aux/tap-driver.sh

AM_CXXFLAGS = -I $(top_srcdir)/src

TESTS = action arch heap error_reporter memory_model reaction type
check_PROGRAMS = $(TESTS)

action_SOURCES = action.cpp tap.hpp
action_LDADD = $(top_builddir)/src/librc.la

arch_SOURCES = arch.cpp tap.hpp
arch_LDADD = $(top_builddir)/src/librc.la

heap_SOURCES = heap.cpp tap.hpp
heap_LDADD = $(top_builddir)/src/librc.la

error_reporter_SOURCES = error_reporter.cpp tap.hpp
error_reporter_LDADD = $(top_builddir)/src/librc.la

memory_model_SOURCES = memory_model.cpp tap.hpp
memory_model_LDADD = $(top_builddir)/src/librc.la

reaction_SOURCES = reaction.cpp tap.hpp
reaction_LDADD = $(top_builddir)/src/librc.la

type_SOURCES = type.cpp tap.hpp
type_LDADD = $(top_builddir)/src/librc.la
