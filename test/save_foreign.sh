#!/bin/bash

echo 1..1

if $RC $srcdir/save_foreign.rc 2>&1 | grep -q 'assignment leaks mutable pointer'
then
    echo 'ok 1 - save_foreign'
else
    echo 'not ok 1 - save_foreign'
fi
