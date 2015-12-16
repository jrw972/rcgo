LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/build-aux/tap-driver.sh

#AM_TESTS_ENVIRONMENT = RC='$(top_builddir)/src/rc'; export RC; PATH="$(top_builddir)/src:$$PATH"; export PATH;
#LOG_COMPILER = $(SH)

AM_CXXFLAGS = -I $(top_srcdir)/src

TESTS = heap
check_PROGRAMS = heap

heap_SOURCES = heap.cpp
heap_LDADD = $(top_builddir)/src/librc.la
