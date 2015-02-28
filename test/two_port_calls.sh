#!/bin/bash

expected=`cat <<EOF
Bang
Bang
EOF`

actual=`$FBU $srcdir/two_port_calls.fbu 2>&1`

echo "Expected"
echo "$expected"

echo "Actual"
echo "$actual"

test "$actual" == "$expected"
