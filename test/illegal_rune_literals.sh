#!/bin/bash

source $srcdir/helpers.sh

echo 1..6

expect_error 1  "too many characters"        too_many_characters.rc        E66
expect_error 2  "too few hexadecimal digits" too_few_hexadecimal_digits.rc E66
expect_error 3  "too few octal digits"       too_few_octal_digits.rc       E66
expect_error 4  "surrogate half"             surrogate_half.rc             E136
expect_error 5  "invalid Unicode code point" invalid_unicode_code_point.rc E135
expect_error 6  "illegal octal"              illegal_octal.rc              E134
