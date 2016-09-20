#!/bin/bash

source $srcdir/helpers.sh

echo 1..4

expect_error2 1  "2147483648.0 to int8"   illegal_conversion1.rc 'cannot convert'
expect_error2 2  "2147483648.0 to int16"  illegal_conversion2.rc 'cannot convert'
expect_error2 3  "2147483648.0 to int32"  illegal_conversion3.rc 'cannot convert'
expect_error2 4  "2147483648.0 to string" illegal_conversion4.rc 'cannot convert'
