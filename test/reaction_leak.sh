#!/bin/bash

$FBU $srcdir/reaction_leak.fbu 2>&1 | grep -q 'signature leaks pointers'
