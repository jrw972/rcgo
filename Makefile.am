# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -I $(top_srcdir)/src

# Executables
bin_PROGRAMS = rcgo
rcgo_SOURCES = src/main.cpp
rcgo_LDADD = librcgo.la
rcgo_CXXFLAGS = $(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
rcgo_LDFLAGS = $(AM_LDFLAGS) $(COVERAGE_LDFLAGS)

# Libraries
noinst_LTLIBRARIES = librcgo.la
librcgo_la_SOURCES = \
src/Ast.hpp src/Ast.cpp \
src/AstVisitor.hpp src/AstVisitor.cpp \
src/ByteStream.hpp src/ByteStream.cpp \
src/DigitTable.cpp \
src/Error.hpp src/Error.cpp \
src/ErrorReporter.hpp src/ErrorReporter.cpp \
src/ImportLocation.hpp src/ImportLocation.cpp \
src/LetterTable.cpp \
src/Location.hpp src/Location.cpp \
src/Package.hpp src/Package.cpp \
src/Parser.hpp src/Parser.cpp \
src/Scanner.hpp src/Scanner.cpp \
src/Symbol.hpp src/Symbol.cpp \
src/SymbolTable.hpp src/SymbolTable.cpp \
src/SymbolVisitor.hpp src/SymbolVisitor.cpp \
src/Token.hpp src/Token.cpp \
src/Type.hpp src/Type.cpp \
src/TypeVisitor.hpp src/TypeVisitor.cpp \
src/UpperTable.cpp \
src/Utf8.hpp src/Utf8.cpp \
src/Value.hpp src/Value.cpp \
src/compile.cpp \
src/define_symbol.cpp \
src/define_symbols.cpp \
src/enter_predeclared_symbols.cpp \
src/evaluate_constant_expression.cpp \
src/find_package_source_directory.cpp \
src/functions.hpp \
src/get_and_check_package_name.cpp \
src/get_import_locations.cpp \
src/get_package_source_file_paths.cpp \
src/get_search_paths.cpp \
src/insert_symbol.cpp \
src/populate_file_block.cpp \
src/populate_package_block.cpp \
src/process_qualified_identifier.cpp \
src/process_type.cpp \
src/types.hpp
librcgo_la_CXXFLAGS = $(AM_CXXFLAGS) $(COVERAGE_CXXFLAGS)
librcgo_la_LDFLAGS = $(AM_LDFLAGS) $(COVERAGE_LDFLAGS)  -lgmpxx -lgmp

# Unit testing
LOG_DRIVER = $(SHELL) $(top_srcdir)/build-aux/catch-driver.sh

unit_tests_SOURCES = \
src/Ast.test.cpp \
src/AstVisitor.test.cpp \
src/ByteStream.test.cpp \
src/Error.test.cpp \
src/ErrorReporter.test.cpp \
src/ImportLocation.test.cpp \
src/Location.test.cpp \
src/Package.test.cpp \
src/Parser.test.cpp \
src/Scanner.test.cpp \
src/Symbol.test.cpp \
src/SymbolTable.test.cpp \
src/SymbolVisitor.test.cpp \
src/Token.test.cpp \
src/Type.test.cpp \
src/TypeVisitor.test.cpp \
src/Utf8.test.cpp \
src/Value.test.cpp \
src/catch.hpp \
src/compile.test.cpp \
src/enter_predeclared_symbols.test.cpp \
src/spies.hpp \
src/unit_tests.cpp
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

# %.hpp.cov %.cpp.cov: %-test
# 	find . -name "*.gcda" -delete
# 	./$*-test --use-colour no
# 	gcov --relative-only src/.libs/librcgo_la-$*.o
# 	if [ -e $*.hpp.gcov ]; then mv $*.hpp.gcov $*.hpp.cov; else touch $*.hpp.cov; fi
# 	mv $*.cpp.gcov $*.cpp.cov

# phony += $(COVERAGE_TARGETS)
# $(COVERAGE_TARGETS): %-coverage: %.hpp.cov %.cpp.cov
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
