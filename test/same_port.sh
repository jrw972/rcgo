#!/bin/bash

echo 1..1

if $RC $srcdir/same_port.rc 2>&1 | grep -q 'system is non-deterministic'
then
    echo 'ok 1 - same_port'
else
    echo 'not ok 1 - same_port'
fi
