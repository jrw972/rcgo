#                                               -*- Autoconf -*-

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([rcgo], [0.1.0], [https://github.com/jrw972/rcgo/issues], [rcgo], [https://github.com/jrw972/rcgo])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_MAINTAINER_MODE([disable])
AC_CONFIG_SRCDIR([src/rcgo.cc])
AC_CONFIG_HEADERS([config.hpp])
AC_REQUIRE_AUX_FILE([catch-driver.sh])

# Checks for programs.
LT_INIT([])
AC_PROG_CXX()

# Checks for libraries.
AC_SEARCH_LIBS(__gmpz_init, gmp, , [AC_MSG_ERROR([GNU MP not found, see https://gmplib.org/])])
AC_SEARCH_LIBS(_ZN4YAML8LoadFileERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE, yaml-cpp, , [AC_MSG_ERROR([yaml-cpp not found])])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Enable coverage builds
AC_COVERAGE()

AC_OUTPUT([Makefile
           setenv.sh])
