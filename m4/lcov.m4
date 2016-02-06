AC_DEFUN([AC_LCOV],
[
  AC_ARG_ENABLE([lcov],
    AS_HELP_STRING([--enable-lcov], [enable code coverage with lcov]))

  AS_IF([test "x$enable_lcov" = "xyes"], [

    changequote({,})
    CXXFLAGS=`echo "$CXXFLAGS" | $SED -e 's/-O[0-9]*//g'`
    changequote([,])

    COVERAGE_CXXFLAGS="--coverage"
    COVERAGE_LDFLAGS="--coverage -lgcov"

    AC_DEFINE([COVERAGE], [1], [Indicates a coverage build])
    AC_SUBST([COVERAGE_CXXFLAGS])
    AC_SUBST([COVERAGE_LDFLAGS])
  ])

  AM_CONDITIONAL([COVERAGE], [test "x$enable_lcov" = "xyes"])
]) # AC_LCOV
