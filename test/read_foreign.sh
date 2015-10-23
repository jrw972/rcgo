#!/bin/bash

echo 1..1

if $RC $srcdir/read_foreign.rc
then
    echo 'ok 1 - read_foreign'
else
    echo 'not ok 1 - read_foreign'
fi
