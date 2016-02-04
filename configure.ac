#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Reactive Components],[0.01],[jrw972@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wno-portability])
AM_MAINTAINER_MODE([disable])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_REQUIRE_AUX_FILE([tap-driver.sh])

# Checks for programs.
LT_INIT([])
AC_PROG_CXX()
AM_PROG_LEX()
AC_PROG_YACC()

# Checks for libraries.
AC_SEARCH_LIBS([clock_gettime],[rt])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 ftest/Makefile
                 utest/Makefile])
AC_OUTPUT
