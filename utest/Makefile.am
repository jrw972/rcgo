LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/build-aux/tap-driver.sh

AM_CXXFLAGS = -I $(top_srcdir)/src

TESTS = heap error_reporter
check_PROGRAMS = $(TESTS)

heap_SOURCES = heap.cpp tap.hpp
heap_LDADD = $(top_builddir)/src/librc.la

error_reporter_SOURCES = error_reporter.cpp tap.hpp
error_reporter_LDADD = $(top_builddir)/src/librc.la
