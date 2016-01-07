LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/build-aux/tap-driver.sh

AM_CXXFLAGS = -I $(top_srcdir)/src

check_PROGRAMS = heap
heap_SOURCES = heap.cpp
heap_LDADD = $(top_builddir)/src/librc.la

TESTS = heap
