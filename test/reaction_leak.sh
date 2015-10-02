#!/bin/bash

$RC $srcdir/reaction_leak.fbu 2>&1 | grep -q 'signature leaks pointers'
