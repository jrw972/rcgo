#!/bin/bash

$FBU $srcdir/save_foreign.fbu 2>&1 | grep -q 'cannot assign to read-only location'
