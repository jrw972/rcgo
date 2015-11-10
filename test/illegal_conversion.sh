#!/bin/bash

source $srcdir/helpers.sh

echo 1..4

expect_error 1  "2147483648.0 to int8"   illegal_conversion1.rc E90
expect_error 2  "2147483648.0 to int16"  illegal_conversion2.rc E90
expect_error 3  "2147483648.0 to int32"  illegal_conversion3.rc E90
expect_error 4  "2147483648.0 to string" illegal_conversion4.rc E90
