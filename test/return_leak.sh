#!/bin/bash

echo 1..1

if $RC $srcdir/return_leak.rc 2>&1 | grep -q 'return leaks mutable pointers'
then
    echo 'ok 1 - return_leak'
else
    echo 'not ok 1 - return_leak'
fi
