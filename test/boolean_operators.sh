#!/bin/bash

expected=`cat <<EOF
false: false
true: true
const false!: true
const true!: false
const false || false: false
const false || true: true
const true || false: true
const true || true: true
const false && false: false
const false && true: false
const true && false: false
const true && true: true
var false!: true
var true!: false
var false || false: false
var false || true: true
var true || false: true
var true || true: true
var false && false: false
var false && true: false
var true && false: false
var true && true: true
short circuit &&
short circuit ||
EOF`

actual=`$RC $srcdir/boolean_operators.fbu 2>&1`

echo "Expected"
echo "$expected"

echo "Actual"
echo "$actual"

test "$actual" == "$expected"
