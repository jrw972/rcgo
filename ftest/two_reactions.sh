#!/bin/bash

echo 1..1

expected=`cat <<EOF
Bang Immutable
Bang Immutable
Bang Mutable
Bang Mutable
EOF`

actual=`$RCGO $srcdir/two_reactions.rc 2>&1`

echo "Expected"
echo "$expected"

echo "Actual"
echo "$actual"

if test "$actual" == "$expected"
then
    echo 'ok 1 - two_reactions'
else
    echo 'not ok 1 - two_reactions'
fi
