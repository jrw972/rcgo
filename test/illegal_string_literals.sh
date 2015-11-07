#!/bin/bash

source $srcdir/helpers.sh

echo 1..2

expect_error 1  "surrogate half"             string_surrogate_half.rc             E136
expect_error 2  "invalid Unicode code point" string_invalid_unicode_code_point.rc E135
