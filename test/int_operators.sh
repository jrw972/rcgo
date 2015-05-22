#!/bin/bash

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

actual=`$FBU $srcdir/int_operators.fbu 2>&1`

echo "Expected"
echo "$expected"

echo "Actual"
echo "$actual"

test "$actual" == "$expected"
