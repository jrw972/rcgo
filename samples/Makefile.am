check_PROGRAMS = sample1 sample2 sample3 sample4 sample5 sample6 sample7 sample8 sample9 sample10 sample11 sample12

LINK = gcc -o $@

sample1_SOURCES = sample1.fbu
sample2_SOURCES = sample2.fbu
sample3_SOURCES = sample3.fbu
sample4_SOURCES = sample4.fbu
sample5_SOURCES = sample5.fbu
sample6_SOURCES = sample6.fbu
sample7_SOURCES = sample7.fbu
sample8_SOURCES = sample8.fbu
sample9_SOURCES = sample9.fbu
sample10_SOURCES = sample10.fbu
sample11_SOURCES = sample11.fbu
sample12_SOURCES = sample12.fbu

.fbu.o:
	$(top_builddir)/src/fbu -c $< -o $@

AM_TESTS_ENVIRONMENT = \
PATH=$(top_builddir)/samples:$$PATH; export PATH;

TESTS = \
sample1.sh \
sample2.sh \
sample3.sh \
sample4.sh \
sample5.sh \
sample6.sh \
sample7.sh \
sample8.sh \
sample9.sh \
sample10.sh \
sample11.sh \
sample12.sh

EXTRA_DIST = $(TESTS)