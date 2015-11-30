#!/bin/bash

source $srcdir/helpers.sh

echo 1..1

expect_error 1 "return casts away +const or +foreign" return_leak.rc E149
