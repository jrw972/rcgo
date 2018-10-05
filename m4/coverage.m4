# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

AC_DEFUN([AC_COVERAGE],
[
  AC_ARG_ENABLE([coverage],
    AS_HELP_STRING([--enable-coverage], [enable code coverage with gcov]))

  AS_IF([test "x$enable_coverage" = "xyes"], [

    changequote({,})
    CXXFLAGS="$(echo "$CXXFLAGS" | $SED -e 's/-O[0-9]*/-O0/g') -fprofile-arcs -ftest-coverage -fno-inline -fkeep-inline-functions "
    changequote([,])

    COVERAGE_CXXFLAGS="--coverage"
    COVERAGE_LDFLAGS="--coverage -lgcov"

    AC_DEFINE([COVERAGE], [1], [Indicates a coverage build])
    AC_SUBST([COVERAGE_CXXFLAGS])
    AC_SUBST([COVERAGE_LDFLAGS])
  ])

  AM_CONDITIONAL([COVERAGE], [test "x$enable_coverage" = "xyes"])
]) # AC_COVERAGE
