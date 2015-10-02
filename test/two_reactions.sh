#!/bin/bash

expected=`cat <<EOF
Bang Immutable
Bang Immutable
Bang Mutable
Bang Mutable
EOF`

actual=`$RC $srcdir/two_reactions.fbu 2>&1`

echo "Expected"
echo "$expected"

echo "Actual"
echo "$actual"

test "$actual" == "$expected"
