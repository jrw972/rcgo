#!/bin/bash

echo 1..1

expected=`cat <<EOF
-10017 -10017
-3 -3
30 30
378 378
94 94
137 137
52 52
136 136
242 242
-1 -1
-138 -138
false false
true true
false false
false false
true true
true true
-10017 -10017
-3 -3
30 30
378 378
94 94
137 137
52 52
136 136
242 242
-1 -1
-138 -138
false false
true true
false false
false false
true true
true true
-10017 -10017
-3 -3
30 30
378 378
94 94
137 137
52 52
136 136
242 242
-1 -1
-138 -138
false false
true true
false false
false false
true true
true true
EOF`

actual=`$RC $srcdir/int_operators.rc 2>&1`

echo "Expected"
echo "$expected"

echo "Actual"
echo "$actual"

if test "$actual" == "$expected"
then
    echo 'ok 1 - int_operators.sh'
else
    echo 'not ok 1 - int_operators.sh'
fi
