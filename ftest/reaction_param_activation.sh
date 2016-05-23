#!/bin/bash

echo 1..1

if $RC $srcdir/reaction_param_activation.rc 2>&1 | grep -q 'E5'
then
    echo 'ok 1 - reaction_param_activation'
else
    echo 'not ok 1 - reaction_param_activation'
fi
