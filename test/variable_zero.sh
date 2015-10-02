#!/bin/bash

expected=`cat <<EOF
0
EOF`

actual=`$RC $srcdir/variable_zero.fbu 2>&1`

echo "Expected"
echo "$expected"

echo "Actual"
echo "$actual"

test "$actual" == "$expected"
