#!/bin/bash

$RC $srcdir/reaction_param_trigger.fbu 2>&1 | grep -q 'x is not accessible in this scope'
