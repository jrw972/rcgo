#!/bin/bash

$RC $srcdir/change_root-2.fbu 2>&1 | grep -q 'change_root-2.fbu:3: E66: change leaks mutable pointers'
