#!/bin/bash

echo 1..1

if $RC $srcdir/write_foreign.rc 2>&1 | grep -q 'cannot assign to read-only location'
then
    echo 'ok 1 - write_foreign'
else
    echo 'not ok 1 - write_foreign'
fi
