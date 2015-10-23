#!/bin/bash

echo 1..1

expected=`cat <<EOF
0
EOF`

actual=`$RC $srcdir/variable_zero.rc 2>&1`

echo "Expected"
echo "$expected"

echo "Actual"
echo "$actual"

if test "$actual" == "$expected"
then
    echo 'ok 1 - variable_zero'
else
    echo 'not ok 1 - variable_zero'
fi
