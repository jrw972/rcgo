ACLOCAL_AMFLAGS=-I m4
SUBDIRS = src utest ftest

.PHONY: test
test: check

if COVERAGE
coverage:
	if ! [ -e src/parser.cpp ] ; then ln -s $(abs_top_srcdir)/src/parser.cpp src/parser.cpp ; fi
	if ! [ -e src/parser.y ] ; then ln -s $(abs_top_srcdir)/src/parser.y src/parser.y ; fi
	if ! [ -e src/scanner.cpp ] ; then ln -s $(abs_top_srcdir)/src/scanner.cpp src/scanner.cpp ; fi
	if ! [ -e src/scanner.l ] ; then ln -s $(abs_top_srcdir)/src/scanner.l src/scanner.l ; fi
	-rm -rf utest.info ftest.info html
	lcov --zerocounters --directory src --directory src/.libs
	cd utest && make check
	lcov --capture --directory src --directory src/.libs --output-file utest.info
	lcov --extract utest.info '*rc/*src/*' --output-file utest.filtered.info
	sed -i -e 's@$(abs_builddir)@$(realpath $(abs_top_srcdir))@g' utest.filtered.info
	genhtml -o html/utest utest.filtered.info
	lcov --zerocounters --directory src --directory src/.libs
	cd ftest && make check
	lcov --capture --directory src --directory src/.libs --output-file ftest.info
	lcov --extract ftest.info '*rc/*src/*' --output-file ftest.filtered.info
	sed -i -e 's@$(abs_builddir)@$(realpath $(abs_top_srcdir))@g' ftest.filtered.info
	genhtml -o html/ftest ftest.filtered.info
	@echo "file://$(PWD)/html/utest/index.html"
	@echo "file://$(PWD)/html/ftest/index.html"
endif
