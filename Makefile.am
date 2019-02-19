# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -std=gnu++11 -I $(top_srcdir)/src

# Executables
bin_PROGRAMS = rcgo
rcgo_SOURCES = src/rcgo.cc
rcgo_LDADD = librcgo.la
rcgo_CXXFLAGS = $(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
rcgo_LDFLAGS = $(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

# Libraries
noinst_LTLIBRARIES = librcgo.la
librcgo_la_SOURCES = \
src/argument_parser.h src/argument_parser.cc \
src/argument_scanner.h \
src/ast.h src/ast.cc \
src/block.h src/block.cc \
src/byte_stream.h src/byte_stream.cc \
src/check_types.h src/check_types.cc \
src/compile.h src/compile.cc \
src/define_symbol.h src/define_symbol.cc \
src/define_symbols.h src/define_symbols.cc \
src/digit_table.cc \
src/error.h src/error.cc \
src/error_reporter.h src/error_reporter.cc \
src/get_and_check_package_name.h src/get_and_check_package_name.cc \
src/get_import_locations.h src/get_import_locations.cc \
src/import_location.h \
src/insert_symbol.h src/insert_symbol.cc \
src/letter_table.cc \
src/location.h src/location.cc \
src/package.h \
src/parser.h src/parser.cc \
src/paths.h src/paths.cc \
src/populate_file_block.h src/populate_file_block.cc \
src/populate_package_block.h src/populate_package_block.cc \
src/populate_universe_block.h src/populate_universe_block.cc \
src/process_qualified_identifier.h src/process_qualified_identifier.cc \
src/process_type.h src/process_type.cc \
src/scanner.h src/scanner.cc \
src/symbol.h src/symbol.cc \
src/token.h src/token.cc \
src/type.h src/type.cc \
src/untyped_constant.h src/untyped_constant.cc \
src/upper_table.cc \
src/utf8.h src/utf8.cc \
src/value.h src/value.cc
librcgo_la_CXXFLAGS = $(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
librcgo_la_LDFLAGS = $(AM_LDFLAGS) $(COVERAGE_LDFLAGS)  -lgmpxx -lgmp

# Unit testing
LOG_DRIVER = $(SHELL) $(top_srcdir)/build-aux/catch-driver.sh

unit_tests_SOURCES = \
src/ast_test.cc \
src/block_test.cc \
src/byte_stream_test.cc \
src/catch.hpp \
src/compile_test.cc \
src/error_reporter_test.cc \
src/error_test.cc \
src/import_location_test.cc \
src/location_test.cc \
src/package_test.cc \
src/parser_test.cc \
src/populate_universe_block_test.cc \
src/scanner_test.cc \
src/symbol_test.cc \
src/test_helpers.h \
src/token_test.cc \
src/type_test.cc \
src/unit_tests.cc \
src/untyped_constant_test.cc \
src/utf8_test.cc \
src/value_test.cc
unit_tests_CXXFLAGS = $(AM_CXXFLAGS) -DUNIT_TEST_DIR="\"$(top_srcdir)/unit-tests\""
unit_tests_LDADD = $(top_builddir)/librcgo.la

TESTS = unit_tests
check_PROGRAMS = unit_tests
EXTRA_DIST = unit-tests/hello.txt unit-tests/src

# Add target for TravisCI
phony = test
test: check

# Coverage build
if COVERAGE

GCNO_FILES := $(shell find src -name '*.gcno')
GCDA_FILES := $(shell find src -name '*.gcda')
GCOV_FILES := $(shell find . -name '*.gcov')
COV_FILES := $(shell find . -name '*.cov')
# COVERAGE_TARGETS := $(UNIT_TESTS:-test=-coverage)

# %.h.cov %.cc.cov: %-test
# 	find . -name "*.gcda" -delete
# 	./$*-test --use-colour no
# 	gcov --relative-only src/.libs/librcgo_la-$*.o
# 	if [ -e $*.h.gcov ]; then mv $*.h.gcov $*.h.cov; else touch $*.h.cov; fi
# 	mv $*.cc.gcov $*.cc.cov

# phony += $(COVERAGE_TARGETS)
# $(COVERAGE_TARGETS): %-coverage: %.h.cov %.cc.cov
# 	sed -E -n -e '/NOT_COVERED/ d' -e '/^ *-: *0:Source:.+$$/ p' -e '/^ *#####: *[0-9]+:.*$$/ p' $^

# phony += coverage
# coverage: $(COVERAGE_TARGETS)

phony += coverage
coverage: unit_tests
	$(top_srcdir)/coverage.sh $(top_srcdir)

clean-local:
	-rm -rf $(GCNO_FILES) $(GCDA_FILES) $(GCOV_FILES) $(COV_FILES)

endif

.PHONY: $(phony)
