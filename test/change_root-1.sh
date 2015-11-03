#!/bin/bash

echo 1..1

if $RC $srcdir/change_root-1.rc 2>&1 | grep -q 'E49'
then
    echo 'ok 1 - change_root-1'
else
    echo 'not ok 1 - change_root-1'
fi
