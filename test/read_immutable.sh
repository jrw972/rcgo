#!/bin/bash

echo 1..1

if $RC $srcdir/read_immutable.rc
then
    echo 'ok 1 - read_immutable'
else
    echo 'not ok 1 - read_immutable'
fi
