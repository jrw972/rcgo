#!/bin/bash

echo 1..1

if $RC $srcdir/reaction_leak.rc 2>&1 | grep -q 'signature leaks pointers'
then
    echo 'ok 1 - reaction_leak'
else
    echo 'not ok 1 - reaction_leak'
fi
